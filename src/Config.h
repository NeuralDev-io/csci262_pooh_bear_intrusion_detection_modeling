/*********************************************************************************
* CSCI262 (Spring 2018) - Assignment 03
* Pooh Bear Intrusion Detection System Config.h
* Purpose: Header file for configurations of constants.
*
* @version 1.0.0
* @date 2018.10.20
*
* @authors Dinh Che (codeninja55) & Duong Le (daltonle)
* Emails andrew at codeninja55.me & duong.daltonle at gmail.com
* Students Dinh Che (5721970 | dbac496) & Duong Le (5560536 | ndl991)
*********************************************************************************/

#ifndef POOH_BEAR_INTRUSION_DETECTION_SYSTEM_SIMULATIONCONFIGURATION_H
#define POOH_BEAR_INTRUSION_DETECTION_SYSTEM_SIMULATIONCONFIGURATION_H


#include <cmath>

bool DEBUG_MODE = true;
const double T_ARRIVAL_LIMIT = (22.0F * 60 * 60) - 1.0F;       // Set the limit for time last car to arrive
const double T_DAY_LIMIT = (23.5 * 60.0F * 60.0F) - 1.0F;      // Set the limit for last time for events to occur

#endif //POOH_BEAR_INTRUSION_DETECTION_SYSTEM_SIMULATIONCONFIGURATION_H
