#include "stdio.h"
#include "ncurses.h"
#include "stdlib.h"
#include "time.h"

#define UP 1
#define DOWN 2 
#define LEFT 3 
#define RIGHT 4 


#define WIDTH 20//蛇可以移动的宽度
#define HEIGHT 15//蛇可以移动的高度

char g_screen[HEIGHT + 2][WIDTH + 2] = {0};

//蛇的结点
struct Node{
    int x;
    int y;
    struct Node *next;
};

typedef struct Node *pNode;

struct Snake{
    int length;
    int direction;
    pNode head;
    pNode tail;
};

typedef struct Snake *pSnake;

struct Food{
    int x;
    int y;
};

struct Food g_food;
struct Snake g_snake;

int g_direction = RIGHT;//蛇的方向，一开始默认为右
int g_speed = 200;//蛇的移动速度,越大越慢

int g_row;
int g_col;

void Init();//初始化
pNode CreateNode(int x, int y);
void InitSnake();//初始化蛇
int IsRearchFood(int x, int y);//是否吃到食物
int IsEatSelf(int x, int y);//是否吃到自己
void AddSnakeHead(pNode pn);//蛇变长，在头部添加一个结点
void MoveSnakeBody();//移动蛇的身体，不包括头
void HandleMoveEvent(int nextX, int nextY);//处理蛇移动后发生的事件(吃到食物...)
void MoveSnake();//蛇移动
void GenerateFood();//随机生成食物
void Refresh();//刷新界面
void DisPlayBorder();//显示边界
void GameOver();//游戏结束


int main()
{
    int i = 0;
    int ch;
    int isWaitMove = 0;
    Init();
    while(1){
        //isWaitMove避免在蛇未移动情况下, 方向经过多次改变.否则会出现吃自己的错误情况
        if(isWaitMove == 0){
            ch = getch(); 
            switch(ch){
                case KEY_LEFT :
                    //不能反方向
                    if(g_snake.direction != RIGHT){
                        g_snake.direction = LEFT; 
                        isWaitMove = 1;
                    }
                    break; 
                case KEY_RIGHT :
                    if(g_snake.direction != LEFT){
                        g_snake.direction = RIGHT; 
                        isWaitMove = 1;
                    }
                    break; 
                case KEY_UP : 
                    if(g_snake.direction != DOWN){
                        g_snake.direction = UP; 
                        isWaitMove = 1;
                    }
                    break; 
                case KEY_DOWN : 
                    if(g_snake.direction != UP){
                        g_snake.direction = DOWN; 
                        isWaitMove = 1;
                    }
                    break; 
                default :
                    break;
            }
        }
        if(++i >= g_speed){
            i = 0;
            MoveSnake(); 
            Refresh();
            isWaitMove = 0;
        } 
    }
    return 0;
}

void Init()
{
    initscr();//初始化terminal处于curses模式 
    curs_set(0);//光标不可见
    keypad(stdscr, TRUE);//使能键盘读取函数能够读到F1,F2和方向键
    //nodelay(stdscr, TRUE);
    timeout(1);//键盘读取超时, 1ms

    srand((unsigned)time(NULL));
    clear();
    InitSnake();
    GenerateFood();
}

pNode CreateNode(int x, int y)
{
    pNode pn = malloc(sizeof(struct Node));
    pn->x = x;
    pn->y = y;
    pn->next = NULL;
    return pn;
}

void InitSnake()
{
    g_snake.length = 2;
    g_snake.direction = RIGHT;
    g_snake.head = CreateNode(2, 1);
    g_snake.tail = CreateNode(1, 1); 
    g_snake.tail->next = g_snake.head;
}

int IsRearchFood(int x, int y)
{
    return (x == g_food.x) && (y == g_food.y);  
}

int IsHitBorder(int x, int y)
{
    return (x <= 0 || x > WIDTH) || (y <= 0 || y > HEIGHT);
}


//x，y是否已经到达蛇的身体(即吃到自己)
int IsEatSelf(int x, int y)
{
    int i;
    int isEat = 0;
    pNode pn = g_snake.tail;
    for(i = 0;i < g_snake.length - 1;i++){
        if((pn->x == x) && (pn->y == y)){
            isEat = 1;
            break;
        } 
        pn = pn->next;
    }
    return isEat;
}

