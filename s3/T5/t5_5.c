#include <stdlib.h> 
#include <stdio.h> 
#include <fcntl.h>
#include <sys/stat.h>

#define NEWFILE (O_WRONLY | O_CREAT | O_TRUNC )
#define MODE600 (S_IRUSR | S_IWUSR)
#define SIZE 10
#define BUF_SIZE 1024

int my_read(int fd,char *buf,size_t count)
{
	static char read_buf[BUF_SIZE];
	static int read_offset =BUF_SIZE;
	static int read_max=BUF_SIZE;
	int i;
	for(i=0;i<count;i++)
	{
		if(read_offset==read_max)
		{
			read_offset=0;
			read_max=read(fd,read_buf,sizeof(read_buf));

			if(read_max==-1)
				return -1;
			if(read_max==0)
				return i;
		}

		buf[i]=read_buf[read_offset++];
	}
	return i;
}

static char write_buf[BUF_SIZE];
static int write_offset = 0;
int my_write(int fd,char *buf,size_t count)
{
	int i,n;
	for(i=0;i<count;i++)
	{
		write_buf[write_offset++]=buf[i];

		if(write_offset==BUF_SIZE)
		{
			write_offset=0;
			n=write(fd,write_buf,sizeof(write_buf));

			if(n != BUF_SIZE)
				return -1;
		}
	}
	return i;
}

int write_close(int fd)
{
	if(write_offset>0)
		write(fd,write_buf,write_offset);

	write_offset=0;
	return close(fd);
}

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
	while((count=my_read(in,buf,sizeof(buf)))>0)
	{
		if(my_write(out,buf,count)!=count)
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
	write_close(out);
}


int main()
{
	fileCopy("in.txt","out.txt");
}
