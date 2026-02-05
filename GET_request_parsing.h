#ifndef GET_REQUEST_PARSING_H
#define GET_REQUEST_PARSING_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define BUFFER_SIZE 8192


//=============对外提供的函数================
//解析GET请求
void* parse_get(void *arg);


//============内部使用的函数=================
//根据get请求的路径，处理get请求
void handle_request(int client_fd, char *path);

//响应客户端
void path_api_status(int client_fd);
void path_404(int client_fd);
void path_index(int client_fd);

#endif //GET_REQUEST_PARSING_H