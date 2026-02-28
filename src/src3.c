#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <ctype.h>
# define COMPUTER 1
#define PLAYER 2
# define COMPUTER_MOVE 'O'
#define PLAYER_MOVE 'X'


// grid  and basic booleans for the ending of the game
int gameEndedNormal=0;
int whoseTurn = PLAYER;
char grid[3][3];
int gridIsFull=0;
int exitCondition = 0;

static struct termios oldTermios, newTermios;
int checkVictory(){
     // Row check for victory
    for (int row = 0; row < 3; row++) {
        if (grid[row][0] == grid[row][1]
            && grid[row][1] == grid[row][2]) {
            if (grid[row][0] == PLAYER_MOVE)
                return 1;
            else if (grid[row][0] == COMPUTER_MOVE)
                return -1;
        }
    }

    // Column check for victory
    for (int col = 0; col < 3; col++) {
        if (grid[0][col] == grid[1][col]
            && grid[1][col] == grid[2][col]) {
            if (grid[0][col] == PLAYER_MOVE)
                return +1;
            else if (grid[0][col] == COMPUTER_MOVE)
                return -1;
        }
    }

    // Diagonal check for victory
    if (grid[0][0] == grid[1][1] && grid[1][1] == grid[2][2]) {
        if (grid[0][0] == PLAYER_MOVE)
            return +1;
        else if (grid[0][0] == COMPUTER_MOVE)
            return -1;
    }

    if (grid[0][2] == grid[1][1] && grid[1][1] == grid[2][0]) {
        if (grid[0][2] == PLAYER_MOVE)
            return +1;
        else if (grid[0][2] == COMPUTER_MOVE)
            return -1;
    }
    return 0;
}
void signalHandler(int signal) {
if (signal == SIGINT || signal == SIGTERM) {
    exitCondition = 1;
    }
}
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
void initGrid() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            grid[i][j] = '.';  // Use single quotes for character
        }
    }
    gridIsFull=0;
}
void displayGrid(){
     printf("\e[H\e[J");
    if(whoseTurn==1){
        printf("%s","It is computer turn\n");
        
    }else{
       printf("%s","It is your turn, Press numbers from 1-9 to put X\n");
    }
    fflush(stdout);
   
    printf("%c | %c | %c \n", grid[0][0],
           grid[0][1], grid[0][2]);
    printf("-------\n");
    printf("%c | %c | %c \n", grid[1][0],
           grid[1][1], grid[1][2]);
    printf("-------\n");
    printf("%c | %c | %c \n\n", grid[2][0],
           grid[2][1], grid[2][2]);
    fflush(stdout);
}
void computerPlay(){
    if(whoseTurn==COMPUTER){
        // randomly put a O to an empty place
        int select;
        do{
            select= rand()%9;
        }while(grid[select/3][select%3]!='.');
        grid[select/3][select%3]='O';
        whoseTurn=PLAYER;
        gridIsFull++;
        
    }
}
void keyboardInterrupt(){
    char ch;
    if(read(STDIN_FILENO, &ch, 1)>0 && whoseTurn==2){
        ch= tolower(ch);
        if(ch=='q'){
         exitCondition=1; return;
        }
        int select= ch- '0';
        if(whoseTurn==PLAYER&& grid[(select-1)/3][(select-1)%3]=='.'){
             switch (ch) {
                case '1': grid[0][0] = PLAYER_MOVE; whoseTurn = COMPUTER; gridIsFull++; break;
                case '2': grid[0][1] = PLAYER_MOVE; whoseTurn = COMPUTER; gridIsFull++;  break;
                case '3': grid[0][2] = PLAYER_MOVE; whoseTurn = COMPUTER; gridIsFull++; break;
                case '4': grid[1][0] = PLAYER_MOVE;whoseTurn = COMPUTER; gridIsFull++;  break;
                case '5': grid[1][1] = PLAYER_MOVE;whoseTurn = COMPUTER; gridIsFull++; break;
                case '6': grid[1][2] = PLAYER_MOVE;whoseTurn = COMPUTER; gridIsFull++; break;
                case '7': grid[2][0] = PLAYER_MOVE;whoseTurn = COMPUTER; gridIsFull++; break;
                case '8': grid[2][1] = PLAYER_MOVE;whoseTurn = COMPUTER; gridIsFull++; break;
                case '9': grid[2][2] = PLAYER_MOVE;whoseTurn = COMPUTER; gridIsFull++; break;
                default: break;
            }

    }
    }
}
int main(int argc, char *argv[]){
    srand(time(NULL)); 
    configureTerminal();
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    while(!exitCondition){
        initGrid();
        sleep(1);
        keyboardInterrupt();
        while(!exitCondition&& !gameEndedNormal){
            if(checkVictory()==1){
                printf("%s","YOU WON!");
                fflush(stdout);
                gameEndedNormal=1;
                break;
            }
            keyboardInterrupt();
            displayGrid();
            if(checkVictory()==-1){
                printf("%s","YOU LOST!");
                fflush(stdout);
                gameEndedNormal=1;
                break;
            };
            if(checkVictory()==0 &&gridIsFull==9 ){
                printf("%s","DRAW!");
                fflush(stdout);
                gameEndedNormal=1;
                break;
            };
            sleep(1);
            if(whoseTurn==COMPUTER){
                computerPlay();
                fflush(stdout);
            }
        }
    }
}


