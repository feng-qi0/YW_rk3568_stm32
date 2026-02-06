#ifndef GET_RESPONSE_H
#define GET_RESPONSE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <ctype.h>
#include <errno.h>

//响应客户端
void path_api_status(int client_fd);
void path_404(int client_fd);
void path_index(int client_fd);
void path_helloworld(int client_fd);

#endif //GET_RESPONSE_H