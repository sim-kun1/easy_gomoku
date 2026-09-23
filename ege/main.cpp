#include <graphics.h>
#include <iostream>
#include <vector>
#include <cmath>
#include "button.cpp"
using namespace std;

const int BOARD_SIZE = 15;
const int CELL = 40;
const int MARGIN = 60;//margin
const int WIN_W = MARGIN * 2 + CELL * (BOARD_SIZE - 1);
const int WIN_H = WIN_W;
const int STONE_EMPTY = 0;
const int STONE_BLACK = 1;
const int STONE_WHITE = 2;

int previewI = -1;
int previewJ = -1;

int board[BOARD_SIZE][BOARD_SIZE] = {0};
vector<Button> btnList;
int gameState = 0;//0:菜单 1:game 2:over
bool blackTurn = 1;
int winner = -1;

void updatePreview(int mx, int my);
void drawPreviewHint();
void drawAllChess();
bool checkWin(int i, int j, int color);

void resetState(){
    memset(board,0,sizeof(board));
    blackTurn = 1;
    winner = -1;
}

void drawBoard(){
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        if(i == 7) {setcolor(EGERGB(142,128,75));setlinewidth(2);}
        else {setcolor(EGERGB(173,158,85));setlinewidth(1);}
        line(MARGIN, MARGIN + i * CELL, MARGIN + (BOARD_SIZE - 1) * CELL, MARGIN + i * CELL);
        line(MARGIN + i * CELL, MARGIN, MARGIN + i * CELL, MARGIN + (BOARD_SIZE - 1) * CELL);
    }
}

void drawMenu(){
    setfont(48, 0, "微软雅黑");
    setcolor(EGERGB(0,0,0));
    const char* title = "Gomoku";
    outtextxy((WIN_W - textwidth(title)) / 2, WIN_H/2-100, title);
}

void drawTurn(){
    if(blackTurn == 1){
        setcolor(EGERGB(255,255,255));
        setfillcolor(EGERGB(0,0,0));
    }
    else{
        setcolor(EGERGB(0,0,0));
        setfillcolor(EGERGB(255,255,255));
    }
    fillcircle(40,30,15);
}

void updateButtons(){
    for(auto &btn : btnList)
    {
        if(btn.onlyInGame) btn.Hide(gameState != 1);
        else if(btn.text == "Restart") btn.Hide(gameState != 2);
        else btn.Hide(gameState != 0);
    }
}

void startGame(){
    resetState();
    gameState = 1;
    updateButtons();
}

void restartGame(){
    resetState();
    gameState = 0;
    updateButtons();
}

void netGame(){
    xyprintf(8, 8, "netcheck");
}

void aiGame(){
    xyprintf(8, 8, "aicheck");
}

void surrender(){
    if(blackTurn == 1) winner = STONE_WHITE;
    else winner = STONE_BLACK;
    gameState = 2;
    updateButtons();
}

void initButtons(){
    Button btnStart(265,400,150,50,"StartGame");
    btnStart.onClick = startGame;
    btnList.push_back(btnStart);

    Button btnNetStart(265,500,150,50,"NetGame");
    btnNetStart.onClick = netGame;
    btnList.push_back(btnNetStart);

    Button btnAIStart(265,600,150,50,"AIGame");
    btnAIStart.onClick = aiGame;
    btnList.push_back(btnAIStart);

    Button btnSurrender(275,WIN_H-50,120,40,"Surrender");
    btnSurrender.onClick = surrender;
    btnSurrender.onlyInGame = true;
    btnList.push_back(btnSurrender);

    Button btnRestart(265,500,150,50,"Restart");
    btnRestart.onClick = restartGame;
    btnRestart.Hide(1);
    btnList.push_back(btnRestart);
}

