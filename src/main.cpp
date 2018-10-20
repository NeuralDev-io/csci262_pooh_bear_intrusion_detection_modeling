/*********************************************************************************
* CSCI262 (Spring 2018) - Assignment 03
* Pooh Bear Intrusion Detection System main.cpp
* Purpose: main() driver for implementation of specifications
*
* @version 0.6-dev
* @date 2018.10.06
*
* @authors Dinh Che (codeninja55) & Duong Le (daltonle)
* Emails andrew at codeninja55.me & duong.daltonle at gmail.com
* Students Dinh Che (5721970 | dbac496) & Duong Le (5560536 | ndl991)
*********************************************************************************/

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <string.h>
#include <cstring>
#include <fstream>
#include "Vehicles.h"
#include "ActivityEngine.h"
#include "AnalysisEngine.h"
using namespace std;

#define BUFFER_SZ 100
#ifdef __linux__
    char OS[] = "linux";
#elif _linux_
    char OS[] = "linux";
#elif _WIN32
    char OS[] = "win";
#elif _WIN64
    char OS[] = "win";
#endif

/* STRUCT DEFINITION */

/* GLOBAL VARIABLES */
unsigned int g_n_vehicles = 0;
unsigned g_days = 0;

/* FUNCTION PROTOTYPES */
void check_directories();
void read_vehicles_file(ifstream &fin, char *vehicles_file, Vehicles &vehicles_dict);
void read_stats_file(ifstream &fin, char *stats_file, Vehicles &vehicles_dict, ActivityEngine &activity_engine);


int main(int argc, char * argv[])
{
    // when starting the system, delete all the old logs before creating new ones.
    check_directories();

    char days_str[sizeof(int)];
    char vehicles_file[BUFFER_SZ], stats_file[BUFFER_SZ];
    ifstream fin;

    // check the correct amount of args has been passed
    if (argc < 4 || argc > 4) {
        cout << "Usage: Traffic [VEHICLES FILE] [STATS FILE] [DAYS]\n" << flush;
        exit(0);
    } else if (argc == 4) {  // correct number of args passed
        strncpy(vehicles_file, argv[1], BUFFER_SZ);
        strncpy(stats_file, argv[2], BUFFER_SZ);
        // attempting to do int conversion from args safely
        strncpy(days_str, argv[3], sizeof(int));
        g_days = safe_int_convert(days_str, "Incorrect number used for number of days");
    }

    FILE *fp = fopen("Welcome_ascii", "r");
    char c = getc(fp);
    cout << "\n\n";
    while (c != EOF) {
        cout << c;
        c = getc(fp);
    }
    cout << endl << endl;
    fclose(fp);

    // vehicles_dict is a Vehicles wrapper object for a hash map dictionary of VehicleType structs.
    // key: name of the type. value: VehicleType structure
    Vehicles vehicles_dict = Vehicles();
    read_vehicles_file(fin, vehicles_file, vehicles_dict);

    ActivityEngine activity_engine;
    read_stats_file(fin, stats_file, vehicles_dict, activity_engine);

    // TODO: debug
    cout << "[*****SYSTEM*****]" << left << setw(27) << " Vehicle Types " << endl;
    vehicles_dict.print();

    activity_engine.run(vehicles_dict);

    /* FIXME: Not sure, just take a look at it. */
    AnalysisEngine analysis_engine;
    analysis_engine.run(vehicles_dict);

    char command;

    /*do {
        cout << "\n" << setw(20) << "[*****SYSTEM*****]" << real_formatted_time_now() << "\n"
             << "Do you want to continue by inputting a new Statistics file for simulation? [y/N]: ";
        cin >> command;
        command = static_cast<char>(tolower(command));

        if (strncmp(&command, "y", sizeof(command)) == 0) {
            cout << "Statistics filename: ";
            char user_stats[BUFFER_SZ];
            cin >> user_stats;

            ifstream another_fin;
            ActivityEngine live_activity_engine(user_stats);
            read_stats_file(another_fin, user_stats, vehicles_dict, live_activity_engine);
            live_activity_engine.run(vehicles_dict);

            AnalysisEngine live_analysis_engine;
            live_analysis_engine.run(vehicles_dict);

        } else if (strncmp(&command, "n", sizeof(command)) == 0) {
            fp = fopen("Exit_ascii", "r");
            c = getc(fp);
            cout << "\n\n";
            while (c != EOF) {
                cout << c;
                c = getc(fp);
            }
            cout << endl << endl;
            fclose(fp);
            break;
        }
    } while (strncmp(&command, "n", sizeof(command)) != 0);*/

    return 0;
}

