#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(void){
  int pid;
  int pfd[2];

  if(pipe(pfd)<0){
    printf("Eroare la crearea pipe-ului\n");
    exit(-1);
  }
  
  if( ( pid=fork() ) < 0){
      perror("Eroare");
      exit(-1);
  }

  if(pid == 0){
    close(pfd[1]);
    char caracter;
    read(pfd[0], &caracter, sizeof(char));
    printf("Caracterul este: %c\n", caracter);
    close(pfd[0]);
  }
  else{
    close(pfd[0]);
    char caracter;
    scanf("%c", &caracter);
    write(pfd[1], &caracter, sizeof(char));
    close(pfd[1]);
    
    int status;
    if( wait(&status) == -1){
      perror("Eroare wait\n");
      exit(-1);
    }
    
    if (WIFEXITED(status)) {
      printf("Procesul copil s-a incheiat cu status=%d\n", WEXITSTATUS(status));
    }
  }
  
  return 0;
}
