#include "stdio.h"
#include "ncurses.h" 
#include "stdlib.h"

#define UP_HEIGHT 2 //按键每按一次飞2个高度
#define TIME 6 //小鸟下降一个高度所需时间
#define PIPE_NUM 3 //一个界面管道对数量
#define PIPE_Y_GAP 8 //上下管道之间的空隙
#define PIPE_X_GAP 20 //前后管道空隙
#define PIPE_WIDTH 10 //管道宽度

#define X 15 //小鸟x坐标
#define WIDTH 2//小鸟宽度
#define HEIGHT 2//小鸟高度

int g_row;//terminal行数
int g_col;//terminal列数


//把小鸟当成矩形
struct Bird{
    int timeDown1Step;
    int x;
    int y;
    int width;
    int height;
};

typedef struct Bird *pBird;

//管道对(即上下管道)
struct PipePair{
    int x[2];
    int y[2];
    int width[2];
    int height[2];
};

typedef struct PipePair *pPipePair;


void GameOver();
void MovePipes(struct PipePair pipePairs[], int num);//管道移动，每次向左移动一格，当完全移出terminal时重新调整管道让它在右端出现
void DisplayPipe(struct PipePair pipePairs[], int num);
void RevisePipe(pPipePair pipePair);//调整管道对高度
void InitPipes(struct PipePair pipePairs[], int num);//初始化管道，上管道高度+下管道高度+GAP = terminal高度
void SetBirdY(pBird pb, int y);//设置小鸟位置，因为x坐标一直不变，所以只修改y
void DecTimeDown1Step(pBird pb);//减少小鸟下降一个高度所需时间，以此模拟加速下降
int GetTimeDown1Step(pBird pb);//获得小鸟下降一个高度所需时间
pBird CreateBird(int timeDown1Step, int x, int y, int width, int height);
void DisPlayBird(pBird pb);
void Up(pBird pb);//小鸟向上飞
void Init();
int IsHitBorder(pBird pb);//是否撞到边界
int IsHitPipe(pBird pb, struct PipePair pipePairs[], int num);//是否撞到管道
int IsCollision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);//矩阵碰撞检查，管道和小鸟都是矩形


int main()
{
    int ch;
    int i = 0;
    struct PipePair pipePairs[PIPE_NUM];
    Init();
    InitPipes(pipePairs, PIPE_NUM);//初始化管道
    pBird pb = CreateBird(TIME, X, g_row/2, WIDTH, HEIGHT);//创建小鸟
    
    while(1){
        ch = getch();
        switch(ch){
            case KEY_UP : 
                Up(pb);
                i = 0;
                break;
        }
        if(++i >= GetTimeDown1Step(pb)){
            i = 0;
            DecTimeDown1Step(pb);//减少每次下降所需时间，模拟重力加速
            SetBirdY(pb, pb->y + 1);
        }
        MovePipes(pipePairs, PIPE_NUM);
        clear();
        DisPlayBird(pb);
        DisplayPipe(pipePairs, PIPE_NUM);
        if(IsHitPipe(pb, pipePairs, PIPE_NUM) || IsHitBorder(pb)){
            GameOver(); 
        }
    }
    return 0;
}

void GameOver() 
{
    mvprintw(g_row/2, g_col/2, "GAME OVER");
    timeout(-1);
    getch();
    endwin();
    exit(0);
}

//矩阵碰撞检查，管道和小鸟都是矩形
int IsCollision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
    if(x1 + w1 > x2 && x1 < x2 + w2 && y1 + h1 > y2 && y1 < y2 + h2){
        return 1; 
    }else{
        return 0; 
    }
}

//是否撞到管道
int IsHitPipe(pBird pb, struct PipePair pipePairs[], int num)
{
    int i = 0;
    for(i = 0;i < num;i++){
        if(IsCollision(pb->x, pb->y, pb->width, pb->height,
            pipePairs[i].x[0], pipePairs[i].y[0], pipePairs[i].width[0], pipePairs[i].height[0])){
                return 1; 
        }  
        if(IsCollision(pb->x, pb->y, pb->width, pb->height,
            pipePairs[i].x[1], pipePairs[i].y[1], pipePairs[i].width[1], pipePairs[i].height[1])){
                return 1; 
        }  
    }
    return 0;
}

