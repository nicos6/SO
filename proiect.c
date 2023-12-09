#include <stdio.h>
#include <stdlib.h>   //u_int32_t
#include <unistd.h>   //close, read, write, fseek
#include <fcntl.h>    //oflag
#include <sys/stat.h> //pt stat
#include <time.h>     //pentru functia ctime
#include <dirent.h>
#include <string.h>
#include <sys/wait.h>
#include <stdint.h>

#define STATISTICA_SIZE 256

void verificare_argument(char *argument){
  struct stat infos;
  if (stat(argument, &infos) == -1)
  {
    perror("Eroare functia stat");
    exit(-1);
  }

  if (S_ISDIR(infos.st_mode) == 0)
  {
    printf("Parametrul transmis nu este director\n");
    exit(-1);
  }
}

DIR *deschidere_director(char *director_input){
  DIR *director = NULL;
  if ((director = opendir(director_input)) == NULL)
  {
    printf("Eroare deschidere director");
    exit(-1);
  }
  return director;
}

void write_in_file(int fd, char *statistica, int size){

  if (write(fd, statistica, size) == -1)
  {
    perror("Eroare scriere fisier output");
    exit(-1);
  }
}

void close_file(int fd){

  if (close(fd) == -1)
  {
    printf("Eroare la inchiderea fisierului");
    exit(-1);
  }
}

char *drepturi(struct stat infos){
  mode_t mode = infos.st_mode;
  char *drepturi = (char *)malloc(100 * sizeof(char));

  if (drepturi == NULL){
    printf("Eroare alocare dinamica drepturi");
    exit(-1);
  }

  sprintf(drepturi, "Drepturi de acces user: %c%c%c\nDrepturi de acces grup: %c%c%c\nDrepturi de acces altii: %c%c%c\n", ((mode & S_IRUSR) ? 'r' : '-'), ((mode & S_IWUSR) ? 'w' : '-'), ((mode & S_IXUSR) ? 'x' : '-'), ((mode & S_IRGRP) ? 'r' : '-'), ((mode & S_IWGRP) ? 'w' : '-'), ((mode & S_IXGRP) ? 'x' : '-'), ((mode & S_IROTH) ? 'r' : '-'), ((mode & S_IWOTH) ? 'w' : '-'), ((mode & S_IXOTH) ? 'x' : '-'));

  return drepturi;
}

char *creare_pathName(char *director_output, char *name){
  // cream pathname-ul pentru fisierul in care se va scrie
  int size_fileName = strlen(director_output) + 1 + strlen(name) + strlen("_statistica.txt") + 1; // 1 pt '/' si 1 pt \0 de la final
  char *fileName = malloc(size_fileName);

  if (fileName == NULL){
    perror("Eroare alocare memorie dinamic");
    exit(-1);
  }

  snprintf(fileName, size_fileName, "%s/%s_statistica.txt", director_output, name);

  return fileName;
}

int nrLinii(char *fileName){

  int fd;
  if ((fd = open(fileName, O_RDONLY)) == -1)
  {
    perror("Eroare deschidere fisier de statistica din director output");
    exit(-1);
  }

  int size = 0;
  int count = 0;
  char buffer[STATISTICA_SIZE];
  while ((size = read(fd, buffer, STATISTICA_SIZE)) > 0){
    for (int i = 0; i < size; i++)
      if (buffer[i] == '\n'){
        count++;
      }
  }

  close_file(fd);

  return count;
}

int statistica_regularFile(char *director_output, struct stat infos, char *name){

  char *fileName = creare_pathName(director_output, name);

  int fd = 0;
  if ((fd = creat(fileName, S_IRUSR | S_IWUSR)) == -1){
    perror("Eroare creare fisier statistica");
    exit(-1);
  }

  int size = 0;
  char statistica[STATISTICA_SIZE];
  size = sprintf(statistica, "Nume fisier obisnuit: %s\nDimensiune: %ld\nIdentificatorul utilizatorului: %d\n", name, infos.st_size, infos.st_uid);
  write_in_file(fd, statistica, size);

  char last_modified[11];
  strftime(last_modified, sizeof(last_modified), "%d.%m.%Y", localtime(&infos.st_mtime));
  size = sprintf(statistica, "Timpul ultimei modificari: %s\nContorul de legaturi: %ld\n", last_modified, infos.st_nlink);
  write_in_file(fd, statistica, size);

  size = sprintf(statistica, "%s", drepturi(infos));
  write_in_file(fd, statistica, size);
  close_file(fd);

  int count = nrLinii(fileName);
  return count;
}

