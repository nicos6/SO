#include <stdio.h>
#include <stdint.h>
#include <stdlib.h> //u_int32_t
#include <unistd.h> //close, read, write, fseek
#include <fcntl.h> //oflag
#include <sys/stat.h> //pt stat
#include <time.h> //pentru functia ctime

int open_file(char *file){

  int fd = 0;
  if((fd = open(file, O_RDONLY)) == -1){
    perror("Eroare deschidere fisier");
    exit(-1);
  }

  return fd;
}

int32_t read_from_file(int fd, int size){

  int32_t info;
  if(read(fd, &info, size) == -1){
    perror("Eroare citire inaltime fisier bmp");
  }

  return info;
}

void close_file(int fd){

  if( close(fd) == -1){
    printf("Eroare la inchiderea fisierului");
    exit(-1);
  }
}

int main(int argc, char *argv[]){
  if(argc != 2){
    printf("Numar gresit de argumente\n");
    exit(-1);
  }
  
  char *file_input = argv[1];
  
  struct stat infos;
  if(stat(file_input, &infos) == -1){
    perror("Eroare functia stat");
    exit(-1);
  }
  
  if(S_ISREG(infos.st_mode) == 0){
    printf("Tip fisier gresit\n");
    exit(-1);
  }

  int fd1 = open_file(file_input);
  
  int32_t height=0, width=0;
  lseek(fd1, 18, SEEK_SET); //punem cursorul la informatiile necesare, dupa 18B (la inaltime si latime)
  height = read_from_file(fd1, 4);
  width = read_from_file(fd1, 4);

  int fd2;
  if( (fd2 = creat ("statistica.txt", S_IRUSR | S_IWUSR)) == -1){
    perror("Eroare creare fisier statistica");
    exit(-1);
  }

  char last_modified[11];
  strftime(last_modified, sizeof(last_modified), "%d.%m.%Y", localtime(&infos.st_mtime));

  mode_t mode = infos.st_mode;
  char statistica[300];
  int size=0;
  size = sprintf(statistica, "Nume fisier: %s\nInaltime: %d\nLatime: %d\nDimensiune: %ld\nIdentificatorul utilizatorului: %d\nTimpul ultimei modificari: %s\nContorul de legaturi: %ld\nDrepturi de acces user: %c%c%c\nDrepturi de acces grup: %c%c%c\nDrepturi de acces altii: %c%c%c", file_input, height, width, infos.st_size, infos.st_uid, last_modified, infos.st_nlink, ((mode & S_IRUSR) ? 'r' : '-'), ((mode & S_IWUSR) ? 'w' : '-'), ((mode & S_IXUSR) ? 'x' : '-'), ((mode & S_IRGRP) ? 'r' : '-'), ((mode & S_IWGRP) ? 'w' : '-'), ((mode & S_IXGRP) ? 'x' : '-'), ((mode & S_IROTH) ? 'r' : '-'), ((mode & S_IWOTH) ? 'w' : '-'), ((mode & S_IXOTH) ? 'x' : '-'));

  if (write(fd2, statistica, size) == -1) {
        perror("Eroare scriere fisier output");
  }
  
  close_file(fd1);
  close_file(fd2);

  return 0;
}

