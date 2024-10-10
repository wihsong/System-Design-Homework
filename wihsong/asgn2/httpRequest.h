#pragma once

#include "asgn2_helper_funcs.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <regex.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

typedef struct Request {
    char *command;
    char *target_path;
    char *version;
    char *message_body;
    int infd;
    int content_length;
    int remaining_bytes;
} Request;

int process_http_request(Request *R);

int parse_http_request(Request *R, char *buf, ssize_t bytes_read);
