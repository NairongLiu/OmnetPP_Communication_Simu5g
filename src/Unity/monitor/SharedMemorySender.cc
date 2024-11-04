#include "SharedMemorySender.h"

SharedMemorySender::SharedMemorySender(const std::string& name) : sharedMemoryName(name) {
    initializeSharedMemory();
}

void SharedMemorySender::initializeSharedMemory() {
    hMapFile = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, BUFFER_SIZE, sharedMemoryName.c_str());
    if (hMapFile == NULL) {
        std::cerr << "Could not create file mapping object (" << GetLastError() << ")\n";
        pBuf = nullptr;
        return;
    }

    pBuf = static_cast<char*>(MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, BUFFER_SIZE));
    if (pBuf == NULL) {
        std::cerr << "Could not map view of file (" << GetLastError() << ")\n";
        CloseHandle(hMapFile);
        hMapFile = NULL;
    }
}

SharedMemorySender::~SharedMemorySender() {
    if (pBuf != NULL) {
        UnmapViewOfFile(pBuf);
        pBuf = NULL;
    }
    if (hMapFile != NULL) {
        //CloseHandle(hMapFile);
        //hMapFile = NULL;
    }
}

bool SharedMemorySender::isMemoryAccessible() const {
    return (hMapFile != NULL && pBuf != NULL);
}

void SharedMemorySender::writeData(const std::string& data) {
    if (!isMemoryAccessible()) {
        std::cerr << "Shared memory not accessible, attempting reinitialization." << std::endl;
        initializeSharedMemory();
        if (!isMemoryAccessible()) {
            std::cerr << "Failed to reinitialize shared memory." << std::endl;
            return;
        }
    }

    if (data.size() < BUFFER_SIZE) {
        memcpy(pBuf, data.c_str(), data.size() + 1);
    } else {
        std::cerr << "Data too large for shared memory" << std::endl;
    }
}

std::string SharedMemorySender::readData() {
    if (!isMemoryAccessible()) {
        std::cerr << "Shared memory not accessible, attempting reinitialization." << std::endl;
        initializeSharedMemory();
        if (!isMemoryAccessible()) {
            std::cerr << "Failed to reinitialize shared memory." << std::endl;
            return "";
        }
    }

    return std::string(pBuf);
}
