// ============================================================
// 联机对战（局域网 TCP）
//
// 总体思路：
//   1. 房主(Host)：socket -> bind(端口) -> listen -> accept，等对方连进来
//      加入者(Join)：socket -> connect(房主IP, 端口)
//      连上之后两边就平等了，互相 send/recv。
//   2. 所有 socket 都用 FIONBIO 设成"非阻塞"：accept/recv/connect 没数据时
//      立刻返回，不会把游戏主循环卡住。每帧 updateNet() 推进一点，
//      和回放、AI 的帧驱动是同一个套路。
//   3. TCP 是"字节流"，它只保证字节顺序，不保证你 send 一次、对方 recv 就收到一条。
//      所以自己定协议：固定长度的 NetMsg 结构体。收到半条先攒在 net.buf 里，
//      攒够 sizeof(NetMsg) 才当成一条完整消息处理。
//   4. 规则约定：房主执黑先行。谁本地落子就把坐标发给对方，
//      对方收到 MOVE 就调 placeStone 替他落子 —— 用的是同一套胜负判断，
//      所以两边的棋盘天然保持一致，不需要同步整张棋盘。
// ============================================================
#include "gomoku.h"

#pragma comment(lib, "ws2_32.lib")

Net net;


const int NET_MSG_MOVE = 1;
const int NET_MSG_SURRENDER = 2;
const int NET_MSG_RESTART = 3;

// 三个 int，一共 12 字节。两边跑的是同一个程序、同一个结构体，
// 直接把这 12 字节原样发过去就行（自己跟自己通信，不用考虑字节序和兼容）
struct NetMsg{
    int type;
    int i;
    int j;
};

bool netInited = false;

bool netInit(){
    if(netInited) return true;

    WSADATA wsa;
    if(WSAStartup(MAKEWORD(2,2), &wsa) != 0){
        net.state = NET_LOST;
        net.info = "WSAStartup failed";
        return false;
    }
    netInited = true;
    return true;
}

//tool
void netSetNonBlock(SOCKET s){
    u_long mode = 1;
    ioctlsocket(s, FIONBIO, &mode);
}


void netClose(){
    if(net.sock != INVALID_SOCKET){
        closesocket(net.sock);
        net.sock = INVALID_SOCKET;
    }
    if(net.listenSock != INVALID_SOCKET){
        closesocket(net.listenSock);
        net.listenSock = INVALID_SOCKET;
    }
    net.bufLen = 0;
    net.tick = 0;
    net.state = NET_IDLE;
    net.info = "";
}


void netFindLocalIP(){
    char name[128] = {0};
    if(gethostname(name, sizeof(name)) != 0) return;

    hostent *host = gethostbyname(name);
    if(host == nullptr) return;

    for(int i = 0; host->h_addr_list[i] != nullptr; i++){
        in_addr addr;
        memcpy(&addr, host->h_addr_list[i], sizeof(addr));
        char *ip = inet_ntoa(addr);
        if(strcmp(ip, "127.0.0.1") == 0) continue;
        if(strncmp(ip, "169.254.", 8) == 0) continue;
        if(strncmp(ip, "192.168.", 8) == 0) continue;

        if(!net.ip.empty()) net.ip += " / ";
        net.ip += ip;
    }
}


void netSend(int type, int i, int j){
    if(net.sock == INVALID_SOCKET) return;

    NetMsg m;
    m.type = type;
    m.i = i;
    m.j = j;
    send(net.sock, (char*)&m, static_cast<int>(sizeof(m)), 0);
}

void netLost(const char* msg){
    bool playing = (net.state == NET_PLAY);

    netClose();
    net.state = NET_LOST;
    net.info = msg;

    if(playing && game.state == 1){
        game.winner = net.myColor;
        game.state = 2;
    }
    updateButtons();
}

void netHandle(NetMsg m){
    if(m.type == NET_MSG_MOVE){
        if(m.i < 0 || m.i >= BOARD_SIZE || m.j < 0 || m.j >= BOARD_SIZE) return;
        if(game.board[m.i][m.j] != STONE_EMPTY) return;
        if(!netWaiting()) return;

        placeStone(m.i, m.j);
    }
    else if(m.type == NET_MSG_SURRENDER){
        game.winner = net.myColor;
        game.state = 2;
        updateButtons();
    }
    else if(m.type == NET_MSG_RESTART){
        startGame();
    }
}

