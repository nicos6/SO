#include <stdio.h>
#include <stdlib.h>   //u_int32_t
#include <unistd.h>   //close, read, write, fseek
#include <fcntl.h>    //oflag
#include <sys/stat.h> //pt stat
#include <time.h>     //pentru functia ctime
#include <dirent.h>
#include <string.h>

void close_file(int fd){

  if( close(fd) == -1){
    printf("Eroare la inchiderea fisierului");
    exit(-1);
  }
}

void write_in_file(int fd, char *statistica, int size){

  if (write(fd, statistica, size) == -1){
    perror("Eroare scriere fisier output");
    exit(-1);
  }
}

char *drepturi(struct stat infos){
  mode_t mode = infos.st_mode;
  char *drepturi = (char *)malloc(100 * sizeof(char));

    if (drepturi == NULL) {
        printf("Eroare alocare dinamica drepturi");
        exit(-1);
    }

  sprintf(drepturi, "Drepturi de acces user: %c%c%c\nDrepturi de acces grup: %c%c%c\nDrepturi de acces altii: %c%c%c", ((mode & S_IRUSR) ? 'r' : '-'), ((mode & S_IWUSR) ? 'w' : '-'), ((mode & S_IXUSR) ? 'x' : '-'), ((mode & S_IRGRP) ? 'r' : '-'), ((mode & S_IWGRP) ? 'w' : '-'), ((mode & S_IXGRP) ? 'x' : '-'), ((mode & S_IROTH) ? 'r' : '-'), ((mode & S_IWOTH) ? 'w' : '-'), ((mode & S_IXOTH) ? 'x' : '-'));
  
  return drepturi;
}

int main(int argc, char *argv[])
{
  if (argc != 2){
    printf("Numar gresit de argumente\n");
    exit(-1);
  }

  char *file_input = argv[1];

  struct stat infos;
  if (stat(file_input, &infos) == -1){
    perror("Eroare functia stat");
    exit(-1);
  }

  if (S_ISDIR(infos.st_mode) == 0){
    printf("Parametrul transmis nu este director\n");
    exit(-1);
  }

  DIR *director = NULL;
  if ((director = opendir(file_input)) == NULL){
    printf("Eroare deschidere director");
    exit(-1);
  }

  int fd2;
  if ( (fd2 = creat("statistica.txt", S_IRUSR | S_IWUSR)) == -1){
    perror("Eroare creare fisier statistica");
  }

  struct dirent *entry;
  while ((entry = readdir(director)) != NULL){

    int size_path = strlen(file_input) + 1 + strlen(entry->d_name) + 1; //1 pt '/' si 1 pt \0 de la final
    char *path = malloc(size_path);

    if (path == NULL){
      perror("Eroare alocare memorie dinamic");
      exit(-1);
    }

    // calea absoluta
    snprintf(path, size_path, "%s/%s", file_input, entry->d_name);

    if (lstat(path, &infos) == -1){ //daca e legatura simbolica, ia inf despre ea, nu despre parinte
      perror("Eroare functia stat");
      exit(-1);
    }
    
    char statistica[500];

    if (S_ISREG(infos.st_mode) == 1){ // daca fisierul este obisnuit
      char *extensie = strchr(path, '.');

      char last_modified[11];
      strftime(last_modified, sizeof(last_modified), "%d.%m.%Y", localtime(&infos.st_mtime));

      if (extensie != NULL && strcmp(extensie, ".bmp") == 0){ // daca e bmp file

        // deschidem imaginea
        int fd1;
        if ((fd1 = open(path, O_RDONLY)) == -1){
          perror("Eroare deschidere fisier bmp");
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


        
        int size = 0;
        size = sprintf(statistica, "Nume fisier: %s\nInaltime: %d\nLatime: %d\nDimensiune: %ld\nIdentificatorul utilizatorului: %d\nTimpul ultimei modificari: %s\nContorul de legaturi: %ld\n%s\n\n", entry->d_name, height, width, infos.st_size, infos.st_uid, last_modified, infos.st_nlink, drepturi(infos));

        
        write_in_file(fd2, statistica, size);
      }
      
      else //fisier obisnuit care nu e bmp
	{
	  
	  int size = 0;
	  size = sprintf(statistica, "Nume fisier: %s\nDimensiune: %ld\nIdentificatorul utilizatorului: %d\nTimpul ultimei modificari: %s\nContorul de legaturi: %ld\n%s\n\n", entry->d_name, infos.st_size, infos.st_uid, last_modified, infos.st_nlink, drepturi(infos));
	  
	  write_in_file(fd2, statistica, size);
	}
    }
    else if (S_ISLNK(infos.st_mode) == 1) { // daca e legatura simbolica
	
      struct stat infoTarget;
      if (stat(path, &infoTarget) == -1){ //infos despre fisierul target
        perror("Eroare functia stat");
        exit(-1);
      }

      int size = 0;
      size = sprintf(statistica, "Nume legatura simbolica: %s\nDimensiune: %ld\nDimensiune fisier target: %ld\n%s\n\n", entry->d_name, infos.st_size, infoTarget.st_size, drepturi(infos));
	
      write_in_file(fd2, statistica, size); 
      }
    else if (S_ISDIR(infos.st_mode) == 1) { // daca fisierul este director
	    
	    int size = 0;
	    size = sprintf(statistica, "Nume director: %s\nIdentificatorul userului: %d\n%s\n\n", entry->d_name, infos.st_uid, drepturi(infos));
	
	    write_in_file(fd2, statistica, size);
    }
    
  free(path);
  }
  
  close_file(fd2);

  
  return 0;
}
