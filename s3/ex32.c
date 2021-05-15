#include <dirent.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <string.h>
#include <time.h>

#define INPUT_SIZE 2048
typedef struct student_report {
	int score;
	const char* name;
	const char* r_info;
	struct student_report* next;
}student_report;

student_report* add(student_report* sr, const char* name, const char* info, int score) {
	if (sr == NULL) {
		sr = malloc(sizeof(student_report));
		sr->score = score;
		sr->name = name;
		sr->r_info = info;
		sr->next = NULL;
	}
	else {
		student_report* temp = sr;
		while (temp->next != NULL) {
			temp = temp->next;
		}
		temp->next = malloc(sizeof(student_report));
		temp->next->score = score;
		temp->next->name = name;
		temp->next->r_info = info;
		temp->next->next = NULL;
	}
	return sr;
}
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
	dup2(out, STDOUT_FILENO);
	close(out);
}

void set_input(const char* file) {
	int in = open(file, O_RDONLY);
	if (dup2(in, STDIN_FILENO) < 0) {
		printf("unable to dup stdin\n");
	}
	close(in);
}

void set_error(const char* file) {
	int err = open(file, O_WRONLY | O_CREAT, 0666);
	if (err < 0) {
		printf("failed to create errors.txt");
		exit(1);
	}
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

student_report enter_and_run(const char* folder, const char* inputfile, const char* currect_output, const char* comp) {
	DIR* dir = opendir(folder);
	struct dirent* dn;
	chdir(folder);
	student_report report;
	while ((dn = readdir(dir)) != NULL) {
		char* file = dn->d_name;
		if (isCFile(file)) { // c file found.
			time_t begin, end;
			time(&begin);
			pid_t pid = fork();
			if (pid == 0) {
				set_error("errors.txt");
				pid_t pid2 = fork();
				if (pid2 == 0) {
					if (execl("/usr/bin/gcc", "gcc", file, (char*)NULL) < 0) {
						exit(2);
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
			int rs = WEXITSTATUS(status);
			int cp = compareFiles(comp, getFullPath("output.txt"), currect_output);
			remove("output.txt");
			remove("a.out");
			remove("errors.txt");
			report.name = folder;
			if (elapsed > 4) {
				report.score = 20;
				report.r_info = "TIMEOUT";
			}
			else if (rs == 2) {
				report.score = 10;
				report.r_info = "COMPILATION_ERROR";
			}
			else if (cp == 1) {
				report.score = 100;
				report.r_info = "EXCELLENT";
			}
			else if (cp == 2) {
				report.score = 50;
				report.r_info = "WRONG";
			}
			else if (cp == 3) {
				report.score = 75;
				report.r_info = "SIMILAR";
			}
			closedir(dir);
			chdir("..");
			return report;
		}
	}
	//fail no c file
	report.score = 0;
	report.name = folder;
	report.r_info = "NO_C_FILE";
	closedir(dir);
	chdir("..");
	return report;
}

student_report* eex(const char* path) {
	int fd = open(path, O_RDONLY);
	if(fd < 0){
		printf("failed to open file");
	}
	char buffer[INPUT_SIZE + 1];
	int bytes_read = INPUT_SIZE;
	bytes_read = read(fd, buffer, INPUT_SIZE); //taking a risk here that file content is larger then 2048 (not likly at all)
	close(fd);

	const char* _dir = strtok(buffer, "\n");
	const char* _input_file = getFullPath(strtok(NULL, "\n"));
	const char* _correct_output_file = getFullPath(strtok(NULL, "\n"));
	const char* comp = getFullPath("comp.out");

	DIR* dir = opendir(_dir);
	struct dirent* dn;
	chdir(_dir);

	student_report* list;
	while ((dn = readdir(dir)) != NULL) {
		char* folder = dn->d_name;
		if (isDirectory(folder) && strcmp(folder, "..") != 0 && strcmp(folder, ".") != 0) {
			student_report _report = enter_and_run(folder, _input_file, _correct_output_file, comp); // look for a .c file compile and run it.
			list = add(list, _report.name, _report.r_info, _report.score);
		}
	}
	closedir(dir);
	return list;
}

void create_report(student_report* r) {
	student_report* temp = r;
	FILE* fpt = fopen("results.txt", "w+");
	while (r != NULL) {
		fprintf(fpt, "%s,%d,%s\n", r->name, r->score, r->r_info);
		printf("%s,%d,%s\n", r->name, r->score, r->r_info);
		r = r->next;
	}
	fclose(fpt);
}
int main(int argc, char* argv[]) {
	if (argc < 2) {
		printf("not enough args");
		exit(1);
	}
	create_report(eex(argv[1]));
	return 0;
}
