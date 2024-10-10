#include "asgn2_helper_funcs.h"
#include "connection.h"
#include "queue.h"
#include "request.h"
#include "response.h"
#include "rwlock.h"

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/file.h>

queue_t *q;
pthread_mutex_t mutex;
rwlock_t *rwlock;

void handle_connection(int);
void handle_get(conn_t *);
void handle_put(conn_t *);
void handle_unsupported(conn_t *);

void *workerThread() {
    while (1) {
        void *data;
        if (queue_pop(q, &data)) {
            int connfd = (int) (intptr_t) data;
            handle_connection(connfd);
            close(connfd);
        }
    }
    return NULL;
}

int main(int argc, char **argv) {
    int thread_count = 4;

    pthread_mutex_init(&mutex, NULL);
    rwlock = rwlock_new(N_WAY, 1);
    q = queue_new(thread_count);

    int opt = 0;
    while ((opt = getopt(argc, argv, "t:")) != -1) {
        switch (opt) {
        case 't':
            thread_count = atoi(optarg);
            if (thread_count < 0) {
                fprintf(stderr, "Invalid thread size.\n");
                return EXIT_FAILURE;
            }
        }
    }

    if (argc < 2) {
        warnx("wrong arguments: %s port_num", argv[0]);
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *endptr = NULL;
    size_t port = (size_t) strtoull(argv[optind], &endptr, 10);
    if (endptr && *endptr != '\0') {
        warnx("invalid port number: %s", argv[optind]);
        return EXIT_FAILURE;
    }

    if (optind < argc - 1) {
        fprintf(stderr, "Unexpected extra arguments provided.\n");
        for (int i = optind + 1; i < argc; i++) {
            fprintf(stderr, "Extraneous argument: %s\n", argv[i]);
        }
        return EXIT_FAILURE;
    }

    signal(SIGPIPE, SIG_IGN);
    Listener_Socket socket;
    listener_init(&socket, port);

    pthread_t threads[thread_count];
    for (int i = 0; i < thread_count; ++i) {
        if (pthread_create(&threads[i], NULL, workerThread, NULL) != 0) {
            fprintf(stderr, "Failed to create thread %d\n", i);
            return EXIT_FAILURE;
        }
    }

    while (1) {
        int connfd = listener_accept(&socket);
        if (connfd >= 0) {
            queue_push(q, (void *) (intptr_t) connfd);
        }
    }

    return EXIT_SUCCESS;
}

void handle_connection(int connfd) {

    conn_t *conn = conn_new(connfd);

    const Response_t *res = conn_parse(conn);

    if (res != NULL) {
        conn_send_response(conn, res);
    } else {
        const Request_t *req = conn_get_request(conn);
        if (req == &REQUEST_GET) {
            handle_get(conn);
        } else if (req == &REQUEST_PUT) {
            handle_put(conn);
        } else {
            handle_unsupported(conn);
        }
    }

    conn_delete(&conn);
}

void handle_get(conn_t *conn) {
    char *uri = conn_get_uri(conn);
    const Response_t *res = NULL;
    char *header;

    reader_lock(rwlock);

    int fd = open(uri, O_RDONLY);
    if (fd < 0) {
        res = (errno == EACCES)   ? &RESPONSE_FORBIDDEN
              : (errno == ENOENT) ? &RESPONSE_NOT_FOUND
                                  : &RESPONSE_INTERNAL_SERVER_ERROR;

        conn_send_response(conn, res);
        header = conn_get_header(conn, "Request-Id");
        if (!header)
            header = "0";
        fprintf(stderr, "GET,/%s,%d,%s\n", uri, response_get_code(res), header);

        reader_unlock(rwlock);
        return;
    }

    struct stat finfo;
    if (fstat(fd, &finfo) < 0 || S_ISDIR(finfo.st_mode)) {
        res = S_ISDIR(finfo.st_mode) ? &RESPONSE_FORBIDDEN : &RESPONSE_INTERNAL_SERVER_ERROR;
        conn_send_response(conn, res);
        header = conn_get_header(conn, "Request-Id");
        if (!header)
            header = "0";
        fprintf(stderr, "GET,/%s,%d,%s\n", uri, response_get_code(res), header);

        close(fd);
        reader_unlock(rwlock);
        return;
    }

    res = conn_send_file(conn, fd, finfo.st_size);
    header = conn_get_header(conn, "Request-Id");
    if (!header)
        header = "0";
    fprintf(stderr, "GET,/%s,200,%s\n", uri, header);

    close(fd);
    reader_unlock(rwlock);
}

void handle_unsupported(conn_t *conn) {
    conn_send_response(conn, &RESPONSE_NOT_IMPLEMENTED);
}

void handle_put(conn_t *conn) {
    char *uri = conn_get_uri(conn);
    const Response_t *res = NULL;
    char *header;

    writer_lock(rwlock);

    bool existed = access(uri, F_OK) == 0;
    int fd = open(uri, O_CREAT | O_TRUNC | O_WRONLY, 0600);

    if (fd < 0) {
        res = (errno == EACCES || errno == EISDIR || errno == ENOENT)
                  ? &RESPONSE_FORBIDDEN
                  : &RESPONSE_INTERNAL_SERVER_ERROR;
        conn_send_response(conn, res);
        header = conn_get_header(conn, "Request-Id") ? conn_get_header(conn, "Request-Id") : "0";
        fprintf(stderr, "PUT,/%s,%d,%s\n", uri, response_get_code(res), header);
        writer_unlock(rwlock);
        return;
    }

    ftruncate(fd, 0);
    res = conn_recv_file(conn, fd);

    res = (!res && existed)    ? &RESPONSE_OK
          : (!res && !existed) ? &RESPONSE_CREATED
                               : &RESPONSE_INTERNAL_SERVER_ERROR;
    conn_send_response(conn, res);
    header = conn_get_header(conn, "Request-Id") ? conn_get_header(conn, "Request-Id") : "0";
    fprintf(stderr, "PUT,/%s,%d,%s\n", uri, response_get_code(res), header);

    close(fd);
    writer_unlock(rwlock);
}
