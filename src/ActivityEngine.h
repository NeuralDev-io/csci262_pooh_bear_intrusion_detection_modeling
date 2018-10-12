#include <utility>

/*********************************************************************************
* CSCI262 (Spring 2018) - Assignment 03
* Pooh Bear Intrusion Detection System ActivityEngine.h
* Purpose: Header file for ActivityEngine class.
*
* @version 0.3-dev
* @date 2018.10.06
*
* @authors Dinh Che (codeninja55) & Duong Le (daltonle)
* Emails andrew at codeninja55.me & duong.daltonle at gmail.com
* Students Dinh Che (5721970 | dbac496) & Duong Le (5560536 | ndl991)
*********************************************************************************/

#ifndef POOH_BEAR_INTRUSION_DETECTION_SYSTEM_ACTIVITYENGINE_H
#define POOH_BEAR_INTRUSION_DETECTION_SYSTEM_ACTIVITYENGINE_H

#include <zconf.h>
#include <queue>
#include <iostream>
#include <random>
#include <fstream>
#include <math.h>
#include <cmath>
#include <iomanip>
#include <chrono>
#include "Utils.h"
#include "Vehicles.h"
#include "Logger.h"
using namespace std;

/*
 * @brief: a wrapper structure that is passed into the logger object to be printed.
 *         As a requirement, it must overload the << operator so when printing, the
 *         desired properties are printed as required.
 * */
typedef struct ActivityLog {
    EVENT_TYPE ev_type = UNKNOWN;
    string log_name = "Activity Log";
    float speed = 0;
    string msg = "";

    ActivityLog(EVENT_TYPE ev_type,
                string log_name,
                float speed,
                string msg) : ev_type(ev_type),
                              log_name(std::move(log_name)),
                              speed(speed),
                              msg(std::move(msg)) { }

    friend ostream& operator<<(ostream& os, ActivityLog const& activity_log)
    {
        return os << activity_log.log_name << ":" << event_name(activity_log.ev_type) << ":" << activity_log.msg
                  << ":" << activity_log.speed;
    }
} ActivityLog;

typedef struct VehicleStats {
    string registration_id;
    SimTime arrival_time;
    double arrival_speed;
    float prob_parking, prob_side_exit, prob_end_exit;

    VehicleStats() : registration_id(""), arrival_time(SimTime()), arrival_speed(0), prob_parking(0), prob_side_exit(0),
                     prob_end_exit(0) {}
} VehicleStats;

typedef struct {
    EVENT_TYPE ev_type;
    SimTime time;
    VehicleStats stats;
} Event;

struct event_compare {
    bool operator()(const Event &lhs, const Event &rhs) {
        if (lhs.time.tm_hour == rhs.time.tm_hour) {
            return (lhs.time.tm_min == rhs.time.tm_min) ? lhs.time.tm_sec < rhs.time.tm_sec :
                lhs.time.tm_min < rhs.time.tm_min;
        }
        return lhs.time.tm_hour < rhs.time.tm_hour;
    }
};

class ActivityEngine {
public:
    ActivityEngine() : n_vehicles_monitored(0), n_parking_spots(0), road_length(0),
                       speed_limit(0), simulate_days(0), time_seed(0) {}; // default
    ActivityEngine(uint days, uint vehicles_monitored, float road_len, float speed_lim, uint parking_spots);
    void run(Vehicles&);  // run the activity engine simulation
private:
    void generate_arrivals(Vehicles &vehicles);
    void simulate_events();
    long double next_arrival(double rate_param, uniform_real_distribution<double> random);
    unsigned long time_seed;
    default_random_engine default_engine;
    minstd_rand0 linear_congruential_engine;
    mt19937_64 mersenne_twister_engine;
    uint n_vehicles_monitored, n_parking_spots, simulate_days;
    float road_length, speed_limit;
    priority_queue<Event, vector<Event>, event_compare> event_q;
};

#endif //POOH_BEAR_INTRUSION_DETECTION_SYSTEM_ACTIVITYENGINE_H
