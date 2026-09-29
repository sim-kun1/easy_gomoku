#include "gomoku.h"

#include "board.cpp"
#include "ai.cpp"
#include "network.cpp"
#include "draw.cpp"
#include "ui.cpp"

int main(){
    initgraph(WIN_W,WIN_H,INIT_RENDERMANUAL);
    setbkcolor(EGERGB(240,220,180));
    initButtons();
    updateButtons();

    while(true)
    {
        handleMouse();
        updateAI();
        updateNet();
        updateReplay();
        mainDraw();
        delay_ms(10);
    }

    closegraph();
    return 0;
}
