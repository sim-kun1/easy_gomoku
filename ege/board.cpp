// 棋盘状态与胜负判断

Game game;

void resetState(){
    memset(game.board,0,sizeof(game.board));
    game.blackTurn = 1;
    game.winner = -1;
}

void placeStone(int i, int j)
{
    int curColor;
    if(game.blackTurn){
        game.board[i][j] = STONE_BLACK;
        curColor = STONE_BLACK;
    }else{
        game.board[i][j] = STONE_WHITE;
        curColor = STONE_WHITE;
    }
    game.gameRecord.push_back({i,j,curColor});
    if(checkWin(i,j,curColor)){
        game.winner = curColor;
        game.state = 2;
        updateButtons();
    }
    if(checkFull()){
        game.winner = 3;
        game.state = 2;
        updateButtons();
    }
    else game.blackTurn = !game.blackTurn;
}

bool checkFull()
{
    for (const auto& row : game.board)
    {
        for (int val : row)
        {
            if (val == 0)
            {
                return false;
            }
        }
    }
    return true;
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
