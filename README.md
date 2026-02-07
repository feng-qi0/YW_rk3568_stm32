# RK3568 嵌入式设备网页控制系统

## 项目简介
这是一个基于C语言开发的嵌入式设备远程控制平台，通过网页界面实现对RK3568等嵌入式设备的远程控制。系统采用HTTP服务器作为前端控制界面，并通过MQTT协议与嵌入式设备进行双向通信，实现对传感器、执行器等外设的实时监控与控制。

## 功能特性
- **网页远程控制**：提供直观的Web界面，支持实时传感器数据显示和设备控制
- **MQTT通信**：通过MQTT协议与嵌入式设备进行可靠的消息传输
- **实时传感器监控**：显示温度、湿度、光照强度、红外检测等传感器数据
- **设备控制功能**：
  - LED照明灯开关及亮度调节
  - 直流电机启停、速度和方向控制
  - 蜂鸣器报警控制
  - 摄像头功能（拍照、录像、视频流）
- **实时视频监控**：支持HLS协议的实时视频流播放
- **多线程处理**：支持并发处理多个客户端请求

## 系统要求
- **操作系统**：Linux (推荐Ubuntu 18.04+)
- **编译器**：GCC
- **构建工具**：Make
- **库依赖**：pthread库、Mosquitto MQTT客户端库
- **运行环境**：需要MQTT Broker（如Mosquitto）服务

## 项目结构
```
.
├── GET_request_parsing.c/h     # HTTP请求解析模块
├── GET_response.c/h           # HTTP响应生成模块
├── MQTT_process.c/h           # MQTT通信处理模块
├── Server_Get_80.c            # 主服务器程序（监听80端口）
├── Makefile                   # 构建脚本
├── compile.bat                # Windows批处理编译脚本
├── compile.ps1                # Windows PowerShell编译脚本
├── bin/                       # 编译输出目录
│   ├── index.html             # 网页控制界面
│   └── server                 # 可执行服务器程序
├── README.md                  # 项目说明文档
└── ...
```

## 快速开始

### 1. 环境准备
```bash
# 安装依赖库
sudo apt-get update
sudo apt-get install libmosquitto-dev mosquitto libmosquitto-dev libssl-dev

# 启动MQTT Broker服务
sudo systemctl start mosquitto
sudo systemctl enable mosquitto
```

### 2. 克隆并编译项目
```bash
# 克隆项目
git clone -b YW_rk3568_server --single-branch https://github.com/feng-qi0/YW_rk3568_stm32.git
cd YW_rk3568_server

# 清理旧文件
make clean

# 编译服务器程序
make
```

### 3. 配置MQTT参数
编辑 `MQTT_process.h` 文件，根据实际环境修改MQTT服务器配置：
```c
#define MQTT_HOST "127.0.0.1"      // MQTT Broker地址
#define MQTT_PORT 1883             // MQTT Broker端口
```

### 4. 运行服务器
```bash
# 启动服务器（可能需要sudo权限，因为监听80端口）
sudo ./bin/server
```

### 5. 访问控制界面
- 打开浏览器访问：http://设备IP地址 或 http://设备IP地址/index.html
- 通过网页界面可实时查看传感器数据并控制设备外设

## MQTT通信协议

### 订阅主题
- `/api_reply` - 接收来自设备的数据反馈

### 发布主题
系统会根据不同的控制命令向相应主题发布消息：
- 设备控制命令通过MQTT发送到嵌入式设备
- 传感器数据通过MQTT从设备上报

## API接口

### GET接口
- `/api/status` - 获取设备状态和传感器数据
- `/api/camera/snapshot` - 触发摄像头拍照
- `/api/camera/start_record` - 开始录像
- `/api/camera/stop_record` - 停止录像
- `/api/camera/start_stream` - 开始视频流
- `/api/camera/stop_stream` - 停止视频流

### 数据格式
传感器数据以JSON格式返回：
```json
{
  "temp": 25.6,
  "humi": 60.3,
  "light": 300,
  "ir": 1500,
  "led_on": 1,
  "led_br": 75,
  "motor_on": 0,
  "motor_sp": 0,
  "motor_dir": 0,
  "buzzer": 0
}
```

## 使用说明

### 添加新的控制功能
要添加新的设备控制功能，请按以下步骤操作：

1. 在`GET_response.h`中声明处理函数
2. 在`GET_response.c`中实现处理函数
3. 在`GET_request_parsing.c`的`handle_request`函数中添加路径匹配逻辑
4. 如需MQTT通信，在`MQTT_process.c`中添加相应的消息发送/接收逻辑

## 错误处理
- 未知路径会返回404 Not Found响应
- 服务器内部错误会返回500 Internal Server Error响应
- MQTT连接失败会在控制台输出错误信息
- 网络错误会在控制台输出错误信息

## 安全注意事项
- 由于服务器监听80端口，运行时可能需要管理员权限
- 该系统仅适用于开发和测试环境，不建议在生产环境中直接使用
- 未实现完整的身份验证机制，部署时应考虑网络安全措施
- MQTT通信未加密，敏感数据传输需额外安全措施

## 维护与扩展
- 代码采用模块化设计，便于维护和扩展
- 支持多线程并发处理，提高性能
- 易于添加新的传感器和控制功能
- 可扩展支持更多设备类型和通信协议

## 故障排除

### 编译问题
- 确保已安装GCC编译器和Make工具
- 检查是否安装了pthread库和Mosquitto开发库

### 运行问题
- 端口被占用：确认80端口未被其他服务占用
- 权限不足：尝试使用sudo运行服务器程序
- MQTT连接失败：检查MQTT Broker是否正常运行
- 文件路径错误：确保index.html等资源文件位于正确位置

### 网络问题
- 确认设备间网络连通性
- 检查防火墙设置是否阻止了相关端口
- 验证MQTT Broker地址和端口配置是否正确
