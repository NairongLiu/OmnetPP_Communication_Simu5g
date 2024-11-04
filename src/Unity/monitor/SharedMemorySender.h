#ifndef SHAREDMEMORYCOMMUNICATOR_H
#define SHAREDMEMORYCOMMUNICATOR_H

#include <windows.h>
#include <string>
#include <iostream>

class SharedMemorySender {
public:
    explicit SharedMemorySender(const std::string& name);
    ~SharedMemorySender();

    void writeData(const std::string& data);
    std::string readData();
    bool isMemoryAccessible() const;

private:
    void initializeSharedMemory();
    HANDLE hMapFile;
    char* pBuf;
    const size_t BUFFER_SIZE = 1024;
    std::string sharedMemoryName;
};

#endif
