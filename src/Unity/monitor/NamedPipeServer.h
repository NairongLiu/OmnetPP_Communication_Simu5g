#ifndef NAMEDPIPESERVER_H
#define NAMEDPIPESERVER_H

#include <windows.h>
#include <string>
#include <stdexcept>

class NamedPipeServer {
private:
    HANDLE hPipe;

public:
    NamedPipeServer(const std::string& pipeName);
    ~NamedPipeServer();

    void createPipe();
    void waitForClient();
    void writeData(const std::string& data);
    void closePipe();
};

#endif // NAMEDPIPESERVER_H
