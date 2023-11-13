#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <time.h>

char* permissions(char* text, mode_t mode){
  char nts[100]={};
  strcat(text,"drepturi de acces user: ");
  sprintf(nts,"%s%s%s\n", mode & S_IRUSR ? "R" : "-",mode & S_IWUSR ? "W" : "-",mode & S_IXUSR ? "X" : "-");
  strcat(text,nts);
  strcat(text,"drepturi de acces grup: ");
  sprintf(nts,"%s%s%s\n", mode & S_IRGRP ? "R" : "-",mode & S_IWGRP ? "W" : "-",mode & S_IXGRP ? "X" : "-");
  strcat(text,nts);
  strcat(text,"drepturi de acces altii: ");
  sprintf(nts,"%s%s%s\n", mode & S_IROTH ? "R" : "-",mode & S_IWOTH ? "W" : "-",mode & S_IXOTH ? "X" : "-");
  strcat(text,nts);
  strcat(text,"\n");
  return text;
}

void statisticile(char *file_name, char* text)
{
  //STATISTICILE
  char* out = file_name;
  struct stat st_st;
  if(stat(out,&st_st) == -1)
    {
      perror("out(statistici.txt) stat error");
      exit(1);
    }
  int out_fd;
  out_fd=open(out, O_WRONLY | O_APPEND);
  if(out_fd == -1)
    {
      perror("error open statisitci.txt in modul de scriere ");
      exit(1);
    }
  if(write(out_fd,text,strlen(text))==-1)
    {
      perror("error scriere file");
      exit(1);
    }
  if(close(out_fd)==-1)
    {
      perror("error close statistici");
      exit(1);
    }
}


void cerinte_fisier(char* path, char* nume,struct stat *st_file)//.bmp+regulat sau regulat
{
  int dimName = strlen(nume);
  char text[1000] = {};
  //printf("%s",nume_fisier + dimName - 4);//extensia
    
  //numele
  strcpy(text,"nume fisier: ");
  strcat(text,nume);
  strcat(text,"\n");
  
  //dimenisuni poza
  char nts[100]={};//num to str
  if(S_ISREG(st_file->st_mode) && strcmp(nume + dimName - 4, ".bmp")==0)
    {
      char *file=path;//poza pt dimensiuni
      int fd;
      fd=open(file,O_RDONLY);
      if(fd==-1)
	{
	  perror("error open poza");
	  exit(1);
	}
      int lungime;
      int inaltime;

      lseek(fd, 18, SEEK_SET);

      if(read(fd,&inaltime, sizeof(int))!=sizeof(int))
	{
	  perror("error read");
	  if(close(fd)==-1)
	    {
	      perror("error close");
	      exit(1);
	    }
	  exit(1);
	}
      if(read(fd,&lungime, sizeof(int))!=sizeof(int))
	{
	  perror("error read");
	  if(close(fd)==-1)
	    {
	      perror("error close");
	      exit(1);
	    }
	  exit(1);
	}
      strcat(text,"inaltime: ");
      sprintf(nts, "%d\n", inaltime);
      strcat(text,nts);
      strcat(text,"lungime: ");
      sprintf(nts, "%d\n", lungime);
      strcat(text,nts);
      if(close(fd)==-1)
	{
	  perror("error close poza");
	  exit(1);
	}
    }

  //dimensiune poza, fisi reg
  strcat(text,"dimensiune: ");
  sprintf(nts, "%ld\n", st_file->st_size);
  strcat(text,nts);

  //identificatorul utilizatorului pt poza+regulat, regulat
  strcat(text,"identificatorul utilizatorului: ");
  sprintf(nts, "%d\n", st_file->st_uid);
  strcat(text,nts);

  //timp pt poza, orice fisi reg
  strcat(text,"timpul ultimei modificari: ");
  sprintf(nts, "%s", ctime(&st_file->st_mtime));
  strcat(text,nts);
  
  //nr legaturi
  strcat(text,"contorul de legaturi: ");
  sprintf(nts, "%ld\n", st_file->st_nlink);
  strcat(text,nts);
  
  //drepturi
  permissions(text, st_file->st_mode);

  //statisticile
  statisticile("statistici.txt",text);
}

