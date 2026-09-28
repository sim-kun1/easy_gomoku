// AI 模式：选边、回合驱动。算法在 aiDecide 里，留空待实现
#include "gomoku.h"

Ai ai;

// ============ 这里是你之后要写的地方 ============
// 轮到 AI 时被调用，把要落子的位置写进 i、j
// 写不出合法的位置（比如棋盘满了）就把 i 设成 -1
void aiDecide(int &i, int &j){
    i = -1;
    j = -1;
}
// ==============================================

int turnColor(){
    return game.blackTurn ? STONE_BLACK : STONE_WHITE;
}

bool isAiTurn(){
    return ai.enabled && turnColor() == ai.color;
}

void startAiGame(int color){
    ai.choosing = false;
    ai.thinkTick = 0;
    startGame();
    ai.enabled = true;
    ai.color = color;
}

void aiChooseBlack(){
    startAiGame(STONE_BLACK);
}

void aiChooseWhite(){
    startAiGame(STONE_WHITE);
}

void aiGame(){
    ai.choosing = !ai.choosing;
    updateButtons();
}

void updateAI(){
    if(!ai.enabled || game.state != 1 || game.isReplaying) return;

    if(!isAiTurn()){
        ai.thinkTick = 0;
        return;
    }

    ai.thinkTick++;
    if(ai.thinkTick < AI_THINK_FRAMES) return;
    ai.thinkTick = 0;

    int i = -1;
    int j = -1;
    aiDecide(i, j);
    if(i < 0 || i >= BOARD_SIZE || j < 0 || j >= BOARD_SIZE) return;
    if(game.board[i][j] != STONE_EMPTY) return;
    placeStone(i, j);
}
