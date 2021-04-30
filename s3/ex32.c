#include <dirent.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>

char* run(const char* exe){
	return "hello from run";
}

char* enter_and_run(const char* folder) {
	// look for a single .c file 
	// run it 
	// return output using pipe and dupe2 or dupe
	return "none";
}
int main(int argc, char* argv[]) {
	DIR* dir = opendir(argv[1]);
	struct dirent* dn;
	while((dn = readdir(dir)) != NULL) {
		char* f = dn->d_name;
		struct stat path_stat;
   	 	stat(f, &path_stat);
    		printf("%s : %d\n",f,S_ISREG(path_stat.st_mode));
	}
}
