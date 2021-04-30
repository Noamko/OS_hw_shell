#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#define NEWFILE (O_WRONLY | O_CREAT | O_TRUNC )
#define MODE600 (S_IRUSR | S_IWUSR)
#define SIZE 10

void fileCopy(char* inPath, char* outPath)
{
	char buf[SIZE];
	int in,out,count;
	
	if((in=open(inPath,O_RDONLY))==-1)
	{
		printf("error\n");
		exit(0);
	}
	if((out=open(outPath,NEWFILE,MODE600))==-1)
	{
		printf("error\n");
		exit(0);
	}
	while((count=read(in,buf,sizeof(buf)))>0)
	{
		if(write(out,buf,count)!=count)
		{
			printf("error\n");
			exit(0);
		}
	}
	if(count==-1)
	{
		printf("error\n");
		exit(0);
	}
	close(in);
	close(out);
}


int main()
{
	fileCopy("in.txt","out.txt");
}
