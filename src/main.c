#include <stdio.h>
#include "./http/http.h"

#define PORT 8080

char *request_handler(HTTPRequest *r) {
    printf("method gotten %d\n", r->method);
    printf("path gotten: %.*s\n", r->path_len, r->path);
    char *res = "HTTP/1.1 200\nContent type: text/html\n\n<h1>oh yeah yeah</h1>";
    return res;
}

int main() {
    printf("running server...\n");
    http_listen_and_serve(PORT, request_handler);
    return 0;
}