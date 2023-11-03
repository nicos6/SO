#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/stat.h>

int main(int argc, char *argv[]){
  
  if(argc != 4){
    printf("Numar gresit de argumente\n");
  }

  
  char *file_input = argv[1];
  char *file_output = argv[2];
  char ch = argv[3][0];
  int fd, fd2;
  
  if((fd=open(file_input, O_RDONLY)) == -1){
    perror("Nu se poate citi fisierul");
  }
  
  if((fd2=open(file_output, O_WRONLY | O_TRUNC)) == -1){
    perror("Eroare fisier scriere");
  }
 
  char buffer[1];
  int litereMici=0, litereMari=0, cifre=0, aparitii=0;
  while(read(fd, buffer, 1) > 0){
    if(buffer[0] == ch){
      aparitii++;
    }
    if(islower(buffer[0])){
      litereMici++;
    }
    else if(isupper(buffer[0])){
      litereMari++;
    }
    else if(isdigit(buffer[0])){
     cifre++;
   }
 }

  struct stat st;
  stat(file_input, &st);
 
  char statistica[256];
  int size=0;
  size=sprintf(statistica, "numar litere mici: %d\nnumar litere mari: %d\nnumar cifre: %d\nnumar aparitii caracter: %d\ndimensiune fisier: %ld ", litereMici, litereMari, cifre, aparitii, st.st_size);

  if (write(fd2, statistica, size) == -1) {
        perror("Eroare fisier output");
    }
  
  close(fd);
  close(fd2);

 return 0;
}
