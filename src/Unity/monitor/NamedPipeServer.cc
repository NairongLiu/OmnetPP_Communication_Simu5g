#include "NamedPipeServer.h"
#include <iostream>

NamedPipeServer::NamedPipeServer(const std::string& pipeName) {
    hPipe = CreateNamedPipe(
        ("\\\\.\\pipe\\" + pipeName).c_str(),
        PIPE_ACCESS_OUTBOUND,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        1,
        0,
        0,
        0,
        NULL
    );

    if (hPipe == INVALID_HANDLE_VALUE) {
        throw std::runtime_error("Error creating named pipe: " + std::to_string(GetLastError()));
    }
}

NamedPipeServer::~NamedPipeServer() {
    closePipe();
}

void NamedPipeServer::createPipe() {
    // Pipe creation logic handled in constructor
}

void NamedPipeServer::waitForClient() {
    if (!ConnectNamedPipe(hPipe, NULL)) {
        throw std::runtime_error("Error connecting to named pipe client: " + std::to_string(GetLastError()));
    }
}

void NamedPipeServer::writeData(const std::string& data) {
    DWORD bytesWritten;
    if (!WriteFile(hPipe, data.c_str(), data.size(), &bytesWritten, NULL)) {
        throw std::runtime_error("Error writing to named pipe: " + std::to_string(GetLastError()));
    }
}

void NamedPipeServer::closePipe() {
    if (hPipe != INVALID_HANDLE_VALUE) {
        CloseHandle(hPipe);
        hPipe = INVALID_HANDLE_VALUE;
    }
}
