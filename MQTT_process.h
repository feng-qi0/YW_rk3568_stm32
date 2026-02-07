#ifndef MQTT_PROCESS_H
#define MQTT_PROCESS_H
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
#include <mosquitto.h>

#define HTTP_PORT 8080
#define MQTT_HOST "127.0.0.1"
#define MQTT_PORT 1883
#define MQTT_CLIENT_ID "mqtt_client"  //客户端ID

//本机需要接收的主题
#define MQTTMESSAGE_API "/api_reply" //传感器数据回复主题

//=====================外部函数声明=====================

//初始化
void mqtt_init();

/*
    @param topic 主题
    @param message 消息内容
*/
//发送MQTT消息
void send_mqtt_command(const char* topic, const char* message);

#endif //MQTT_PROCESS_H
