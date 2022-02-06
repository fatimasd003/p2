#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#ifdef main
#undef main2

#endif
#include <SDL_video.h>
#include <stdio.h>
#include <stdbool.h>
#define ENEMY_TYPE 0
#define BLANK_TYPE -1
#define MY_TYPE 3
SDL_Renderer* renderer;
Uint32 colors[3]={0xffa3a9a1,0xff3383ff,0xffff333f};

struct Barrack{
    int soldier;
    int type;
    int x;
    int y;
    int max;
};
struct Game_Level{
    int barracksCount;
    struct Barrack barracks[16];
};
struct Game_Level level;/*={9,{
        {20,BLANK_TYPE,50,30,25},
        {16,ENEMY_TYPE,280,120,35},
        {9,MY_TYPE,600,60,45},
        {20,MY_TYPE,60,120,30},
        {8,BLANK_TYPE,45,450,55},
        {17,BLANK_TYPE,50,600,40},
        {18,BLANK_TYPE,300,400,60},
        {11,BLANK_TYPE,550,450,50},
        {13,MY_TYPE,350,580,65}
}};*/

char buffer[10];
const int SCREEN_WIDTH = 700;
const int SCREEN_HEIGHT = 650;

void add_solder(bool t){
    for (int i = 0; i < level.barracksCount; i++) {
        if(level.barracks[i].type!=BLANK_TYPE && level.barracks[i].soldier<level.barracks[i].max)
            level.barracks[i].soldier++;
        else if(level.barracks[i].type==BLANK_TYPE && level.barracks[i].soldier<15)
            level.barracks[i].soldier++;
    }
}
void create_solder(struct Barrack a,struct Barrack b[],int m){
    b[m].x=a.x;
    b[m].y=a.y;
    b[m].type=a.type;
}
void show_solder(struct Barrack b[],int m){
    int h;
    for (int i = 0; i < m; i++) {
        switch (b[i].type) {
            case ENEMY_TYPE:h = 1;break;
            case MY_TYPE:h = 2;break;
            case BLANK_TYPE:h = 0;break;
        }
        Uint32 color = colors[h];
        Sint16 x = b[i].x, y = b[i].y, r = 6;
        filledCircleColor(renderer, x, y, r, color);
    }
}
void show_Barrack(struct Barrack b){
    int i;
    switch (b.type) {
        case ENEMY_TYPE: i=1;break;
        case MY_TYPE:i=2;break;
        case BLANK_TYPE:i=0;break;
    }
    Uint32 color =colors[i];
    Sint16 x =b.x , y =b.y , r = 20;
    boxColor(renderer,x-r,y-r,x+r,y+r,color);
    sprintf(buffer,"%d",b.soldier);
    stringRGBA(renderer,x-5,y,buffer,0,0,0,255);
}
void Draw(){
    for(int i=0;i<level.barracksCount;i++){
        show_Barrack(level.barracks[i]);
    }
}
void move_solder(struct Barrack a,struct Barrack a2,struct Barrack b[],int m){
    for (int i = 0; i < m; i++) {
        int fx=a.x-a2.x,fy=a.y-a2.y;
        if(fx<0)fx*=-1;
        if(fy<0)fy*=-1;
        if(100<fx || 50>fy) {
            if (b[i].x != a2.x) {
                if (b[i].x < a2.x)
                    b[i].x += 10;
                else
                    b[i].x -= 10;
                int d = (a.y - a2.y), d2 = (a.x - a2.x);
                double m = (double) d / (double) d2;
                b[i].y = m * (b[i].x - a.x) + a.y;
            }
        }
        else{
            if (b[i].y != a2.y) {
                if (b[i].y < a2.y)
                    b[i].y += 10;
                else
                    b[i].y -= 10;
                int d = (a.y - a2.y), d2 = (a.x - a2.x);
                double m = (double) d / (double) d2;
                double f=(double)(b[i].y-a.y)/m;
                b[i].x=f+ a.x;
            }
        }
    }
}
bool move(struct Barrack s[],int q1,int q2,int *m,int *k){
    static int h;
    if(*m==0) {
        h = level.barracks[q1].soldier;
    }
    create_solder(level.barracks[q1], s, *m);
    if(*m<h) {
        level.barracks[q1].soldier--;
        (*m)++;
    }
    move_solder(level.barracks[q1],level.barracks[q2],s,*m);
    int fx=level.barracks[q2].x-s[*k].x,fy=level.barracks[q2].y-s[*k].y;
    if(fx<0)fx*=-1;
    if(fy<0)fy*=-1;
    if(10>fx && 10>fy) {
        if(level.barracks[q1].type==level.barracks[q2].type)
            level.barracks[q2].soldier++;
        else
            level.barracks[q2].soldier--;
        if(level.barracks[q2].soldier<0){
            level.barracks[q2].soldier=1;
            level.barracks[q2].type=level.barracks[q1].type;
        }
        ++(*k);
    }
    show_solder(s, *m);
    SDL_Delay(125);
    //printf("k: %d  h: %d\n",*k,h);
    if(*k==h)
        return true;
    else
        return false;
}
int which_barrack(SDL_Event input){
    int r=20;
    for (int i = 0; i < level.barracksCount; i++)
        if(input.button.x<=level.barracks[i].x+r && input.button.x>=level.barracks[i].x-r && input.button.y<=level.barracks[i].y+r && input.button.y>=level.barracks[i].y-r)
            return i;
    return -1;
}
bool win(){
    for (int i = 0; i < level.barracksCount; i++) {
        if(level.barracks[i].type==ENEMY_TYPE)
            return false;
    }
    Uint32 color =colors[2];
    Sint16 x =350 , y =325 , r = 200;
    boxColor(renderer,x-r,y-r,x+r,y+r,color);
    sprintf(buffer,"%s","YOU WIN");
    stringRGBA(renderer,x,y,buffer,0,0,0,255);
    return true;
}
void draw_rectangle(int i){
    SDL_Rect rect;
    rect.x=level.barracks[i].x-22;
    rect.y=level.barracks[i].y-22;
    rect.w=46;
    rect.h=46;
    int r = 255, g = 0, b = 255,a=255;
    SDL_SetRenderDrawColor(renderer, r, g, b,a);
    SDL_RenderDrawRect(renderer,&rect);
}
bool choice(int *p,int *m,int *k,bool *u,int *q1,int *q2,int q){
    if (*p == 1) {
        *q1 = q;
        if (level.barracks[*q1].type!=MY_TYPE)
            *p = 0;
        else
            *u = true;
    } else if (*p == 2) {
        *q2 = q;
        *p = 0;
        *u = false;
        *m = 0;
        *k = 0;
        if (*q1 != *q2)
            return false;
    }
    return true;
}
void write_file(char s[]){
    FILE *fp;
    fp= fopen(s,"wb");
    if(!fp){
        printf("Error in opening file!\n");
        exit(1);
    }
    fwrite(&level,sizeof(struct Game_Level),1,fp);
    fclose(fp);
}
void reed_file(char s[]){
    FILE *fp;
    fp= fopen(s,"rb");
    if(!fp){
        printf("Error in open file!\n");
        exit(1);
    }
    fread(&level,sizeof(struct Game_Level),1,fp);
    fclose(fp);
}
void which_file(int flag){
    switch (flag) {
        case 0:reed_file("previous_game.bin");break;
        case 1:reed_file("file1.bin");break;
        case 2:reed_file("file2.bin");break;
        default:reed_file("file1.bin");break;
    }
}
bool check_time(int start,int timer){
    int end;
    end=SDL_GetTicks();
    if(end-start>timer)
        return true;
    return false;
}
void Menu(){
    int flag;
    printf("Enter shomare file (1 or 2):");
    scanf("%d",&flag);
    which_file(flag);
}

