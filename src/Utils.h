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

#ifdef __linux__
static const char dir_slash = '/';
#elif _linux_
static const char dir_slash = '/';
#elif _WIN32
static const char dir_slash = '\\';
#elif _WIN64
static const char dir_slash = '\\';
#else
static const char dir_slash = '/';
#endif

typedef double simtime_t;

// const auto SYSTEM_SEED = static_cast<unsigned long>(std::chrono::system_clock::now().time_since_epoch().count());
const unsigned long SYSTEM_SEED = 0;

// bool DEBUG_MODE = true;
const static std::string LOGS_FILENAME = "logs_baseline"; // NOLINT(cert-err58-cpp)
const double T_ARRIVAL_LIMIT = (22.5F * 60 * 60);           // Set the limit for time last car to arrive
const double T_PARKING_LIMIT = (23.5F * 60 * 60);           // Set the limit for time last car to park
const double T_DAY_LIMIT = (24.0 * 60.0F * 60.0F) - 1.0F;   // Set the limit for last time for events to occur
const double DEPART_SIDE_PROBABILITY = 0.1;                // Set the probability for the bernoulli distribution
const double DEPART_SIDE_UPPER_BOUND = 0.02;                // Upper bound value to be used in biased expovariate function
const double PARKING_PROBABILITY = 0.1;                    // Set the probability for parking for the binomial distribution
const double AVG_PARKING_DURATION = (10.0 * 60);

static int FILENAME_COUNTER = 0;

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
     * @brief: overloaded operator to be used in ordered containers.
     * */
    friend bool operator<(const SimTime &lhs, const SimTime &rhs)
    {
        if (rhs.tm_year == lhs.tm_year) {
            if (rhs.tm_mon == lhs.tm_mon) {
                if (rhs.tm_mday == lhs.tm_mday) {
                    if (rhs.tm_hour == lhs.tm_hour)
                        return (rhs.tm_min == lhs.tm_min) ? rhs.tm_sec < lhs.tm_sec : rhs.tm_min < lhs.tm_min;
                    return rhs.tm_hour < lhs.tm_hour;
                }
                return rhs.tm_mday < lhs.tm_mday;
            }
            return rhs.tm_mon < lhs.tm_mon;
        }
        return rhs.tm_year < lhs.tm_year;
    }

    /*
     * @brief: constructor using a formatted string of time and date
     *
     * @param time_date_str: a c++ string with format '<DD-MM-YYYY HH:MM:ss>'
     */
    explicit SimTime(const string &time_date_str) {
        this->tm_mday = stoi(time_date_str.substr(1,2));
        this->tm_mon = stoi(time_date_str.substr(4,2));
        this->tm_year = stoi(time_date_str.substr(7,4));
        this->tm_hour = stoi(time_date_str.substr(12,2));
        this->tm_min = stoi(time_date_str.substr(15,2));
        this->tm_sec = stoi(time_date_str.substr(18,2));
    }

    /*
     * @brief: takes a timestamp value and converts it to a SimTime time.
     *
     * @param timestamp: double value to be converted to a SimTIme struct.
     * */
    void mktime(simtime_t timestamp) {
        this->tm_hour = static_cast<int>(lround(timestamp)) / 60 / 60;
        this->tm_min = static_cast<int>(lround(timestamp)) / 60 % 60;
        this->tm_sec = static_cast<int>(lround(timestamp)) % 60;
        this->tm_timestamp = timestamp;
    }

    /*
    * Convert a SimTime struct to a human-readable string time.
    *
    * @param t: a reference to the SimTime structure to format to a string.
    * @return: string of the time from the SimTime structure.
    * */
    string formatted_time() const {
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
    string formatted_date() const {
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
    string formatted_time_date() const {
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
    int compare(const SimTime &other) const {
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
    int compare_date(SimTime other) const {
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
    int diff(SimTime other) const {
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
    void next_day() {
        switch(this->tm_mon) {
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

typedef struct VehicleStats {
    string veh_name, registration_id = "";
    SimTime arrival_time, departure_time = SimTime();
    simtime_t arrival_timestamp, departure_timestamp = 0;
    int n_parking = 0;
    vector<simtime_t> ts_parking_ls;
    vector<simtime_t> ts_parking_duration;
    vector<double> speed_vehicle_move;
    vector<double> ts_vehicle_move_ls;
    
    double arrival_speed, avg_speed = 0;
    double estimated_travel_delta, estimated_depart_timestamp = 0;  // TODO:
    bool depart_side_flag, permit_speeding_flag = false;

    // default constructor for VehicleStats
    VehicleStats() : veh_name(""), registration_id(""), arrival_time(SimTime()),
                     departure_time(SimTime()), arrival_speed(0),
                     avg_speed(0), depart_side_flag(false), n_parking(0),
                     estimated_travel_delta(0), arrival_timestamp(0),
                     departure_timestamp(0) { }
} VehicleStats;

enum EVENT_TYPE { UNKNOWN = 0, ARRIVAL = 1, DEPART_SIDE_ROAD, DEPART_END_ROAD, PARKING_START, VEHICLE_MOVE };
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

/*
 * Custom compare method for the FUTURE EVENT LIST
 * */
struct event_compare {
    bool operator()(const Event &lhs, const Event &rhs) {
        return (rhs.time.compare(lhs.time) == -1);
    }
};

template<typename T>
double mean(vector<T>& vector1) {
    T sum = 0;
    unsigned int count = (int) vector1.size();
    if (count == 0) return 0;
    sum += accumulate(next(vector1.begin()), vector1.end(), vector1[0]);
    return (sum / count);
};

template<typename T>
double std_dev(vector<T>& vector1)
{
    T sum = 0;
    unsigned int count = (int) vector1.size();
    if (count == 0) return 0;
    sum += accumulate(next(vector1.begin()), vector1.end(), vector1[0]);
    double mean = sum / count;
    sum = 0 ;

    for (int i = 0; i < count; i++)
        sum += ((vector1[i] - mean) * (vector1[i] - mean));
    return sqrt(sum / count);
}

unsigned int safe_int_convert(const char *, const char *);
SimTime init_time_date();
string real_formatted_time_now();
string event_name(EVENT_TYPE);
EVENT_TYPE event_type(string evt_name);
long long int fact(int x);
bool is_dir_exists(const char *pathname);
void console_log(string type, string msg);

#endif //POOH_BEAR_INTRUSION_DETECTION_SYSTEM_HELPER_H
