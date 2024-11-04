#include "StatsLogger.h"

void StatsLogger::logMacDelayToFile(double MacDelay) {
    std::ofstream outputFile("MacLayerDelay_output.txt", std::ios::trunc);
    if (outputFile.is_open()) {
        outputFile << "MacLayerDelay: " << MacDelay << std::endl;
        outputFile.close();
    }
}

void StatsLogger::logMacPacketLossToFile(double MacPacketLoss) {
    std::ofstream outputFile("MACPacketLoss_output.txt", std::ios::trunc);
    if (outputFile.is_open()) {
        outputFile << "MACPacketLoss: " << MacPacketLoss << std::endl;
        outputFile.close();
    }
}
