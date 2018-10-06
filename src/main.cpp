/*********************************************************************************
* CSCI262 (Spring 2018) - Assignment 03
* Pooh Bear Intrusion Detection System main.cpp
* Purpose: main() driver for implementation of specifications
*
* @version 0.1-dev 2018.10.06
*
* @authors Dinh Che (codeninja55) & Duong Le (daltonle)
* Emails andrew at codeninja55.me & duong.daltonle at gmail.com
* Students Dinh Che (5721970 | dbac496) & Duong Le (5560536 | ndl991)
*********************************************************************************/

#include <iostream>
#include <cstdlib>
#include <limits.h>
#include <string.h>
#include <cstring>
#include <fstream>
#include "Vehicles.h"
#include "ActivityEngine.h"

using namespace std;

#define BUFFER_SZ 100

/* STRUCT DEFINITION */

/* GLOBAL VARIABLES */
unsigned int g_n_vehicles;

/* FUNCTION PROTOTYPES */
unsigned int safe_int_convert(const char *, const char *);

int main(int argc, char * argv[]) {

    char days_str[sizeof(int)];
    char vehicles_file[BUFFER_SZ] = "data/";
    char stats_file[BUFFER_SZ] = "data/";
    int days = 0;

    // check the correct amount of args has been passed
    if (argc < 4 || argc > 4) {
        cout << "Usage: Traffic [VEHICLES FILE] [STATS FILE] [DAYS]\n" << flush;
        exit(0);
    } else if (argc == 4) {  // correct number of args passed
        strncat(vehicles_file, argv[1], sizeof(vehicles_file) - (strlen(vehicles_file) + 1));
        strncat(stats_file, argv[2], sizeof(stats_file) - (strlen(vehicles_file) + 1));
        // attempting to do int conversion from args safely
        strncpy(days_str, argv[3], sizeof(int));
        days = safe_int_convert(days_str, "Incorrect number used for number of days");
    }

    ifstream fin;
    fin.open(vehicles_file, ifstream::in);
    if (!fin.good()) {
        cout << "[!!] Unable to read Vehicles file from: " << vehicles_file << "\nExiting...\n" << flush;
        exit(1);
    }

    // vehicles_dict is a Vehicles wrapper object for a hash map dictionary of VehicleType structs.
    // key: name of the type. value: VehicleType structure
    Vehicles vehicles_dict = Vehicles();

    fin >> g_n_vehicles;  // read the first line as number of vehicle types and store it globally

    // read subsequent lines from Vehicles.txt as:
    // "Vehicle name:Parking flag:Registration format:Volume weight:Speed weight:"
    while (!fin.eof()) {
        fin.ignore();  // ignore left over newline

        string tmp_name;
        getline(fin, tmp_name, ':');

        if (tmp_name.empty())  // check if blank line before EOF
            break;

        VehicleType new_vehicle;
        new_vehicle.name = tmp_name;

        // read each parking value as 0 (false flag) and 1 (true flag) and convert to bool
        string parking;
        getline(fin, parking, ':');
        new_vehicle.parking_flag = (parking == "1");

        getline(fin, new_vehicle.reg_format, ':');

        // read both volume and speed weights as a string initially and use safe_int_convert()
        // function to safely convert them to an unsigned int
        string vol_str, speed_str;
        getline(fin, vol_str, ':');
        getline(fin, speed_str, ':');
        new_vehicle.vol_weight = safe_int_convert(vol_str.c_str(),
                "Incorrect number used for volume weight. Must be integer.");
        new_vehicle.speed_weight = safe_int_convert(speed_str.c_str(),
                "Incorrect number used for speed weight. Must be integer.");
        // insert each new_vehicle struct into the Vehicles hash map dictionary
        vehicles_dict.insert(new_vehicle);
    }
    fin.close();

    // TODO: debug
    // vehicles_dict.print();

    fin.open(stats_file, ifstream::in);

    if (!fin.good()) {
        cout << "[!!] Unable to read Stats file from: " << stats_file << "\nExiting...\n" << flush;
        exit(1);
    }

    // read the first line as positive integers
    // number of vehicle types monitored | length of road in km | speed limit in km/h | number of parking spaces
    char *end;
    uint veh_monitored, parking_spots;
    float speed_lim, road_len;
    string veh_monitored_str, parking_spots_str, speed_lim_str, road_len_str;

    fin >> veh_monitored_str >> road_len_str >> speed_lim_str >> parking_spots_str;
    veh_monitored = safe_int_convert(veh_monitored_str.c_str(),
            "Incorrect number used for number of vehicles monitored. Must be integer.");
    parking_spots = safe_int_convert(parking_spots_str.c_str(),
            "Incorrect number used for number of vehicles monitored. Must be integer.");
    speed_lim = strtof(speed_lim_str.c_str(), &end);
    road_len = strtof(road_len_str.c_str(), &end);

    ActivityEngine TrafficEngine = ActivityEngine(veh_monitored, road_len, speed_lim, parking_spots);

    // read subsequent lines from Stats.txt as:
    // Vehicle type:Number mean:Number standard deviation:Speed mean: Speed standard deviation:
    while (!fin.eof()) {
        fin.ignore(); // ignore left over newline

        // read both each int type as a string initially and use safe_int_convert()
        // function to safely convert them to an unsigned int
        string name;
        string num_mean_str, num_stddev_str, speed_mean_str, speed_stddev_str;
        uint num_mean, num_stddev, speed_mean, speed_stddev;
        getline(fin, name, ':');

        if (name.empty())
            break;

        getline(fin, num_mean_str, ':');
        getline(fin, num_stddev_str, ':');
        getline(fin, speed_mean_str, ':');
        getline(fin, speed_stddev_str, ':');

        num_mean = safe_int_convert(num_mean_str.c_str(), "Invalid number used for number mean. Must be integer.");
        num_stddev = safe_int_convert(num_stddev_str.c_str(),
                "Invalid number used for number standard deviation. Must be integer.");
        speed_mean = safe_int_convert(speed_mean_str.c_str(), "Invalid number used for speed mean. Must be integer.");
        speed_stddev = safe_int_convert(speed_stddev_str.c_str(),
                "Invalid number used for speed standard deviation. Must be integer.");

        // check to see if vehicle type stats was added successfully
        // TODO: should log this to a file
        if (!vehicles_dict.add_stats(name, num_mean, num_stddev, speed_mean, speed_stddev))
            cout << "Vehicle type " << name << " cannot be found." << endl;
    }

    return 0;
}

/* HELPER UTILS */

/*
 * Utility function to convert read in string to unsigned int, making sure it fits into int
 * size otherwise, print to stdout an error message defined from parameter.
 *
 * @param int_str: const char pointer to a c-style string which can be converted to an int
 * @param err_msg: a const char pointer to a c-style string a message to print to stdout for errors
 * @return: a c++ static_cast unsigned int if successful.
 * */
unsigned int safe_int_convert(const char *int_str, const char *err_msg)
{
    char *end;
    long tmp = strtol(int_str, &end, sizeof(long));
    if (tmp >= INT_MIN && tmp <= INT_MAX)
        return static_cast<unsigned int>(tmp);
    else {
        cout << "[!!] " << err_msg << "\nExiting..." << endl;
        exit(1);
    }
}
