#include "GET_request_parsing.h"

//解析GET请求
void* parse_get(void *arg) {

    //获取客户端的socket文件描述符
    int client_fd = *(int *)arg;

    //创建一个缓冲区
    char buffer[BUFFER_SIZE] = {0};
    
    //读取客户端发送的请求，内容存入buffer中
    ssize_t bytes_read = read(client_fd, buffer, BUFFER_SIZE);
    if (bytes_read < 0) {
        perror("read failed");
        return NULL;
    }

    //解析请求
    //检查请求类型是否为GET
    if(strncmp(buffer, "GET ", 4) != 0){
        //不是GET请求
        printf("Not a GET request\n");
        return NULL;
    }

     printf("GET request for path: %s\n", buffer);

    // get请求报文格式
    // GET /cs453/index.html HTTP/1.1
    // Host: gaia.cs.umass.edu
    // User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.7.2) Gecko/20040804 Netscape/7.2 (ax)
    // Accept: ext/xml, application/xml, application/xhtml+xml, text/html;q=0.9, text/plain;q=0.8, image/png, */*;q=0.5
    // Accept-Language: en-us,en;q=0.5
    // Accept-Encoding: zip,deflate
    // Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7
    // Keep-Alive: 300
    // Connection: keep-alive
    
    //解析请求路径
    //第一个空格的位置的后面就是请求路径
    char *path_start = strchr(buffer, ' ') + 1;
    //第二个空格的位置就是请求路径的结束位置
    char *path_end = strchr(path_start, ' ');
    char path[256] = {0};
    
    if(path_end == NULL){
        //请求路径不正确
        printf("Invalid request path\n");
        return NULL;
    }

    //获取请求路径的长度
    long unsigned int path_len = path_end - path_start;
    //如果请求路径过长，则截断
    if(path_len >= sizeof(path)){
        path_len = sizeof(path) - 1;
    }
    //复制请求路径
    strncpy(path, path_start, path_len);

    //根据get请求的路径，处理get请求
    handle_request(client_fd, path);

    close(client_fd);

    return NULL;
}


//=============================
//根据get请求的路径，处理get请求
void handle_request(int client_fd, char *path)
{
    //匹配路径/api/status
    if(strcmp(path, "/api/status") == 0)
    {
        //匹配成功,响应客户端
        path_api_status(client_fd);
    }
    //匹配路径/index.html 或者 路径为空 或者 路径为/
    else if(strcmp(path, "") == 0 || strcmp(path,"/") == 0 || strcmp(path,"/index.html") == 0)
    {
        //响应html页面
        path_index(client_fd);
    }
    else {
        // 对于未处理的路径，返回404
        path_404(client_fd);
    }
}

//=============================
//响应客户端
/*
    路径
    /api/status
*/
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
