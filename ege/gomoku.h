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

struct Game{
    int board[BOARD_SIZE][BOARD_SIZE] = {0};
    bool blackTurn = 1;
    int winner = -1;
    int state = 0;//0:菜单 1:game 2:over
    int previewI = -1;
    int previewJ = -1;
    vector<Button> btnList;
};

extern Game game;

// board.cpp
void resetState();
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
void aiGame();
void surrender();
