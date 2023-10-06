#ifndef _HTTP_H_
#define _HTTP_H_

    #define METHOD_GET 1
    #define METHOD_POST 2
    #define METHOD_PUT 3
    #define METHOD_PATCH 4
    #define METTHOD_DELETE 5

    struct HTTPRequest {
        int method;
        int path_len;
        char *path;
        int full_request_len;
        char *full_request;
    };
    typedef struct HTTPRequest HTTPRequest;

    extern int http_listen_and_serve(int port, char *(*request_handler)(HTTPRequest *));
#endif