#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <time.h>
#include <sys/wait.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
// termios to the control the game
static struct termios oldTermios, newTermios;

// program array to run them and pointer to find the pid from the command
char* programArray[3]={"./game_tictactoe","./game_snake","./game_invadespace"};
char *programName= NULL;
 FILE *runningGame=NULL;
__pid_t pid=-1;
int currentProgram=1;
int gameRunning=0;
int exitCondition=0;

void reset_terminal(){
    /*TCSANOW: change the setting immediately
    STDIN_FILENO: standard input
    &oldTermios: a pointer to saved terminal settings
    */
    printf("\e[H\e[J");
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

void keyboardInterrupt(){
    char ch;
    ssize_t bytesRead;
    bytesRead=read(STDIN_FILENO, &ch, 1);
    while(bytesRead<0&& errno==EINTR){
    }
    if(bytesRead>0){
         ch= tolower(ch);
         if(ch=='q' ){
               exitCondition=1;
               reset_terminal();
        }
        else if((ch=='w')&& currentProgram+1<3 ){
                currentProgram++;

        } else if(ch=='s'&& currentProgram-1>-1){
            currentProgram--;
        } else if (ch == '\n') { 
            pid =fork();
            if (pid==0){
              execlp(programArray[currentProgram],programArray[currentProgram],NULL);
        }else{
        wait(NULL);
        
    }
}
}
}
void displayMainScreen(){
    printf("\e[H\e[J");
    printf("\033[1;32m"); // Green bold text
    printf("*******************************\n");
    printf("      Welcome to the Game!     \n");
    printf("*******************************\n");
    

    printf("\033[0m");

    programName = programArray[currentProgram]+2;
    printf("\033[1;34m");// blue text
    printf("Current Program: %s\n", programName);
    printf("Press Enter to start or use W/S to navigate.\n");

    printf("\033[1;33m");
    printf("\nPress 'Q' to quit.\n");
    printf("*******************************\n");
    printf("\033[0m");
    fflush(stdout);
}
void signal_handler(int sig) {
    // Create the command string dynamically based on the current program
    if(pid>0){
      kill(pid,sig);
      pid=-1;
      }
    reset_terminal();
    printf("\033[?1049l");
    exit(0);
}

int main(){
    signal(SIGINT, signal_handler); 
    signal(SIGTERM, signal_handler); 
    srand(time(NULL)); 
    configureTerminal();
    const int frameDelay = 100000;
    while(!exitCondition){
        keyboardInterrupt();
        displayMainScreen(); 
        usleep(frameDelay);
    }
    return 0;
}
