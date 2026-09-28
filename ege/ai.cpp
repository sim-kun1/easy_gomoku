// AI 模式：选边、回合驱动；决策算法在下面的 aiDecide
#include "gomoku.h"

Ai ai;


const int SCORE_FIVE = 100000;
const int SCORE_OPEN_FOUR = 10000;
const int SCORE_FOUR = 1000;
const int SCORE_OPEN_THREE = 1000;
const int SCORE_THREE = 200;
const int SCORE_OPEN_TWO = 100;
const int SCORE_TWO = 20;
const int SCORE_ONE = 5;

const int DIRS[4][2] = {{1,0}, {0,1}, {1,1}, {1,-1}};

struct Cand{
    int i;
    int j;
    int score;
};

int lineLen(int x, int y, int dx, int dy, int color, int &open)
{
    int cnt = 1;
    open = 0;

    int ni = x + dx;
    int nj = y + dy;
    while(ni >= 0 && ni < BOARD_SIZE && nj >= 0 && nj < BOARD_SIZE && game.board[ni][nj] == color)
    {
        cnt++;
        ni += dx;
        nj += dy;
    }
    if(ni >= 0 && ni < BOARD_SIZE && nj >= 0 && nj < BOARD_SIZE && game.board[ni][nj] == STONE_EMPTY) open++;

    ni = x - dx;
    nj = y - dy;
    while(ni >= 0 && ni < BOARD_SIZE && nj >= 0 && nj < BOARD_SIZE && game.board[ni][nj] == color)
    {
        cnt++;
        ni -= dx;
        nj -= dy;
    }
    if(ni >= 0 && ni < BOARD_SIZE && nj >= 0 && nj < BOARD_SIZE && game.board[ni][nj] == STONE_EMPTY) open++;

    return cnt;
}

int lineScore(int x, int y, int dx, int dy, int color)
{
    int open = 0;
    int len = lineLen(x, y, dx, dy, color, open);

    if(len >= 5) return SCORE_FIVE;
    if(len == 4)
    {
        if(open == 2) return SCORE_OPEN_FOUR;
        if(open == 1) return SCORE_FOUR;
    }
    if(len == 3)
    {
        if(open == 2) return SCORE_OPEN_THREE;
        if(open == 1) return SCORE_THREE;
    }
    if(len == 2)
    {
        if(open == 2) return SCORE_OPEN_TWO;
        if(open == 1) return SCORE_TWO;
    }
    if(len == 1 && open == 2) return SCORE_ONE;
    return 0;
}

int pointScore(int x, int y, int color)
{
    int score = 0;
    for(int d = 0; d < 4; d++)
        score += lineScore(x, y, DIRS[d][0], DIRS[d][1], color);
    return score;
}

bool makesFive(int x, int y, int color)
{
    for(int d = 0; d < 4; d++)
    {
        int open = 0;
        if(lineLen(x, y, DIRS[d][0], DIRS[d][1], color, open) >= 5) return true;
    }
    return false;
}

bool makesOpenFour(int x, int y, int color)
{
    for(int d = 0; d < 4; d++)
    {
        int open = 0;
        if(lineLen(x, y, DIRS[d][0], DIRS[d][1], color, open) == 4 && open == 2) return true;
    }
    return false;
}

void collectCand(vector<Cand> &cand)
{
    bool used[BOARD_SIZE][BOARD_SIZE] = {false};

    for(int i = 0; i < BOARD_SIZE; i++)
    {
        for(int j = 0; j < BOARD_SIZE; j++)
        {
            if(game.board[i][j] == STONE_EMPTY) continue;

            for(int a = -2; a <= 2; a++)
            {
                for(int b = -2; b <= 2; b++)
                {
                    if(abs(a) + abs(b) > 2) continue;

                    int ni = i + a;
                    int nj = j + b;
                    if(ni < 0 || ni >= BOARD_SIZE || nj < 0 || nj >= BOARD_SIZE) continue;
                    if(game.board[ni][nj] != STONE_EMPTY || used[ni][nj]) continue;

                    used[ni][nj] = true;
                    cand.push_back({ni, nj, 0});
                }
            }
        }
    }

    if(cand.empty()) cand.push_back({BOARD_SIZE/2, BOARD_SIZE/2, 0});
}


void aiDecide(int &i, int &j)
{
    i = -1;
    j = -1;

    int my = ai.color;
    int opp = (my == STONE_BLACK) ? STONE_WHITE : STONE_BLACK;

    vector<Cand> cand;
    collectCand(cand);

    for(auto &c : cand)
    {
        if(makesFive(c.i, c.j, my))
        {
            i = c.i;
            j = c.j;
            return;
        }
    }
    for(auto &c : cand)
    {
        if(makesOpenFour(c.i, c.j, my))
        {
            i = c.i;
            j = c.j;
            return;
        }
    }

    for(auto &c : cand)
    {
        if(makesFive(c.i, c.j, opp) || makesOpenFour(c.i, c.j, opp))
        {
            i = c.i;
            j = c.j;
            return;
        }
    }

    int best = -1;
    vector<Cand> top;
    for(auto &c : cand)
    {
        c.score = pointScore(c.i, c.j, my) + pointScore(c.i, c.j, opp);
        if(c.score > best)
        {
            best = c.score;
            top.clear();
            top.push_back(c);
        }
        else if(c.score == best) top.push_back(c);
    }

    int bestDist = BOARD_SIZE * 2;
    vector<Cand> pick;
    for(auto &c : top)
    {
        int dist = abs(c.i - BOARD_SIZE/2) + abs(c.j - BOARD_SIZE/2);
        if(dist < bestDist)
        {
            bestDist = dist;
            pick.clear();
            pick.push_back(c);
        }
        else if(dist == bestDist) pick.push_back(c);
    }

    int k = random(static_cast<int>(pick.size()));
    i = pick[k].i;
    j = pick[k].j;
}


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
