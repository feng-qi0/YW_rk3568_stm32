# 编译器设置
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g -pthread -O2
LDFLAGS = -lpthread

# 源文件和目标文件
SOURCES = Server_Get_80.c GET_request_parsing.c
HEADERS = GET_request_parsing.h
OBJECTS = $(SOURCES:.c=.o)
TARGET_DIR = bin
TARGET = $(TARGET_DIR)/server

# 默认目标
all: $(TARGET)

# 创建目标目录
$(TARGET_DIR):
	@mkdir -p $(TARGET_DIR)

# 链接可执行文件
$(TARGET): $(OBJECTS) | $(TARGET_DIR)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

# 编译源文件为对象文件（考虑头文件依赖）
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# 清理生成的文件
clean:
	rm -f $(OBJECTS)
	rm -rf $(TARGET_DIR)

# 重新构建
rebuild: clean all

# 安装（可选）
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/server

# 运行服务器（在后台运行）
run: $(TARGET)
	@echo "启动服务器..."
	@$(TARGET) &

# 停止服务器
stop:
	@echo "停止服务器..."
	@pkill -f "server" || echo "服务器未运行或已停止"

# 测试服务器（发送一个简单的GET请求）
test: $(TARGET)
	@echo "测试服务器..."
	@$(TARGET) &
	@sleep 2
	@curl -v http://localhost:80/api/status || echo "请确保端口80可用且有curl命令"
	@pkill -f "server"

# 检查语法（不实际编译）
lint:
	$(CC) $(CFLAGS) -fsyntax-only $(SOURCES)

# 详细版本信息
version:
	@echo "服务器版本信息:"
	@echo "编译器: $(CC)"
	@echo "编译标志: $(CFLAGS)"
	@echo "链接标志: $(LDFLAGS)"
	@echo "源文件: $(SOURCES)"
	@echo "头文件: $(HEADERS)"

# 帮助信息
help:
	@echo "可用的目标:"
	@echo "  all      - 构建项目 (默认)"
	@echo "  clean    - 清理生成的文件"
	@echo "  rebuild  - 重新构建项目"
	@echo "  install  - 安装服务器到系统路径"
	@echo "  run      - 启动服务器（后台）"
	@echo "  stop     - 停止服务器"
	@echo "  test     - 测试服务器（需要curl）"
	@echo "  lint     - 检查语法错误"
	@echo "  version  - 显示版本信息"
	@echo "  help     - 显示此帮助信息"

.PHONY: all clean rebuild install run stop test lint version help