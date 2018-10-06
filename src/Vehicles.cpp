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

#include <iomanip>
#include "Vehicles.h"

void Vehicles::insert(Vehicle &v)
{
    v.id = ++n_vehicles;
    map<int, Vehicle>::iterator iter = vehicles_dict.find(v.id);
    vehicles_dict.insert(iter, pair<int, Vehicle>(v.id, v));
}

void Vehicles::print()
{
    cout <<"| "<<right<<setw(3)<<"ID"<<" | "<<left<<setw(15)<<"Name"
         <<" | "<<left<<setw(7)<<"Parking"<<" | "<<left<<setw(11)<<"Reg. Format"
         <<" | "<<right<<setw(13)<<"Volume Weight"<<" | "<<right<<setw(12)<<"Speed Weight"<<" |"<<left<<endl;

    cout <<"|"<<setfill('-')<<setw(5)<<"-"<<"|"<<setw(17)<< "-"<<"|"<<setw(9)<<"-"
         <<"|"<<setw(13)<<"-"<<"|"<<setw(15)<<"-"<<"|"<<setw(14)<<"-"<<setfill(' ')<<"|"<<left<<endl;

    map<int, Vehicle>::iterator iter = vehicles_dict.begin();
    for (; iter != vehicles_dict.end(); iter++) {
        string parking = ((*iter).second.parking_flag) ? "Yes" : "No";
        cout <<"|"<<right<<setw(4)<< (*iter).first <<" | "<<left<<setw(15)<< (*iter).second.name
             <<" | "<<left<<setw(7)<< parking <<" | "<<left<<setw(11)<< (*iter).second.reg_format
             <<" | "<<right<<setw(13)<< (*iter).second.vol_weight <<" | "<<setw(12)<< (*iter).second.speed_weight
             <<" |"<<left<<endl;
    }
}

