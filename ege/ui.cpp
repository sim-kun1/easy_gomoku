// 按钮与鼠标交互
#include "gomoku.h"

int curScreen(){
    if(game.isReplaying) return 3;
    if(game.state == 1) return 1;
    if(game.state == 2) return 2;
    return 0;
}

void updateButtons(){
    int screen = curScreen();
    for(auto &btn : game.btnList)
    {
        bool show = (btn.screen == screen);
        if(ai.choosing) show = show && btn.onlyInAi;
        else if(net.choosing) show = show && btn.onlyInNet;
        else if(btn.onlyInAi) show = false;
        else if(btn.onlyInNet && net.state == NET_IDLE) show = false;
        btn.Hide(!show);
    }
}

void startGame(){
    ai.enabled = false;
    ai.choosing = false;
    resetState();
    game.gameRecord.clear();
    game.state = 1;
    updateButtons();
}

void restartGame(){
    if(net.state == NET_PLAY){
        netSendRestart();
        startGame();
        return;
    }
    resetState();
    game.state = 0;
    updateButtons();
}

void playRecord(){
    if(game.gameRecord.empty()) {
        setfont(36,0,"微软雅黑");
        setcolor(EGERGB(239,52,115));
        string msg = "NOT FIND RECORD";
        const char* str = msg.c_str();
        int tw = textwidth(str);
        outtextxy((WIN_W - tw)/2, 300, str);
        delay_ms(1000);
        while(mousemsg()) getmouse();
        return;
    }
    game.isReplaying = true;
    game.replayIndex = 0;
    game.replayTick = 0;
    memset(game.board, 0, sizeof(game.board));
    game.previewI = -1;
    game.previewJ = -1;
    game.state = 1;
    game.blackTurn = true;
    updateButtons();
}

void updateReplay(){
    if(!game.isReplaying) return;

    game.replayTick++;
    if(game.replayTick < REPLAY_FRAMES) return;
    game.replayTick = 0;

    if(game.replayIndex >= static_cast<int>(game.gameRecord.size())){
        game.isReplaying = false;
        game.state = 2;
        if(game.gameRecord.back().color == STONE_BLACK)
            game.winner = STONE_BLACK;
        else
            game.winner = STONE_WHITE;
        updateButtons();
        return;
    }

    auto &step = game.gameRecord[game.replayIndex];
    game.board[step.i][step.j] = step.color;
    game.blackTurn = !game.blackTurn;
    game.replayIndex++;
}

void stopReplay(){
    game.isReplaying = false;
    game.state = 0;
    updateButtons();
}

void surrender(){
    if(net.state == NET_PLAY){
        if(net.myColor == STONE_BLACK) game.winner = STONE_WHITE;
        else game.winner = STONE_BLACK;
    }
    else if(game.blackTurn == 1) game.winner = STONE_WHITE;
    else game.winner = STONE_BLACK;
    game.state = 2;
    netSendSurrender();
    updateButtons();
}

void initButtons(){
    Button btnStart(265,400,150,50,"StartGame");
    btnStart.onClick = startGame;
    game.btnList.push_back(btnStart);

    Button btnNetStart(265,475,150,50,"NetGame");
    btnNetStart.onClick = netGame;
    game.btnList.push_back(btnNetStart);

    Button btnAIStart(265,550,150,50,"AIGame");
    btnAIStart.onClick = aiGame;
    game.btnList.push_back(btnAIStart);

    Button btnSurrender(275,WIN_H-50,120,40,"Surrender");
    btnSurrender.onClick = surrender;
    btnSurrender.screen = 1;
    game.btnList.push_back(btnSurrender);

    Button btnRestart(265,500,150,50,"Restart");
    btnRestart.onClick = restartGame;
    btnRestart.screen = 2;
    game.btnList.push_back(btnRestart);
    
    Button btnRecord(265,625,150,50,"PlayRecord");
    btnRecord.onClick = playRecord;
    game.btnList.push_back(btnRecord);

    Button btnStop(275,WIN_H-50,120,40,"StopReplay");
    btnStop.onClick = stopReplay;
    btnStop.screen = 3;
    game.btnList.push_back(btnStop);

    Button btnAiBlack(265,400,150,50,"AI-Black");
    btnAiBlack.onClick = aiChooseBlack;
    btnAiBlack.onlyInAi = true;
    game.btnList.push_back(btnAiBlack);

    Button btnAiWhite(265,475,150,50,"AI-White");
    btnAiWhite.onClick = aiChooseWhite;
    btnAiWhite.onlyInAi = true;
    game.btnList.push_back(btnAiWhite);

    Button btnAIBack(265,550,150,50,"Back");
    btnAIBack.onlyInAi = true;
    btnAIBack.onClick = aiGame;
    game.btnList.push_back(btnAIBack);

    Button btnHost(265,400,150,50,"HostGame");
    btnHost.onlyInNet = true;
    btnHost.onClick = netHost;
    game.btnList.push_back(btnHost);

    Button btnJoin(265,475,150,50,"JoinGame");
    btnJoin.onlyInNet = true;
    btnJoin.onClick = netJoin;
    game.btnList.push_back(btnJoin);

    Button btnNetBack(265,550,150,50,"Back");
    btnNetBack.onlyInNet = true;
    btnNetBack.onClick = netGame;
    game.btnList.push_back(btnNetBack);

    Button btnLeave(275,WIN_H-50,120,40,"LeaveNet");
    btnLeave.onlyInNet = true;
    btnLeave.screen = 2;
    btnLeave.onClick = netLeave;
    game.btnList.push_back(btnLeave);
}

void updatePreview(int mx, int my)
{
    if(game.state != 1 || game.isReplaying || isAiTurn() || netWaiting())
    {
        game.previewI = -1;
        game.previewJ = -1;
        return;
    }

    double fi = (mx - MARGIN) / (double)CELL;
    double fj = (my - MARGIN) / (double)CELL;
    int i = static_cast<int>(round(fi));
    int j = static_cast<int>(round(fj));

    if(i >=0 && i < BOARD_SIZE && j >=0 && j < BOARD_SIZE && game.board[i][j] == STONE_EMPTY)
    {
        game.previewI = i;
        game.previewJ = j;
    }
    else
    {
        game.previewI = -1;
        game.previewJ = -1;
    }
}

void handleMouse(){
    ege::mouse_msg m;
    while (mousemsg())
    {
        m = getmouse();
        updatePreview(m.x,m.y);
        for(auto &btn : game.btnList)
        {
            btn.hover = btn.isHit(m.x, m.y);
        }

        if (m.is_left() && m.is_down())
        {
            for(auto &btn : game.btnList)
            {
                if(btn.isHide) continue;
                if(btn.isHit(m.x, m.y) && btn.onClick != nullptr)
                {
                    btn.onClick();
                    break;
                }
            }
            if(game.state == 1 && game.previewI != -1 && !isAiTurn() && !netWaiting()){
                placeStone(game.previewI, game.previewJ);
                netSendMove(game.previewI, game.previewJ);
            }
        }
    }
}
