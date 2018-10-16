/*********************************************************************************
* CSCI262 (Spring 2018) - Assignment 03
* Pooh Bear Intrusion Detection System Vehicles.h
* Purpose: Header file for Vehicles class and structure definition of Vehicle
*
* @version 0.3-dev
* @date 2018.10.06
*
* @authors Dinh Che (codeninja55) & Duong Le (daltonle)
* Emails andrew at codeninja55.me & duong.daltonle at gmail.com
* Students Dinh Che (5721970 | dbac496) & Duong Le (5560536 | ndl991)
*********************************************************************************/

#ifndef POOH_BEAR_INTRUSION_DETECTION_SYSTEM_VEHICLES_H
#define POOH_BEAR_INTRUSION_DETECTION_SYSTEM_VEHICLES_H

#include <string>
#include <sstream>
#include <map>
#include <set>
#include <iostream>
#include <random>
#include <iomanip>
using namespace std;

const char LETTERS[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char DIGITS[11] = "0123456789";
static set<string> UNIQUE_REGISTRATIONS;

typedef struct {
    int id;
    string name, reg_format;
    bool parking_flag;
    unsigned vol_weight, speed_weight;
    float num_mean, num_stddev, speed_mean, speed_stddev;
} VehicleType;

class Vehicles {
public:
    Vehicles() : n_vehicles(0) {};
    void insert(VehicleType&);
    bool add_stats(string, float, float, float, float);
    void print();
    int size();
    map<string, VehicleType> *get_vehicles_dict();
    static string generate_registration(string &reg_format, default_random_engine &generator);
private:
    unsigned n_vehicles;
    map<string, VehicleType> vehicles_dict;
};

#endif //POOH_BEAR_INTRUSION_DETECTION_SYSTEM_VEHICLES_H
