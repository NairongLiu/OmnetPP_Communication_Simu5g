#ifndef SHAREDMEMORYHANDLER_H
#define SHAREDMEMORYHANDLER_H

#include <omnetpp.h>

#include "SharedMemoryHandler.h"

using namespace omnetpp;

class SharedMemoryHandler : public cSimpleModule {
protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;

private:
    cMessage *timer;
    const std::string sharedMemoryName = "SharedMemory_MacDelayUL";

    void sendAllData();
    void readData();
    void receiveData();
};

#endif
