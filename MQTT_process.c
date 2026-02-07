#include "MQTT_process.h"

//定义MQTT客户端结构体指针
struct mosquitto *mosq = NULL;

//定义一个变量用于表示消息回复状态
/* volatile */int message_reply = 0;
//定义一个变量用于消息回复内容
char message_reply_content[1024];

//MQTT连接回调函数
void on_connect(struct mosquitto *mosq, void *obj, int rc)
{
    if (rc == 0)
    {
        printf("MQTT连接成功\n");
        //订阅主题
        mosquitto_subscribe(mosq, NULL, MQTTMESSAGE_API, 0);
    }
    else
    {
        printf("MQTT连接失败，错误码: %d\n", rc);
    }
}

//MQTT消息回调函数
void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg)
{
    if (msg->payloadlen)
    {
        printf("收到MQTT消息: %s\n", msg->payload);
    }
}

//初始化
void mqtt_init()
{
    mosquitto_lib_init();

    //创建MQTT客户端实例
    mosq = mosquitto_new(MQTT_CLIENT_ID, true, NULL);
    if (mosq == NULL)
    {
        printf("Failed to create MQTT client\n");
        return;
    }   

    //设置回调函数
    mosquitto_connect_callback_set(mosq, on_connect);
    mosquitto_message_callback_set(mosq, on_message);

    //连接到MQTT服务器
    int rc = mosquitto_connect(mosq, MQTT_HOST, MQTT_PORT, 60);
    if (rc != MOSQ_ERR_SUCCESS)
    {
        printf("Failed to connect to MQTT broker\n");
        mosquitto_destroy(mosq);
        return;
    }

    //循环处理网络事件，保持连接状态
    while (true)
    {
        //处理网络事件，保持连接状态
        mosquitto_loop(mosq, -1, 1);
    }
}

/*
    @param topic 主题
    @param message 消息内容
*/
//发送MQTT消息
void send_mqtt_command(const char* topic, const char* message)
{
    //发布消息到指定主题
    int rc = mosquitto_publish(mosq, NULL, topic, strlen(message), message, 0, false);

    if (rc != MOSQ_ERR_SUCCESS)
    {
        printf("Failed to publish MQTT message\n");
        return;
    }else{
        printf("MQTT消息发送成功\n");
    }
}

