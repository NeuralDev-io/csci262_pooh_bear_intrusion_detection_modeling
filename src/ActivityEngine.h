/*********************************************************************************
* CSCI262 (Spring 2018) - Assignment 03
* Pooh Bear Intrusion Detection System ActivityEngine.h
* Purpose: Header file for ActivityEngine class.
*
* @version 0.6-dev
* @date 2018.10.06
*
* @authors Dinh Che (codeninja55) & Duong Le (daltonle)
* Emails andrew at codeninja55.me & duong.daltonle at gmail.com
* Students Dinh Che (5721970 | dbac496) & Duong Le (5560536 | ndl991)
*********************************************************************************/

#include <utility>

#ifndef POOH_BEAR_INTRUSION_DETECTION_SYSTEM_ACTIVITYENGINE_H
#define POOH_BEAR_INTRUSION_DETECTION_SYSTEM_ACTIVITYENGINE_H

#include <iostream>
#include <string.h>
#include <algorithm>
#include <queue>
#include <random>
#include <fstream>
#include <math.h>
#include <iomanip>
#include <chrono>
#include "Utils.h"
#include "Vehicles.h"
#include "Logger.cpp"
using namespace std;

/*
 * @brief: a wrapper structure that is passed into the logger object to be printed.
 *         As a requirement, it must overload the << operator so when printing, the
 *         desired properties are printed as required.
 * */
typedef struct ActivityLog {
    string ev_type = "NOTICE";
    string log_name = "Activity Log";
    string msg = "";

    ActivityLog(string ev_type, string log_name, string msg) : ev_type(ev_type),
                                                               log_name(log_name),
                                                               msg(msg) { }

    friend ostream& operator<<(ostream& os, ActivityLog const& activity_log)
    {
        return os << activity_log.log_name << DELIMITER << activity_log.ev_type << DELIMITER
                  << activity_log.msg;
    }
} ActivityLog;

typedef struct VehicleLog {
    EVENT_TYPE ev_type = ARRIVAL;
    string log_name = "Vehicle Log";
    string veh_name = "";
    string veh_registration = "NA";
    double speed = 0;

    VehicleLog(EVENT_TYPE ev_type, string log_name, string veh_name,
                string veh_registration, double speed) : ev_type(ev_type),
                                                         log_name(std::move(log_name)),
                                                         veh_name(veh_name),
                                                         veh_registration(veh_registration),
                                                         speed(speed) { }

    friend ostream& operator<<(ostream& os, VehicleLog const& veh_log)
    {
        return os << veh_log.log_name << DELIMITER << event_name(veh_log.ev_type) << DELIMITER
                  << veh_log.veh_name << DELIMITER << veh_log.veh_registration << DELIMITER
                  << fixed << setprecision(2) << veh_log.speed;
    }
} VehicleLog;

typedef struct GenericLog {
    EVENT_TYPE ev_type = PARKING_START;
    string log_name = "Vehicle Log";
    string veh_name = "";
    string veh_registration = "NA";
    string additional_msg = "";  // must pass delimiter

    GenericLog(EVENT_TYPE ev_type, string log_name, string veh_name,
               string veh_registration) : ev_type(ev_type), log_name(std::move(log_name)),
                                          veh_name(veh_name), veh_registration(veh_registration) { }
    GenericLog(EVENT_TYPE ev_type, string log_name, string veh_name,
               string veh_registration, string additional_msg) : ev_type(ev_type), log_name(std::move(log_name)),
                                                                 veh_name(veh_name), veh_registration(veh_registration),
                                                                 additional_msg(additional_msg) { }

    friend ostream& operator<<(ostream& os, GenericLog const& log)
    {
        return os << log.log_name << DELIMITER << event_name(log.ev_type) << DELIMITER
                  << log.veh_name << DELIMITER << log.veh_registration << log.additional_msg;
    }
} GenericLog;

class ActivityEngine {
public:
    ActivityEngine(); // default
    explicit ActivityEngine(string log_file);
    void set_statistics(unsigned days, unsigned vehicles_monitored, float road_len,
                        float speed_lim, unsigned parking_spots);
    void run(Vehicles&);  // run the activity engine simulation
private:
    void start_generating_discrete_events(SimTime &sim_time, Vehicles &vehicles);
    void process_parking_events(SimTime &sim_time, bool parking_flag, VehicleStats *veh_stats);
    void process_departure_events(SimTime &sim_time, VehicleType &veh_type, VehicleStats *veh_stats);
    void simulate_events();
    long double biased_expovariate(double rate_param, double lower_bound, double upper_bound);
    double estimate_departure_time(VehicleStats &veh, simtime_t start_timestamp);
    double estimate_departure_delta(VehicleStats &veh, double speed);
    mt19937_64 mersenne_twister_engine;
    unsigned n_vehicles_monitored, n_parking_spots, simulate_days;
    float road_length, speed_limit;
    string log_file;
    Logger<SimTime, ActivityLog> logger;
    Logger<SimTime, VehicleLog> veh_logger;
    Logger<SimTime, GenericLog> other_veh_logger;
    priority_queue<Event, vector<Event>, event_compare> future_event_list;

};

#endif //POOH_BEAR_INTRUSION_DETECTION_SYSTEM_ACTIVITYENGINE_H
