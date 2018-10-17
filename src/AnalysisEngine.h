//
// Created by lnduo on 17/10/18.
//

#ifndef POOH_BEAR_INTRUSION_DETECTION_SYSTEM_ANALYSISENGINE_H
#define POOH_BEAR_INTRUSION_DETECTION_SYSTEM_ANALYSISENGINE_H

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <queue>
#include "Utils.h"
#include "Vehicles.h"
#include "Logger.h"
using namespace std;

typedef struct AnalysisStats {
    int day_count;
    map<string, unsigned long> day_volume;     // number of vehicles today
    map<string, unsigned long> total_volume;   // total number of vehicles
    map<string, vector<double>> speed;
    map<string, VehicleStats> curr_vehicles;        // current vehicles on the road
    vector<VehicleStats> speeding_tickets;

    // default constructor for AnalysisStats
    AnalysisStats(): day_count(0), day_volume(map<string, unsigned long>()),
                     total_volume(map<string, unsigned long>()),
                     speed(map<string, vector<double>>()),
                     speeding_tickets(vector<VehicleStats>()) {}
} AnalysisStats;

typedef struct AnalysisLog {
    string ev_type = "NOTICE";
    string log_name = "Analysis Log";
    string msg = "";

    AnalysisLog(string ev_type, string log_name, string msg) : ev_type(ev_type),
    log_name(log_name),
    msg(msg) { }

    friend ostream& operator<<(ostream& os, AnalysisLog const& activity_log)
    {
        return os << activity_log.log_name << DELIMITER << activity_log.ev_type << DELIMITER
                  << activity_log.msg;
    }
} AnalysisLog;

class AnalysisEngine {
public:
    AnalysisEngine();   // default
    AnalysisEngine(string log_file);
    void run(Vehicles vehicles_dict, char* filename);
private:
    void import_vehicles(Vehicles);
    void read_log(string log_file);
    void process_log();
    void add_speeding(VehicleStats, double speed);
    void end_day();
    void end_analysis();
    map<string, VehicleStats> curr_vehicles;
    AnalysisStats stats;
    float road_length, speed_limit;
    Logger<SimTime, AnalysisLog> logger;
    queue<string> activity_logs;
};

#endif //POOH_BEAR_INTRUSION_DETECTION_SYSTEM_ANALYSISENGINE_H
