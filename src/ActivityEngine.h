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


typedef struct {
    string registration_id;
    SimTime arrival_time;
    double arrival_speed;
    float prob_parking, prob_side_exit, prob_end_exit;
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
    ActivityEngine(uint, uint, float, float, uint);
    void run(Vehicles&);  // run the activity engine simulation
private:
    void generate_arrivals(Vehicles &vehicles);
    long double next_arrival(double rate_param,
                             uniform_real_distribution<double> &random,
                             mt19937_64 &mt_engine);
    unsigned long time_seed;
    default_random_engine default_engine;
    minstd_rand0 linear_congruential_engine;
    mt19937_64 mersenne_twister_engine;
    uint n_vehicles_monitored, n_parking_spots, simulate_days;
    float road_length, speed_limit;
    priority_queue<Event, vector<Event>, event_compare> event_q;
};

#endif //POOH_BEAR_INTRUSION_DETECTION_SYSTEM_ACTIVITYENGINE_H
