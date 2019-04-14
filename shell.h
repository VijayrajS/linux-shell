#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <grp.h>
#include <limits.h>
#include <pwd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <syscall.h>
#include <time.h>
#include <unistd.h>

typedef struct {
    int id;
    int crtime;
    char name[256];
} pid_det;


int cd(char *argv1[], int argc1, char* home);
void rtc_drive();
void clock_def(char* argv1[], int argc1);
int echo(char *argv1[], int argc1);
void init();
void fin_bg_proc();
void exec(char* argv1[], int argc1, int bg);
void longlist(char *directory, char *filename);
void ls(char *argv1[], int argc1, char* home);
char* ishome(char* home, char* path);
void bash_loop();
void pinfo(char* argv1[], int argc1, char* home);
void pwd();
int reminder(char* argv1[], int argc1);
void seten(char* argv1[], int argc1);
void unseten(char* argv1[], int argc1);
void okill();
void jobs();
void kjob(char* argv1[], int argc1);
void bg(char* argv1[], int argc1);
void fg(char* argv1[], int argc1);
void fgkiller(int pid);
void pipe_exec(char* argv1[], int argc1, char* home);
void sigint_handler();
void redir(char* argv1[], int argc1, char* home);
