
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
    if(game.blackTurn == 1){
        setcolor(EGERGB(255,255,255));
        setfillcolor(EGERGB(0,0,0));
    }
    else{
        setcolor(EGERGB(0,0,0));
        setfillcolor(EGERGB(255,255,255));
    }
    fillcircle(40,30,15);
}

void drawPreviewHint()
{
    if(game.previewI == -1) return;

    int px = MARGIN + game.previewI * CELL;
    int py = MARGIN + game.previewJ * CELL;

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
            if(game.board[i][j] == STONE_EMPTY) continue;

            int cx = MARGIN + i * CELL;
            int cy = MARGIN + j * CELL;
            if(game.board[i][j] == STONE_BLACK)
            {
                setfillcolor(EGERGB(0,0,0));
                setcolor(EGERGB(0,0,0));
            }
            else if(game.board[i][j] == STONE_WHITE)
            {
                setfillcolor(EGERGB(255,255,255));
                setcolor(EGERGB(0,0,0));
            }
            fillcircle(cx, cy,16);
        }
    }
}

void drawButtons(){
    for(auto &btn : game.btnList)
    {
        if(!btn.isHide) btn.draw();
    }
}

void mainDraw(){
    cleardevice();
    if(game.state == 0){
        drawMenu();
    }
    else if(game.state == 1){
        drawBoard();
        drawAllChess();
        drawPreviewHint();
        drawTurn();
        if(game.isReplaying){
            setfont(24,0,"微软雅黑");
            setcolor(EGERGB(200,0,0));
            outtextxy(WIN_H-120,10,"RECORDING");
        }
    }else if(game.state == 2){
        drawBoard();
        drawAllChess();
        setfont(36,0,"微软雅黑");
        setcolor(EGERGB(200,0,0));
        string msg;
        if(game.winner == STONE_BLACK) msg = "Black WIN!";
        else if(game.winner == STONE_WHITE) msg = "White WIN!";
        const char* str = msg.c_str();
        int tw = textwidth(str);
        outtextxy((WIN_W - tw)/2, 30, str);
    }
    drawNetInfo();
    drawButtons();
}
