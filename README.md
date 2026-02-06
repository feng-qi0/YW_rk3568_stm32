# Web服务器项目

## 项目简介
这是一个基于C语言开发的轻量级HTTP服务器程序，专门用于处理GET请求。服务器监听80端口，能够响应静态HTML页面请求以及提供REST API接口服务。

## 功能特性
- 支持标准HTTP GET请求处理
- 提供静态HTML页面服务（如index.html）
- 实现REST API接口，包括：
  - `/api/status` - 返回传感器状态信息
  - `/api/camera/snapshot` - 摄像头拍照
  - `/api/camera/start_record` - 开始录像
  - `/api/camera/stop_record` - 停止录像
  - `/api/camera/start_stream` - 开始视频流
  - `/api/camera/stop_stream` - 停止视频流
- 多线程处理并发请求
- 标准HTTP响应码支持（200, 404等）

## 系统要求
- 操作系统：Linux/macOS (或Windows WSL)
- 编译器：GCC
- 构建工具：Make
- 库依赖：pthread库

## 项目结构
```
.
├── GET_request_parsing.c/h     # HTTP请求解析模块
├── GET_response.c/h           # HTTP响应生成模块
├── Server_Get_80.c            # 主服务器程序（监听80端口）
├── Makefile                   # 构建脚本
├── compile.bat                # Windows批处理编译脚本
├── compile.ps1                # Windows PowerShell编译脚本
├── bin/                       # 编译输出目录
│   ├── index.html             # 示例HTML页面
│   └── server                 # 可执行服务器程序
└── README.md                  # 项目说明文档
```

## 快速开始

### 1. 克隆项目
```bash
git clone -b <分支名> --single-branch <仓库地址>
```

### 2. 编译项目
```bash
# 清理旧文件
make clean

# 编译服务器程序
make
```

### 3. 运行服务器
```bash
# 启动服务器（可能需要sudo权限，因为监听80端口）
sudo ./bin/server
```

### 4. 访问服务
- 访问主页：http://ip 或 http://ip/index.html

## 使用说明

### 添加新的GET请求处理
要添加新的GET请求路径处理，请按以下步骤操作：

1. 在`GET_response.h`中声明处理函数
2. 在`GET_response.c`中实现处理函数
3. 在`GET_request_parsing.c`的`handle_request`函数中添加路径匹配逻辑

### 示例：添加新API端点
```c
// 在GET_response.h中添加
void path_api_new_endpoint(int client_fd);

// 在GET_response.c中实现
void path_api_new_endpoint(int client_fd) {
    const char *response = "HTTP/1.1 200 OK\r\n"
                          "Content-Type: application/json\r\n"
                          "Content-Length: 13\r\n"
                          "Connection: close\r\n"
                          "\r\n"
                          "{\"status\":\"ok\"}";
    write(client_fd, response, strlen(response));
}

// 在GET_request_parsing.c的handle_request函数中添加
else if(strcmp(path, "/api/new_endpoint") == 0) {
    path_api_new_endpoint(client_fd);
}
```

## 错误处理
- 未知路径会返回404 Not Found响应
- 服务器内部错误会返回500 Internal Server Error响应
- 网络错误会在控制台输出错误信息

## 安全注意事项
- 由于服务器监听80端口，运行时可能需要管理员权限
- 该服务器仅适用于开发和测试环境，不建议在生产环境中使用
- 未实现输入验证和安全过滤，可能存在安全风险

## 维护与扩展
- 代码采用模块化设计，便于维护和扩展
- 支持多线程并发处理，提高性能
- 易于添加新的API端点和功能

## 故障排除

### 编译问题
- 确保已安装GCC编译器和Make工具
- 检查是否安装了pthread库

### 运行问题
- 端口被占用：确认80端口未被其他服务占用
- 权限不足：尝试使用sudo运行服务器程序
- 文件路径错误：确保index.html等资源文件位于正确位置

## 许可证
此项目为学习用途，遵循MIT许可证。