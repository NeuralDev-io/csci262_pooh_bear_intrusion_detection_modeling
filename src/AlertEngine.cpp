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
    alert_logger = Logger<SimTime, AlertLog>("Alert Engine", INFO, log_filename, false);
    data_file = "data_" + log_file.substr(5, log_file.length() - 5);
}

void AlertEngine::run(Vehicles &vehicles)
{
    SimTime sim_time = init_time_date();

    // console log Alert Engine started
    char pwd[100];
    getcwd(pwd, sizeof(pwd));
    stringstream console_msg;
    console_msg << "Alert Engine started and logging to: " << pwd << dir_slash << "logs" << dir_slash
                << log_file << endl;
    // log Alert Engine started
    stringstream log_msg;
    log_msg << "Started Alert Engine";
    alert_logger.info(sim_time, AlertLog(log_msg.str()));

    calculate_thresholds(vehicles);
    read_data();
    process_data(vehicles);
}

void AlertEngine::calculate_thresholds(Vehicles& vehicles)
{
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
}

void AlertEngine::read_data()
{
    string filename = "data/" + data_file;
    ifstream fin(filename, ios::in);

    if (!fin.good()) {
        console_log("FILE ERROR", "Failed to open data file: " + data_file);
        return;
    }

    // ignore header line
    string header;
    getline(fin, header);

    while (!fin.eof()) {
        string veh_name, date_str, day_str, vol_mean_str, speed_mean_str;
        unsigned volume;
        double speed_mean;

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

        volume = safe_int_convert(vol_mean_str.c_str(), "Error: not an int");
        speed_mean = stod(speed_mean_str);

        auto iter = data_map.find(date);
        if (iter != data_map.end()) {  // date, already in the map --> update
            VehicleTypeStats vehicle = { veh_name, volume, speed_mean };
            (*iter).second.vehicles_ls.push_back(vehicle);
        } else {  // date, not in the map  --> create
            DailyAlertStats alert_stats = { SimTime(date) };
            VehicleTypeStats vehicle = { veh_name, volume, speed_mean };
            alert_stats.vehicles_ls.push_back(vehicle);
            data_map.insert(pair<SimTime, DailyAlertStats>(SimTime(date), alert_stats));
        }
    }
}

void AlertEngine::process_data(Vehicles &vehicles)
{
    auto iter = data_map.begin();
    auto iter_end = data_map.end();
    int day_count = 0;
    for (; iter != iter_end; iter++) {
        // loop through vehicles
        for (auto& vehicle : (*iter).second.vehicles_ls) {
            auto it = vehicles.get_vehicles_dict()->find(vehicle.veh_type);
            if (it != vehicles.get_vehicles_dict()->end()) {
                VehicleType* veh = &it->second;
                // volume anomaly counter
                unsigned volume = vehicle.volume;
                if (veh->num_stddev != 0)
                    (*iter).second.daily_volume_anomaly_ctr += (abs(volume - veh->num_mean)/veh->num_stddev)*veh->vol_weight;
                // speed anomaly counter
                double speed = vehicle.speed_mean;
                if (veh->speed_stddev != 0)
                    (*iter).second.daily_speed_anomaly_ctr += (abs(speed - veh->speed_mean)/veh->speed_stddev)*veh->speed_weight;
            }
            else {
                // TODO: Anomaly when reading from files happened. Data file has an unknown vehicle type not in stats
            }
        }

        bool volume_anomaly = (*iter).second.daily_volume_anomaly_ctr > volume_anomaly_threshold;
        bool speed_anomaly = (*iter).second.daily_speed_anomaly_ctr > speed_anomaly_threshold;

        // log report
        SimTime sim_time = init_time_date();
        stringstream msg;
        msg << "Day " << (day_count+1) << " " << (*iter).first.formatted_date() << " finished"
            << DELIMITER << "volume anomaly counter=" << (*iter).second.daily_volume_anomaly_ctr
            << DELIMITER << ((volume_anomaly) ? "volume anomaly detected" : "no volume anomaly")
            << DELIMITER << "speed anomaly counter=" << (*iter).second.daily_speed_anomaly_ctr
            << DELIMITER << ((speed_anomaly) ? "speed anomaly detected" : "no speed anomaly");
        alert_logger.info(sim_time, AlertLog(msg.str()));

        //console log report
        stringstream msg_console;
        msg_console << "Day " << (day_count+1) << " finished. "
                    << ((volume_anomaly) ? "Volume anomaly detected." : "No volume anomaly.")
                    << ((speed_anomaly) ? " Speed anomaly detected." : " No speed anomaly.");
        console_log("SYSTEM", msg_console.str());
        cout << left << setw(27) << "Volume Anomaly Counter: " << (*iter).second.daily_volume_anomaly_ctr << '\n'
             << left << setw(27) << "Speed Anomaly Counter: " << (*iter).second.daily_speed_anomaly_ctr << endl;

        day_count++;
    }
}
