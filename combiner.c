#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<fcntl.h>


int main(int argc, char* argv[])
{

	int fds[2];
	int pipecheck =	pipe(fds);
	if(pipecheck == -1)
		perror("Broken Pipe");
	pid_t notchild = fork();
	if(notchild < 0)
		perror("Fork failed");

	else if(!notchild)
	{
		close(fds[0]);
		dup2(fds[1],1);
		execlp("./map","./map", argv[1], NULL);
	}
	
	else if(notchild > 0)
	{
		pid_t notchild2 = fork();
		if(!notchild2)
		{
			close(fds[1]);
			dup2(fds[0], 0);	
			execlp("./reduce","./reduce",NULL);
			
		}
		else
		{
			waitpid(notchild, NULL);
			close(fds[1]);
			waitpid(notchild2, NULL);
			close(fds[0]);
		}
	}
	

	return 0;
}
