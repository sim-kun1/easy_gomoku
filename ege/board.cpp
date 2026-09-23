// 棋盘状态与胜负判断

Game game;

void resetState(){
    memset(game.board,0,sizeof(game.board));
    game.blackTurn = 1;
    game.winner = -1;
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
        while(ni >=0 && ni < BOARD_SIZE && nj >=0 && nj < BOARD_SIZE && game.board[ni][nj]==color)
        {
            cnt++;
            ni += dx;
            nj += dy;
        }

        ni = i - dx;
        nj = j - dy;
        while(ni >=0 && ni < BOARD_SIZE && nj >=0 && nj < BOARD_SIZE && game.board[ni][nj]==color)
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
