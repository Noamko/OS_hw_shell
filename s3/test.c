// Noam Koren 308192871
#include <dirent.h>

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/fcntl.h>


int check_equal(int f1, int f2, int _size){
	char f1_buffer[_size + 1];
	char f2_buffer[_size + 1];
	int bytes_read = _size;
	do {
		bytes_read = read(f1,f1_buffer, _size);
		int t = read(f2,f2_buffer,_size);
		if(t != bytes_read){
			return 0;
		}
		for(int i = 0; i < bytes_read; i++) {
			if(f1_buffer[i] != f2_buffer[i]) {
				return 0;
			}
		}
	} while(bytes_read == _size);
	return 1;
}

int main(int argc, char* argv[]){
	int file1,file2;
	file1 = open(argv[1], O_RDONLY);
	file2 = open(argv[2], O_RDONLY);
	if(file1 < 0 || file2 < 0) {
		printf("%s\n","Error Opening files");
		exit(1);
	}
	if(check_equal(file1,file2,1)) {
		printf("%s\n","files are equal");
	}
	else {
		printf("%s\n","files are not equal");
	}
	close(file1);
	close(file2);
	return 0;
}
