#ifndef SHAREDMEMORYRECEIVER_H
#define SHAREDMEMORYRECEIVER_H

#include <windows.h>
#include <string>
#include <iostream>

class SharedMemoryReceiver {
public:
    explicit SharedMemoryReceiver(const std::string& name);
    ~SharedMemoryReceiver();

    bool isMemoryAccessible() const;
    std::string readData();

private:
    void initializeSharedMemory();

    const std::string sharedMemoryName;
    HANDLE hMapFile;
    char* pBuf;
};

#endif
