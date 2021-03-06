/*********************************************************************************
* CSCI262 (Spring 2018) - Assignment 03
* Pooh Bear Intrusion Detection System Helper.cpp
* Purpose: main() driver for implementation of specifications
*
* @version 0.6-dev
* @date 2018.10.06
*
* @authors Dinh Che (codeninja55) & Duong Le (daltonle)
* Emails andrew at codeninja55.me & duong.daltonle at gmail.com
* Students Dinh Che (5721970 | dbac496) & Duong Le (5560536 | ndl991)
*********************************************************************************/

#include <sys/stat.h>
#include <sys/types.h>
#include "Utils.h"

/*
 * Initialise a sim_time struct to start the simulation with a date of when the sim
 * start relative to the real time.
 *
 * @return: the SimTime structure that was initialised.
 * */
SimTime init_time_date()
{
    // Get the real time based on system
    time_t raw_time = time(nullptr);
    struct tm *real_time_now;
    time(&raw_time);
    real_time_now = localtime(&raw_time);

    // Create fake time based on simulation start date and starting time of 0
    SimTime time_now;

    time_now.tm_sec = time_now.tm_hour = time_now.tm_min = 0;
    time_now.tm_mday = real_time_now->tm_mday;
    time_now.tm_mon = real_time_now->tm_mon + 1;
    time_now.tm_year = real_time_now->tm_year + 1900;
    return time_now;
}

/*
 * Utility function to convert read in string to unsigned int, making sure it fits into int
 * size otherwise, print to stdout an error message defined from parameter.
 *
 * @param int_str: const char pointer to a c-style string which can be converted to an int
 * @param err_msg: a const char pointer to a c-style string a message to print to stdout for errors
 * @return: a c++ static_cast unsigned int if successful.
 * */
unsigned int safe_int_convert(const char *int_str, const char *err_msg)
{
    char *unused_end;
    double tmp = strtod(int_str, &unused_end);
    if (tmp >= INT_MIN && tmp <= INT_MAX)
        return static_cast<unsigned int>(tmp);
    else {
        stringstream ss;
        ss << err_msg << "\nExiting..." << endl;
        perror(ss.str().c_str());
        exit(1);
    }
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
 * @param ev: an enum EVENT_TYPE
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
        case VEHICLE_MOVE:
            return "VEHICLE_MOVE";
        case UNKNOWN:
            return "UNKNOWN";
    }
}

/*
 * @param: evt_name: a string representation of the enum EVENT.
 * @return: an enum EVENT_TYPE
 * */
EVENT_TYPE event_type(string evt_name)
{
    if (evt_name == "ARRIVAL")
        return ARRIVAL;
    else if (evt_name == "DEPART_SIDE_ROAD")
        return DEPART_SIDE_ROAD;
    else if (evt_name == "DEPART_END_ROAD")
        return DEPART_END_ROAD;
    else if (evt_name == "PARKING_START")
        return PARKING_START;
    else if (evt_name == "VEHICLE_MOVE")
        return VEHICLE_MOVE;
    else
        return UNKNOWN;
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
 * @brief: check if a directory exists given a path name. Works both in UNIX and Windows environments.
 *
 * @param pathname: char pointer to a pathname.
 *
 * @return: true if the directory exists, false otherwise.
 * */
bool is_dir_exists(const char *pathname)
{
    struct stat info{};

    if (stat( pathname, &info ) != 0)
        return false;
    else
        return (info.st_mode & S_IFDIR) != 0;
}
