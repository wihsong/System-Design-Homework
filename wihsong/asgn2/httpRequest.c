#include "httpRequest.h"
#include "asgn2_helper_funcs.h"

#define REQEX  "^([a-zA-Z]{1,8}) /([a-zA-Z0-9.-]{1,63}) (HTTP/[0-9]\\.[0-9])\r\n"
#define HEADEX "([a-zA-Z0-9.-]{1,128}): ([ -~]{1,128})\r\n"

ssize_t complete_write(int target, char data[], size_t dataSize) {
    size_t data_written = 0;
    while (data_written < dataSize) {
        ssize_t res = write(target, data + data_written, dataSize - data_written);
        if (res < 0) {
            if (errno == EINTR)
                continue;
            return -1;
        } else if (res == 0) {
            errno = EIO;
            return -1;
        }
        data_written += res;
    }
    return data_written;
}

int handle_get_request(Request *request_obj) {
    // Verify content length
    if (request_obj->content_length != -1 || request_obj->remaining_bytes > 0) {
        dprintf(request_obj->infd,
            "HTTP/1.1 400 Bad Request\r\nContent-Length: %d\r\n\r\nBad Request\n", 12);
        return (EXIT_FAILURE);
    }

    int fileDescriptor;
    // Check if it's a directory
    if ((fileDescriptor = open(request_obj->target_path, O_RDONLY | O_DIRECTORY)) != -1) {
        dprintf(request_obj->infd,
            "HTTP/1.1 403 Forbidden\r\nContent-Length: %d\r\n\r\nForbidden\n", 10);
        return (EXIT_FAILURE);
    }

    // Attempt to open the target file
    if ((fileDescriptor = open(request_obj->target_path, O_RDONLY)) == -1) {
        switch (errno) {
        case ENOENT:
            dprintf(request_obj->infd,
                "HTTP/1.1 404 Not Found\r\nContent-Length: %d\r\n\r\nNot Found\n", 10);
            break;
        case EACCES:
            dprintf(request_obj->infd,
                "HTTP/1.1 403 Forbidden\r\nContent-Length: %d\r\n\r\nForbidden\n", 10);
            break;
        default:
            dprintf(request_obj->infd,
                "HTTP/1.1 500 Internal Server Error\r\nContent-Length: %d\r\n\r\nInternal Server "
                "Error\n",
                22);
            break;
        }
        return (EXIT_FAILURE);
    }

    struct stat fileInfo;
    fstat(fileDescriptor, &fileInfo);
    off_t contentSize = fileInfo.st_size;

    // Sending success response
    dprintf(request_obj->infd, "HTTP/1.1 200 OK\r\nContent-Length: %ld\r\n\r\n", contentSize);
    int transferred_bytes = pass_n_bytes(fileDescriptor, request_obj->infd, contentSize);
    if (transferred_bytes == -1) {
        dprintf(request_obj->infd,
            "HTTP/1.1 500 Internal Server Error\r\nContent-Length: %d\r\n\r\nInternal Server "
            "Error\n",
            22);
        return (EXIT_FAILURE);
    }

    close(fileDescriptor);
    return (EXIT_SUCCESS);
}

