#include "DataStorage.h"

std::map<std::string, std::string> DataStorage::dataStore;
const std::string DataStorage::SHARED_MEMORY_NAME = "Omnetpp_SharedMemory";
SharedMemoryCommunicator DataStorage::sharedMemoryCommunicator(DataStorage::SHARED_MEMORY_NAME);

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
    std::string memoryRead = sharedMemoryCommunicator.readData();
    return memoryRead;
}

void DataStorage::sendAllDataToSharedMemory() {
    std::string combinedData;
    for (const auto& pair : dataStore) {
        combinedData += pair.first + ": " + pair.second + "\n";
    }
    sharedMemoryCommunicator.writeData(combinedData);
}
