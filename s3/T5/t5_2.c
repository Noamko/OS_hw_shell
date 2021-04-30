#include <stdio.h>
#include <sys/fcntl.h>
#include <errno.h>
#include<stdlib.h>
#define SIZE 10 

main(int argc , char argv) 
{
	int fdin;           /* input file descriptor */ 
	int fdout;          /* out   file descriptor */ 
	char buf[SIZE+1]; /* input (output) buffer */
	int charsr;          /* how many chars were actually red */ 
	int charsw;          /* how many chars were actually written */ 


	fdin = open("dugma1.txt",O_RDONLY);
	if (fdin < 0) /* means file open did not take place */ 
	{                
		perror("after open ");   /* text explaining why */ 
		exit(-1); 
	}
	/* create the file with read-write premissions */
	fdout = open("dugma2.txt", O_CREAT | O_RDWR, 0666);  	
	if (fdout < 0)	/* means file open did not take place */ 
	{ 
		perror("after create ");   /* text explaining why */ 
		exit(-1); 
	}

	do 
	{
		charsr = read(fdin,buf,SIZE);
		/* why writing SIZE can be wrong...*/
		charsw = write(fdout,buf,charsr);		
		if (charsw < charsr) 
		{
			printf("error reading\n");
		} 
	}while ( (charsr == SIZE) && (charsw == SIZE));
	close(fdin);		/* free allocated structures */
	close(fdout);		/* free allocated structures */
}

