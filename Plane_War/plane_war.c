#include "ncurses.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"



#define PLANE_SYMBOL '#'//飞机符号
#define WIDTH 40//飞机可移动宽度


enum LAYOUT_VAL{
    EMPTY,
    BULLET,
    ENEMY,
    BORDER,
    MAX_VALUE
};
static char s_layoutSymbol[MAX_VALUE] = {' ', '.', '$', '|'};//布局中每个值对应的符号

static char *s_layout;//对应界面每个点,空值为0, 子弹值为1，敌军值为2, 边界线值为3;
static int s_planeXPos;//飞机初始位置
static int s_planeYPos;//飞机初始位置
static int s_bulletSpeed = 50;//子弹移动速度值越小，移动速度越快
static int s_enemySpeed = 100;//敌军移动速度值越小，移动速度越快
static int s_enemyDensity = 200;//密度值越大，敌军生成速度越慢
static int s_row;//终端(terminal)大小
static int s_col;
static int s_killNumber = 0;//击杀敌军数量

void Init();//初始化
void MoveRight();//飞机向右移动
void MoveLeft();//飞机向左移动
void MoveEnemy();//敌军移动
void MoveBullet();//子弹移动
void Refresh();//刷新显示
void DisplayScore();//显示得分
void DisplayInstructions();//显示说明
void GameOver();//游戏结束

int main()
{
    int c;
    int i = 0;
    int j = 0;
    int k = 0;
    Init();
    while(c = getch()){
        switch(c){
            //空格发射子弹
            case ' ' : 
                s_layout[s_planeYPos * s_col + s_planeXPos]  = BULLET;
                break;
            case KEY_LEFT : 
                MoveLeft();
                break;  
            case KEY_RIGHT : 
                MoveRight();
                break;  
            //按下ESC退出
            case 27 : 
                GameOver();
                break;
        }
        //生成敌军 
        if(++k >= s_enemyDensity){
            k = 0;
            s_layout[rand() % WIDTH] = ENEMY;
        }
        //敌军移动
        if(++j >= s_enemySpeed){
            j = 0; 
            MoveEnemy();
        }
        //子弹移动
        if(++i >= s_bulletSpeed){
            i = 0;
            MoveBullet();
        }
        Refresh();
    }
    return 0;
}

void Init()
{
    int i;
    initscr();//初始化terminal处于curses模式 
    curs_set(0);//光标不可见
    keypad (stdscr, TRUE);//使能键盘读取函数能够读到F1,F2和方向键
    timeout(1);//设置getch()超时时间，避免一直阻塞,造成页面不刷新
    getmaxyx(stdscr,s_row,s_col);//获得terminal大小

    s_layout = malloc(sizeof(char) * (s_row * s_col));
    memset(s_layout, 0, s_row * s_col);

    //初始化边界线
    for(i = 0;i < s_row;i++){
        s_layout[WIDTH + i * s_col] = BORDER; 
    };

    s_planeXPos = WIDTH / 2;
    s_planeYPos = s_row - 1;

    srand((unsigned)time(NULL));
    clear();
}

void MoveRight()
{
    if((s_planeXPos + 1) < WIDTH){
        s_planeXPos++;
        //向右移动碰到敌军
        if(s_layout[s_planeYPos * s_col + s_planeXPos] == ENEMY){
            GameOver(); 
        }
    }
}

void MoveLeft()
{
    if(s_planeXPos > 0){
        s_planeXPos--;
        //向左移动碰到敌军
        if(s_layout[s_planeYPos * s_col + s_planeXPos] == ENEMY){
            GameOver(); 
        }
    }
}

void MoveEnemy()
{
    int i;
    //从最后面移动，否则s_layout[i + s_col] = ENEMY语句刚执行完，下一次循环又马上符合if(s_layout[i] == ENEMY)条件，造成错误
    for(i = s_col * s_row - 1;i >= 0;i--){
        if(s_layout[i] == ENEMY){
            s_layout[i] = EMPTY; 
            //敌军向下还没越界
            if((i + s_col) < (s_row * s_col)){
                //是否被敌军撞到
                if(i + s_col == s_planeYPos * s_col + s_planeXPos){
                    GameOver();
                }else if(s_layout[i + s_col] == EMPTY){
                    //前方为空，敌军继续前进 
                    s_layout[i + s_col] = ENEMY;
                }else if(s_layout[i + s_col] == BULLET){
                    //敌军撞到子弹，敌军子弹一起消失 
                    s_layout[i + s_col] = EMPTY;
                    s_killNumber++;
                } 
            }
        }   
    }
}

void MoveBullet()
{
    int i;
    for(i = 0;i < s_col * s_row;i++){
        if(s_layout[i] == BULLET){
            s_layout[i] = EMPTY; 
            //子弹继续向上还没越界
            if((i - s_col) >= 0){
                //前方空，子弹继续向上
                if(s_layout[i - s_col] == EMPTY){
                    s_layout[i - s_col] = BULLET; 
                }else if(s_layout[i - s_col] == ENEMY){
                    //前方有敌军，击杀敌军，同时子弹消失
                    s_layout[i - s_col] = EMPTY; 
                    s_killNumber++;
                }
            }
        }   
    }
}

void Refresh()
{
    int i;
    int xPos, yPos;
    clear();
    //显示布局(子弹，敌军，边界线)
    for(i = 0;i < s_row * s_col;i++){
         xPos = i % s_col;
         yPos = i / s_col;
         mvaddch(yPos, xPos, s_layoutSymbol[s_layout[i]]);
    } 
    mvaddch(s_planeYPos, s_planeXPos, PLANE_SYMBOL);//显示飞机
    DisplayScore();//显示得分
    DisplayInstructions();//显示说明
}

//显示得分
void DisplayScore()
{
    mvprintw(0, WIDTH + 1, "KILL: %d.", s_killNumber);
    mvprintw(2, WIDTH + 1, "SCORE: %d.", s_killNumber * 10);
}

void DisplayInstructions()
{
    mvprintw(s_row - 3, WIDTH + 1, "Shoot: space key");
    mvprintw(s_row - 2, WIDTH + 1, "Move: direction key");
    mvprintw(s_row - 1, WIDTH + 1, "Press ESC to exit");
}

void GameOver()
{
    mvprintw(s_row/2, s_col/2, "%s", "GAME OVER.");
    free(s_layout);
    timeout(-1);
    getch();
    endwin();
    exit(0);
}




