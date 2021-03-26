//Noam Koren 308192871

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#define PROMPT "$ "

void user_input_loop(){
	char* input = malloc(sizeof(char) * 10);

	while(1){
		printf("%s", PROMPT);
		scanf("%s",input);
	}
}
int main(int argc, char* argv[])
{
	char** command_history
	user_input_loop();
	return 0;
}