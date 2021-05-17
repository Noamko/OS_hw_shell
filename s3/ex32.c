// Noam Koren 308192871
#include <dirent.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <string.h>
#include <time.h>

#define INPUT_SIZE 150
typedef struct student_report {
	int score;
	const char* name;
	const char* r_info;
}student_report;

int isDirectory(const char* path) {
	struct stat statbuf;
	if (stat(path, &statbuf) != 0)
		return 0;
	return S_ISDIR(statbuf.st_mode);
}

int compareFiles(const char* comp, const char* file1, const char* file2) {
	pid_t pid = fork();
	if (pid == 0) {
		if (execl(comp, comp, file1, file2, (char*)NULL) < 0) {
			printf("failed to exec");
			return -1;
		}
	}
	else {
		int status;
		wait(&status);
		return WEXITSTATUS(status);
	}
	return -1;
}

void set_output(const char* file) {
	int out = open(file, O_WRONLY | O_CREAT, 0666);
	if (out < 0) {
		printf("Output file not exist");
	}
	if (dup2(out, STDOUT_FILENO) < 0) {
		//errorno
	}
	close(out);
}

void set_input(const char* file) {
	int in = open(file, O_RDONLY);
	if (in < 0) {
		printf("Input file not exist");
		exit(-1);
	}
	if (dup2(in, STDIN_FILENO) < 0) {
		printf("unable to dup stdin\n");
		//need to do errrono
	}
	close(in);
}

void set_error(const char* file) {
	int err = open(file, O_WRONLY | O_CREAT | O_APPEND, 0666);
	if (dup2(err, STDERR_FILENO) < 0) {
		printf("failed to dup err");
	}
	close(err);
}

int isCFile(const char* file) {
	int len = strlen(file);
	if (len >= 3) {
		if (file[len - 2] == '.' && file[len - 1] == 'c') {
			return 1;
		}
	}
	return 0;
}

const char* getFullPath(const char* file) {
	char* str = malloc(255);
	char path[255];
	getcwd(path, sizeof(path));
	strcat(path, "/");
	strcat(path, file);
	strcpy(str, path);
	return str;
}

int enter_and_run(const char* folder, const char* inputfile, const char* currect_output, const char* comp) {
	DIR* dir = opendir(folder);
	if (dir == NULL) {
		//errorno
		printf("error");
	}
	struct dirent* dn;
	chdir(folder);
	while ((dn = readdir(dir)) != NULL) {
		char* file = dn->d_name;
		if (isCFile(file)) { // c file found.
			time_t begin, end;
			time(&begin);
			pid_t pid = fork();
			if (pid == 0) {
				pid_t pid2 = fork();
				if (pid2 == 0) {
					char* argv[2];
					argv[0] = file;
					argv[1] = NULL;

					if (execvp("gcc", argv) < 0) {
						exit(3);
					}
				}
				else {
					wait(NULL);
					set_input(inputfile);
					set_output("output.txt");
					if (execl("./a.out", (char*)NULL) < 0) {
						exit(2);
					}
				}
			}
			int status;
			wait(&status);
			time(&end);
			time_t elapsed = end - begin;
			int exit_stat = WEXITSTATUS(status);
			const char* o_file = getFullPath("output.txt");
			int comp_result = compareFiles(comp, o_file, currect_output);
			remove("output.txt");
			remove("a.out");
			closedir(dir);
			chdir("..");
			if (elapsed > 5) {
				return 20;
			}
			else if (exit_stat == 2) {
				return 10;
			}
			else if (comp_result == 1) {
				return 100;
			}
			else if (comp_result == 2) {
				return 50;
			}
			else if (comp_result == 3) {
				return 75;
			}
			continue;
		}
	}
	//fail no c file
	closedir(dir);
	chdir("..");
	return 0;
}

void eex(const char* path) {
	int fd = open(path, O_RDONLY);
	if (fd < 0) {
		printf("failed to open file");
	}
	char buffer[INPUT_SIZE + 1];
	int bytes_read = INPUT_SIZE;
	bytes_read = read(fd, buffer, INPUT_SIZE); //taking a risk here that file content is larger then 2048 (not likly at all)
	close(fd);
	set_error(("errors.txt"));
	const char* _dir = strtok(buffer, "\n");
	const char* _input_file = getFullPath(strtok(NULL, "\n"));
	const char* _correct_output_file = getFullPath(strtok(NULL, "\n"));
	const char* comp = getFullPath("comp.out");

	DIR* dir = opendir(_dir);
	if (dir == NULL) {
		printf("Not a valid directory\n");
		exit(-1);
	}
	struct dirent* dn;
	chdir(_dir);

	char result[1024];
	strcpy(result, "");
	while ((dn = readdir(dir)) != NULL) {
		char* folder = dn->d_name;
		if (isDirectory(folder) && strcmp(folder, "..") != 0 && strcmp(folder, ".") != 0) {
			int r_score = enter_and_run(folder, _input_file, _correct_output_file, comp); // look for a .c file compile and run it.
			strcat(result, folder);
			strcat(result, ",");
			switch (r_score) {
			case 100:
				strcat(result, "100,EXCELLENT\n");
				break;;
			case 75:
				strcat(result, "75,SIMILAR\n");
				break;
			case 50:
				strcat(result, "50,WRONG\n");
				break;
			case 20:
				strcat(result, "20,TIMEOUT\n");
				break;
			case 10:
				strcat(result, "10,COMPILATION_ERROR\n");
				break;
			case 0:
				strcat(result, "0,NO_C_FILE\n");
				break;
			}
		}
	}

	//Cleanup
	chdir("..");
	int csv_file = open("results.csv", O_CREAT | O_WRONLY, 0666);
	write(csv_file, result, strlen(result));
	close(csv_file);
	closedir(dir);
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		printf("not enough args");
		exit(1);
	}
	eex(argv[1]);
	return 0;
}
