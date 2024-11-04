#include "SharedMemoryHandler.h"
#include "DataStorage.h"

Define_Module(SharedMemoryHandler);

void SharedMemoryHandler::initialize() {
    timer = new cMessage("memorySendTimer");

    scheduleAt(simTime() + 0.1, timer);
}

void SharedMemoryHandler::handleMessage(cMessage *msg) {
    if (msg == timer) {
        sendAllData();

        readData();

        receiveData();

        scheduleAt(simTime() + 0.1, timer);
    }
}

void SharedMemoryHandler::sendAllData() {
    DataStorage::sendAllDataToSharedMemory();
}

void SharedMemoryHandler::receiveData() {
    DataStorage::receiveDataFromSharedMemory();
}

void SharedMemoryHandler::readData() {
    std::string receivedData = DataStorage::readMemory();
    if (!receivedData.empty()) {
        std::cout << "Data received from shared memory: " << receivedData << std::endl;
    } else {
        std::cout << "No data received from shared memory." << std::endl;
    }
}

void SharedMemoryHandler::finish() {
    cancelAndDelete(timer);
}
