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

//=====================外部函数声明=====================
//404错误响应
/*
    路径
    异常路径
*/
void path_404(int client_fd);


//首页响应
/*
    路径
    /index.html 或 / 或 空路径 
*/
void path_index(int client_fd);


//API响应
/*
    路径
    /api/status
*/
//获取传感器数据
void path_api_status(int client_fd);


//API响应
/*
    路径
    /api/camera/snapshot
*/
//摄像头拍照
void path_api_camera_snapshot(int client_fd);


//API响应
/*
    路径
    /api/camera/stop_record
*/
//停止录像
void path_api_camera_stop_record(int client_fd);


//API响应
/*
    路径
    /api/camera/start_record
*/
//开始录像
void path_api_camera_start_record(int client_fd);


//API响应
/*
    路径
    /api/camera/stop_stream
*/
//停止视频流
void path_api_camera_stop_stream(int client_fd);


//API响应
/*
    路径
    /api/camera/start_stream
*/
//开始视频流
void path_api_camera_start_stream(int client_fd);


//======================内部函数声明====================



#endif //GET_RESPONSE_H