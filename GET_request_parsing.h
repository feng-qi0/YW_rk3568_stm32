#ifndef GET_REQUEST_PARSING_H
#define GET_REQUEST_PARSING_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define BUFFER_SIZE 8192

//解析GET请求
void parse_get(int client_fd);

#endif //GET_REQUEST_PARSING_H
