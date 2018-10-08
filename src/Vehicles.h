/*********************************************************************************
* CSCI262 (Spring 2018) - Assignment 03
* Pooh Bear Intrusion Detection System Vehicles.h
* Purpose: Header file for Vehicles class and structure definition of Vehicle
*
* @version 0.1-dev
* @date 2018.10.06
*
* @authors Dinh Che (codeninja55) & Duong Le (daltonle)
* Emails andrew at codeninja55.me & duong.daltonle at gmail.com
* Students Dinh Che (5721970 | dbac496) & Duong Le (5560536 | ndl991)
*********************************************************************************/

#ifndef POOH_BEAR_INTRUSION_DETECTION_SYSTEM_VEHICLES_H
#define POOH_BEAR_INTRUSION_DETECTION_SYSTEM_VEHICLES_H

#include <string>
#include <map>
#include <iostream>
#include <zconf.h>
using namespace std;

typedef struct {
    int id;
    string name, reg_format;
    bool parking_flag;
    uint vol_weight, speed_weight;
    float num_mean, num_stddev, speed_mean, speed_stddev;
} VehicleType;

class Vehicles {
public:
    Vehicles() : n_vehicles(0) {};
    void insert(VehicleType&);
    bool add_stats(string, float, float, float, float);
    void print();

private:
    uint n_vehicles;
    map<string, VehicleType> vehicles_dict;
};

#endif //POOH_BEAR_INTRUSION_DETECTION_SYSTEM_VEHICLES_H
