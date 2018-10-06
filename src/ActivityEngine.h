/*********************************************************************************
* CSCI262 (Spring 2018) - Assignment 03
* Pooh Bear Intrusion Detection System ActivityEngine.h
* Purpose: Header file for ActivityEngine class.
*
* @version 0.1-dev 2018.10.06
*
* @authors Dinh Che (codeninja55) & Duong Le (daltonle)
* Emails andrew at codeninja55.me & duong.daltonle at gmail.com
* Students Dinh Che (5721970 | dbac496) & Duong Le (5560536 | ndl991)
*********************************************************************************/

#ifndef POOH_BEAR_INTRUSION_DETECTION_SYSTEM_ACTIVITYENGINE_H
#define POOH_BEAR_INTRUSION_DETECTION_SYSTEM_ACTIVITYENGINE_H

#include <zconf.h>

class ActivityEngine {
public:
    ActivityEngine() : n_vehicles_monitored(0), n_parking_spots(0), road_length(0), speed_limit(0) {}; // default
    ActivityEngine(uint, float, float, uint);
private:
    uint n_vehicles_monitored, n_parking_spots;
    float road_length, speed_limit;
};

#endif //POOH_BEAR_INTRUSION_DETECTION_SYSTEM_ACTIVITYENGINE_H
