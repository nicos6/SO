#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){

  if(argc != 2){
    printf("Numar gresit de argumente\n");
    exit(-1);
  }

  int grupa = atoi(argv[1]);

  switch(grupa){
  case 1:
    printf("Ion\nVasile\nCostel\n");
    break;
  case 2:
    printf("Ana\nDan\nRadu");
  case 3:
    printf("Nico\nEli\nCosmina\n");
    break;
  default:
    printf("Nu ati introdus o grupa existenta");
  }
  
  return 0;
}
