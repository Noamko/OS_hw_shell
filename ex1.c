//Noam Koren 308192871

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define PROMPT "$ "
#define HISTORY_LEN 100
#define COM_LEN_LIM 20

typedef struct Command{
	char args[10][10];
	char status[8];
	int argc;
}Command;


Command* parse_command(char in[]){
	Command* c = malloc(sizeof(Command));
	int argc = 0;
	int len = strlen(in);
	for (int i = 0; i < len; i++) {
		if (in[i] == ' ') {
			argc++;
		}
		else {
			char k = in[i];
			strncat(c->args[argc], &k, 1);
		}
	}
	c->argc = argc + 1;
	strcpy(c->status,"RUNNING");
	return c;
}


void user_input_loop() {
	int history_counter = 0;
	Command* history[HISTORY_LEN];

	char working_directory[100];
	char prev_working_dir[100];
	strcpy(prev_working_dir,working_directory);

	getcwd(working_directory,sizeof(working_directory));

	while(1){

		printf("%s", PROMPT);
		fflush(stdout);
		
		char input[10];
		fgets(input, COM_LEN_LIM, stdin);
		if ((strlen(input) > 0) && (input[strlen(input) - 1] == '\n')) {
			input[strlen(input) - 1] = '\0';
		}
		Command* c = parse_command(input);
		history[history_counter++] = c;

		//BUILT IN COMMANDS
		if(!strcmp(c->args[0],"history")) {
			for(int i = 0; i < history_counter; i++) {
				printf("%s",history[i]->args[0]);
				fflush(stdout);
				printf("	%s\n", history[i]->status);
			}
		}

		if(!strcmp(c->args[0], "cd")){
			if(c->argc != 2) {
				printf("chdir failed\n");
			}
			else if(!strcmp(c->args[1],"..")) {
				for(int i = strlen(working_directory) -1; i > 0 != '/'; i--) {
					if(working_directory[i] == '/'){
						strcpy(prev_working_dir, working_directory);
						working_directory[i] = '\0';
						chdir(working_directory);
						break;
					}
				}
			}
			else if (!strcmp(c->args[1], "~")) {
				if(chdir(getenv("HOME")) == -1) {
					printf("chdir failed\n");
				}
				strcpy(prev_working_dir, working_directory);
				getcwd(working_directory, sizeof(working_directory));
			}
			else if(!strcmp(c->args[1],"-")) {
				if(chdir(prev_working_dir) == -1) {
					printf("chdir failed\n");
				}
				strcpy(prev_working_dir, working_directory);
				getcwd(working_directory, sizeof(working_directory));
			}

			else {
				if(chdir(c->args[1]) == -1) {
					printf("chdir failed\n");
				}
				strcpy(prev_working_dir, working_directory);
				getcwd(working_directory, sizeof(working_directory));
			}
		}

		else if (!strcmp(c->args[0], "exit")) {
			exit(0);
		}


		// if (c->name[strlen(c->name) -1] == '&' && c->name[strlen(c->name) -2] == ' '){
		// 	pid_t p = fork();
		// 	if(p == 0) {
		// 		execlp(c->name,c->name, NULL);
		// 	}
		// }
		// pid_t p = fork();
		// if(p == 0) {
		// 	execlp(c.name,c.name, NULL);
		// 	exit(0);
		// }
		// else{
		// 	wait(&c.status);
		// }
		strcpy(c->status, "DONE");
	}
}

int main(int argc, char* argv[])
{
	user_input_loop();
	return 0;
}