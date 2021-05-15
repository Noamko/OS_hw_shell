// Noam Koren 308192871
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/fcntl.h>

int check_equal(int f1, int f2, int _size) {
	char f1_buffer[_size + 1];
	char f2_buffer[_size + 1];
	int bytes_read = _size;
	do {
		bytes_read = read(f1, f1_buffer, _size);
		int t = read(f2, f2_buffer, _size);
		if (bytes_read == -1 || t == -1) {
			fprintf(stderr, "Error in: %s\n", __func__);
			return -1;
		}
		if (t != bytes_read) {
			return 0;
		}
		int i;
		for (i = 0; i < bytes_read; i++) {
			if (f1_buffer[i] != f2_buffer[i]) {
				return 0;
			}
		}
	} while (bytes_read == _size);
	return 1;
}
char get_next_nonbreak_char(int fd) {
	char temp_buffer[2];
	do {
		int r = read(fd, temp_buffer, 1);
		if (r == -1) {
			fprintf(stderr, "Error in: %s\n", __func__);
			return 0;
		}
		if (r == 0) {
			return EOF;
		}
	} while (temp_buffer[0] == '\n' || temp_buffer[0] == '\t' || temp_buffer[0] == 32 || temp_buffer[0] == '\r');
	return temp_buffer[0];
}
int check_similar(int fd1, int fd2, int _size) {
	char c1, c2;
	do {
		c1 = get_next_nonbreak_char(fd1);
		c2 = get_next_nonbreak_char(fd2);
		if (c1 == 0 || c2 == 0) {
			return -1;
		}
		if (c1 != c2) {
			return 2;
		}

	} while (c1 != EOF && c2 != EOF);
	return 3;
}

int main(int argc, char* argv[]) {
	int file1, file2;
	file1 = open(argv[1], O_RDONLY);
	file2 = open(argv[2], O_RDONLY);
	if (file1 < 0 || file2 < 0) {
		fprintf(stderr, "Error in: %s\n", __func__);
		close(file1);
		close(file2);
		return -1;
	}
	int equal = check_equal(file1, file2, 100);
	if (equal == -1) {
		close(file1);
		close(file2);
		return -1;
	}
	else if (equal) {
		close(file1);
		close(file2);
		return 1;
	}
	lseek(file1, 0, SEEK_SET);
	lseek(file2, 0, SEEK_SET);
	int similar = check_similar(file1, file2, 100);
	close(file1);
	close(file2);
	return similar;

}
