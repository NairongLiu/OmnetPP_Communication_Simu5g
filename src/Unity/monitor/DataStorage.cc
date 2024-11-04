#include "DataStorage.h"
#include <sstream>

std::map<std::string, std::string> DataStorage::dataStore;
std::map<std::string, std::string> DataStorage::receivedDataStore;
const std::string DataStorage::SHARED_MEMORY_SEND_NAME = "Omnetpp_SharedMemorySend";
const std::string DataStorage::SHARED_MEMORY_RECEIVE_NAME = "Omnetpp_SharedMemoryReceive";

SharedMemorySender DataStorage::sharedMemorySend(DataStorage::SHARED_MEMORY_SEND_NAME);
SharedMemoryReceiver DataStorage::sharedMemoryReceive(DataStorage::SHARED_MEMORY_RECEIVE_NAME);

void DataStorage::setData(const std::string& key, const std::string& value) {
    dataStore[key] = value;
}

std::string DataStorage::getData(const std::string& key) {
    if (dataStore.find(key) != dataStore.end()) {
        return dataStore[key];
    }
    return "";
}

std::string DataStorage::readMemory() {
    std::string memoryRead = sharedMemorySend.readData();
    return memoryRead;
}

void DataStorage::sendAllDataToSharedMemory() {
    std::string combinedData;
    for (const auto& pair : dataStore) {
        combinedData += pair.first + ": " + pair.second + "\n";
    }
    sharedMemorySend.writeData(combinedData);
}

void DataStorage::receiveDataFromSharedMemory() {
    std::string memoryData = sharedMemoryReceive.readData();
    std::istringstream stream(memoryData);
    std::string line;

    while (std::getline(stream, line)) {
        size_t delimiterPos = line.find(": ");
        if (delimiterPos != std::string::npos) {
            std::string key = line.substr(0, delimiterPos);
            std::string value = line.substr(delimiterPos + 2); // +2 to skip ": "
            receivedDataStore[key] = value;
        }
    }
}

std::string DataStorage::getReceivedData(const std::string& key) {
    if (receivedDataStore.find(key) != receivedDataStore.end()) {
        return receivedDataStore[key];
    }
    return "";
}
