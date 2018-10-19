/*********************************************************************************
* CSCI262 (Spring 2018) - Assignment 03
* Pooh Bear Intrusion Detection System Helper.h
* Purpose: main() driver for implementation of specifications
*
* @version 0.6-dev
* @date 2018.10.06
*
* @authors Dinh Che (codeninja55) & Duong Le (daltonle)
* Emails andrew at codeninja55.me & duong.daltonle at gmail.com
* Students Dinh Che (5721970 | dbac496) & Duong Le (5560536 | ndl991)
*********************************************************************************/

#ifndef POOH_BEAR_INTRUSION_DETECTION_SYSTEM_HELPER_H
#define POOH_BEAR_INTRUSION_DETECTION_SYSTEM_HELPER_H

#include <limits.h>
#include <stdlib.h>
#include <set>
#include <string>
#include <random>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <iostream>
using namespace std;

typedef double simtime_t;

/*
 * SimTime structure to hold the time values. Based heavily on C stdlib tm struct.
 * REF: http://www.cplusplus.com/reference/ctime/tm/
 * */
typedef struct SimTime {
    int tm_sec = 0;      // seconds after the minute [0-59] (not accommodating for leap seconds)
    int tm_min = 0;      // minutes after the hour [0-59]
    int tm_hour = 0;     // hours after midnight [0-23]
    int tm_mday = 0;     // day of the month | [1-31]
    int tm_mon = 0;      // months since January [0-11]
    int tm_year = 1900;  // years since 1900
    simtime_t tm_timestamp = 0;    // timestamp since 0 time of simulation

    SimTime() : tm_sec(0), tm_min(0), tm_hour(0), tm_mday(1), tm_mon(1), tm_year(2018), tm_timestamp(0) {}
    SimTime(const SimTime &ST) = default;  // trivial copy constructor

    /*
     * @brief: takes a timestamp value and converts it to a SimTime time.
     *
     * @param timestamp: double value to be converted to a SimTIme struct.
     * */
    void mktime(simtime_t timestamp)
    {
        this->tm_hour = static_cast<int>(lround(timestamp)) / 60 / 60;
        this->tm_min = static_cast<int>(lround(timestamp)) / 60 % 60;
        this->tm_sec = static_cast<int>(lround(timestamp)) % 60;
        this->tm_timestamp = timestamp;
    }

    /*
     * @brief: get a simtime_t type timestamp in seconds.
     *
     * @return: a simetime_t double timestamp in seconds.
     * */
    simtime_t get_timestamp()
    {
        return (this->tm_hour * 60 * 60) + (this->tm_min * 60) + (this->tm_sec);
    }

    /*
     * Constructor using a formatted string of time and date
     * Format: '<DD-MM-YYYY HH:MM:ss>'
     */
    explicit SimTime(const string &time_str)
    {
        this->tm_mday = stoi(time_str.substr(1,2));
        this->tm_mon = stoi(time_str.substr(4,2));
        this->tm_year = stoi(time_str.substr(7,4));
        this->tm_hour = stoi(time_str.substr(12,2));
        this->tm_min = stoi(time_str.substr(15,2));
        this->tm_sec = stoi(time_str.substr(18,2));
    }

    /*
    * Convert a SimTime struct to a human-readable string time.
    *
    * @param t: a reference to the SimTime structure to format to a string.
    * @return: string of the time from the SimTime structure.
    * */
    string formatted_time() const
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
    string formatted_date() const
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
    string formatted_time_date() const
    {
        stringstream ss;
        ss << "<" << setfill('0') << setw(2) << this->tm_mday << "-" << this->tm_mon << "-" << setw(4) << this->tm_year
           << " " << setfill('0') << setw(2) << this->tm_hour << ":"
           << setfill('0') << setw(2) << this->tm_min << ":"
           << setfill('0') << setw(2) << this->tm_sec << ">";
        return ss.str();
    }

    /*
     * Compare to another SimTime struct
     *
     * @param: other: a SimTime struct
     * @return:
     *     0 if equal
     *     -1 if this SimTime happens before the other
     *     1 if this SimTime happens after the other
     */
    int compare(const SimTime &other) const
    {
        if (compare_date(other) > 0)
            return 1;
        else if (compare_date(other) < 0)
            return -1;
        else if (this->tm_hour < other.tm_hour)
            return -1;
        else if (this->tm_hour > other.tm_hour)
            return 1;
        else if (this->tm_min < other.tm_min)
            return -1;
        else if (this->tm_min > other.tm_min)
            return 1;
        else if (this->tm_sec < other.tm_sec)
            return -1;
        else if (this->tm_sec > other.tm_sec)
            return 1;
        else return 0;
    }

    /*
     * Compare to another SimTime struct based on date only
     *
     * @param: other: a SimTime struct
     * @return:
     *     0 if equal
     *     -1 if this SimTime happens before the other
     *     1 if this SimTime happens after the other
     */
    int compare_date(SimTime other) const
    {
        if (this->tm_year < other.tm_year)
            return -1;
        else if (this->tm_year > other.tm_year)
            return 1;
        else if (this->tm_mon < other.tm_mon)
            return -1;
        else if (this->tm_mon > other.tm_mon)
            return 1;
        else if (this->tm_mday < other.tm_mday)
            return -1;
        else if (this->tm_mday > other.tm_mday)
            return 1;
        else return 0;
    }

    /*
     * Find the difference between SimTimes in seconds,
     * given SimTimes are in the same day
     *
     * @param: other: another SimTime
     * @return: the difference of this SimTime and the other SimTime in seconds
     */
    // TODO: code for SimTimes of different dates
    int diff(SimTime other) const
    {
        SimTime larger;
        SimTime smaller;
        int tmp = compare(other);
        if (tmp == 0)
            return 0;
        else if (tmp > 0) {
            larger = SimTime(*this);
            smaller = SimTime(other);
        }
        else {
            smaller = SimTime(*this);
            larger = SimTime(other);
        }

        int hour_diff = larger.tm_hour - smaller.tm_hour;
        return (3600 * hour_diff + (60*larger.tm_min + larger.tm_sec)
                                 - (60*smaller.tm_min + smaller.tm_sec));
    }

    /*
     * Increase the time to 00:00:00 the next day
     */
    void next_day()
    {
        switch(tm_mon) {
            case 1: case 3: case 5:
            case 7: case 8: case 10:
                if (this->tm_mday < 31)
                    this->tm_mday++;
                else {
                    this->tm_mday = 1;
                    this->tm_mon++;
                }
                break;
            case 12:
                if (this->tm_mday < 31)
                    this->tm_mday++;
                else {
                    this->tm_mday = 1;
                    this->tm_mon = 1;
                    this->tm_year++;
                }
                break;
            case 4: case 6: case 9: case 11:
                if (this->tm_mday < 30)
                    this->tm_mday++;
                else {
                    this->tm_mday = 1;
                    this->tm_mon++;
                }
                break;
            case 2:
                int last_day;
                bool leap;
                if (this->tm_year % 4 == 0) {
                    if (this->tm_year % 100 == 0) leap = this->tm_year % 400 == 0;
                    else leap = true;
                } else leap = false;

                if (leap) last_day = 29;
                else last_day = 28;
                if (this->tm_mday < last_day)
                    this->tm_mday++;
                else {
                    this->tm_mday = 1;
                    this->tm_mon++;
                }
                break;
            default: break;
        }

        // reset hours
        this->tm_hour = 0;
        this->tm_min = 0;
        this->tm_sec = 0;
    }
} SimTime;

