//
// Created by lnduo on 17/10/18.
//

#ifndef POOH_BEAR_INTRUSION_DETECTION_SYSTEM_ANALYSISENGINE_H
#define POOH_BEAR_INTRUSION_DETECTION_SYSTEM_ANALYSISENGINE_H

#include <iostream>
#include <fstream>
#include <map>
#include "Utils.h"
#include "Vehicles.h"
#include "Logger.h"
using namespace std;

typedef struct {
    
} AnalysisStats;

class AnalysisEngine {
public:
    AnalysisEngine();
    AnalysisEngine(char* filename);

private:
    map<string, VehicleType> curr_vehicles;
};

#endif //POOH_BEAR_INTRUSION_DETECTION_SYSTEM_ANALYSISENGINE_H
