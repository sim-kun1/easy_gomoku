// 按钮与鼠标交互
#include "gomoku.h"

void updateButtons(){
    for(auto &btn : game.btnList)
    {
        if(game.isReplaying) btn.Hide(btn.text != "StopReplay");
        else if(btn.text == "StopReplay") btn.Hide(1);
        else if(btn.onlyInGame) btn.Hide(game.state != 1);
        else if(btn.text == "Restart") btn.Hide(game.state != 2);
        else btn.Hide(game.state != 0);
    }
}

void startGame(){
    resetState();
    game.gameRecord.clear();
    game.state = 1;
    updateButtons();
}

void restartGame(){
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

void netGame(){
    xyprintf(8, 8, "netcheck");
}

void aiGame(){
    xyprintf(8, 8, "aicheck");
}

void surrender(){
    if(game.blackTurn == 1) game.winner = STONE_WHITE;
    else game.winner = STONE_BLACK;
    game.state = 2;
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
    btnSurrender.onlyInGame = true;
    game.btnList.push_back(btnSurrender);

    Button btnRestart(265,500,150,50,"Restart");
    btnRestart.onClick = restartGame;
    btnRestart.Hide(1);
    game.btnList.push_back(btnRestart);
    
    Button btnRecord(265,625,150,50,"PlayRecord");
    btnRecord.onClick = playRecord;
    game.btnList.push_back(btnRecord);

    Button btnStop(275,WIN_H-50,120,40,"StopReplay");
    btnStop.onClick = stopReplay;
    game.btnList.push_back(btnStop);
}

void updatePreview(int mx, int my)
{
    if(game.state != 1 || game.isReplaying)
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
                    break;//一次点击只触发一个按钮
                }
            }
            if(game.state == 1 && game.previewI != -1){
                int curColor;
                if(game.blackTurn){
                    game.board[game.previewI][game.previewJ] = STONE_BLACK;
                    curColor = STONE_BLACK;
                }else{
                    game.board[game.previewI][game.previewJ] = STONE_WHITE;
                    curColor = STONE_WHITE;
                }
                game.gameRecord.push_back({game.previewI,game.previewJ,curColor});
                if(checkWin(game.previewI,game.previewJ,curColor)){
                    game.winner = curColor;
                    game.state = 2;
                    updateButtons();
                }
                else game.blackTurn = !game.blackTurn;
            }
        }
    }
}
