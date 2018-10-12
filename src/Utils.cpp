/*********************************************************************************
* CSCI262 (Spring 2018) - Assignment 03
* Pooh Bear Intrusion Detection System Helper.cpp
* Purpose: main() driver for implementation of specifications
*
* @version 0.1-dev
* @date 2018.10.06
*
* @authors Dinh Che (codeninja55) & Duong Le (daltonle)
* Emails andrew at codeninja55.me & duong.daltonle at gmail.com
* Students Dinh Che (5721970 | dbac496) & Duong Le (5560536 | ndl991)
*********************************************************************************/

#include "Utils.h"

/*
 * Initialise a sim_time struct to start the simulation with a date of when the sim
 * start relative to the real time.
 *
 * @return: the SimTime structure that was initialised.
 * */
SimTime time_now()
{
    // Get the real time based on system
    time_t raw_time = time(0);
    struct tm *real_time_now;
    time(&raw_time);
    real_time_now = localtime(&raw_time);

    // Create fake time based on simulation start date and starting time of 0
    SimTime time_now;

    time_now.tm_sec = time_now.tm_hour = time_now.tm_min = 0;
    time_now.tm_mday = real_time_now->tm_mday;
    time_now.tm_mon = real_time_now->tm_mon + 1;
    time_now.tm_year = real_time_now->tm_year + 1900;
    time_now.tm_wday = real_time_now->tm_wday;
    return time_now;
}

/*
 * Convert a SimTime struct to a human-readable string time.
 *
 * @param t: a reference to the SimTime structure to format to a string.
 * @return: string of the time from the SimTime structure.
 * */
string formatted_time(SimTime &t)
{
    stringstream ss;
    ss << setfill('0') << setw(2) << t.tm_hour << ":" << setfill('0') << setw(2) << t.tm_min
       << ":" << setfill('0') << setw(2) << t.tm_sec;
    return ss.str();
}

/*
 * Convert a SimTime struct to a human-readable string date
 *
 * @param t: a reference to the SimTime structure to format to a string.
 * @return: string of the time from the SimTime structure.
 * */
string formatted_date(SimTime &t)
{
    stringstream ss;
    ss << setfill('0') << setw(2) << t.tm_mday << "-" << setfill('0') << setw(2) << t.tm_mon << "-" << setw(4) << t
    .tm_year;
    return ss.str();
}

/*
 * @brief: convert a SimTime struct to a human-readable string time and date
 *
 * @param t: a reference to the SimTime structure to format to a string
 * @return: string of the time and date formatted as DD-MM-YYYY HH:MM:ss
 * */
string formatted_time_date(SimTime &t) {
    stringstream ss;
    ss << setfill('0') << setw(2) << t.tm_mday << "-" << t.tm_mon << "-" << setw(4) << t.tm_year
       << " " << setfill('0') << setw(2) << t.tm_hour << ":"
       << setfill('0') << setw(2) << t.tm_min << ":"
       << setfill('0') << setw(2) << t.tm_sec;
    return ss.str();
}

/*
 * Convert a real time tm struct and time_t to a local system time and return a string
 * of this time formatted just like the system time.
 *
 * @return: a string of the real time based on the system formatted.
 * */
string real_formatted_time_now()
{
    // Get the real time based on system
    time_t raw_time = time(0);
    struct tm *time_now;
    time(&raw_time);
    time_now = localtime(&raw_time);

    stringstream ss;
    ss << setfill('0') << setw(2) << time_now->tm_mday << "-" << time_now->tm_mon + 1
       << "-" << setw(4) << time_now->tm_year + 1900 << " " << setfill('0') << setw(2)
       << time_now->tm_hour << ":" << time_now->tm_min << ":" << time_now->tm_sec;
    return ss.str();
}

/*
 * @param ev: an enum EVENT
 * @return: a string representation of the enum EVENT.
 * */
string event_name(EVENT_TYPE ev) {
    switch (ev) {
        case ARRIVAL:
            return "ARRIVAL";
        case DEPART_SIDE_ROAD:
            return "DEPART_SIDE_ROAD";
        case DEPART_END_ROAD:
            return "DEPART_END_ROAD";
        case PARKING_START:
            return "PARKING_START";
        case PARKING_STOP:
            return "PARKING_STOP";
        case VEHICLE_MOVE:
            return "VEHICLE_MOVE";
        case UNKNOWN:
            return "UNKNOWN";
    }
}

long long int fact(int x)
{
    if (x == 0) return 1;
    long long int factorial = 1;
    int i;
    for (i = 2; i <= x; i++)
        factorial *= i;
    return factorial;
}

/*
 * @param lambda: the rate of occurrence
 * @param t: time interval to check probability
 * */
double exponential_probability(float lambda, int t)
{
    return lambda * pow(exp(1), (-lambda * t));
    // return 1.0f - pow(exp(1), (-lambda * t));
}

/*
 * @param mu: the expected occurrence over a time interval
 * @param X: the number of expected occurrences.
 * */
double poisson_probability(float mu, int X)
{
    return 1.0f - (pow(mu, X) / fact(X)) * pow(exp(1), (-mu));
}


void generate_distribution_csv(default_random_engine randomEngine)
{
    /* Bus generator test (num_mean = 3, num_std_dev = 1) */
    fstream file;
    file.open("data/normal.csv", ios::out | ios::trunc);
    if (!file.good())
        cout << "[!!] Open file error for csv." << endl;

    /* REF: http://www.cplusplus.com/reference/random/ */
    const char delim = ',';
    /* Log Normal distribution */
    lognormal_distribution<float> normal(3, 1);  // @param mean, std. dev.
    float normal_val[1379] = {};

    file << "raw" << delim << "lround" << "\n";
    for (float &i : normal_val) {
        float val = normal(randomEngine);
        i = val;
        file << val << delim << lround(val) << "\n";
    }
    file.close();

    /* Poisson distribution */
    const int n_times = 1379;
    poisson_distribution<int> poisson(3);  // @param mean
    int poisson_val[n_times] = {};

    file.open("data/poisson.csv", ios::out | ios::trunc);

    file << "raw" << "\n";
    for (int &i : poisson_val) {
        int val = poisson(randomEngine);
        i = val;
        file << val << delim << lround(val) << "\n";
    }
    file.close();
}
