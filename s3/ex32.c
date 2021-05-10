#include <dirent.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <string.h>

char* run(const char* exe) {
	return "hello from run";
}

char* enter_and_run(const char* folder) {
	// look for a single .c file 
	// run it 
	// return output using pipe and dupe2 or dupe
	return "none";
}
int main(int argc, char* argv[]) {

	int info_file = open(argv[1], O_RDONLY);
	int size = 2024;
	char buffer[size + 1];
	int bytes_read = size;
	do {
		bytes_read = read(info_file, buffer, size);

	} while (bytes_read == size);
	char* _dir = strtok(buffer, "\n");
	char* _input = strtok(NULL, "\n");
	char* _correct_output = strtok(NULL, "\n");

	DIR* dir = opendir(_dir);
	struct dirent* dn;
	while ((dn = readdir(dir)) != NULL) {
		char* f = dn->d_name;
		struct stat path_stat;
		stat(f, &path_stat);
		printf("%s : %d\n", f, S_ISDIR(path_stat.st_mode));
	}
}
