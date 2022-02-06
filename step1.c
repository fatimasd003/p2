#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#ifdef main
#undef main
#endif

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
struct Game_Level level={8,{
        {30,ENEMY_TYPE,50,50,25},
        {8,BLANK_TYPE,110,120,32},
        {7,BLANK_TYPE,210,150,40},
        {25,MY_TYPE,310,320,35},
        {12,MY_TYPE,600,600,50},
        {21,MY_TYPE,400,500,40},
        {18,BLANK_TYPE,550,80,30},
        {16,ENEMY_TYPE,40,350,50}
}};
char buffer[10];
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 640;

void menu(){


}
void add_solder(bool t){
    for (int i = 0; i < level.barracksCount; ++i) {
        if(level.barracks[i].type!=BLANK_TYPE && level.barracks[i].soldier<level.barracks[i].max)
            level.barracks[i].soldier++;
    }
    if(t==true)
     SDL_Delay(3000);
}
void create_solder(struct Barrack a,struct Barrack b[],int m){
    b[m].x=a.x;
    b[m].y=a.y;
    b[m].type=a.type;
}


void move_solder(struct Barrack a,struct Barrack a2,struct Barrack b[],int m){
    for (int i = 0; i < m; i++) {
        if(b[i].x!=a2.x){
            if(b[i].x<a2.x)
                b[i].x+=10;
            else
                b[i].x-=10;
            int d=(a.y-a2.y),d2=(a.x-a2.x);
            double f=(double)d/(double)d2;
            b[i].y=f*(b[i].x-a.x)+a.y;
        }
    }
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
    stringRGBA(renderer,x,y,buffer,0,0,0,255);
}
void Draw(){
    for(int i=0;i<level.barracksCount;i++){
        show_Barrack(level.barracks[i]);
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
    if(s[*k].x==level.barracks[q2].x) {
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
    SDL_Delay(250);
    if(*k==h)
        return true;
    else
        return false;
}

int main(){
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window* window = SDL_CreateWindow("Getting Started", SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, 0);
    //Struct for handling events
    SDL_Event input;
    //Boolean to find out whether the game is stopped or not
    //Of course, it will be false at start
    bool quit = false;
    //For selecting renderer's draw color
    int r = 255, g = 255, b = 255,a=255;
    int m=0,k=0;
    struct Barrack s[50];
    bool t=false;
    for(int i=0;!quit;i++){
        //---Event polling method---
        //It runs until the number of events to be polled gets to zero
        while (SDL_PollEvent(&input) > 0) {
            //If the user did something which should
            //result in quitting of the game then...
            if (input.type == SDL_QUIT) {
                //...go out of the while loop
                quit = true;
            }
        }
        SDL_SetRenderDrawColor(renderer, r, g, b,a);
        SDL_RenderClear(renderer);
        if(!t)
            t=move(s,3,7,&m,&k);
        if(t || i%12==0) {
            add_solder(t);
        }
        Draw();
        SDL_RenderPresent(renderer);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
