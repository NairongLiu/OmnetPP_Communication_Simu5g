#ifndef DATASTORAGE_H
#define DATASTORAGE_H

#include <string>
#include <map>
#include "SharedMemoryCommunicator.h"

class DataStorage {
public:
    static void setData(const std::string& key, const std::string& value);
    static std::string getData(const std::string& key);
    static std::string readMemory();
    static void sendAllDataToSharedMemory();

private:
    static std::map<std::string, std::string> dataStore;
    static SharedMemoryCommunicator sharedMemoryCommunicator;
    static const std::string SHARED_MEMORY_NAME;
};

#endif