int statistica_bmpFile(char *director_output, struct stat infos, char *name, char *path){

  // deschidem imaginea
  int fd1;
  if ((fd1 = open(path, O_RDONLY)) == -1){
    perror("Eroare deschidere fisier bmp");
    exit(-1);
  }

  int32_t height = 0, width = 0;
  lseek(fd1, 18, SEEK_SET);

  if (read(fd1, &height, 4) == -1){
    perror("Eroare citire inaltime fisier bmp");
  }

  if (read(fd1, &width, 4) == -1){
    perror("Eroare citire inaltime fisier bmp");
  }
  close_file(fd1);

  char *fileName = creare_pathName(director_output, name);

  int fd = 0;
  if ((fd = creat(fileName, S_IRUSR | S_IWUSR)) == -1){
    perror("Eroare creare fisier statistica");
    exit(-1);
  }

  int size = 0;
  char statistica[STATISTICA_SIZE];
  size = sprintf(statistica, "Nume fisier obisnuit: %s\n", name);
  write_in_file(fd, statistica, size);

  size = sprintf(statistica, "Inaltime: %d\nLatime: %d\n", height, width);
  write_in_file(fd, statistica, size);

  size = sprintf(statistica, "Dimensiune: %ld\nIdentificatorul utilizatorului: %d\n", infos.st_size, infos.st_uid);
  write_in_file(fd, statistica, size);

  char last_modified[11];
  strftime(last_modified, sizeof(last_modified), "%d.%m.%Y", localtime(&infos.st_mtime));
  size = sprintf(statistica, "Timpul ultimei modificari: %s\nContorul de legaturi: %ld\n", last_modified, infos.st_nlink);
  write_in_file(fd, statistica, size);

  size = sprintf(statistica, "%s", drepturi(infos));
  write_in_file(fd, statistica, size);
  close_file(fd);

  int count = nrLinii(fileName);
  return count;
}

int statistica_symbolicLink(char *director_output, char *path, char *name, struct stat infos){

  char *fileName = creare_pathName(director_output, name);

  int fd = 0;
  if ((fd = creat(fileName, S_IRUSR | S_IWUSR)) == -1)
  {
    perror("Eroare creare fisier statistica");
    exit(-1);
  }

  struct stat infoTarget;
  if (stat(path, &infoTarget) == -1)
  { // infos despre fisierul target
    perror("Eroare functia stat sym");
    exit(-1);
  }

  int size = 0;
  char statistica[STATISTICA_SIZE];
  size = sprintf(statistica, "Nume legatura simbolica: %s\nDimensiune: %ld\nDimensiune fisier target: %ld\n%s", name, infos.st_size, infoTarget.st_size, drepturi(infos));
  write_in_file(fd, statistica, size);
  close_file(fd);

  int count = nrLinii(fileName);
  return count;
}

int statistica_director(char *director_output, struct stat infos, char *name){
  char *fileName = creare_pathName(director_output, name);

  int fd = 0;
  if ((fd = creat(fileName, S_IRUSR | S_IWUSR)) == -1)
  {
    perror("Eroare creare fisier statistica");
    exit(-1);
  }

  int size = 0;
  char statistica[STATISTICA_SIZE];
  size = sprintf(statistica, "Nume director: %s\nIdentificatorul userului: %d\n%s", name, infos.st_uid, drepturi(infos));
  write_in_file(fd, statistica, size);
  close(fd);

  int count = nrLinii(fileName);
  return count;
}

