#ifndef MAC_STATS_LOGGER_H
#define MAC_STATS_LOGGER_H

#include <omnetpp.h>
#include <fstream>

class StatsLogger {
public:
    static void logMacDelayToFile(double MacDelay);
    static void logMacPacketLossToFile(double MacPacketLoss);
};

#endif // MAC_STATS_LOGGER_H