int main(){
    Menu();
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window* window = SDL_CreateWindow("State.io", SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, 0);
    //Struct for handling events
    SDL_Event input;
    //Boolean to find out whether the game is stopped or not
    //Of course, it will be false at start
    bool quit = false;
    //For selecting renderer's draw color
    int r = 255, g = 255, b = 255,a=255;
    int m=0,k=0,q1,q2,q,p=0,timer=3000,e=0;
    int startTicks,endTicks,checkTicks;
    struct Barrack s[100];
    bool t=true,press=false,u=false;
    startTicks=SDL_GetTicks();
    for(int i=0;!quit;i++){
        //---Event polling method---
        //It runs until the number of events to be polled gets to zero
        while (SDL_PollEvent(&input) > 0) {
            //If the user did something which should
            //result in quitting of the game then...
            switch (input.type) {
                case SDL_QUIT:quit = true;break;
                case SDL_MOUSEBUTTONUP:
                    q=which_barrack(input);
                    if(q!=-1) {
                        p++;
                        press = true;
                    }
                    break;
            }
        }
        SDL_SetRenderDrawColor(renderer, r, g, b,a);
        SDL_RenderClear(renderer);
        if(win())
            e++;
        if(e<1) {
            if(t && press) {
                t = choice(&p, &m, &k, &u, &q1, &q2, q);
                press = false;
            }
            if (u) draw_rectangle(q1);
            if (!t) t = move(s, q1, q2, &m, &k);
            Draw();
            if (check_time(startTicks,timer)) {
                add_solder(t);
                startTicks=SDL_GetTicks();
            }
        }
        if(e==2) {
            SDL_Delay(4000);
            quit = true;
        }
        SDL_RenderPresent(renderer);
    }
    write_file("previous_game.bin");
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}



































































































































































