int handle_put_request(Request *request_obj) {
    // Validate content length
    if (request_obj->content_length == -1) {
        dprintf(request_obj->infd,
            "HTTP/1.1 400 Bad Request\r\nContent-Length: %d\r\n\r\nBad Request\n", 12);
        return (EXIT_FAILURE);
    }

    int fileDescriptor;
    int statusCode = 0;

    // Attempt to open as directory (to check if the target is a directory)
    if ((fileDescriptor = open(request_obj->target_path, O_WRONLY | O_DIRECTORY, 0666)) != -1) {
        dprintf(request_obj->infd,
            "HTTP/1.1 403 Forbidden\r\nContent-Length: %d\r\n\r\nForbidden\n", 10);
        return (EXIT_FAILURE);
    }

    // Attempt to create the file if it doesn't exist
    if ((fileDescriptor = open(request_obj->target_path, O_WRONLY | O_CREAT | O_EXCL, 0666))
        == -1) {
        switch (errno) {
        case EEXIST: statusCode = 200; break;
        case EACCES:
            dprintf(request_obj->infd,
                "HTTP/1.1 403 Forbidden\r\nContent-Length: %d\r\n\r\nForbidden\n", 10);
            return (EXIT_FAILURE);
        default:
            dprintf(request_obj->infd,
                "HTTP/1.1 500 Internal Server Error\r\nContent-Length: %d\r\n\r\nInternal Server "
                "Error\n",
                22);
            return (EXIT_FAILURE);
        }
    } else if (fileDescriptor != -1) {
        statusCode = 201;
    }

    // Handle existing file
    if (statusCode == 200) {
        if ((fileDescriptor = open(request_obj->target_path, O_WRONLY | O_CREAT | O_TRUNC, 0666))
            == -1) {
            switch (errno) {
            case EACCES:
                dprintf(request_obj->infd,
                    "HTTP/1.1 403 Forbidden\r\nContent-Length: %d\r\n\r\nForbidden\n", 10);
                return (EXIT_FAILURE);
            default:
                dprintf(request_obj->infd,
                    "HTTP/1.1 500 Internal Server Error\r\nContent-Length: %d\r\n\r\nInternal "
                    "Server Error\n",
                    22);
                return (EXIT_FAILURE);
            }
        }
    }

    // Writing to file
    int transferred_bytes
        = complete_write(fileDescriptor, request_obj->message_body, request_obj->remaining_bytes);
    if (transferred_bytes == -1) {
        dprintf(request_obj->infd,
            "HTTP/1.1 500 Internal Server Error\r\nContent-Length: %d\r\n\r\nInternal Server "
            "Error\n",
            22);
        return (EXIT_FAILURE);
    }

    int total_transferred = request_obj->content_length - request_obj->remaining_bytes;
    transferred_bytes = pass_n_bytes(request_obj->infd, fileDescriptor, total_transferred);
    if (transferred_bytes == -1) {
        dprintf(request_obj->infd,
            "HTTP/1.1 500 Internal Server Error\r\nContent-Length: %d\r\n\r\nInternal Server "
            "Error\n",
            22);
        return (EXIT_FAILURE);
    }

    // Send response
    if (statusCode == 201) {
        dprintf(
            request_obj->infd, "HTTP/1.1 201 Created\r\nContent-Length: %d\r\n\r\nCreated\n", 8);
    } else {
        dprintf(request_obj->infd, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\n\r\nOK\n", 3);
    }

    close(fileDescriptor);
    return (EXIT_SUCCESS);
}

int process_http_request(Request *request) {
    // Check HTTP Version
    if (strcmp(request->version, "HTTP/1.1") != 0) {
        dprintf(request->infd,
            "HTTP/1.1 505 Version Not Supported\r\nContent-Length: %d\r\n\r\nVersion Not "
            "Supported\n",
            22);
        return EXIT_FAILURE;
    }

    // Process GET Command
    if (strcmp(request->command, "GET") == 0) {
        return handle_get_request(request);
    }

    // Process PUT Command
    if (strcmp(request->command, "PUT") == 0) {
        return handle_put_request(request);
    }

    // Unsupported HTTP Command
    dprintf(request->infd,
        "HTTP/1.1 501 Not Implemented\r\nContent-Length: %d\r\n\r\nNot Implemented\n", 16);
    return EXIT_FAILURE;
}

int parse_http_request(Request *requestObj, char *dataBuffer, ssize_t bufferSize) {
    int accumulated_offset = 0;
    regex_t regexPattern;
    regmatch_t patternMatches[4];
    int result;

    // Compile and match the request regex
    result = regcomp(&regexPattern, REQEX, REG_EXTENDED);
    result = regexec(&regexPattern, dataBuffer, 4, patternMatches, 0);

    if (result == 0) {
        requestObj->command = dataBuffer;
        requestObj->target_path = dataBuffer + patternMatches[2].rm_so;
        requestObj->version = dataBuffer + patternMatches[3].rm_so;

        dataBuffer[patternMatches[1].rm_eo] = '\0';
        requestObj->target_path[patternMatches[2].rm_eo - patternMatches[2].rm_so] = '\0';
        requestObj->version[patternMatches[3].rm_eo - patternMatches[3].rm_so] = '\0';

        dataBuffer += patternMatches[3].rm_eo + 2;
        accumulated_offset += patternMatches[3].rm_eo + 2;
    } else {
        dprintf(requestObj->infd,
            "HTTP/1.1 400 Bad Request\r\nContent-Length: %d\r\n\r\nBad Request\n", 12);
        regfree(&regexPattern);
        return EXIT_FAILURE;
    }

    requestObj->content_length = -1;
    result = regcomp(&regexPattern, HEADEX, REG_EXTENDED);
    result = regexec(&regexPattern, dataBuffer, 3, patternMatches, 0);

    while (result == 0) {
        dataBuffer[patternMatches[1].rm_eo] = '\0';
        dataBuffer[patternMatches[2].rm_eo] = '\0';

        if (strcmp(dataBuffer, "Content-Length") == 0) {
            int value = strtol(dataBuffer + patternMatches[2].rm_so, NULL, 10);
            if (errno == EINVAL) {
                dprintf(requestObj->infd,
                    "HTTP/1.1 400 Bad Request\r\nContent-Length: %d\r\n\r\nBad Request\n", 12);
            }
            requestObj->content_length = value;
        }

        dataBuffer += patternMatches[2].rm_eo + 2;
        accumulated_offset += patternMatches[2].rm_eo + 2;
        result = regexec(&regexPattern, dataBuffer, 3, patternMatches, 0);
    }

    if ((result != 0) && (dataBuffer[0] == '\r' && dataBuffer[1] == '\n')) {
        requestObj->message_body = dataBuffer + 2;
        accumulated_offset += 2;
        requestObj->remaining_bytes = bufferSize - accumulated_offset;
    } else if (result != 0) {
        dprintf(requestObj->infd,
            "HTTP/1.1 400 Bad Request\r\nContent-Length: %d\r\n\r\nBad Request\n", 12);
        regfree(&regexPattern);
        return EXIT_FAILURE;
    }

    regfree(&regexPattern);
    return EXIT_SUCCESS;
}
