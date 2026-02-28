#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <time.h>
#include <ctype.h>
#define ROWS 15
#define COLS 20

// grid and direction for main features of the game
char* grid[ROWS][COLS]; 
typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT
}Direction;
static struct termios oldTermios, newTermios;
// bait struct and bait array for the game
struct bait{
    int x;
    int y;
    Direction direction;
};
struct bait baitArray[6];
int baitNumber;
int lastBaits;
int playerX, playerY;
int score;
int exitCondition;
int gunX,gunY;
int isThereGun;
int winCondition;


void reset_terminal(){
    /*TCSANOW: change the setting immediately
    STDIN_FILENO: standard input
    &oldTermios: a pointer to saved terminal settings
    */
    printf("\e[?25h");
    fflush(stdout);
    tcsetattr(STDIN_FILENO,TCSANOW, &oldTermios);
}

void exitTheGame(){
    reset_terminal();
    exit(0);
    }
void signalHandler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        exitTheGame();
    }
}

int collisionChecker(){
    // collision of the gun and bait
    if(!isThereGun){
        return 0;
    }
    for (int i=0;i<baitNumber;i++){
        if(gunX== baitArray[i].x&& gunY== baitArray[i].y){
            score++;
            gunX=-1;
            gunY=-1;
            isThereGun=0;
            baitArray[i].x=-1;
            baitArray[i].y=-1;
            lastBaits--;
            return 1;
        }
    }
    return 0;
}

void baitsMovement(){
    // baits randomly move from left and right
        for (int i = 0; i < baitNumber; i++) {
            if(baitArray[i].x!=-1){
                if(baitArray[i].direction==LEFT){
                    if(baitArray[i].y!=0){
                        baitArray[i].y--;
                    }else{
                        baitArray[i].direction=RIGHT;
                    }
                }else{
                    if(baitArray[i].y!=COLS-1){
                        baitArray[i].y++;
                    }else{
                        baitArray[i].direction=LEFT;
                    }
                }
            }
     }
}
int isBait(int x , int y ){
    // function to detect where are the baits in the grid
    for (int i = 0; i < baitNumber; i++) {
        if (baitArray[i].x == x && baitArray[i].y == y) {
            return 1;
        }
    }
    return 0;
}
int isPositionTaken(int x, int y,int count) {
    // to not put the baits into same place
    for (int i = 0; i < baitNumber; i++) {
        if (baitArray[i].x == x || baitArray[i].y == y) {
            return 1; 
        }
    }
    return 0; 
}
void initBaits(){
    for (int i=0;i<baitNumber;i++){
        int x;
        int y;
        int direction;
        do{
            x=rand()%8;
            y=rand()%20;
            direction= rand()%2;
        }while(isPositionTaken(x,y,i));
            baitArray[i].y=y;
            baitArray[i].x=x;
            baitArray[i].direction=direction; 
    }
}
void gunMovement(){
    if(isThereGun&& gunX!=0){
        gunX--;
    }else if(gunX==0){
        isThereGun=0;
        gunX=-1;
        gunY=-1;
    }
}
void initGrid(){
    for(int i =0; i<ROWS;i++){
        for(int j=0; j<COLS ;j++){
            grid[i][j]= ".";
        }
    }
    exitCondition=0;
    winCondition=0;
    baitNumber=6;
    lastBaits=6;
    playerX= ROWS-1;
    playerY= COLS/2;
    exitCondition=0;
    score=0;
    gunX=-1;
    gunY=-1;
    isThereGun=0;
    initBaits();
    

}
void displayGrid(){
    printf("\e[H\e[J");
    for (int i=0;i<ROWS;i++){
        for (int j=0;j<COLS;j++){
            if(playerX==i && playerY==j){
                printf("%s","*");
            }else if (gunX==i && gunY==j){
                printf("%s","^");
            }else if(isBait(i,j)==1){
                printf("%s", "O");
            }
            else{
            printf("%s",grid[i][j]);
            };
        }printf("\n");
    }

    collisionChecker();
    gunMovement();
    baitsMovement();
}


void configureTerminal(){
    tcgetattr(STDIN_FILENO, &oldTermios); // store current in the old Termios
    newTermios= oldTermios; // save it to be able to reset on exit
    newTermios.c_lflag &=  ~(ICANON | ECHO); // turn off echo + non-canonical mode so that pressing is not seen in the terminal
    newTermios.c_cc[VMIN]=0;
    newTermios.c_cc[VTIME]=0;
    tcsetattr(STDIN_FILENO, TCSANOW, &newTermios); // apply new settings
    atexit(reset_terminal); // when exit reset the terminal

}


void keyboardInterrupt(){
    char ch;
    if(read(STDIN_FILENO, &ch, 1)>0){
        ch= tolower(ch);
        if(ch=='a'&& playerY!=0){
            playerY--;
        }
        else if(ch=='q'){
            exitTheGame();
        }
        else if(ch=='d'&& playerY!=COLS-1){
            playerY++;
        }else if(ch== ' '&& !isThereGun){
            isThereGun=1;
            gunX= playerX-1;
            gunY= playerY;

        }

    }
}      

int main(int argc, char *argv[]){
    srand(time(NULL)); 
    configureTerminal();
    initGrid();
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    const int frameDelay = 100000;
    while (!exitCondition) {
        keyboardInterrupt();
        while(!winCondition && !exitCondition){
            displayGrid();      
            keyboardInterrupt();      
            usleep(frameDelay);
            if(lastBaits==0){
                winCondition=1;
                printf("YOU WIN!");
                fflush(stdout);
            }

    }
}
}
