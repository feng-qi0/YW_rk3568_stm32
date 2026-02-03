@echo off
echo 正在构建 RK3568 Qt 项目...

REM 设置 Qt 环境变量 (根据你的 Qt 安装路径调整)
set QTDIR=C:\Qt\5.14.2\mingw73_64
set PATH=%QTDIR%\bin;%PATH%

REM 检查 qmake 是否可用
where qmake >nul 2>nul
if %errorlevel% neq 0 (
    echo 错误: 找不到 qmake，请确保 Qt 已正确安装并添加到 PATH
    pause
    exit /b 1
)

REM 创建构建目录
if not exist build mkdir build
cd build

REM 运行 qmake 生成 Makefile
qmake ..\YW_rk3568_client\YW_rk3568_client.pro -spec win32-g++ "CONFIG+=debug"

if %errorlevel% neq 0 (
    echo 错误: qmake 执行失败
    pause
    exit /b 1
)

REM 编译项目
mingw32-make -f Makefile.Debug -j4

if %errorlevel% neq 0 (
    echo 错误: 编译失败
    pause
    exit /b 1
)

echo.
echo 项目构建成功！
echo 可执行文件位于 build/debug/YW_rk3568_client.exe
echo.
pause