#include "GET_response.h"
#include <sys/sendfile.h>

//====================外部函数定义====================
// 404响应
void path_404(int client_fd)
{
    const char *response = "HTTP/1.1 404 Not Found\r\n"
                          "Content-Type: text/plain\r\n"
                          "Content-Length: 9\r\n"
                          "Connection: close\r\n"
                          "\r\n"
                          "Not Found";
    if(write(client_fd, response, strlen(response)) < 0)
    {
        perror("write failed");
    }
}

// 首页响应
void path_index(int client_fd)
{
    // 尝试打开index.html文件
    FILE *file = fopen("index.html", "r");
    if (file == NULL) {
        // 如果index.html不存在，尝试index.htm
        file = fopen("index.htm", "r");
    }
    
    if (file == NULL) {
        // 如果都没有找到，返回默认页面
        const char *default_html = "<!DOCTYPE html>\n"
                                  "<html>\n"
                                  "<head>\n"
                                  "    <title>Web服务器首页</title>\n"
                                  "</head>\n"
                                  "<body>\n"
                                  "    <h1>欢迎访问Web服务器</h1>\n"
                                  "    <p>找不到index.html文件，这是默认页面。</p>\n"
                                  "    <ul>\n"
                                  "        <li><a href=\"/api/status\">API状态</a></li>\n"
                                  "    </ul>\n"
                                  "</body>\n"
                                  "</html>";
        
        char response[1024];
        int body_length = strlen(default_html);
        int header_length = snprintf(response, sizeof(response),
                                    "HTTP/1.1 200 OK\r\n"
                                    "Content-Type: text/html\r\n"
                                    "Content-Length: %d\r\n"
                                    "Connection: close\r\n"
                                    "\r\n",
                                    body_length);
        strncat(response, default_html, sizeof(response) - header_length - 1);

        if(write(client_fd, response, strlen(response)) < 0)
        {
            perror("write failed");
        }
        return;
    }
    
    // 获取文件大小
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);
    
    // 分配内存存储文件内容
    char *file_content = malloc(file_size + 1);
    if (file_content == NULL) {
        fclose(file);
        // 内存分配失败，返回错误页面
        const char *error_response = "HTTP/1.1 500 Internal Server Error\r\n"
                                    "Content-Type: text/plain\r\n"
                                    "Content-Length: 22\r\n"
                                    "Connection: close\r\n"
                                    "\r\n"
                                    "Internal Server Error";
        if(write(client_fd, error_response, strlen(error_response)) < 0)
        {
            perror("write failed");
        }
        return;
    }
    
    // 读取文件内容
    size_t bytes_read = fread(file_content, 1, file_size, file);
    fclose(file);
    file_content[bytes_read] = '\0'; // 确保字符串结尾
    
    // 构造HTTP响应
    char *response = malloc(512 + bytes_read); // 为HTTP头和内容分配空间
    if (response == NULL) {
        free(file_content);
        // 内存分配失败
        const char *error_response = "HTTP/1.1 500 Internal Server Error\r\n"
                                    "Content-Type: text/plain\r\n"
                                    "Content-Length: 22\r\n"
                                    "Connection: close\r\n"
                                    "\r\n"
                                    "Internal Server Error";
        if(write(client_fd, error_response, strlen(error_response)) < 0)
        {
            perror("write failed");
        }
        return;
    }
    
    int header_length = snprintf(response, 512,
                                "HTTP/1.1 200 OK\r\n"
                                "Content-Type: text/html\r\n"
                                "Content-Length: %ld\r\n"
                                "Connection: close\r\n"
                                "\r\n",
                                bytes_read);
    
    // 复制响应头和内容
    memcpy(response + header_length, file_content, bytes_read);
    
    // 发送响应
    if(write(client_fd, response, header_length + bytes_read) < 0)
    {
        perror("write failed");
    }

    // 释放内存
    free(file_content);
    free(response);
}

//API响应
/*
    路径
    /api/status
*/
//返回传感器状态信息
void path_api_status(int client_fd)
{
    //响应逻辑
    //测试案例:
    char response[512];
    //构建响应体
    const char *json_response = "{\"temp\":25.6,\"humi\":60.3,\"light\":300,\"ir\":1500,\"led_on\":1,\"led_br\":75,\"motor_on\":0,\"motor_sp\":0,\"motor_dir\":0,\"buzzer\":0}";
    int body_length = strlen(json_response);

    //构建响应头
    snprintf(response, sizeof(response),
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: %d\r\n"
            "Connection: close\r\n"
            "\r\n",
            body_length);

    //将响应体添加到响应头中
    strcat(response, json_response);

    if(write(client_fd, response, strlen(response)) < 0)
    {
        perror("write failed");
    }
}

//API响应
/*
    路径
    /api/camera/snapshot
*/
//摄像头拍照
void path_api_camera_snapshot(int client_fd)
{
    //响应逻辑
}

//API响应
/*
    路径
    /api/camera/stop_record
*/
//停止录像
void path_api_camera_stop_record(int client_fd)
{
    //响应逻辑
}

//API响应
/*
    路径
    /api/camera/start_record
*/
//开始录像
void path_api_camera_start_record(int client_fd)
{
    //响应逻辑
}

//API响应
/*
    路径
    /api/camera/stop_stream
*/
//停止视频流
void path_api_camera_stop_stream(int client_fd)
{
    //响应逻辑
}

//API响应
/*
    路径
    /api/camera/start_stream
*/
//开始视频流
void path_api_camera_start_stream(int client_fd)
{
    //响应逻辑
}
//=====================内部函数定义====================

