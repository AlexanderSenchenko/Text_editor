#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>

int main()
{
	char *file_name = "hello.txt";
	int fd_write;
	ssize_t size_write;

	fd_write = open(file_name, O_WRONLY | O_TRUNC);
	if (fd_write == -1) {
		printf("Error: file not found\n");
		exit(1);
	}

	size_write = write(fd_write, "Hello!\n", 7);
	if (size_write == -1) {
		printf("Error: write\n");
	} else if (size_write != 7) {
		printf("Warning: \n");
	}

	close(fd_write);

	int fd_read;
	off_t size_file, pos;
	ssize_t size_read;
	char buff;
	int i;

	fd_read = open(file_name, O_RDONLY);
	if (fd_read == -1) {
		printf("Error: file not found\n");
		exit(1);
	}

	size_file = lseek(fd_read, 0, SEEK_END);
	if (size_file == (off_t) -1) {
		printf("Error: lseek\n");
	} else {
		printf("Size_file: %d\n", (int) size_file);
	}
	
	for (i = 0; i < (int) size_file; ++i) {
		pos = lseek(fd_read, -1 - i, SEEK_END);
		if (pos == (off_t) -1) {
			printf("Error: lseek\n");
		} else {
			size_read = read(fd_read, &buff, 1);
			if (size_read == -1) {
				printf("Error: read\n");
			} else if (size_read != 1) {
				printf("Warning: \n");
			} else {
				printf("%c\n", buff);
			}
		}
	}

	close(fd_read);

	return 0;
}
