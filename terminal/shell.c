#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#define FALSE 0
#define TRUE 1
#define INPUT_STRING_SIZE 80
#define INITIAL_FD -5//any negative no. but -1 bcoz fd can be -1 or non-negative

#include "io.h"
#include "parse.h"
#include "process.h"
#include "shell.h"
#include "myerror.h"

int cmd_quit(tok_t arg[]) {
  printf("Bye\n");
  exit(0);
  return 1;
}

int cmd_cd(tok_t arg[]){
    char *pwd=NULL;
    MYERRORPA(getcwd(NULL,0),pwd);
    else if(*arg==NULL || !strcmp("~",*arg))
    {
        MYERROR(chdir(getenv("HOME")));
        else{
            MYERROR(setenv("OLDPWD",pwd,1));
        }
        free(pwd);
    }
    else if(!strcmp("-",*arg))
    {
        MYERROR(chdir(getenv("OLDPWD")));
        else{
            MYERROR(setenv("OLDPWD",pwd,1));
        }
        free(pwd);
    }
    else
    {
        MYERROR(chdir(*arg));
        else{
            MYERROR(setenv("OLDPWD",pwd,1));
        }
        free(pwd);
    }
    return 1;
}

int cmd_help(tok_t arg[]);


/* Command Lookup table */
typedef int cmd_fun_t (tok_t args[]); /* cmd functions take token array and return int */
typedef struct fun_desc {
  cmd_fun_t *fun;
  char *cmd;
  char *doc;
} fun_desc_t;

fun_desc_t cmd_table[] = {
  {cmd_help, "?", "show this help menu"},
  {cmd_quit, "quit", "quit the command shell"},
  {cmd_cd, "cd", "change directory"}
};

int cmd_help(tok_t arg[]) {
  int i;
  for (i=0; i < (sizeof(cmd_table)/sizeof(fun_desc_t)); i++) {
    printf("%s - %s\n",cmd_table[i].cmd, cmd_table[i].doc);
  }
  return 1;
}

int lookup(char cmd[]) {
  int i;
  for (i=0; i < (sizeof(cmd_table)/sizeof(fun_desc_t)); i++) {
    if (cmd && (strcmp(cmd_table[i].cmd, cmd) == 0)) return i;
  }
  return -1;
}

void init_shell()
{
  /* Check if we are running interactively */
  shell_terminal = STDIN_FILENO;

  /** Note that we cannot take control of the terminal if the shell
      is not interactive */
  shell_is_interactive = isatty(shell_terminal);

  if(shell_is_interactive){

    /* force into foreground */
    while(tcgetpgrp (shell_terminal) != (shell_pgid = getpgrp()))
      kill( - shell_pgid, SIGTTIN);

    shell_pgid = getpid();
    /* Put shell in its own process group */
    if(setpgid(shell_pgid, shell_pgid) < 0){
      perror("Couldn't put the shell in its own process group");
      exit(1);
    }

    /* Take control of the terminal */
    tcsetpgrp(shell_terminal, shell_pgid);
    tcgetattr(shell_terminal, &shell_tmodes);
  }
  /** YOUR CODE HERE */
}

/**
 * Add a process to our process list
 */
void add_process(process* p)
{
  /** YOUR CODE HERE */
}

/**
 * Creates a process given the inputString from stdin
 */
process* create_process(char* inputString)
{
  /** YOUR CODE HERE */
  return NULL;
}



int shell (int argc, char *argv[]) {
  char *s = malloc(INPUT_STRING_SIZE+1);			/* user input string */
  char *pwd=NULL;
  tok_t *t;			/* tokens parsed from input */
  int lineNum = 0;
  int fundex = -1;
  pid_t pid = getpid();		/* get current processes PID */
  pid_t ppid = getppid();	/* get parents PID */
  pid_t cpid, tcpid, cpgid;

  init_shell();

  printf("%s running as PID %d under %d\n",argv[0],pid,ppid);

  lineNum=0;
  MYERRORPA(getcwd(NULL,0),pwd);   //Assuming no error
  fprintf(stdout, "[%s] %d: ",pwd, lineNum);
  free(pwd);
  while ((s = freadln(stdin))){
    t = getToks(s); /* break the line into tokens */
    int i=0,fd=INITIAL_FD,bak_fd_i=INITIAL_FD,bak_fd_o=INITIAL_FD;
    while(t[i]){
        if(!strcmp(t[i],"<")){
            MYERRORA(open(t[i+1],O_RDONLY),fd);
            else{
                MYERRORA(dup(STDIN_FILENO),bak_fd_i);
                MYERROR(dup2(fd,STDIN_FILENO));
                MYERROR(close(fd));
            }
            t[i]=NULL;
            break;
        }
        else if(!strcmp(t[i],">")){
            MYERRORA(open(t[i+1],O_WRONLY|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH),fd);
            else{
                MYERRORA(dup(STDOUT_FILENO),bak_fd_o);
                MYERROR(dup2(fd,STDOUT_FILENO));
                MYERROR(close(fd));
            }
            t[i]=NULL;
            break;
        }
        i++;
    }
    if(fd>=0 || fd==INITIAL_FD){
        fundex = lookup(t[0]); /* Is first token a shell literal */
        if(fundex >= 0) cmd_table[fundex].fun(&t[1]);
        else {
            pid_t pid;
            MYERRORA(fork(),pid);
            else if(pid==0)
            {
                MYERROR(execvp(t[0],t));
                exit(0);
            }
            else
            {
                MYERROR(wait(NULL));
            }
        }
    }
    if(bak_fd_i>=0){
        MYERROR(dup2(bak_fd_i,STDIN_FILENO));
        MYERROR(close(bak_fd_i));
    }
    if(bak_fd_o>=0){
        MYERROR(dup2(bak_fd_o,STDOUT_FILENO));
        MYERROR(close(bak_fd_o));
    }
    MYERRORPA(getcwd(NULL,0),pwd);   //Assuming no error
    fprintf(stdout, "[%s] %d: ",pwd, lineNum);
    free(pwd);
  }
  return 0;
}
