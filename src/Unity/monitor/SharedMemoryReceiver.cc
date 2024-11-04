#include <windows.h>
#include <iostream>
#include <string>

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

SharedMemoryReceiver::SharedMemoryReceiver(const std::string& name)
    : sharedMemoryName(name), hMapFile(NULL), pBuf(NULL) {
    initializeSharedMemory();
}

SharedMemoryReceiver::~SharedMemoryReceiver() {
    if (pBuf != NULL) {
        UnmapViewOfFile(pBuf);
        pBuf = NULL;
    }
    if (hMapFile != NULL) {
        CloseHandle(hMapFile);
        hMapFile = NULL;
    }
}

void SharedMemoryReceiver::initializeSharedMemory() {
    hMapFile = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, sharedMemoryName.c_str());
    if (hMapFile == NULL) {
        std::cerr << "Could not open file mapping object (" << GetLastError() << ")\n";
        pBuf = nullptr;
        return;
    }

    pBuf = static_cast<char*>(MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0));
    if (pBuf == NULL) {
        std::cerr << "Could not map view of file (" << GetLastError() << ")\n";
        CloseHandle(hMapFile);
        hMapFile = NULL;
    }
}

bool SharedMemoryReceiver::isMemoryAccessible() const {
    return (hMapFile != NULL && pBuf != NULL);
}

std::string SharedMemoryReceiver::readData() {
    if (!isMemoryAccessible()) {
        std::cerr << "Shared memory not accessible, attempting reinitialization." << std::endl;
        initializeSharedMemory();
        if (!isMemoryAccessible()) {
            std::cerr << "Failed to reinitialize shared memory." << std::endl;
            return "";
        }
    }
    std::string data(pBuf);
    std::cout << "Read data from shared memory: " << data << std::endl;
    return data;
}
