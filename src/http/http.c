#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <ctype.h>
#include <stdbool.h>
#include "http.h"

#define BUFFER_SIZE 1048

struct {
    int len;
    char *s;
} typedef StrLen;

StrLen *_get_http_path(char *req, int req_len) {
    bool is_in_path = false;
    int size;
    char *path_start;

    for (int i = 0; i < req_len; i++) {
        if (is_in_path) size++;

        if (req[i] == ' ') {
            if (is_in_path) {
                return &(StrLen) {
                    .len = size,
                    .s = path_start,
                };
            }

            is_in_path = true;
            printf("i : %d, sise req: %lu", i, sizeof(req));
            path_start = req + i;
        }
    }

    return NULL;
}

void http_handle_client(int client_fd, char *(*request_handler)(HTTPRequest *)) {
    char *request_buffer = (char *)malloc(BUFFER_SIZE * sizeof(char));
    int recived_bytes = recv(client_fd, request_buffer, BUFFER_SIZE, 0);
    if (recived_bytes == 0) {
        return;
    }

    HTTPRequest req;
    if (strncmp(request_buffer, "GET", sizeof("GET")-1) == 0) {
        req.method = METHOD_GET;
    } else if (strncmp(request_buffer, "PUT", sizeof("PUT")-1) == 0) {
        req.method = METHOD_PUT;
    } else if (strncmp(request_buffer, "POST", sizeof("POST")-1) == 0) {
        req.method = METHOD_POST;
    } else if (strncmp(request_buffer, "PATCH", sizeof("PATCH")-1) == 0) {
        req.method = METHOD_PATCH;
    }  else if (strncmp(request_buffer, "DELETE", sizeof("DELETE")-1) == 0) {
        req.method = METTHOD_DELETE;
    } else {
        printf("TODO, handle all http verbs\n");
        return;
    }
    req.full_request = request_buffer;
    req.full_request_len = recived_bytes;

    StrLen *path = _get_http_path(request_buffer, recived_bytes);
    if (path == NULL) {
        printf("TODO, bad req");
        return;
    }
    req.path = path->s;
    req.path_len = path->len;

    char *res = request_handler(&req);

    send(client_fd, res, strlen(res), 0);

    free(request_buffer);
    free(res); 
    return;
}

int http_listen_and_serve(int port, char *(*request_handler)(HTTPRequest *)) {
    // Create socket.
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        printf("error creating socket: %s\n", strerror(errno));
        return -1;
    }

    // Bind socket.
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(socket_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        printf("error binding socket: %s\n", strerror(errno));
        return -1;
    }
    
    // Listen to socket.
    if (listen(socket_fd, 10) < 0) {
        printf("error listeing on port %d: %s\n", port, strerror(errno));
        return -1;
    }
    
    // Accept requests.
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_fd = accept(socket_fd, (struct sockaddr *) &client_addr, &client_addr_len);
        if (client_fd < 0) {
            printf("error accepting from socket: %s\n", strerror(errno));
            continue;
        }

        http_handle_client(client_fd, request_handler);
    }
}
