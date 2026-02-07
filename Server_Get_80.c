#include "GET_request_parsing.h"
#define PORT 80

int main() 
{
    //初始化MQTT
    mqtt_init();

    //创建套接字
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd <= 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    //创建地址结构
    struct sockaddr_in address;
    //清空地址结构，防止残留数据影响后续操作
    memset(&address, 0, sizeof(address));
    //填充地址结构
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    //绑定地址到套接字
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) 
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    //监听
    if (listen(server_fd, 10) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    //接受连接,使用多线程
    int num = 0;      //线程编号
    pthread_t tid;    //PID
    while (1) {
        //创建客户端地址结构
        struct sockaddr_in client;
        //获取客户端地址长度
        socklen_t len = sizeof(client);

        //接受连接
        int client_fd = accept(server_fd, (struct sockaddr *)&client, &len);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        //打印连接信息
        printf("客户端连接来自: %s\n", inet_ntoa(client.sin_addr));

        //     //处理客户端get请求
        //     parse_get(client_fd);
            
        //     // 关闭客户端连接
        //     close(client_fd);

        //创建线程处理客户端请求
        if (pthread_create(&tid, NULL, parse_get, &client_fd) != 0) {
            perror("pthread create failed");
        }
        num++;
    }
}