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

#include <chrono>
#include <string>

// const auto SYSTEM_SEED = static_cast<unsigned long>(std::chrono::system_clock::now().time_since_epoch().count());
const unsigned long SYSTEM_SEED = 0;

// bool DEBUG_MODE = true;
const static std::string LOGS_FILENAME = "logs_baseline";
const double T_ARRIVAL_LIMIT = (22.5F * 60 * 60);           // Set the limit for time last car to arrive
const double T_PARKING_LIMIT = (23.5F * 60 * 60);           // Set the limit for time last car to park
const double T_DAY_LIMIT = (24.0 * 60.0F * 60.0F) - 1.0F;   // Set the limit for last time for events to occur
const double DEPART_SIDE_PROBABILITY = 0.08;                // Set the probability for the bernoulli distribution
const double DEPART_SIDE_UPPER_BOUND = 0.02;                // Upper bound value to be used in biased expovariate function
const double PARKING_PROBABILITY = 0.08;                    // Set the probability for parking for the binomial distribution

#endif //POOH_BEAR_INTRUSION_DETECTION_SYSTEM_SIMULATIONCONFIGURATION_H