void cerinte_legatura(char* path, char* nume,struct stat *st_file, struct stat *st_leg)
{
  char text[1000] = {};
  char nts[100]={};
  
  //numele
  strcpy(text,"nume legatura: ");
  strcat(text,nume);
  strcat(text,"\n");
  
  //dim legat
  strcat(text,"dimensiune: ");
  sprintf(nts, "%ld\n", st_leg->st_size);
  strcat(text,nts);
  
  //dim target file
  strcat(text,"dimensiune fisier target: ");
  sprintf(nts, "%ld\n", st_file->st_size);
  strcat(text,nts);
  
  //drepturi
  permissions(text, st_leg->st_mode);
  
  //statisticile
  statisticile("statistici.txt",text);
}

void cerinte_director(char* nume,struct stat *st_file)
{
  char text[1000] = {};
  char nts[100]={};
    
  //numele
  strcpy(text,"nume director: ");
  strcat(text,nume);
  strcat(text,"\n");
  
  //identificatorul utilizatorului
  strcat(text,"identificatorul utilizatorului: ");
  sprintf(nts, "%d\n", st_file->st_uid);
  strcat(text,nts);
  
  //drepturi
  permissions(text, st_file->st_mode);

  //statisticile
  statisticile("statistici.txt",text);
}

void citire_director(char *director)
{
  DIR *dir = NULL;
  dir=opendir(director);
  if(dir == NULL)
    {
      perror("error open director");
      exit(1);
    }
  struct dirent *entry = NULL;//in structura aceasta vom salva datele despre intrarile din folder
  while((entry=readdir(dir)) != NULL)//cat timp readdir returneaza o intrare din folder
    {
      //sarim peste intrarile '.' si '..'
      if(strcmp(entry->d_name,"..") != 0 && strcmp(entry->d_name,".") != 0)
	{
	  char path[500] = {};
	  //sprintf ca sa construim calea
	  //sprintf(path,"%s/%s",director,entry->d_name); sau
	  strcat(path,director);
	  strcat(path,"/");
	  strcat(path,entry->d_name);
	  	  
	  struct stat st_file;
	  if(stat(path,&st_file) == -1)
	    {
	      perror("stat error");
	      exit(1);
	    }
	  
	  struct stat st_leg;
	  if(lstat(path,&st_leg) == -1)
	    {
	      perror("stat legatura error");
	      exit(1);
	    }
	  if(S_ISDIR(st_file.st_mode))//verificam daca e director
	    {
	      cerinte_director(entry->d_name, &st_file);
	      citire_director(path);
	    }
	  else
	    {
	      if(entry->d_type == DT_LNK)//verificam daca e legatura simbolica
		{
		  cerinte_legatura(path,entry->d_name, &st_file, &st_leg);
		  continue;//ca sa nu mi mai trateze legatura si ca fisier
		}
	      if(S_ISREG(st_file.st_mode) || (S_ISREG(st_file.st_mode) && strcmp(entry->d_name + strlen(entry->d_name) - 4, ".bmp")==0))//verificam daca e fisier regulat/ fisier regulat+bmp
		{
		  cerinte_fisier(path, entry->d_name, &st_file);
		}
	    }
	}
    }
  if (closedir(dir) == -1)
    {
      perror("error close director");
      exit(1);
    }
}

int main(int argc, char *argv[])
{
  if(argc!=2)//verificam daca am primit cele doua argumente, nu mai multe, nu mai putine
    {
      printf("nr argumente incorect");
      exit(1);
    }

  char *director=argv[1];

  citire_director(director);
  
  return 0;
}