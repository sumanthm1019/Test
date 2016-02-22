#include<stdio.h>
#include<string.h>
#include<stdlib.h>

int main(int argc, char* argv[])

{	
	char list[100][100];
	char string[100];
	char reading[1], first;
	char ch;
	char buffer[1000];
	int i = 0, j = 0, k = 0, l = 0, m = 0;
	int counter[100];
	int flag = 0;
	int lengthoflist = 0;
	int pipefd[2];
			
	while(1)
	{
		ch = getchar();
		
		if (isalpha(ch) || ch == ',' || ch == EOF)
		{
			if(l == 0)
				first = ch;
			l++;
			if (i == 0)
			{
				if(ch != first || ch ==  EOF)
				{
					for(k = 0; k < lengthoflist;k++)
						printf("(%s, %d)\n", list[k], counter[k]);
					j = 0;
					lengthoflist = 0;
					first = ch;
					if(ch == EOF)
						exit(1);
				}
			}
			if (ch == ',')
				string[i] = '\0';
			else 
				string[i] = ch;
			i++;
		}
		if(ch == ',')
		{
			flag = 0;
			for(k = 0; k < lengthoflist; k++)
			{
				if (strcmp(string, list[k]) == 0)
				{
					counter[k]++;
					flag = 1;
					break;
				}			
			}
			if(!flag)
			{
				strcpy(list[lengthoflist], string);
				counter[lengthoflist++] = 1;
			}	
			j++;
			i = 0;
		}

	}
exit(1);					
				

}
