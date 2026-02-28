#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <time.h>
#include <ctype.h>
#include <errno.h>
#define ROWS 15
#define COLS 15
/*
    direction enumeration to keep track of the direction
    termios 
    instance of termios to control terminal as desired
*/
typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT
}Direction;
static struct termios oldTermios, newTermios;


/*
grid: main grid  to keep the back ground
snake tail length, bait coordinates , head and tail coordinates to keep track of the snake and grid details
head direction to keep track of movement
*/
char* grid[ROWS][COLS];  
int snakeTailLength=0;
int baitX,baitY;
int snakeHeadX,snakeHeadY;
int** snakeTailXY=NULL;
Direction headDirection; 


void reset_terminal(){
    /*TCSANOW: change the setting immediately
    STDIN_FILENO: standard input
    &oldTermios: a pointer to saved terminal settings
    */
    printf("\e[?25h");
    fflush(stdout);
    tcsetattr(STDIN_FILENO,TCSANOW, &oldTermios);
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
void initGrid(){
    for(int i =0; i<ROWS;i++){
        for(int j=0; j<COLS ;j++){
            grid[i][j]= ".";
        }
    }
    snakeHeadX= ROWS/2;
    snakeHeadY= COLS/2;
    do{
        baitX= rand()%ROWS;
        baitY=rand()%ROWS;
    }while(baitY==snakeHeadY && baitX==snakeHeadX);
    headDirection=(Direction)(rand()%4); 
}
int isItTail(int r, int c){
    // control does the coordinate correspond tail or not
    for (int i = 0; i < snakeTailLength; i++) {
        if (snakeTailXY[i][0] == r && snakeTailXY[i][1] == c) {
            return 1;
        }
    }
   return 0;

}
void exitTheGame(){
    reset_terminal();

      // Free dynamically allocated memory for snake tail
    for (int i = 0; i <snakeTailLength; i++) {
        free(snakeTailXY[i]);
    }
    free(snakeTailXY);
    exit(0);
    }
void displayGrid(){
    printf("\e[H\e[J");
    for (int i=0;i<ROWS;i++){
        for (int j=0;j<COLS;j++){
            if(isItTail(i,j)){ 
                printf("%s","#");
            }else if(snakeHeadX==i && snakeHeadY==j){
                printf("%s","O");
            }else if(baitX==i && baitY==j){
                  printf("%s","X");
            }else{
            printf("%s",grid[i][j]);
            };
        }printf("\n");
    }
}
void keyboardInterrupt(){
    char ch;
    ssize_t bytesRead;
    bytesRead=read(STDIN_FILENO, &ch, 1);
    while(bytesRead<0&& errno==EINTR){
    }
    if(bytesRead>0){
        ch= tolower(ch);
        if((ch=='w'&& headDirection!=DOWN)&& snakeHeadX!=0 ){
            headDirection=UP;
        } 
        else if((ch=='d'&& headDirection!=LEFT) &&snakeHeadY!=COLS-1 ){
            headDirection=RIGHT;
        }
        else if((ch=='a'&& headDirection!=RIGHT) && snakeHeadY!=0){
            headDirection=LEFT;
        }
        else if((ch=='s'&& headDirection!=UP) && snakeHeadX!=ROWS-1){
            headDirection=DOWN;
        }
        else if(ch=='q'){
          exitTheGame();
        }

    }      

}
int shouldGo(int headX, int headY){
    // check whether the snake can move or not
    if((snakeHeadY==0 && headDirection== LEFT)||(snakeHeadY==COLS-1 && headDirection== RIGHT)||(snakeHeadX==0 && headDirection==UP)||(snakeHeadX==ROWS-1 && headDirection==1)){
        return 0;}
    else{
        int headNextCoordinates[2];
         switch(headDirection){
            case UP:
                if(headY!=0){
                    headNextCoordinates[0]=headX-1;
                    headNextCoordinates[1]=headY;
                }
                break;
            case DOWN:
                if(headX!=ROWS-1){
                    headNextCoordinates[0]=headX+1;
                    headNextCoordinates[1]=headY;
                }
                break;
            case LEFT:
                if(headY!=0){
                    headNextCoordinates[0]=headX;
                    headNextCoordinates[1]=headY-1;
                }
                break;
            case RIGHT:
                if(headY!=COLS-1){
                    headNextCoordinates[0]=headX;
                    headNextCoordinates[1]=headY+1;
                }
            break;
        default:
            break;

    }
        for (int i=0;i<snakeTailLength;i++){
            if(snakeTailXY[i][0]==headNextCoordinates[0]&& snakeTailXY[i][1]==headNextCoordinates[1]){
                return 0;

            }
        }
    }
    return 1;
}
int isEaten(){
    // the process of eating
    if(snakeHeadX==baitX&& snakeHeadY==baitY){
        do{
        baitX= rand()%ROWS;
        baitY=rand()%ROWS;
    }while(baitY==snakeHeadY && baitX==snakeHeadX);
    snakeTailLength++;
    snakeTailXY = (int **)realloc(snakeTailXY, (snakeTailLength + 1) * sizeof(int *));
    snakeTailXY[snakeTailLength - 1] = (int *)malloc(2 * sizeof(int));
    snakeTailXY[snakeTailLength - 1][0] = -1;
    snakeTailXY[snakeTailLength - 1][1] = -1;
    return 1;
    }
    return 0;
}
void updateSnakeCoordinates(){
    if(shouldGo(snakeHeadX,snakeHeadY)==1){
        if(snakeTailLength!=0){
            int currentX= snakeTailXY[0][0];
            int currentY= snakeTailXY[0][1];
            snakeTailXY[0][0]=snakeHeadX;
            snakeTailXY[0][1]=snakeHeadY;
            int previousX, previousY;
            for (int i=1; i<snakeTailLength;i++){
                previousX= snakeTailXY[i][0];
                previousY= snakeTailXY[i][1];
                snakeTailXY[i][0]= currentX;
                snakeTailXY[i][1]= currentY;
                currentX= previousX;
                currentY= previousY;
            }
        }
    switch(headDirection){
        case UP:
            if(snakeHeadX!=0){
                snakeHeadX--;
            }
            break;
        case DOWN:
             if(snakeHeadX!=ROWS-1){
                snakeHeadX++;
            }
            break;
        case LEFT:
            if(snakeHeadY!=0){
                snakeHeadY--;
            }
            break;
        case RIGHT:
            if(snakeHeadY!=COLS-1){
                snakeHeadY++;
            
            }
            break;
        default:
            break;

    }
    }
    isEaten();
    }
void signalHandler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        exitTheGame();
    }
}
int main(int argc, char *argv[]){
    srand(time(NULL)); 
    signal(SIGINT, signalHandler); 
    signal(SIGTERM, signalHandler); 
    configureTerminal();
    initGrid();
    const int frameDelay = 100000;
    while (1) {
        keyboardInterrupt();      
        updateSnakeCoordinates(); 
        displayGrid();            
        usleep(frameDelay);
    }
    exitTheGame();
}
