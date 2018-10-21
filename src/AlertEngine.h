#include <utility>

/*********************************************************************************
* CSCI262 (Spring 2018) - Assignment 03
* Pooh Bear Intrusion Detection System AlertEngine.h
* Purpose: Header file for AlertEngine class.
*
* @version 0.7-dev
* @date 2018.10.21
*
* @authors Dinh Che (codeninja55) & Duong Le (daltonle)
* Emails andrew at codeninja55.me & duong.daltonle at gmail.com
* Students Dinh Che (5721970 | dbac496) & Duong Le (5560536 | ndl991)
*********************************************************************************/

#ifndef POOH_BEAR_INTRUSION_DETECTION_SYSTEM_ALERTENGINE_H
#define POOH_BEAR_INTRUSION_DETECTION_SYSTEM_ALERTENGINE_H

#include <string>
#include <map>
#include "Logger.cpp"
#include "Vehicles.h"
#include "Utils.h"
using namespace std;

typedef struct VehicleTypeStats {
    string veh_type = "";
    double volume_mean, speed_mean = 0;
    double volume_weight, speed_weight = 0;

    VehicleTypeStats(string veh_name, double volume_mean,
                     double speed_mean, double volume_weight,
                     double speed_weight) : veh_type(veh_name), volume_mean(volume_mean),
                                            speed_mean(speed_mean), volume_weight(volume_weight),
                                            speed_weight(speed_weight) { }
} VehicleTypeStats;


typedef struct DailyAlertStats {
    SimTime day = init_time_date();
    double daily_volume_anomaly_ctr = 0;
    double daily_speed_anomaly_ctr = 0;
    vector<VehicleTypeStats> vehicles_ls;

    DailyAlertStats(SimTime day) : day(day), daily_volume_anomaly_ctr(0), daily_speed_anomaly_ctr(0),
                              vehicles_ls(vector<VehicleTypeStats>()) { }
} DailyAlertStats;


typedef struct AlertLog {
    string ev_type = "ALERT";
    string log_name = "Alert Log";
    string msg = "";

    AlertLog(string msg) : msg(move(msg)) { }

    friend ostream& operator<<(ostream& os, AlertLog const &alert_log)
    {
        return os << alert_log.log_name << DELIMITER << alert_log.ev_type
                  << DELIMITER << alert_log.msg;
    }
} AlertLog;

class AlertEngine {
public:
    AlertEngine(string user_stats, string log_filename);
    void run(Vehicles &vehicles, int &days);
private:
    void read_data(SimTime &sim_time, Vehicles &vehicles);
    void process_data(SimTime &sim_time, Vehicles &vehicles);
    Logger<SimTime, AlertLog> alert_logger;
    map<SimTime, DailyAlertStats> data_map;
    string log_file, data_baseline, user_stats_file;
    const string stats_baseline = "stats_baseline";
    double volume_anomaly_threshold;
    double speed_anomaly_threshold;
};

#endif //POOH_BEAR_INTRUSION_DETECTION_SYSTEM_ALERTENGINE_H
