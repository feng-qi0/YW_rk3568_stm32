# PowerShell script to compile C files to bin directory

# Check if gcc is available
try {
    $gcc_version = gcc --version 2>$null
    if (-not $gcc_version) {
        Write-Host "Error: gcc compiler not found. Please install MinGW-w64 or TDM-GCC." -ForegroundColor Red
        exit 1
    }
} catch {
    Write-Host "Error: gcc compiler not found. Please install MinGW-w64 or TDM-GCC." -ForegroundColor Red
    exit 1
}

# Create bin directory if it doesn't exist
if (!(Test-Path "bin")) {
    New-Item -ItemType Directory -Name "bin" | Out-Null
}

Write-Host "Compiling GET_request_parsing.c..." -ForegroundColor Green
gcc -Wall -Wextra -std=c99 -g -c GET_request_parsing.c -o GET_request_parsing.o

if ($LASTEXITCODE -ne 0) {
    Write-Host "Error compiling GET_request_parsing.c" -ForegroundColor Red
    exit 1
}

Write-Host "Compiling Server_Get_80.c..." -ForegroundColor Green
gcc -Wall -Wextra -std=c99 -g -c Server_Get_80.c -o Server_Get_80.o

if ($LASTEXITCODE -ne 0) {
    Write-Host "Error compiling Server_Get_80.c" -ForegroundColor Red
    exit 1
}

Write-Host "Linking objects to create server executable..." -ForegroundColor Green
gcc GET_request_parsing.o Server_Get_80.o -o bin\server -lpthread

if ($LASTEXITCODE -ne 0) {
    Write-Host "Error linking objects" -ForegroundColor Red
    exit 1
}

# Clean up object files
Remove-Item GET_request_parsing.o, Server_Get_80.o

Write-Host "Compilation completed successfully. Server executable is in bin\server" -ForegroundColor Green