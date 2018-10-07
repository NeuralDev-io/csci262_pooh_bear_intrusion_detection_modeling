/*********************************************************************************
* CSCI262 (Spring 2018) - Assignment 03
* Pooh Bear Intrusion Detection System Vehicles.cpp
* Purpose: Implementation for Vehicles wrapper class to be used as a storage
*          container for Vehicles struct in a map based on integer id keys.
*
* @version 0.1-dev
* @date 2018.10.06
*
* @authors Dinh Che (codeninja55) & Duong Le (daltonle)
* Emails: andrew at codeninja55.me & duong.daltonle at gmail.com
* StudentsS Dinh Che (5721970 | dbac496) & Duong Le (5560536 | ndl991)
*********************************************************************************/

#include <iomanip>
#include "Vehicles.h"

/*
 * A wrapper class function to inser the VehicleType structure into a hash map
 * with the VehicleType name as the key.
 *
 * @param v: a passed-by-reference VehicleType structure to be stored.
 * */
void Vehicles::insert(VehicleType &v)
{
    v.id = ++n_vehicles;
    map<string, VehicleType>::iterator iter = vehicles_dict.find(v.name);
    vehicles_dict.insert(iter, pair<string, VehicleType>(v.name, v));
}

/*
 * A function to add the stats for each type by finding them in the hash map first and storing
 * the values in the VehicleType structure variables.
 *
 * @param name: the name of the type to find in the vehicles_dict key.
 * @param num_mean: a float to represent the mean for number of that vehicle type.
 * @param num_stddev: a float to represent the standard deviation for that vehicle type.
 * @param speed_mean: a float to represent the mean speed for that vehicle type.
 * @param speed_stddev: a float to represent the standard deviation speed for that vehicle type.
 * @return: bool true if the vehicle type can be found by name key or false if otherwise.
 * */
bool Vehicles::add_stats(string name, float num_mean, float num_stdev, float speed_mean, float speed_stddev)
{
    map<string, VehicleType>::iterator iter = vehicles_dict.find(name);

    if (iter != vehicles_dict.end()) {
        (*iter).second.num_mean = num_mean;
        (*iter).second.num_stddev = num_stdev;
        (*iter).second.speed_mean = speed_mean;
        (*iter).second.speed_stddev = speed_stddev;
        return true;
    }
    return false;
}

/*
 * Print the stored Vehicle types in a prettified table.
 * */
void Vehicles::print()
{
    cout <<"| "<<right<<setw(3)<<"ID"<<" | "<<left<<setw(15)<<"Name"
         <<" | "<<left<<setw(7)<<"Parking"<<" | "<<left<<setw(11)<<"Reg. Format"
         <<" | "<<right<<setw(13)<<"Volume Weight"<<" | "<<right<<setw(12)<<"Speed Weight"<<" |"<<left<<endl;

    cout <<"|"<<setfill('-')<<setw(5)<<"-"<<"|"<<setw(17)<< "-"<<"|"<<setw(9)<<"-"
         <<"|"<<setw(13)<<"-"<<"|"<<setw(15)<<"-"<<"|"<<setw(14)<<"-"<<setfill(' ')<<"|"<<left<<endl;

    map<string, VehicleType>::iterator iter = vehicles_dict.begin();
    for (; iter != vehicles_dict.end(); iter++) {
        string parking = ((*iter).second.parking_flag) ? "Yes" : "No";
        cout <<"|"<<right<<setw(4)<< (*iter).second.id <<" | "<<left<<setw(15)<< (*iter).first
             <<" | "<<left<<setw(7)<< parking <<" | "<<left<<setw(11)<< (*iter).second.reg_format
             <<" | "<<right<<setw(13)<< (*iter).second.vol_weight <<" | "<<setw(12)<< (*iter).second.speed_weight
             <<" |"<<left<<"\n"<<flush;
    }
    cout<<endl;
}
