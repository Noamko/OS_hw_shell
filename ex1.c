//Noam Koren 308192871

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define PROMPT "$ "
#define HISTORY_LEN 100
#define MAX_COMMAND_LENGTH 100

typedef struct Command {
	char com[100];
	int* status;
	int background;
	int argc;
	pid_t pid;
}Command;


void runInBackground(pid_t pid, char* args[]) {
	if (pid == 0) {
		if(execvp(args[0], args) == -1){
			printf("%s\n", "exec failed");
		}
	}
}

void run(pid_t pid, char* args[]) {
	if (pid == 0) {
		if(execvp(args[0], args) == -1){
			printf("%s\n", "exec failed");
		}
	}
	else {
		wait(NULL);
	}
}

void user_input_loop() {
	int history_counter = 0;
	Command history[HISTORY_LEN];

	char working_directory[100];
	char prev_working_dir[100];
	strcpy(prev_working_dir, working_directory);
	getcwd(working_directory, sizeof(working_directory));

	while (1) {
		printf("%s", PROMPT);
		fflush(stdout);

		char input[MAX_COMMAND_LENGTH];
		// scanf("%s",input);
		fgets(input, MAX_COMMAND_LENGTH, stdin);
		if ((strlen(input) > 0) && (input[strlen(input) - 1] == '\n')) {
			input[strlen(input) - 1] = '\0';
		}

		Command c;
		c.background = 0;
		int argc = 0;
		char* token = strtok(input, " ");
		char* args[100];
		args[0] = token;
		strcpy(c.com, input);
		while (token != NULL) {
			argc++;
			token = strtok(NULL, " ");
			args[argc] = token;
			if(token != NULL){
				strcat(c.com, " ");
				strcat(c.com, token);
			}
		}
		c.argc = argc;
		if(!strcmp(args[argc -1], "&")) {
			c.background = 1;
			c.com[strlen(c.com) - 2] = '\0';
		}
		history[history_counter] = c;
		history_counter++;

		//BUILT IN COMMANDS
		if (!strcmp(c.com, "history")) {
			for (int i = 0; i < history_counter; i++) {

				// printf("%s : %d : %d\n", history[i].com, history[i].pid, waitpid(history[i].pid,NULL,WNOHANG));
				printf("%s", history[i].com);
				fflush(stdout);
				if ((waitpid(history[i].pid, NULL, WNOHANG) == 0) || i == history_counter-1) {
					printf(" RUNNING\n");
				}
				else if (waitpid(history[i].pid, NULL, WNOHANG) == -1 ) {
					printf(" DONE\n");
				}
			}
		}

		else if (!strcmp(args[0], "cd")) {
			if (c.argc > 2) {
				printf("Too many argument\n");
			}
			else if (!strcmp(args[1], "..")) {
				for (int i = strlen(working_directory); i > 0; i--) {
					if (working_directory[i] == '/') {
						strcpy(prev_working_dir, working_directory);
						working_directory[i] = '\0';
						chdir(working_directory);
						break;
					}
				}
			}
			else if (!strcmp(args[1], "~")) {
				if (chdir(getenv("HOME")) == -1) {
					printf("chdir failed\n");
				}
				strcpy(prev_working_dir, working_directory);
				getcwd(working_directory, sizeof(working_directory));
			}
			else if (!strcmp(args[1], "-")) {
				if (chdir(prev_working_dir) == -1) {
					printf("chdir failed\n");
				}
				strcpy(prev_working_dir, working_directory);
				getcwd(working_directory, sizeof(working_directory));
			}

			else {
				if (chdir(args[1]) == -1) {
					printf("chdir failed\n");
				}
			}
		}

		else if (!strcmp(c.com, "jobs")) {
			for (int i = 0; i < history_counter; i++)
			{
				if ((waitpid(history[i].pid, NULL, WNOHANG) == 0) && history[i].background) {
					printf("%s\n", history[i].com);;
				}
			}
		}
		else if (!strcmp(c.com, "exit")) {
			exit(0);
		}

		//NON Built in COMs
		else {
			c.pid = fork();
			if(c.pid != 0) {
				history[history_counter-1].pid = c.pid;
			}
			if (c.background) {
				args[c.argc - 1] = '\0';
				runInBackground(c.pid, args);
			}
			else {
				run(c.pid, args);
			}
		}
	}
	
	for (int i = 0; i < history_counter; i++) {
		free(history[i].status);
	}
}

int main(int argc, char* argv[]) {
	user_input_loop();
	return 0;
}