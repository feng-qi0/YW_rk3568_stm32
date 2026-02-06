@echo off
REM Batch script to compile C files to bin directory

REM Check if gcc is available
where gcc >nul 2>nul
if %errorlevel% neq 0 (
    echo Error: gcc compiler not found. Please install MinGW-w64 or TDM-GCC.
    exit /b 1
)

REM Create bin directory if it doesn't exist
if not exist bin mkdir bin

REM Compile the source files
echo Compiling GET_request_parsing.c...
gcc -Wall -Wextra -std=c99 -g -c GET_request_parsing.c -o GET_request_parsing.o

if %errorlevel% neq 0 (
    echo Error compiling GET_request_parsing.c
    exit /b 1
)

echo Compiling Server_Get_80.c...
gcc -Wall -Wextra -std=c99 -g -c Server_Get_80.c -o Server_Get_80.o

if %errorlevel% neq 0 (
    echo Error compiling Server_Get_80.c
    exit /b 1
)

echo Linking objects to create server executable...
gcc GET_request_parsing.o Server_Get_80.o -o bin\server -lpthread

if %errorlevel% neq 0 (
    echo Error linking objects
    exit /b 1
)

REM Clean up object files
del GET_request_parsing.o Server_Get_80.o

echo Compilation completed successfully. Server executable is in bin\server