enum EVENT_TYPE { ARRIVAL = 1, DEPART_SIDE_ROAD, DEPART_END_ROAD, PARKING_START, VEHICLE_MOVE };
typedef enum EVENT_TYPE EVENT_TYPE;

typedef struct VehicleStats {
    string veh_name, registration_id = "";
    SimTime arrival_time, departure_time = SimTime();
    simtime_t arrival_timestamp, departure_timestamp = 0;
    int n_parking = 0;
    vector<simtime_t> ts_parking_ls;
    vector<simtime_t> ts_parking_duration;
    vector<pair<simtime_t, double> > ts_parking_times;
    double arrival_speed, avg_speed = 0;
    double prob_parking, prob_side_exit, prob_end_exit = 0;
    double estimated_travel_delta = 0;  // TODO:
    bool side_exit_flag = false;

    // default constructor for VehicleStats
    VehicleStats() : veh_name(""), registration_id(""), arrival_time(SimTime()),
                     departure_time(SimTime()), arrival_speed(0),
                     prob_parking(0), prob_side_exit(0), prob_end_exit(0),
                     avg_speed(0), side_exit_flag(false), n_parking(0),
                     estimated_travel_delta(0), arrival_timestamp(0),
                     departure_timestamp(0) { }
} VehicleStats;

enum EVENT_TYPE { ARRIVAL = 1, DEPART_SIDE_ROAD, DEPART_END_ROAD, PARKING_START, VEHICLE_MOVE };
typedef enum EVENT_TYPE EVENT_TYPE;

typedef struct Event {
    EVENT_TYPE ev_type;
    SimTime time;
    VehicleStats *stats;

    Event(EVENT_TYPE ev_type, SimTime time, VehicleStats *stats) : ev_type(ev_type), time(time), stats(stats) {};
    Event(const Event &other) = default; // trivial copy constructor
    /*
     * @brief: overload required to maintain only a reference pointer to stats rather than a new object every time.
     *
     * @return: pointer to this struct.
     * */
    Event& operator=(const Event& rhs)
    {
        if (&rhs == this)
            return *this;

        this->ev_type = rhs.ev_type;
        this->time = rhs.time;
        stats = rhs.stats;
        return *this;
    }

} Event;

struct event_compare {
    bool operator()(const Event &lhs, const Event &rhs) {
        if (rhs.time.tm_hour == lhs.time.tm_hour) {
            return (rhs.time.tm_min == lhs.time.tm_min) ? rhs.time.tm_sec < lhs.time.tm_sec :
                   rhs.time.tm_min < lhs.time.tm_min;
        }
        return rhs.time.tm_hour < lhs.time.tm_hour;
    }
};

template<typename T>
double mean(vector<T>& vector1) {
    T sum;
    unsigned int count = (int) vector1.size();
    for (int i = 0; i < count; i++) {
        sum += vector1[i];
    }
    return (sum / count);
};

template<typename T>
double std_dev(vector<T>& vector1)
{
    double mean = mean(vector1);
    T sum;
    unsigned int count = (int) vector1.size();
    for (int i = 0; i < count; i++)
        sum += (vector1[i] - mean)*(vector1[i] - mean);

    return sqrt(sum / count);
}

unsigned int safe_int_convert(const char *, const char *);
SimTime initialise_time();
string real_formatted_time_now();
string event_name(EVENT_TYPE);
EVENT_TYPE event_type(string evt_name);
long long int fact(int x);
bool is_dir_exists(const char *pathname);
double exponential_probability(float lambda, int t);
double poisson_probability(float mu, int X);
void generate_distribution_csv(default_random_engine randomEngine);

#endif //POOH_BEAR_INTRUSION_DETECTION_SYSTEM_HELPER_H
