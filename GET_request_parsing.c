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
