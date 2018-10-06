/*********************************************************************************
* CSCI262 - Assignment 03
* Traffic Activity Engine
* Vehicles.cpp
* - Implementation for Vehicles wrapper class to be used as a storage container
*   for Vehicles struct in a map based on integer id keys.
*
* Version: 0.1-dev
* Last modified: 2018.10.06
*
* Authors (Github) | email:
*   - Dinh Che (codeninja55) | andrew at codeninja55.me
*   - Duong Le (daltonle) |  duong.daltonle at gmail.com
* UOW Details:
*   - Dinh Che (5721970) | dbac496 at uowmail.edu.au
*   - Duong Le (5560536) | ndl991 at uowmail.edu.au
*********************************************************************************/

#include "Vehicles.h"

void Vehicles::insert(Vehicle &v)
{
    v.id = ++n_vehicles;
    map<int, Vehicle>::iterator iter = vehicles_dict.find(v.id);
    vehicles_dict.insert(iter, pair<int, Vehicle>(v.id, v));
}