void handleMouse(){
    ege::mouse_msg m;
    while (mousemsg())
    {
        m = getmouse();
        updatePreview(m.x,m.y);
        for(auto &btn : btnList)
        {
            btn.hover = btn.isHit(m.x, m.y);
        }

        if (m.is_left() && m.is_down())
        {
            for(auto &btn : btnList)
            {
                if(btn.isHide) continue;
                if(btn.isHit(m.x, m.y) && btn.onClick != nullptr)
                {
                    btn.onClick();
                }
            }
            if(gameState == 1 && previewI != -1){
                int curColor;
                if(blackTurn){
                    board[previewI][previewJ] = STONE_BLACK;
                    curColor = STONE_BLACK;
                }else{
                    board[previewI][previewJ] = STONE_WHITE;
                    curColor = STONE_WHITE;
                }
                if(checkWin(previewI,previewJ,curColor)){
                    winner = curColor;
                    gameState = 2;
                    updateButtons();
                }
                else blackTurn = !blackTurn;
            }
        }
    }
}

void drawButtons(){
    for(auto &btn : btnList)
    {
        if(!btn.isHide) btn.draw();
    }
}

void mainDraw(){
    cleardevice();
    if(gameState == 0){
        drawMenu();
    }
    else if(gameState == 1){
        drawBoard();
        drawAllChess();
        drawPreviewHint();
        drawTurn();
    }else if(gameState == 2){
        drawBoard();
        drawAllChess();
        setfont(36,0,"微软雅黑");
        setcolor(EGERGB(200,0,0));
        string msg;
        if(winner == STONE_BLACK) msg = "Black WIN!";
        else if(winner == STONE_WHITE) msg = "White WIN!";
        const char* str = msg.c_str();
        int tw = textwidth(str);
        outtextxy((WIN_W - tw)/2, 30, str);
    }
    drawButtons();
}

void updatePreview(int mx, int my)
{
    if(gameState != 1)
    {
        previewI = -1;
        previewJ = -1;
        return;
    }

    double fi = (mx - MARGIN) / (double)CELL;
    double fj = (my - MARGIN) / (double)CELL;
    int i = static_cast<int>(round(fi));
    int j = static_cast<int>(round(fj));

    if(i >=0 && i < BOARD_SIZE && j >=0 && j < BOARD_SIZE && board[i][j] == STONE_EMPTY)
    {
        previewI = i;
        previewJ = j;
    }
    else
    {
        previewI = -1;
        previewJ = -1;
    }
}

void drawPreviewHint()
{
    if(previewI == -1) return;

    int px = MARGIN + previewI * CELL;
    int py = MARGIN + previewJ * CELL;

    setfillcolor(EGERGB(160,160,160));
    setcolor(EGERGB(160,160,160));
    fillcircle(px, py, 16);
}

void drawAllChess()
{
    for(int i = 0; i < BOARD_SIZE; i++)
    {
        for(int j = 0; j < BOARD_SIZE; j++)
        {
            if(board[i][j] == STONE_EMPTY) continue;

            int cx = MARGIN + i * CELL;
            int cy = MARGIN + j * CELL;
            if(board[i][j] == STONE_BLACK)
            {
                setfillcolor(EGERGB(0,0,0));
                setcolor(EGERGB(0,0,0));
            }
            else if(board[i][j] == STONE_WHITE)
            {
                setfillcolor(EGERGB(255,255,255));
                setcolor(EGERGB(0,0,0));
            }
            fillcircle(cx, cy,16);
        }
    }
}

bool checkWin(int i, int j, int color)
{
    int dir[4][2] = {{1,0}, {0,1}, {1,1}, {1,-1}};

    for(int d = 0; d < 4; d++)
    {
        int dx = dir[d][0];
        int dy = dir[d][1];
        int cnt = 1;
        int ni = i + dx;
        int nj = j + dy;
        while(ni >=0 && ni < BOARD_SIZE && nj >=0 && nj < BOARD_SIZE && board[ni][nj]==color)
        {
            cnt++;
            ni += dx;
            nj += dy;
        }

        ni = i - dx;
        nj = j - dy;
        while(ni >=0 && ni < BOARD_SIZE && nj >=0 && nj < BOARD_SIZE && board[ni][nj]==color)
        {
            cnt++;
            ni -= dx;
            nj -= dy;
        }

        if(cnt >=5)
        {
            return true;
        }
    }
    return false;
}



int main(){
    initgraph(WIN_W,WIN_H);
    setbkcolor(EGERGB(240,220,180));
    initButtons();
    updateButtons();

    while(true)
    {
        handleMouse();
        mainDraw();
        delay_ms(10);
    }

    closegraph();
    return 0;
}
