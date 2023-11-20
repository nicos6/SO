#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void){
  
  int pid;
  
  if( ( pid=fork() ) < 0){
      perror("Eroare fork");
      exit(-1);
  }
  
  if(pid == 0){ //proces copil
    execl("./studenti", "studenti", "3",  NULL);
    perror("Eroare execl\n");
    exit(-1);
  }

  while(1){
    char caracter;
    scanf(" %c", &caracter); //am pus spatiu inainte de %c ca sa nu afiseze 'caracter' de 2 ori
    printf("caracter: %c\n", caracter);
    usleep(500000);
    if( caracter == '0') //citirea se opreste cand introducem '0'
      break;
  }

  int status;
  if( wait(&status) == -1){
    perror("Eroare wait\n");
    exit(-1);
  }

  if (WIFEXITED(status)) {
    printf("Procesul copil s-a incheiat cu status=%d\n", WEXITSTATUS(status));
  } else if (WIFSIGNALED(status)) {
    printf("killed by signal %d\n", WTERMSIG(status));
  } else if (WIFSTOPPED(status)) {
    printf("stopped by signal %d\n", WSTOPSIG(status));
  } else if (WIFCONTINUED(status)) {
    printf("continued\n");
  }
  
  return 0;
}