void check_directories()
{
    if (is_dir_exists("logs")) {
        if (strcmp(OS, "linux") == 0)
            system("exec rm -rf logs/*");
        else
            system("del logs");
    } else {  // DOS WIN32 or WIN64 systems
        if (strcmp(OS, "linux") == 0)
            system("exec mkdir logs");
        else
            system("mkdir logs");
    }

    if (is_dir_exists("data")) {
        if (strcmp(OS, "linux") == 0)
            system("exec rm -rf data/*");
        else
            system("del data");
    } else {
        if (strcmp(OS, "linux") == 0)
            system("exec mkdir data");
        else
            system("mkdir data");
    }
}

void read_vehicles_file(ifstream &fin, char *vehicles_file, Vehicles &vehicles_dict)
{
    fin.open(vehicles_file, ifstream::in);
    if (!fin.good()) {
        stringstream ss;
        ss << setw(20) << "[*****FILE ERROR*****]" << real_formatted_time_now()
           << "Unable to read Vehicles file from: " << vehicles_file << "\nExiting...\n" << flush;
        perror(ss.str().c_str());
        exit(1);
    }

    fin >> g_n_vehicles;  // read the first line as number of vehicle types and store it globally
    // read subsequent lines from Vehicles.txt as:
    // "Vehicle name:Parking flag:Registration format:Volume weight:Speed weight:"
    while (!fin.eof()) {
        fin.ignore(); // ignore newline before getline()

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
}

void read_stats_file(ifstream &fin, char *stats_file, Vehicles &vehicles_dict, ActivityEngine &activity_engine)
{
    fin.open(stats_file, ifstream::in);
    if (!fin.good()) {
        stringstream ss;
        ss << setw(20) << "[*****FILE ERROR*****]" << real_formatted_time_now()
           << " Unable to read Stats file from: " << stats_file << "\nExiting...\n" << flush;
        perror(ss.str().c_str());
        exit(1);
    }

    // read the first line as positive integers
    // number of vehicle types monitored | length of road in km | speed limit in km/h | number of parking spaces
    char *unused_end;
    unsigned veh_monitored, parking_spots;
    float speed_lim, road_len;
    string veh_monitored_str, parking_spots_str, speed_lim_str, road_len_str;

    fin >> veh_monitored_str >> road_len_str >> speed_lim_str >> parking_spots_str;
    veh_monitored = safe_int_convert(veh_monitored_str.c_str(),
            "Incorrect number used for number of vehicles monitored. Must be integer.");
    parking_spots = safe_int_convert(parking_spots_str.c_str(),
            "Incorrect number used for number of vehicles monitored. Must be integer.");
    speed_lim = strtof(speed_lim_str.c_str(), &unused_end);
    road_len = strtof(road_len_str.c_str(), &unused_end);

    cout << flush << setw(20) << "[*****SYSTEM*****]" << left << setw(27) << " Vehicles Monitored: " << veh_monitored
         << '\n'
         << setw(20) << "[*****SYSTEM*****]" << left << setw(27) << " Road Length: " << road_len << '\n'
         << setw(20) << "[*****SYSTEM*****]" << left << setw(27) << " Parking Spots Available: " << parking_spots
         << '\n'
         << setw(20) << "[*****SYSTEM*****]" << left << setw(27) << " Speed Limit: " << speed_lim << " km/h\n\n";

    activity_engine.set_statistics(g_days, veh_monitored, road_len, speed_lim, parking_spots);

    // read subsequent lines from Stats.txt as:
    // Vehicle type:Number mean:Number standard deviation:Speed mean: Speed standard deviation:
    while (!fin.eof()) {
        fin.ignore(); // ignore left over newline before getline()

        // read both each int type as a string initially and use safe_int_convert()
        // function to safely convert them to an unsigned int
        string name;
        string num_mean_str, num_stddev_str, speed_mean_str, speed_stddev_str;
        float num_mean, num_stddev, speed_mean, speed_stddev;
        getline(fin, name, ':');

        if (name.empty())
            break;

        getline(fin, num_mean_str, ':');
        getline(fin, num_stddev_str, ':');
        getline(fin, speed_mean_str, ':');
        getline(fin, speed_stddev_str, ':');

        num_mean = strtof(num_mean_str.c_str(), &unused_end);
        num_stddev = strtof(num_stddev_str.c_str(), &unused_end);
        speed_mean = strtof(speed_mean_str.c_str(), &unused_end);
        speed_stddev = strtof(speed_stddev_str.c_str(), &unused_end);

        // check to see if vehicle type stats was added successfully
        // TODO: should log this to a file
        if (!vehicles_dict.add_stats(name, num_mean, num_stddev, speed_mean, speed_stddev))
            cout << "Vehicle type " << name << " cannot be found." << endl;
    }
}