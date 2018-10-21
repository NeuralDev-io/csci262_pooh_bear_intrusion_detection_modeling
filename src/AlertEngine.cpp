#include <utility>

/*********************************************************************************
* CSCI262 (Spring 2018) - Assignment 03
* Pooh Bear Intrusion Detection System AlertEngine.h
* Purpose: Creates an Alert Engine object that checks thresholds for vehicles
*          and alerts if anomaly counters are over these thresholds.
*
* @version 0.7-dev
* @date 2018.10.21
*
* @authors Dinh Che (codeninja55) & Duong Le (daltonle)
* Emails andrew at codeninja55.me & duong.daltonle at gmail.com
* Students Dinh Che (5721970 | dbac496) & Duong Le (5560536 | ndl991)
*********************************************************************************/

#include "AlertEngine.h"

AlertEngine::AlertEngine(string log_file, string data_file) : log_file(move(log_file)), data_file(move(data_file)),
                                                              volume_anomaly_ctr_threshold(0), speed_anomaly_ctr_threshold(0)
{
    alert_logger = Logger<SimTime, AlertLog>("Alert Engine", INFO, log_file, true);
}

void AlertEngine::run(Vehicles &vehicles, int &days)
{
    SimTime sim_time = init_time_date();
    int i;

    char pwd[100];
    getcwd(pwd, sizeof(pwd));
    cout << setw(20) << "[*****SYSTEM*****]" << real_formatted_time_now() << " Alert Engine started and logging to: "
         << pwd << dir_slash << "logs" << dir_slash << log_file << endl;
    stringstream msg;
    msg << "Started Alert Engine" << DELIMITER << "Number of days=" << days;
    alert_logger.info(sim_time, AlertLog(msg.str()));

    read_data(sim_time, vehicles);
    /*for (i = 0; i < days; i++ ) {
        // read the data first from the file and store it in the queue to process
    }*/

    // TODO: For each day generated, report on whether there are intrusions detected.

}

void AlertEngine::read_data(SimTime &sim_time, Vehicles &vehicles)
{
    stringstream filename;
    filename << "data/" << data_file;
    ifstream fin(filename.str(), ios::in);

    if (!fin.good()) {
        cout << setw(20) << "[*****FILE ERROR*****]" << real_formatted_time_now() << " Failed to open data file: "
             << data_file << endl;
        return;
    }

    // TODO: read data as follows:
    // Header: day,date(only),vehicle_type,volume_mean,speed_mean
    // Data:   1,<01-01-2018>,Bus,3,50.22

    // header line
    string header;
    getline(fin, header);

    while (!fin.eof()) {
        string veh_name, date_str, day_str, vol_mean_str, speed_mean_str;
        double volume_mean, speed_mean;
        int day;
        getline(fin, day_str, DELIMITER);
        getline(fin, date_str, DELIMITER);
        SimTime date(date_str);
        getline(fin, veh_name, DELIMITER);
        getline(fin, vol_mean_str, DELIMITER);
        getline(fin, speed_mean_str, DELIMITER);

        day = stoi(day_str);
        volume_mean = stod(vol_mean_str);
        speed_mean = stod(speed_mean_str);

        // TODO: check for next day


        // get the volume weight from the vehicles type dictionary.
        double volume_weight, speed_weight;
        volume_weight = speed_weight = 0;
        auto type_iter = vehicles.get_vehicles_dict()->find(veh_name);
        if (type_iter != vehicles.get_vehicles_dict()->end()) {
            volume_weight = (*type_iter).second.vol_weight;
            speed_weight = (*type_iter).second.speed_weight;
        }

        auto iter = data_map.find(date);
        if (iter != data_map.end()) {  // date, already in the map --> update
            VehicleTypeStats vehicle = { veh_name, volume_mean, speed_mean, volume_weight, speed_weight };
            (*iter).second.vehicles_ls.push_back(vehicle);
            // TODO: probably need to do some weights calculations. Confirm with dalts first.
        } else {  // date, not in the map  --> create
            AlertStats alert_stats = { SimTime(sim_time) };
            VehicleTypeStats vehicle = { veh_name, volume_mean, speed_mean, volume_weight, speed_weight };
            alert_stats.vehicles_ls.push_back(vehicle);
            data_map.insert(pair<SimTime, AlertStats>(SimTime(sim_time), alert_stats));
        }
    }
}

void AlertEngine::process_data(SimTime &sim_time, Vehicles &vehicles)
{

}
