#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>

int main(int argc, char* argv[])

{
	char print[50], ch;
	
	int i = 0, j = 0;
	
	
	FILE *input = fopen(argv[1],"r");
	
	
	while(fscanf(input,"%s",print)!=EOF)   
	   	{	
			printf("(%s, 1)\n",print);
	    	}
	
	
	fclose(input);
	
	exit(1);
}
