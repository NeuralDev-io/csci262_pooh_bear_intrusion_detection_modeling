/*********************************************************************************
* CSCI262 (Spring 2018) - Assignment 03
* Pooh Bear Intrusion Detection System Vehicles.cpp
* Purpose: Implementation for Vehicles wrapper class to be used as a storage
*          container for Vehicles struct in a map based on integer id keys.
*
* @version 0.3-dev
* @date 2018.10.06
*
* @authors Dinh Che (codeninja55) & Duong Le (daltonle)
* Emails: andrew at codeninja55.me & duong.daltonle at gmail.com
* StudentsS Dinh Che (5721970 | dbac496) & Duong Le (5560536 | ndl991)
*********************************************************************************/

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
    vehicles_dict.insert(pair<string, VehicleType>(v.name, v));
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
 * @brief: A getter method for size of vehicles_dict.
 *
 * @return: an int which represents the size of the vehicles_dict.
 * */
int Vehicles::size() {
    return n_vehicles;
}

/*
 * @brief: A getter method to return a pointer to the vehicles_dict map.
 *
 * @return: A pointer to the vehicles_dic map
 * */
map<string, VehicleType> *Vehicles::get_vehicles_dict()
{
    map<string, VehicleType> *vehicles_ptr;
    vehicles_ptr = &vehicles_dict;
    return vehicles_ptr;
}

/*
 * @brief: Print the stored Vehicle types in a prettified table.
 * */
void Vehicles::print()
{
    cout <<"| "<<right<<setw(3)<<"ID"<<" | "<<left<<setw(15)<<"Name"
         <<" | "<<setw(7)<<"Parking"<<" | "<<setw(11)<<"Reg. Format"
         <<" | "<<right<<setw(13)<<"Volume Weight"<<" | "<<setw(12)<<"Speed Weight"
         <<" | "<<setw(11)<<"Number Mean"<<" | "<<setw(16)<<"Number Std. Dev."
         <<" | "<<setw(10)<<"Speed Mean"<<" | "<<setw(15)<<"Speed Std. Dev."<<" |"
         <<left<<endl;

    cout <<"|"<<setfill('-')<<setw(5)<<"-"<<"|"<<setw(17)<< "-"<<"|"<<setw(9)<<"-"
         <<"|"<<setw(13)<<"-"<<"|"<<setw(15)<<"-"<<"|"<<setw(14)<<"-"
         <<"|"<<setw(13)<<"-"<<"|"<<setw(18)<<"-"<<"|"<<setw(12)<<"-"
         <<"|"<<setw(17)<<"-"
         <<setfill(' ')<<"|"<<left<<endl;

    map<string, VehicleType>::iterator iter = vehicles_dict.begin();
    for (; iter != vehicles_dict.end(); iter++) {
        string parking = ((*iter).second.parking_flag) ? "Yes" : "No";
        cout <<"|"<<right<<setw(4)<< (*iter).second.id <<" | "<<left<<setw(15)<< (*iter).first
             <<" | "<<setw(7)<< parking <<" | "<<setw(11)<< (*iter).second.reg_format
             <<" | "<<right<<setw(13)<< (*iter).second.vol_weight <<" | "<<setw(12)<< (*iter).second.speed_weight
             <<" | "<<setw(11)<<setprecision(4)<<(*iter).second.num_mean<<" | "<<setw(16)<<(*iter).second.num_stddev
             <<" | "<<setw(10)<<setprecision(4)<<(*iter).second.speed_mean<<" | "<<setw(15)<<(*iter).second.speed_stddev
             <<" |"<<left<<endl;
    }
    cout<<endl;
}

/*
 * @brief: Generate a string registration based on the format reguired.
 *
 * @param reg_form: a c++ basic_string with the format for the vehicle type with 'L' being a letter and 'D' being a
 * digit.
 * @param generator: a generator from outside the method so the random engine is not seeded each time this function runs
 *
 * @return: a c++ string of the registration based on the format required.
 * */
string Vehicles::generate_registration(string &reg_format, mt19937_64 &generator)
{
    string str;
    str.reserve(reg_format.size());
    uniform_int_distribution<int> L_uniform_int(0, 25);
    uniform_int_distribution<int> D_uniform_int(0, 9);

    for (char c : reg_format)
        str += (c == 'L') ? LETTERS[L_uniform_int(generator)] : DIGITS[D_uniform_int(generator)];

    pair<set<string>::iterator, bool> ret = UNIQUE_REGISTRATIONS.insert(str);
    return (ret.second) ? str : generate_registration(reg_format, generator);
}
