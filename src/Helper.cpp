/*********************************************************************************
* CSCI262 (Spring 2018) - Assignment 03
* Pooh Bear Intrusion Detection System Helper.cpp
* Purpose: main() driver for implementation of specifications
*
* @version 0.1-dev 2018.10.06
*
* @authors Dinh Che (codeninja55) & Duong Le (daltonle)
* Emails andrew at codeninja55.me & duong.daltonle at gmail.com
* Students Dinh Che (5721970 | dbac496) & Duong Le (5560536 | ndl991)
*********************************************************************************/

#include "Helper.h"
#include <ctime>
#include <sstream>

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
 * Convert a time struct to a human-readable string.
 *
 * @param t: a reference to the SimTime structure to format to a string.
 * @return: string of the time from the SimTime structure.
 * */
string formatted_time(SimTime &t)
{
    stringstream ss;
    ss << t.tm_mday << "-" << t.tm_mon << "-" << t.tm_year << " "
       << t.tm_hour << ":" << t.tm_min << ":" << t.tm_sec << "\n";

    return ss.str();
}
