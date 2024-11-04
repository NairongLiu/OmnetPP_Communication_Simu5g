#include "SharedMemoryCommunicator.h"
#include <iostream>

SharedMemoryCommunicator::SharedMemoryCommunicator(const std::string& name) {

    hMapFile = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, BUFFER_SIZE, name.c_str());
    if (hMapFile == NULL) {
        std::cerr << "Could not create file mapping object (" << GetLastError() << ")\n";
        return;
    } else {
    }

    pBuf = static_cast<char*>(MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, BUFFER_SIZE));
    if (pBuf == NULL) {
        std::cerr << "Could not map view of file (" << GetLastError() << ")\n";
        CloseHandle(hMapFile);
        return;
    } else {
    }
}

SharedMemoryCommunicator::~SharedMemoryCommunicator() {
    if (pBuf != NULL) {
        UnmapViewOfFile(pBuf);
    }
    if (hMapFile != NULL) {
        //CloseHandle(hMapFile);
    }
}

void SharedMemoryCommunicator::writeData(const std::string& data) {
    if (pBuf == NULL) {
        std::cerr << "Shared memory not initialized." << std::endl;
        return;
    }
    if (data.size() < 1024) {
        memcpy(pBuf, data.c_str(), data.size() + 1);

        std::string receivedData = readData();
    } else {
        std::cerr << "Data too large for shared memory" << std::endl;
    }
}

std::string SharedMemoryCommunicator::readData() {
    if (pBuf == NULL) {
        std::cerr << "Shared memory not initialized" << std::endl;
        return "";
    }
    std::string receivedData(pBuf);

    if (receivedData.empty()) {
        std::cout << "No data received from shared memory" << std::endl;
    } else {
    }

    return receivedData;
}
