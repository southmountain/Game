#include "stdio.h"
#include "ncurses.h" 
#include "stdlib.h"

#define GRAVITY 1 
#define UP_HEIGHT 3 
#define TIME 6 

#define X 5
#define WIDTH 2
#define HEIGHT 2


struct Bird{
    int timeDown1Step;
    int x;
    int y;
    int width;
    int height;
};

typedef struct Bird *pBird;

int g_row;
int g_col;

void Init()
{
    initscr();//初始化terminal处于curses模式
    curs_set(0);//光标不可见
    keypad(stdscr, TRUE);//使能键盘读取函数能够读到F1,F2和方向键
    getmaxyx(stdscr, g_row, g_col);
    timeout(100);
}

void Up(pBird pb)
{
   pb->timeDown1Step = TIME;
   pb->y -= UP_HEIGHT;
}

void DisPlayBird(pBird pb)
{
    int i, j;
    for(i = 0;i < HEIGHT;i++){
        for(j = 0;j < WIDTH;j++){
            mvaddch(pb->y + i, pb->x + j, '0');
        }
    }
}

pBird CreateBird(int timeDown1Step, int x, int y, int width, int height)
{
    pBird pb = malloc(sizeof(struct Bird));
    pb->timeDown1Step = timeDown1Step;
    pb->x = x;
    pb->y = y;
    pb->width = width;
    pb->height = height;
    return pb;
}

int GetTimeDown1Step(pBird pb)
{
    return pb->timeDown1Step; 
}

void DecTimeDown1Step(pBird pb)
{
    if(pb->timeDown1Step > 0){
        pb->timeDown1Step -= 1; 
    } 
}

void SetBirdY(pBird pb, int y)
{
    pb->y = y;
}

int main()
{
    int ch;
    int i = 0;
    Init();
    pBird pb = CreateBird(TIME, X, g_row/2, WIDTH, HEIGHT);
    while(1){
        ch = getch(); 
        switch(ch){
            case ' ' : 
                Up(pb);
                i = 0;
                break;
        }
        if(++i >= GetTimeDown1Step(pb)){
            i = 0;
            DecTimeDown1Step(pb);
            SetBirdY(pb, pb->y + 1);
        }
        clear();
        DisPlayBird(pb);
    }
    return 0;
}