void greyscale(char *fisier){

  int fd;
  if ((fd = open(fisier, O_RDWR)) == -1){
    perror("Eroare deschidere fisier bmp");
    exit(-1);
  }

  lseek(fd, 10, SEEK_SET);
  int dataOffset = 0;
  if (read(fd, &dataOffset, 4) == -1){
    perror("Eroare citire data offset fisier bmp");
    exit(-1);
  }

  lseek(fd, 14, SEEK_CUR);
  uint16_t bitCount = 0; // bits per pixel
  if (read(fd, &bitCount, 2) == -1)
  {
    perror("Eroare citire bit count fisier bmp");
    exit(-1);
  }

  int32_t height = 0, width = 0;
  lseek(fd, 18, SEEK_SET);

  if (read(fd, &height, 4) == -1){
    perror("Eroare citire inaltime fisier bmp");
  }

  if (read(fd, &width, 4) == -1){
    perror("Eroare citire inaltime fisier bmp");
  }

  int imageSize = height * width * bitCount / 8;

  if (bitCount > 8)
  {
    unsigned char pixel[imageSize];

    // sarim la pixel data
    lseek(fd, dataOffset, SEEK_SET);

    if (read(fd, pixel, imageSize) == -1)
    {
      perror("Eroare citire pixeli imagine 24bpp\n");
      exit(-1);
    }

    for (int i = 0; i < imageSize; i += 3)
    {
      // se citesc in ordinea blue, green, red
      unsigned char greyPixel = 0.299 * pixel[i] + 0.587 * pixel[i + 1] + 0.114 * pixel[i + 2];
      pixel[i] = greyPixel;
      pixel[i + 1] = greyPixel;
      pixel[i + 2] = greyPixel;
    }
    lseek(fd, -imageSize, SEEK_CUR);

    if (write(fd, pixel, imageSize) == -1)
    {
      perror("Eroare scriere pixeli modificati\n");
      exit(-1);
    }
  }

  if (bitCount == 8)
  {
    int paletteSize = dataOffset - 54; // 54 bytes pana la color palette

    // color palette
    lseek(fd, 54, SEEK_SET);

    unsigned char colorPalette[paletteSize];
    if (read(fd, colorPalette, paletteSize) == -1)
    {
      perror("Eroare citire pixeli\n");
      exit(-1);
    }

    for (int i = 0; i < paletteSize; i += 4)
    {
      unsigned char grey = 0.299 * colorPalette[i] + 0.587 * colorPalette[i + 1] + 0.114 * colorPalette[i + 2];
      colorPalette[i] = colorPalette[i + 1] = colorPalette[i + 2] = grey;
    }

    lseek(fd, -paletteSize, SEEK_CUR);

    if (write(fd, colorPalette, paletteSize) == -1)
    {
      perror("Eroare scriere pixeli modificati\n");
      exit(-1);
    }
  }

  close_file(fd);
}