void netRecv(){
    while(net.bufLen < static_cast<int>(sizeof(net.buf))){
        int n = recv(net.sock, net.buf + net.bufLen,
                     static_cast<int>(sizeof(net.buf)) - net.bufLen, 0);

        if(n > 0){
            net.bufLen += n;
            continue;
        }
        if(n == 0){
            netLost("peer left");
            return;
        }
        if(WSAGetLastError() != WSAEWOULDBLOCK){
            netLost("recv error");
            return;
        }
        break;
    }

    while(net.bufLen >= static_cast<int>(sizeof(NetMsg))){
        NetMsg m;
        memcpy(&m, net.buf, sizeof(m));
        net.bufLen -= static_cast<int>(sizeof(m));
        memmove(net.buf, net.buf + sizeof(m), net.bufLen);
        netHandle(m);
    }
}

void netHost(){
    if(!netInit()) return;
    net.ip = "";
    netClose();
    netFindLocalIP();

    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//TCP
    if(s == INVALID_SOCKET){
        net.state = NET_LOST;
        net.info = "socket failed";
        return;
    }

    BOOL yes = TRUE;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char*)&yes, sizeof(yes));

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;//IPv4
    addr.sin_port = htons(NET_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;
    if(bind(s, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR){
        closesocket(s);
        net.state = NET_LOST;
        net.info = "bind failed";
        return;
    }

    listen(s, 1);
    netSetNonBlock(s);
    net.listenSock = s;

    net.state = NET_WAIT;
    net.info = "waiting player ...";
    updateButtons();
}

void netJoin(){
    if(!netInit()) return;

    char buf[64] = {0};
    if(inputbox_getline("Net Game", "Input Host IP:", buf, static_cast<int>(sizeof(buf))) == 0) return;
    if(buf[0] == 0) strcpy_s(buf, "127.0.0.1");
    while(mousemsg()) getmouse();

    netClose();

    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(s == INVALID_SOCKET){
        net.state = NET_LOST;
        net.info = "socket failed";
        return;
    }

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(NET_PORT);
    addr.sin_addr.s_addr = inet_addr(buf);
    if(addr.sin_addr.s_addr == INADDR_NONE){
        closesocket(s);
        net.state = NET_LOST;
        net.info = "bad ip";
        return;
    }

    netSetNonBlock(s);
    connect(s, (sockaddr*)&addr, sizeof(addr));

    net.sock = s;
    net.state = NET_CONNECTING;
    net.tick = 0;
    net.info = "connecting ...";
    updateButtons();
}


void updateNet(){
    //waiting
    if(net.state == NET_WAIT){
        SOCKET s = accept(net.listenSock, NULL, NULL);
        if(s == INVALID_SOCKET) return;
        closesocket(net.listenSock);
        net.listenSock = INVALID_SOCKET;
        netSetNonBlock(s);
        net.sock = s;
        net.myColor = STONE_BLACK;
        net.choosing = false;
        net.state = NET_PLAY;
        net.info = "";
        startGame();
        return;
    }

    if(net.state == NET_CONNECTING){
        fd_set wfds;
        FD_ZERO(&wfds);
        FD_SET(net.sock, &wfds);

        timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 0;
        //try write
        if(select(0, NULL, &wfds, NULL, &tv) > 0){
            int err = 0;
            int len = sizeof(err);
            getsockopt(net.sock, SOL_SOCKET, SO_ERROR, (char*)&err, &len);
            if(err == 0){
                net.myColor = STONE_WHITE;
                net.choosing = false;
                net.state = NET_PLAY;
                net.info = "";
                startGame();
            }
            else{
                netClose();
                net.state = NET_LOST;
                net.info = "connect failed";
            }
        }
        else if(++net.tick > NET_TIMEOUT_FRAMES){//timelimited
            netClose();
            net.state = NET_LOST;
            net.info = "connect timeout";
        }
        return;
    }

    if(net.state == NET_PLAY) netRecv();//对局中只管收对方的包
}


bool netWaiting(){
    return net.state == NET_PLAY && turnColor() != net.myColor;
}

void netSendMove(int i, int j){
    if(net.state == NET_PLAY) netSend(NET_MSG_MOVE, i, j);
}

void netSendSurrender(){
    if(net.state == NET_PLAY) netSend(NET_MSG_SURRENDER, -1, -1);
}

void netSendRestart(){
    if(net.state == NET_PLAY) netSend(NET_MSG_RESTART, -1, -1);
}

void netGame(){
    net.choosing = !net.choosing;
    if(!net.choosing) netClose();
    updateButtons();
}

void netLeave(){
    netClose();
    game.state = 0;
    updateButtons();
}

void drawNetInfo(){
    if(net.state == NET_IDLE && net.info.empty()) return;

    setfont(20, 0, "微软雅黑");
    setbkmode(TRANSPARENT);

    if(net.state == NET_PLAY){
        setcolor(EGERGB(0,120,0));
        outtextxy(WIN_W - 60, 12, "NET");
        return;
    }

    setcolor(EGERGB(180,0,0));
    string line = net.info;
    if(!net.ip.empty()) line = line + "   my ip: " + net.ip;
    outtextxy(20, 12, line.c_str());
}