//吃到食物时，在头部增加一个节点，并将head指针指向新节点,而蛇身不动
void AddSnakeHead(pNode pn)
{
    g_snake.head->next = pn; 
    g_snake.length += 1;
    g_snake.head = pn; 
}

//移动蛇的身体,但不移动头部
void MoveSnakeBody()
{
    pNode pn = g_snake.tail;
    while(pn->next != NULL){
        pn->x = pn->next->x;
        pn->y = pn->next->y;
        pn = pn->next;
    }
}


void GameOver()
{
    endwin();
    exit(0); 
}

void HandleMoveEvent(int nextX, int nextY)
{
    pNode pn;
    if(IsRearchFood(nextX, nextY)){
        //吃到食物
        pn = CreateNode(nextX, nextY);     
        AddSnakeHead(pn);//将新结点作为新头部,蛇身体无需移动
        GenerateFood();
    }else{
         MoveSnakeBody();//移动身体，头不动 
         //由于头部移动时，蛇身会动，因此需要先移动身体，再检测头部是否到达身体
         if(IsEatSelf(nextX, nextY) || IsHitBorder(nextX, nextY)){
            //吃到自己或撞到边界
            GameOver();
         }else{
            //蛇头移动
            g_snake.head->x = nextX; 
            g_snake.head->y = nextY; 
         }
    }
}

void MoveSnake()
{
    switch(g_snake.direction){
        case UP :
            HandleMoveEvent(g_snake.head->x, g_snake.head->y - 1);
            break;
        case DOWN :
            HandleMoveEvent(g_snake.head->x, g_snake.head->y + 1);
            break;
        case LEFT :
            HandleMoveEvent(g_snake.head->x - 1, g_snake.head->y);
            break;
        case RIGHT :
            HandleMoveEvent(g_snake.head->x + 1, g_snake.head->y);
            break;
        default :
            break;
    } 
}

void GenerateFood()
{
    int i, j;
    int x, y;
    int foodIndex;
    int cnt = 0;
    int flag = 0;

    pNode pn = g_snake.tail;
    for(i = 0; i < g_snake.length;i++){
        x = pn->x;
        y = pn->y;
        pn = pn->next;
        g_screen[y][x] = 1;//标记该位置被蛇占了
    }
    foodIndex = rand() % (WIDTH * HEIGHT - g_snake.length);  
    //在第foodIndex个空闲位置生成食物, i, j从1开始到HEIGHT是因为蛇在x = 1~WIDTH, y = 1~HEIGHT区域移动 
    for(i = 1; i < HEIGHT + 1;i++){
        if(flag == 1){
            break; 
        }
        for(j = 1; j < WIDTH + 1;j++){
            if(g_screen[i][j] == 0){
                if(cnt == foodIndex){
                    g_food.x = j;
                    g_food.y = i;
                    flag = 1;
                    break;
                }
                cnt++;
            }
        } 
    }
    //清空标记
    for(i = 0;i < HEIGHT + 2;i++){
        for(j = 0;j < WIDTH + 2;j++){
            g_screen[i][j] = 0; 
        } 
    }
}

void DisPlayBorder()
{
    int i;
    //显示左右边界
    int width = WIDTH + 1;
    int height = HEIGHT + 1;
    for(i = 0;i <= height;i++){
        mvprintw(i, 0, "%c", '#');
        mvprintw(i, width, "%c", '#');
    }
    //显示上下边界
    for(i = 0;i <= width;i++){
        mvprintw(0, i, "%c", '#');
        mvprintw(height, i, "%c", '#');
    }
}

void Refresh()
{
    pNode pn = g_snake.tail;
    clear();
    DisPlayBorder();//显示边界
    mvprintw(g_food.y, g_food.x, "%c", '$');//显示食物    
    //显示蛇
    while(pn != NULL){
        mvprintw(pn->y, pn->x, "%c", '*'); 
        pn = pn->next;
    }
}


