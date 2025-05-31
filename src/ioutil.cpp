#include "ioutil.h"

int getch(bool echo) {
    int ch;
    
    struct termios orig;
    struct termios crnt;
    
    tcgetattr(0, &orig);
    
    crnt = orig;
    
    crnt.c_lflag &= ~ICANON;
    
    if (echo) crnt.c_lflag |=  ECHO;
    else      crnt.c_lflag &= ~ECHO;
    
    tcsetattr(0, TCSANOW, &crnt);
    ch = getchar();
    tcsetattr(0, TCSANOW, &orig);
    
    return ch;
}