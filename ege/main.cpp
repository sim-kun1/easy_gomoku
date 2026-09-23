#include "gomoku.h"

// 按功能拆分的实现文件（与 button.cpp 一样直接包含）
#include "board.cpp"
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
        mainDraw();
        delay_ms(10);
    }

    closegraph();
    return 0;
}
