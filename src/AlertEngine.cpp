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

AlertEngine::AlertEngine(string user_stats, string log_filename) : log_file(log_filename), user_stats_file(user_stats),
                                                               volume_anomaly_threshold(0), speed_anomaly_threshold(0)
{
    // FIXME: passed in value not
    alert_logger = Logger<SimTime, AlertLog>("Alert Engine", INFO, log_filename, false);

    // TODO: EXTRACT FOR DATA
    // log_file.substring(5, log_file.length() - 5);
}

void AlertEngine::run(Vehicles &vehicles, int &days)
{
    SimTime sim_time = init_time_date();
    int i;

    char pwd[100];
    getcwd(pwd, sizeof(pwd));
    stringstream console_msg;
    console_msg << "Alert Engine started and logging to: " << pwd << dir_slash << "logs" << dir_slash
                << log_file << endl;

    // calculating the threshold values
    auto iter = vehicles.get_vehicles_dict()->begin();
    for (; iter != vehicles.get_vehicles_dict()->end(); ++iter) {
        volume_anomaly_threshold += (*iter).second.vol_weight;
        speed_anomaly_threshold += (*iter).second.speed_weight;
    }
    volume_anomaly_threshold *= 2;
    speed_anomaly_threshold *= 2;

    cout << endl;
    console_log("SYSTEM", "Threshold values: ");
    cout << left << setw(27) << "Volume Anomaly Threshold: " << volume_anomaly_threshold << '\n'
         << left << setw(27) << "Speed Anomaly Threshold: " << speed_anomaly_threshold << endl;

    stringstream log_msg;
    log_msg << "Started Alert Engine" << DELIMITER << "Number of days=" << days;
    alert_logger.info(sim_time, AlertLog(log_msg.str()));

    read_data(sim_time, vehicles);
    /*for (i = 0; i < days; i++ ) {
        // read the data first from the file and store it in the queue to process
    }*/

    // TODO: For each day generated, report on whether there are intrusions detected.

}

void AlertEngine::read_data(SimTime &sim_time, Vehicles &vehicles)
{
    string filename = "data/" + data_baseline;
    ifstream fin(filename, ios::in);

    if (!fin.good()) {
        console_log("FILE ERROR", "Failed to open data file: " + data_baseline);
        return;
    }

    // header line - dont need
    string header;
    getline(fin, header);

    while (!fin.eof()) {
        string veh_name, date_str, day_str, vol_mean_str, speed_mean_str;
        double volume_mean, speed_mean;
        int day;

        getline(fin, day_str, DELIMITER);

        if (day_str.empty() || fin.fail() || fin.bad())
            break;

        getline(fin, date_str, DELIMITER);
        SimTime date;
        date.tm_mday = stoi(date_str.substr(1,2));
        date.tm_mon = stoi(date_str.substr(4,2));
        date.tm_year = stoi(date_str.substr(7,4));

        getline(fin, veh_name, DELIMITER);
        getline(fin, vol_mean_str, DELIMITER);
        getline(fin, speed_mean_str);

        day = safe_int_convert(day_str.c_str(), "Integer required.");
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
        //     // TODO: probably need to do some weights calculations. Confirm with dalts first.
        } else {  // date, not in the map  --> create
            DailyAlertStats alert_stats = { SimTime(sim_time) };
            VehicleTypeStats vehicle = { veh_name, volume_mean, speed_mean, volume_weight, speed_weight };
            alert_stats.vehicles_ls.push_back(vehicle);
            data_map.insert(pair<SimTime, DailyAlertStats>(SimTime(sim_time), alert_stats));
        }
    }
}

void AlertEngine::process_data(SimTime &sim_time, Vehicles &vehicles)
{

}
