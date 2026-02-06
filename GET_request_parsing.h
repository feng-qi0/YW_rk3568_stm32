#ifndef GET_REQUEST_PARSING_H
#define GET_REQUEST_PARSING_H
#define BUFFER_SIZE 8192
#include "GET_response.h"

//=============对外提供的函数================
//解析GET请求
void* parse_get(void *arg);

//============内部使用的函数=================
//根据get请求的路径，处理get请求
void handle_request(int client_fd, char *path);

#endif //GET_REQUEST_PARSING_H