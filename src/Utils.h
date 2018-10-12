/*********************************************************************************
* CSCI262 (Spring 2018) - Assignment 03
* Pooh Bear Intrusion Detection System Helper.h
* Purpose: main() driver for implementation of specifications
*
* @version 0.1-dev
* @date 2018.10.06
*
* @authors Dinh Che (codeninja55) & Duong Le (daltonle)
* Emails andrew at codeninja55.me & duong.daltonle at gmail.com
* Students Dinh Che (5721970 | dbac496) & Duong Le (5560536 | ndl991)
*********************************************************************************/

#ifndef POOH_BEAR_INTRUSION_DETECTION_SYSTEM_HELPER_H
#define POOH_BEAR_INTRUSION_DETECTION_SYSTEM_HELPER_H

#include <set>
#include <string>
#include <random>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <iostream>
using namespace std;

/*
 * SimTime structure to hold the time values. Based heavily on C stdlib tm struct.
 * REF: http://www.cplusplus.com/reference/ctime/tm/
 * */
typedef struct {
    int tm_sec;   // seconds after the minute [0-59] (not accomodating for leap seconds)
    int tm_min;   // minutes after the hour [0-59]
    int tm_hour;  // hours after midnight [0-23]
    int tm_mday;  // day of the month | [1-31]
    int tm_mon;   // months since January [0-11]
    int tm_year;  // years since 1900
    int tm_wday;  // days since Sunday (0) [0-6]
} SimTime;

enum EVENT_TYPE { UNKNOWN = 0, ARRIVAL = 1, DEPART_SIDE_ROAD, DEPART_END_ROAD, PARKING_START, PARKING_STOP,
        VEHICLE_MOVE };
typedef enum EVENT_TYPE EVENT_TYPE;

SimTime time_now();
string formatted_time(SimTime&);
string formatted_date(SimTime&);
string formatted_time_date(SimTime&);
string real_formatted_time_now();
string event_name(EVENT_TYPE);
long long int fact(int x);
long double next_arrival(double rate_param,
                         uniform_real_distribution<long double> &random,
                         default_random_engine &random_generator);
double exponential_probability(float lambda, int t);
double poisson_probability(float mu, int X);
void generate_distribution_csv(default_random_engine randomEngine);

#endif //POOH_BEAR_INTRUSION_DETECTION_SYSTEM_HELPER_H
