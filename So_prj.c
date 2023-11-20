#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <sys/wait.h>

void statisticile(char *file_name, char* text,char *director_out)
{
  //STATISTICILE
  DIR *dir = NULL;
  dir=opendir(director_out);
  if(dir == NULL)
    {
      perror("error open director scriere ");
      exit(1);
    }
  
  char numele[500]={};
  strcpy(numele,director_out);
  strcat(numele,"/");
  strcat(numele,file_name);
  strcat(numele,"_statistica.txt");
  char* out = numele;
  int out_fd;
  out_fd=open(out, O_WRONLY | O_CREAT | O_TRUNC, S_IWUSR | S_IWGRP | S_IWOTH | S_IRUSR | S_IRGRP | S_IROTH);
  if(out_fd == -1)
    {
      perror("error open statisitci.txt in modul de scriere / citire");
      exit(1);
    }
  struct stat st_st;
  if(stat(out,&st_st) == -1)
    {
      perror("out(statistici.txt) stat error");
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
  if (closedir(dir) == -1)
    {
      perror("error close director");
      exit(1);
    }
}

int creez_proc()
{
  int pid = fork();
  if (pid < 0)
    {
      perror("Eroare fork!");
      exit(-1);
    }
  return pid;
}

void asteapt_procesul(char *director_out)
{
  int status, pid;
  pid = wait(&status);

  if (pid == -1)
    {
      perror("Eroare la asteptarea procesului!");
      exit(-1);
    }

  if(WIFEXITED(status))
    {
      printf("S-a incheiat procesul cu pid-ul %d si codul %d\n",pid,WEXITSTATUS(status));//putem pune in comentariu asta ca textul oricum se scrie in fisier
      char text[500]={}, nts[100]={};
      sprintf(nts, "S-a incheiat procesul cu pid-ul %d", pid);
      strcat(text,nts);
      strcat(text," si codul ");
      sprintf(nts, "%d\n", WEXITSTATUS(status));
      strcat(text,nts);

      char numele[500]={};
      strcpy(numele,director_out);
      strcat(numele,"/");
      strcat(numele,"statistica.txt");
      char* out =numele;
      int out_fd;
      out_fd=open(out, O_WRONLY | O_CREAT | O_APPEND, S_IWUSR | S_IWGRP | S_IWOTH | S_IRUSR | S_IRGRP | S_IROTH);
      if(out_fd == -1)
	{
	  perror("error open statisitci.txt in modul de scriere / citire");
	  exit(1);
	}
      struct stat st_st;
      if(stat(out,&st_st) == -1)
	{
	  perror("out(statistici.txt) stat error");
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
  else
    {
      printf("process cu pid-ul %d nu s-a terminat corect\n",pid);
    }  
}

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
  return text;
}

void cerinte_fisier(char* path, char* nume,struct stat *st_file,char *director_out)//.bmp+regulat sau regulat
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
  statisticile(nume, text, director_out);
}

void cerinte_legatura(char* path, char* nume,struct stat *st_file, struct stat *st_leg, char *director_out)
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
  statisticile(nume, text, director_out);
}

void cerinte_director(char* nume,struct stat *st_file, char *director_out)
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
  statisticile(nume, text, director_out);
}


void creez_proces_pix(char* nume, char *path, char *director_out)
{    
  int pid = creez_proc();
  if (pid == 0)
    {
      char numele[500]={};
      strcpy(numele,path);
      strcat(numele,"/");
      strcat(numele,nume);
      char *file=path;
      int fd;
      fd=open(file,O_RDWR);
      if(fd==-1)
	{
	  perror("error deschidere poza la colorare");
	  exit(1);
	}
      int lungime;
      int inaltime;;
      
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
      int nrpix=lungime*inaltime;
      char Header[54];
      if (read(fd, Header, sizeof(Header)) != sizeof(Header)) {
        perror("error citire a header-ului pozei");
        exit(1);
      }
      for(int i=0;i<nrpix;i++)
	{
	  unsigned char ro_gal_albas[3];
	  if(read(fd,ro_gal_albas,sizeof(ro_gal_albas))!=sizeof(ro_gal_albas))
	    {
	      perror("error citire pixels");
	      exit(1);
	    }
	  unsigned char gri=0.299*ro_gal_albas[0]+0.587*ro_gal_albas[1]+0.114*ro_gal_albas[2];
	  lseek(fd,-3,SEEK_CUR);
	  write(fd,&gri,sizeof(gri));
	  write(fd,&gri,sizeof(gri));
	  write(fd,&gri,sizeof(gri));
	}

      if(close(fd)==-1)
	{
	  perror("error close poza la colorare");
	  exit(1);
	}
      
      exit(-1);
    }
  else
    {
      asteapt_procesul(director_out);
    }
}


void creez_proces(struct dirent *entry, char* path, char* nume,struct stat *st_file, struct stat *st_leg, char *director_out)
{
  int pid = creez_proc();

  if (pid == 0)
    {
      if(S_ISDIR(st_file->st_mode))//verificam daca e director
	{
	  cerinte_director(nume, st_file, director_out);
	}
      else
	{
	  if(entry->d_type == DT_LNK)//verificam daca e legatura simbolica
	    {
	      cerinte_legatura(path, nume, st_file, st_leg, director_out);
	    }
	  if(entry->d_type != DT_LNK && (S_ISREG(st_file->st_mode) || (S_ISREG(st_file->st_mode) && strcmp(entry->d_name + strlen(entry->d_name) - 4, ".bmp")==0)))//verificam daca e fisier regulat/ fisier regulat+bmp
	    {
	      if(strcmp(entry->d_name + strlen(entry->d_name) - 4, ".bmp")==0)
		{
		  creez_proces_pix(nume, path, director_out);
		}
	      cerinte_fisier(path, nume, st_file, director_out);
	    }
	}
      exit(-1);
    }
  else
    {
      asteapt_procesul(director_out);
    }
}

void citire_director(char *director_in, char *director_out)
{
  DIR *dir = NULL;
  dir=opendir(director_in);
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
	  strcat(path,director_in);
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
	      creez_proces(entry,path,entry->d_name, &st_file, &st_leg, director_out);
	      citire_director(path, director_out);
	    }
	  else
	    {
	      creez_proces(entry, path,entry->d_name, &st_file, &st_leg, director_out);
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
  if(argc!=3)//verificam daca am primit cele trei argumente, nu mai multe, nu mai putine
    {
      printf("nr argumente incorect");
      exit(1);
    }

  char *director_in=argv[1];
  char *director_out=argv[2];

  citire_director(director_in, director_out);
  
  return 0;
}