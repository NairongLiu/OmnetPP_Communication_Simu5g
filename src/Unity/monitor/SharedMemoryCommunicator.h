#ifndef SHAREDMEMORYCOMMUNICATOR_H
#define SHAREDMEMORYCOMMUNICATOR_H

#include <windows.h>
#include <string>

class SharedMemoryCommunicator {
public:
    SharedMemoryCommunicator(const std::string& name);
    ~SharedMemoryCommunicator();

    void writeData(const std::string& data);
    std::string readData();

private:
    HANDLE hMapFile;
    char* pBuf;
    const size_t BUFFER_SIZE = 256;
};

#endif // SHAREDMEMORYCOMMUNICATOR_H
