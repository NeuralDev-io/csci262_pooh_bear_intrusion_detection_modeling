//
// Created by lnduo on 17/10/18.
//

#include <utility>

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
    string veh_name;
    vector<unsigned long> volume_dist;
    vector<double> speed_dist;

    AnalysisStats(): veh_name(""), volume_dist(vector<unsigned long>()),
                     speed_dist(vector<double>()) {}

    AnalysisStats(string name): veh_name(name), volume_dist(vector<unsigned long>()),
                                speed_dist(vector<double>()) {}
} AnalysisStats;

typedef struct DailyStats {
    string veh_name;
    unsigned long volume;     // number of vehicles today
    vector<double> speed;
    SimTime date;

    // default constructor for AnalysisStats
    DailyStats(): veh_name(""), volume(0),
                  speed(vector<double>()), date(SimTime()) {}

    DailyStats(string name): veh_name(std::move(name)), volume(0),
                             speed(vector<double>()), date(SimTime()) {}
} DailyStats;

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
    void run(Vehicles &vehicles_dict);
private:
    void import_vehicles(Vehicles&);
    void read_log();
    void process_log();
    void add_speeding(VehicleStats&);
    void end_day();
    void end_analysis();
    map<string, DailyStats> today_stats;
    map<string, AnalysisStats> total_stats;
    map<string, VehicleStats> curr_vehicles;    // current vehicles on the road
    vector<VehicleStats> speeding_tickets;      // speeding tickets
    int day_count;
    float road_length, speed_limit;
    Logger<SimTime, AnalysisLog> logger;
    queue<string> activity_logs;
    string log_file, data_file, stats_file;
};

#endif //POOH_BEAR_INTRUSION_DETECTION_SYSTEM_ANALYSISENGINE_H
