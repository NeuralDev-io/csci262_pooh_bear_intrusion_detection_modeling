#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err34-c"
/*********************************************************************************
* CSCI262 - Assignment 03
* main.cpp
*    - C++ main() driver for implementation of specifications
*
* Last modified: 2018.10.06
*
* Authors (Github) | email:
*   - Dinh Che (codeninja55) | andrew at codeninja55.me
*   - Duong Le (daltonle) |  duong.daltonle at gmail.com
* UOW Details:
*   - Dinh Che (5721970) | dbac496 at uowmail.edu.au
*   - Duong Le (5560536) | ndl991 at uowmail.edu.au
*********************************************************************************/

#include <iostream>
#include <cstdlib>
#include <limits.h>
#include <string.h>
#include <cstring>
#include <fstream>
#include "Vehicles.h"

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
        strncat(stats_file, argv[1], sizeof(stats_file) - (strlen(vehicles_file) + 1));
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

    fin >> g_n_vehicles;

    Vehicles vehicles_dict = Vehicles();
    while (!fin.eof()) {
        fin.ignore();  // ignore left over newline

        string tmp_name;
        getline(fin, tmp_name, ':');

        if (tmp_name.empty())  // check if blank line before EOF
            break;

        Vehicle new_vehicle;
        new_vehicle.name = tmp_name;

        string parking;
        getline(fin, parking, ':');
        new_vehicle.parking_flag = (parking == "1");

        getline(fin, new_vehicle.reg_format, ':');

        string vol_str, speed_str;
        getline(fin, vol_str, ':');
        getline(fin, speed_str, ':');

        new_vehicle.vol_weight = safe_int_convert(vol_str.c_str(),
                "Incorrect number used for volume weight. Must be integer");
        new_vehicle.speed_weight = safe_int_convert(speed_str.c_str(),
                "Incorrect number used for speed weight. Must be integer");
        vehicles_dict.insert(new_vehicle);
    }
    fin.close();

    vehicles_dict.print();

    // TODO: read in stats input file

    return 0;
}


/* HELPER UTILS */
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
