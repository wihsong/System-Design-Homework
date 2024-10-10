// echo.c
// compile with: gcc echo.c -o echo
// run with: ./echo

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFF_SIZE 64

char buff[BUFF_SIZE];

int main(void) {
	while(1) {
		// fill buffer by reading from STDIN
		ssize_t bytesWritten = 0;
		ssize_t bytesRead = read(STDIN_FILENO, buff, BUFF_SIZE);
		if (bytesRead == 0) // reached end of file
			break;
		if (bytesRead == -1) // encountered an error
			exit(1);
		
		// write the unwritten bytes in buffer to STDOUT
		while (bytesWritten < bytesRead) {
			ssize_t w = write(STDOUT_FILENO, buff + bytesWritten, bytesRead - bytesWritten);
			if (w == -1) // encountered an error
				exit(1);
			bytesWritten += w;
		}
	}

	return 0;
}

