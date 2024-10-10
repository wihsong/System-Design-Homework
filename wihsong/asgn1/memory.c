#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>

#define BUFFER_SIZE 4096

void write_to_stdout(const char *buf, int bytes_read) {
    int bytes_written = 0;
    do {
        int bytes = write(STDOUT_FILENO, buf + bytes_written, bytes_read - bytes_written);
        if (bytes <= 0) {
            fprintf(stderr, "Cannot write to stdout\n");
            exit(1);
        }
        bytes_written += bytes;
    } while (bytes_written < bytes_read);
}

int read_line(char *buffer, size_t maxlen) {
    char ch;
    size_t len = 0;

    while (len < maxlen) {
        if (read(STDIN_FILENO, &ch, 1) <= 0) {
            return -1;
        }
        if (ch == '\n') {
            buffer[len] = '\0';
            return len;
        }
        buffer[len++] = ch;
    }

    return -1;
}

int main() {
    char command[5];
    char location[PATH_MAX];
    char content_length_str[11];
    int content_length;
    char buf[BUFFER_SIZE];

    if (read_line(command, sizeof(command)) <= 0
        || (strcmp(command, "get") != 0 && strcmp(command, "set") != 0)) {
        fprintf(stderr, "Invalid Command\n");
        return 1;
    }

    if (read_line(location, sizeof(location)) <= 0) {
        fprintf(stderr, "Invalid Command\n");
        return 1;
    }

    if (strcmp(command, "get") == 0) {
        char extra_char;
        int read_res = read(STDIN_FILENO, &extra_char, 1);
        if (read_res > 0) {
            fprintf(stderr, "Invalid Command\n");
            return 1;
        }
        if (read_line(buf, sizeof(buf)) > 0) {
            fprintf(stderr, "Invalid Command\n");
            return 1;
        }

        struct stat st;
        if (stat(location, &st) != 0) {
            fprintf(stderr, "Invalid Command\n");
            return 1;
        }

        if (S_ISDIR(st.st_mode)) {
            fprintf(stderr, "Invalid Command\n");
            return 1;
        }

        int fd = open(location, O_RDONLY);
        if (fd < 0) {
            fprintf(stderr, "Invalid Command\n");
            return 1;
        }

        int bytes_read;
        do {
            bytes_read = read(fd, buf, BUFFER_SIZE);
            if (bytes_read < 0) {
                fprintf(stderr, "Operation Failed\n");
                close(fd);
                return 1;
            } else if (bytes_read > 0) {
                write_to_stdout(buf, bytes_read);
            }
        } while (bytes_read > 0);
        close(fd);
    } else if (strcmp(command, "set") == 0) {
        if (read_line(content_length_str, sizeof(content_length_str)) <= 0) {
            fprintf(stderr, "Invalid Command\n");
            return 1;
        }
        content_length = atoi(content_length_str);

        int fd = open(location, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) {
            fprintf(stderr, "Operation Failed\n");
            return 1;
        }

        int total_bytes_written = 0;
        while (total_bytes_written < content_length) {
            int to_read = content_length - total_bytes_written > BUFFER_SIZE
                              ? BUFFER_SIZE
                              : content_length - total_bytes_written;
            int bytes_read = read(STDIN_FILENO, buf, to_read);
            if (bytes_read < 0) {
                fprintf(stderr, "Operation Failed\n");
                close(fd);
                return 1;
            }
            if (bytes_read == 0)
                break;
            int write_res = write(fd, buf, bytes_read);
            if (write_res < 0) {
                fprintf(stderr, "Operation Failed\n");
                close(fd);
                return 1;
            }
            total_bytes_written += bytes_read;
        }

        printf("OK\n");
        close(fd);
    } else {
        fprintf(stderr, "Invalid Command\n");
        return 1;
    }

    return 0;
}
