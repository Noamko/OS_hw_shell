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
	int background;
	int argc;
	pid_t pid;
}Command;

void runInBackground(pid_t pid, char* args[]) {
	if (pid == 0) {
		if(execvp(args[0], args) == -1){
			printf("%s\n", "exec failed");
			exit(1);
		}
	}
}

void run(pid_t pid, char* args[]) {
	if (pid == 0) {
		if(execvp(args[0], args) == -1){
			printf("%s\n", "exec failed");
			exit(1);
		}
	}
	else {
		wait(NULL);
	}
}

int cdBack(char* wd) {
	for (int i = strlen(wd)-1; i >= 0; i--) {
		if (wd[i] == '/') {
			wd[i] = '\0';
			if(!strcmp(wd,"")){
				strcpy(wd,"/");
			}
			return chdir(wd);
		}
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
				printf("%s", history[i].com);
				fflush(stdout);
				if (((waitpid(history[i].pid, NULL, WNOHANG) == 0) || i == history_counter - 1) && strcmp(history[i].com, "jobs") != 0) {
					printf(" RUNNING\n");
				}
				else if (waitpid(history[i].pid, NULL, WNOHANG) == -1 || !strcmp(history[i].com, "jobs")) {
					printf(" DONE\n");
				}
			}
		}

		else if (!strcmp(args[0], "cd")) {
			if (c.argc > 2) {
				printf("Too many arguments\n");
			}
			else if(c.argc == 1) {
				if (chdir(getenv("HOME")) == -1) {
					printf("chdir failed\n");
				}
				else {
					strcpy(prev_working_dir, working_directory);
				}
			}
			else if (!strcmp(args[1], "..")) {
				char temp[100];
				strcpy(temp,prev_working_dir);
				strcpy(prev_working_dir,working_directory);
				if(cdBack(working_directory) == -1){
					printf("chdir failed\n");
					strcpy(prev_working_dir, temp);
				}
			}
			else if (!strcmp(args[1], "~")) {
				if (chdir(getenv("HOME")) == -1) {
					printf("chdir failed\n");
				}
				else{
					strcpy(prev_working_dir, working_directory);
				}
			}
			else if (!strcmp(args[1], "-")) {
				if (chdir(prev_working_dir) == -1) {
					printf("chdir failed\n");
				}
				else {
					strcpy(prev_working_dir, working_directory);
				}
			}

			else if (!strcmp(args[1], "/")) {
				if (chdir("/") == -1) {
					printf("chdir failed\n");
				}
				else {
					strcpy(prev_working_dir, working_directory);
				}
			}

			else {
				char* token = strtok(args[1],"/");
				char temp[100];
				char temp2[100];
				strcpy(temp,working_directory);
				strcpy(temp2,prev_working_dir);
				strcpy(prev_working_dir,working_directory);
				if(args[1][0] == '/'){
					chdir("/");
				}
				while (token != NULL) {
					if(!strcmp(token,"~")){
						if (chdir(getenv("HOME")) == -1) {
							printf("chdir failed\n");
							strcpy(working_directory,temp);
							strcpy(prev_working_dir,temp2);
							break;
						}
					}
					else if(!strcmp(token,"..")) {
						char temp[100];
						strcpy(temp, prev_working_dir);
						strcpy(prev_working_dir, working_directory);
						if (cdBack(working_directory) == -1) {
							printf("chdir failed\n");
							strcpy(working_directory, temp);
							strcpy(prev_working_dir, temp2);
							break;
						}
					}
					else {
						if (chdir(token) == -1) {
							printf("chdir failed\n");
							strcpy(working_directory, temp);
							strcpy(prev_working_dir, temp2);
							break;
						}
					}
					token = strtok(NULL, "/");
				}
			}
			getcwd(working_directory, sizeof(working_directory));
		}

		else if (!strcmp(c.com, "jobs")) {
			for (int i = 0; i < history_counter; i++)
			{
				if ((waitpid(history[i].pid, NULL, WNOHANG) == 0) && history[i].background && strcmp(history[i].com,"jobs") != 0) {
					printf("%s\n", history[i].com);;
				}
			}
		}
		else if (!strcmp(c.com, "exit")) {
			exit(0);
		}
		else { //Nonbuilt in commands
			if(!strcmp(args[0],"echo")){
				if(args[1][0] == '"' && args[argc -1][strlen(args[argc-1]) -1] == '"'){
					char* temp;
					temp = args[1] +1;
					args[1] = temp;
					args[argc - 1][strlen(args[argc - 1]) - 1] = '\0';

				}
			}
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
}

int main(int argc, char* argv[]) {
	user_input_loop();
	return 0;
}