//是否撞到边界
int IsHitBorder(pBird pb)
{
    if(pb->y + pb->width > g_row || pb->y < 0){
        return 1; 
    }else{
        return 0; 
    }
}

void Init()
{
    initscr();//初始化terminal处于curses模式
    curs_set(0);//光标不可见
    keypad(stdscr, TRUE);//使能键盘读取函数能够读到F1,F2和方向键
    getmaxyx(stdscr, g_row, g_col);
    timeout(70);
    srand((unsigned)time(NULL));
}

//小鸟向上飞
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

//获得小鸟下降一个高度所需时间
int GetTimeDown1Step(pBird pb)
{
    return pb->timeDown1Step; 
}

//减少小鸟下降一个高度所需时间，以此模拟加速下降，最小减至0
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

//初始化管道，上管道高度+下管道高度+GAP = terminal高度
void InitPipes(struct PipePair pipePairs[], int num)
{
    int h1, h2;
    int y1, y2;
    int x1, x2;  
    int w1, w2;
    int i;
    w1 = w2 = PIPE_WIDTH;
    for(i = 0;i < num;i++){
        h1 = rand() % (g_row - PIPE_Y_GAP - 6) + 3;//上管道高度
        y1 = 0;
        if(i == 0){
            x1 = g_col;//第一个管道起始x坐标，ternimal最右边
        }else{
            x1 = pipePairs[i - 1].x[0] + pipePairs[i - 1].width[0] + PIPE_X_GAP;//每个管道x坐标为前一管道x+widht+gap
        }
        h2 = g_row - h1 - PIPE_Y_GAP;//下管道高度：terminal高度-上管道高度-间隙GAP
        x2 = x1;
        y2 = y1 + h1 + PIPE_Y_GAP;

        pipePairs[i].x[0] = x1;
        pipePairs[i].x[1] = x2;
        pipePairs[i].y[0] = y1;
        pipePairs[i].y[1] = y2;
        pipePairs[i].width[0] = w1;;
        pipePairs[i].width[1] = w2;;
        pipePairs[i].height[0] = h1;
        pipePairs[i].height[1] = h2;
    }
}

//调整管道对高度
void RevisePipe(pPipePair pipePair)
{
    pipePair->width[0] = pipePair->width[1] = PIPE_WIDTH;
    pipePair->height[0] = rand() % (g_row - PIPE_Y_GAP - 6) + 3;
    pipePair->y[0] = 0;
    pipePair->x[0] = g_col;
    pipePair->height[1] = g_row - pipePair->height[0] - PIPE_Y_GAP;
    pipePair->x[1] = pipePair->x[0];
    pipePair->y[1] = pipePair->y[0] + pipePair->height[0] + PIPE_Y_GAP; 
}

void DisplayPipe(struct PipePair pipePairs[], int num)
{
    int i, j, k;
    for(i = 0;i < num;i++){
        for(j = 0;j < pipePairs[i].height[0];j++){
            for(k = 0;k < pipePairs[i].width[0];k++){
                mvaddch(pipePairs[i].y[0] + j, pipePairs[i].x[0] + k, '#'); 
            } 
        } 
        for(j = 0;j < pipePairs[i].height[1];j++){
            for(k = 0;k < pipePairs[i].width[1];k++){
                mvaddch(pipePairs[i].y[1] + j, pipePairs[i].x[1] + k, '#'); 
            } 
        } 
    }
}

//管道移动，每次向左移动一格，当完全移出terminal时重新调整管道让它在右端出现
void MovePipes(struct PipePair pipePairs[], int num)
{
    int i;
    for(i = 0;i < num;i++){
        pipePairs[i].x[0] -= 1; 
        pipePairs[i].x[1] -= 1; 
        if(pipePairs[i].x[0] + pipePairs[i].width[0] < 0){
            RevisePipe(&pipePairs[i]);         
        }
    }
}

