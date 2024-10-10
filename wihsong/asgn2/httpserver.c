#include "httpRequest.h"
#include <sys/socket.h>
#include <sys/select.h>

#define BUFSIZE         4096
#define TIMEOUT_SECONDS 50

// Function prototypes
static ssize_t read_until_delimiter(int in, char buffer[], size_t size, const char *delimiter);
static void setup_server(int argc, char *argv[], Listener_Socket *socket);
static int handle_connection(Listener_Socket *server_socket, char *buffer, size_t bufsize);
static void send_bad_request(int client_fd);

int main(int argc, char *argv[]) {
    Listener_Socket server_socket;
    setup_server(argc, argv, &server_socket);

    char buffer[BUFSIZE + 1] = { '\0' };

    // Continuous loop to accept connections and process HTTP requests
    while (true) {
        if (handle_connection(&server_socket, buffer, sizeof(buffer)) == EXIT_FAILURE) {
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

static int handle_connection(Listener_Socket *server_socket, char *buffer, size_t bufsize) {
    int connection_fd = listener_accept(server_socket);
    if (connection_fd == -1) {
        perror("Error accepting connection");
        return EXIT_FAILURE;
    }

    Request client_request;
    client_request.infd = connection_fd;

    ssize_t bytes_received = read_until_delimiter(connection_fd, buffer, BUFSIZE, "\r\n\r\n");
    if (bytes_received == -1) {
        send_bad_request(client_request.infd);
        close(connection_fd);
        memset(buffer, '\0', bufsize);
        return EXIT_SUCCESS; // Continue listening for other connections
    }

    if (parse_http_request(&client_request, buffer, bytes_received) != EXIT_FAILURE) {
        process_http_request(&client_request);
    }

    close(connection_fd);
    memset(buffer, '\0', bufsize);
    return EXIT_SUCCESS;
}

static void send_bad_request(int client_fd) {
    dprintf(client_fd, "HTTP/1.1 400 Bad Request\r\nContent-Length: %d\r\n\r\nBad Request\n", 12);
}

static void setup_server(int argc, char *argv[], Listener_Socket *socket) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = strtol(argv[1], NULL, 10);
    if (port < 0 || port > 65535) {
        fprintf(stderr, "Invalid port number\n");
        exit(EXIT_FAILURE);
    }

    int socket_status = listener_init(socket, port);
    if (socket_status == -1) {
        perror("Failed to initialize socket");
        exit(EXIT_FAILURE);
    }

    int optval = 1;
    if (setsockopt(socket->fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
        perror("Error setting socket options");
        exit(EXIT_FAILURE);
    }
}

static ssize_t read_until_delimiter(int in, char buffer[], size_t size, const char *delimiter) {
    size_t total_bytes_received = 0;
    ssize_t bytes_received;

    if (!buffer || (delimiter && size <= strlen(delimiter))) {
        errno = EINVAL;
        return -1;
    }

    fd_set read_fds;
    struct timeval timeout;

    size_t delimiter_length = delimiter ? strlen(delimiter) : 0;

    while (total_bytes_received < size) {
        FD_ZERO(&read_fds);
        FD_SET(in, &read_fds);

        timeout.tv_sec = TIMEOUT_SECONDS;
        timeout.tv_usec = 0;

        int select_result = select(in + 1, &read_fds, NULL, NULL, &timeout);
        if (select_result == -1) {
            perror("Select error");
            return -1;
        } else if (select_result == 0) {
            fprintf(stderr, "Connection timed out\n");
            return -1;
        }

        bytes_received = read(in, buffer + total_bytes_received, size - total_bytes_received);
        if (bytes_received <= 0) {
            return (bytes_received < 0) ? -1 : total_bytes_received;
        }

        total_bytes_received += bytes_received;

        if (delimiter_length && strstr(buffer, delimiter)) {
            break;
        }
    }

    return total_bytes_received;
}
