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
typedef struct SimTime {
    int tm_sec;   // seconds after the minute [0-59] (not accomodating for leap seconds)
    int tm_min;   // minutes after the hour [0-59]
    int tm_hour;  // hours after midnight [0-23]
    int tm_mday;  // day of the month | [1-31]
    int tm_mon;   // months since January [0-11]
    int tm_year;  // years since 1900
    int tm_wday;  // days since Sunday (0) [0-6]

    SimTime() : tm_sec(0), tm_min(0), tm_hour(0), tm_mday(1), tm_mon(0), tm_year(1018), tm_wday(0) {}

    /*
    * Convert a SimTime struct to a human-readable string time.
    *
    * @param t: a reference to the SimTime structure to format to a string.
    * @return: string of the time from the SimTime structure.
    * */
    string formatted_time()
    {
        stringstream ss;
        ss << "<" << setfill('0') << setw(2) << this->tm_hour << ":" << setfill('0') << setw(2) << this->tm_min
           << ":" << setfill('0') << setw(2) << this->tm_sec << ">";
        return ss.str();
    }

    /*
     * Convert a SimTime struct to a human-readable string date
     *
     * @param t: a reference to the SimTime structure to format to a string.
     * @return: string of the time from the SimTime structure.
     * */
    string formatted_date()
    {
        stringstream ss;
        ss << "<" << setfill('0') << setw(2) << this->tm_mday << "-" << setfill('0') << setw(2) << this->tm_mon
           << "-" << setw(4) << this->tm_year << ">";
        return ss.str();
    }

    /*
     * @brief: convert a SimTime struct to a human-readable string time and date
     *
     * @param t: a reference to the SimTime structure to format to a string
     * @return: string of the time and date formatted as DD-MM-YYYY HH:MM:ss
     * */
    string formatted_time_date() {
        stringstream ss;
        ss << "<" << setfill('0') << setw(2) << this->tm_mday << "-" << this->tm_mon << "-" << setw(4) << this->tm_year
           << " " << setfill('0') << setw(2) << this->tm_hour << ":"
           << setfill('0') << setw(2) << this->tm_min << ":"
           << setfill('0') << setw(2) << this->tm_sec << ">";
        return ss.str();
    }
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
double exponential_probability(float lambda, int t);
double poisson_probability(float mu, int X);
void generate_distribution_csv(default_random_engine randomEngine);

#endif //POOH_BEAR_INTRUSION_DETECTION_SYSTEM_HELPER_H
