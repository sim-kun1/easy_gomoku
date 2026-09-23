#include <graphics.h>
#include <iostream>
#include <vector>

using namespace std;

class Button{
public:
    int x,y,w,h;
    string text;
    using ClickFunc = void(*)();
    ClickFunc onClick = nullptr;
    bool hover = false;
    bool isHide = false;
    bool onlyInGame = false;

    Button(int x,int y,int w,int h,string text):x(x),y(y),w(w),h(h),text(text){}

    void draw();
    bool Hide(bool hide);
    bool isHit(int mx, int my);
};

bool Button::isHit(int mx, int my)
{
    return mx >= x && mx <= x+w && my >= y && my <= y+h;
}

void Button::draw()
{
    int left = x;
    int top = y;
    int right = x + w;
    int bottom = y + h;
    int r = 10;

    if(hover)
        setfillcolor(EGERGB(200,200,200));
    else
        setfillcolor(EGERGB(230,230,230));

    solidroundrect(left, top, right, bottom, r, r);

    setcolor(EGERGB(0,0,0));
    setbkmode(TRANSPARENT);
    setfont(24, 0, "微软雅黑");
    const char* str = text.c_str();
    int textW = textwidth(str);
    int textX = x + (w - textW) / 2;
    int fontHeight = 24;
    int textY = y + (h - fontHeight)/2;
    outtextxy(textX, textY, str);
}

bool Button::Hide(bool hide){
    isHide = hide;
    return hide;
}