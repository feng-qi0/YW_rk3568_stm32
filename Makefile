# Ubuntu环境下的Makefile for Web服务器项目

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g -pthread
LIBS = -lpthread -lmosquitto  # Mosquitto MQTT库依赖
SRCDIR = .
BINDIR = bin

# 自动查找当前目录下的所有.c文件
SOURCES := $(wildcard $(SRCDIR)/*.c)
OBJECTS := $(SOURCES:.c=.o)
TARGET := $(BINDIR)/server

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS) | $(BINDIR)
	$(CC) $(OBJECTS) -o $@ $(LIBS)

$(BINDIR):
	mkdir -p $(BINDIR)

# 编译.c文件为.o文件的规则
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 显式依赖关系，确保正确的编译顺序
GET_request_parsing.o: GET_request_parsing.c GET_request_parsing.h GET_response.h MQTT_process.h
GET_response.o: GET_response.c GET_response.h MQTT_process.h
MQTT_process.o: MQTT_process.c MQTT_process.h
Server_Get_80.o: Server_Get_80.c GET_request_parsing.h

clean:
	rm -f *.o
	rm -f $(TARGET)

# 保留bin目录中的HTML文件和其他资源
.PRECIOUS: $(BINDIR)/%.html