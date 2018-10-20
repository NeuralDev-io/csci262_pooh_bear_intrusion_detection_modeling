#include <utility>

/*********************************************************************************
* CSCI262 (Spring 2018) - Assignment 03
* Pooh Bear Intrusion Detection System AlertEngine.h
* Purpose: Header file for AlertEngine class.
*
* @version 0.6-dev
* @date 2018.10.20
*
* @authors Dinh Che (codeninja55) & Duong Le (daltonle)
* Emails andrew at codeninja55.me & duong.daltonle at gmail.com
* Students Dinh Che (5721970 | dbac496) & Duong Le (5560536 | ndl991)
*********************************************************************************/

#ifndef POOH_BEAR_INTRUSION_DETECTION_SYSTEM_ALERTENGINE_H
#define POOH_BEAR_INTRUSION_DETECTION_SYSTEM_ALERTENGINE_H

#include <string>
#include "Logger.h"
#include "Vehicles.h"
#include "Utils.h"

using namespace std;

typedef struct AlertStats {
    SimTime day;
    double volume_anomaly_ctr_threshold;
    double speed_anomaly_ctr_threshold;
    double daily_volume_anomaly_ctr;
    double daily_speed_anomaly_ctr;
} AlertStats;


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
    explicit AlertEngine(string log_file, string data_file);
    void run(Vehicles &vehicles, int &days);
private:
    Logger<SimTime, AlertLog> alert_logger;
    string log_file;
    string data_file;
};

#endif //POOH_BEAR_INTRUSION_DETECTION_SYSTEM_ALERTENGINE_H
