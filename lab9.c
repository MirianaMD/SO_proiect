#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/wait.h>
#include<stdlib.h>

int main(int argc, char *argv[])
{
  if(argc!=2)
    {
      printf("nr argumente incorect");
      exit(1);
    }
  char *caracterul=argv[1];
  int pid;
  int pfd[2], pfd2[2];
  if(pipe(pfd)==-1)
    {
      perror("error pipe");
      exit(-1);
    }
  if(pipe(pfd2)==-1)
    {
      perror("error pipe2");
      exit(-1);
    }
  pid = fork();
  if(pid == -1)
    {
      perror("error fork");
      exit(-1);
    }
  if(pid==0)
    {
      close(pfd[1]);
      //dup2(pfd[0],0);
      close(pfd[0]);
      execlp("bash","bash","s.sh",caracterul,NULL);
      perror("execlp\n");
      //exit(1);
    }
  close(pfd[0]);
  close(pfd[1]);
  int status;
  while((pid=wait(&status))!=-1)
    {
      if(WIFEXITED(status))
	{
	  printf("proces incheiat\n");
	}
    }
  return 0;
}