int main(int argc, char *argv[])
{

  if (argc != 4){
    printf("Numar gresit de argumente\n");
    exit(-1);
  }

  char *director_input = argv[1];
  char *director_output = argv[2];
  char caracter = argv[3][0];

  //verificam daca primele 2 argumente sunt directoare
  verificare_argument(director_input);
  verificare_argument(director_output);

  // deschidem directorul transmis ca parametru pentru a-l parcurge
  DIR *director = deschidere_director(director_input);

  struct stat infos;
  int nr = 0; // pt a numara propozitiile corecte cu scriptul
  struct dirent *entry;
  while ((entry = readdir(director)) != NULL)
  {
    if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) // ca sa nu creeze proces daca fisierul citit e .. sau ..
    {

      int pfd1[2];
      if (pipe(pfd1) < 0){
        perror("Eroare la crearea pipe-ului 1\n");
        exit(-1);
      }

      int pfd2[2];
      if (pipe(pfd2) < 0){
        perror("Eroare la crearea pipe-ului 2\n");
        exit(-1);
      }

      // cream proces copil pentru scrierea fisierelor statistica
      int pid;
      if ((pid = fork()) < 0)
      {
        perror("Eroare la crearea primului proces");
        exit(-1);
      }

      int size_path = strlen(director_input) + 1 + strlen(entry->d_name) + 1; // 1 pt '/' si 1 pt \0 de la final
      char *path = malloc(size_path);
      

      if (path == NULL){
        perror("Eroare alocare memorie dinamic");
        exit(-1);
      }

      // construim calea 
      snprintf(path, size_path, "%s/%s", director_input, entry->d_name);

      if (lstat(path, &infos) == -1)
      { // daca e legatura simbolica, ia inf despre ea, nu despre parinte
        perror("Eroare functia stat");
        exit(-1);
      }

      if (pid == 0)
      {
        close(pfd1[0]); // inchidem capatul de citire pt pipe1
        close(pfd2[0]); // inchidem capatul de citire pt pipe2
        close(pfd2[1]); // inchidem capatul de scriere pt pipe2

        int nrLinii = 0;
        if (S_ISREG(infos.st_mode) == 1)
        { // daca fisierul este obisnuit

          char *extensie = strchr(path, '.');

          if (extensie != NULL && strcmp(extensie, ".bmp") == 0)
          { // daca e bmp file

            nrLinii = statistica_bmpFile(director_output, infos, entry->d_name, path);
            close(pfd1[1]); // inchidere capat de scriere
          }
          else
          { // fisier obisnuit care nu e bmp

            nrLinii = statistica_regularFile(director_output, infos, entry->d_name);

            dup2(pfd1[1], 1); //redirectam iesirea standart catre pipe1
            execlp("cat", "cat", path, NULL); // executam comanda 'cat' pentru a citi continutul si a-l trimite prin pipe
            perror("Eroare execlp 'cat'");
            exit(EXIT_FAILURE);
          }
        }
        else if (S_ISLNK(infos.st_mode) == 1)
        {// daca e legatura simbolica
          close(pfd1[1]); // inchidere capat de scriere
          nrLinii = statistica_symbolicLink(director_output, path, entry->d_name, infos);
        }
        else if (S_ISDIR(infos.st_mode) == 1)
        {// daca fisierul este director
          close(pfd1[1]); // inchidere capat de scriere
          nrLinii = statistica_director(director_output, infos, entry->d_name);
        }
        exit(nrLinii);
      } // pid==0

      int pid2;
      if (S_ISREG(infos.st_mode) == 1)
      { // daca fisierul este obisnuit
        char *extensie = strchr(path, '.');

        if (extensie != NULL && strcmp(extensie, ".bmp") == 0)
        { // daca e bmp file
          
          if ((pid2 = fork()) < 0){ //proces pentru imaginile bmp pt a face greyscale
            perror("Eroare");
            exit(-1);
          }

          if (pid2 == 0)
          {
            close(pfd1[1]);
            close(pfd1[0]);
            close(pfd2[1]);
            close(pfd2[0]);
            greyscale(path);
            exit(0);
          }
          else{
            int status2;
            if (waitpid(pid2, &status2, 0) == -1){
            perror("Eroare waitpid\n");
            exit(-1);
            }

            if (WIFEXITED(status2)) {
              printf("Procesul copil cu pid2 %d s-a incheiat cu status=%d\n", pid2, WEXITSTATUS(status2));
            }
            else if (WIFSIGNALED(status2)){
              printf("killed by signal %d\n", WTERMSIG(status2));
            }
          }
        }

        else
        { // fisier obisnuit care nu e bmp
          int pid3;
          if ((pid3 = fork()) < 0){
            perror("Eroare");
            exit(-1);
          }

          if (pid3 == 0)
          {
            close(pfd1[1]); /* inchidem capatul de scriere */
            close(pfd2[0]); /* inchidem capatul de citire; */

            dup2(pfd1[0], 0);
            dup2(pfd2[1], 1);
            execl("./script.sh", "script.sh", &caracter, NULL);
            perror("Eroare execl pid3\n");
            exit(-1);
          }
          else
          {
            close(pfd2[1]);
            close(pfd1[0]);
            close(pfd1[1]);
            FILE *stream;
            char string[100];
            stream = fdopen(pfd2[0], "r");
            if (fscanf(stream, "%s", string) == 1){
              nr += atoi(string);
            }
            else{
              perror("Eroare la citirea din pipe în copil");
            }

            close(pfd2[0]); //inchidem si capatul de citire

            int status3;
            if (waitpid(pid3, &status3, 0) == -1){
              perror("Eroare waitpid\n");
              exit(-1);
            }

            if (WIFEXITED(status3)){
              printf("Procesul copil cu pid3 %d s-a incheiat cu status=%d\n", pid3, WEXITSTATUS(status3));
            }
            else if (WIFSIGNALED(status3)){
              printf("killed by signal %d\n", WTERMSIG(status3));
            }
          }
        }
      }

      int status;
      if (wait(&status) == -1){
        perror("Eroare wait\n");
        exit(-1);
      }

      if (WIFEXITED(status)){
        printf("Procesul copil cu pid1 %d s-a incheiat cu status=%d\n", pid, WEXITSTATUS(status));
      }
      else if (WIFSIGNALED(status)){
        printf("killed by signal %d\n", WTERMSIG(status));
      }
      else if (WIFSTOPPED(status)){
        printf("stopped by signal %d\n", WSTOPSIG(status));
      }
      else if (WIFCONTINUED(status)){
        printf("continued\n");
      }

    free(path);
    }// if its not . or ..
  }// while

  printf("Au fost identificate in total %d propozitii corecte care contin caracterul %c\n", nr, caracter);

  closedir(director);
  return 0;
}