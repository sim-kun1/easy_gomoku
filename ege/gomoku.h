#pragma once

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
const int REPLAY_FRAMES = 80;//停留帧数
const int AI_THINK_FRAMES = 40;//AI思考帧数

struct Step{
    int i;
    int j;
    int color;
};

struct Game{
    int board[BOARD_SIZE][BOARD_SIZE] = {0};
    bool blackTurn = 1;
    int winner = -1;
    int state = 0;//0:菜单 1:game 2:over
    int previewI = -1;
    int previewJ = -1;
    bool isReplaying = false;
    int replayIndex = 0;
    int replayTick = 0;
    vector<Button> btnList;
    vector<Step> gameRecord;
};



struct Ai{
    bool enabled = false;
    bool choosing = false;
    int color = STONE_BLACK;
    int thinkTick = 0;
};

extern Game game;
extern Ai ai;

// board.cpp
void resetState();
void placeStone(int i, int j);
bool checkWin(int i, int j, int color);

// draw.cpp
void drawBoard();
void drawMenu();
void drawTurn();
void drawPreviewHint();
void drawAllChess();
void drawButtons();
void mainDraw();

// ui.cpp
void initButtons();
void updateButtons();
void updatePreview(int mx, int my);
void handleMouse();
void startGame();
void restartGame();
void netGame();
void surrender();
void updateReplay();
void stopReplay();

// ai.cpp
void aiGame();
void aiChooseBlack();
void aiChooseWhite();
int turnColor();
bool isAiTurn();
void updateAI();
void aiDecide(int &i, int &j);
