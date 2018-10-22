//
// Created by lnduo on 17/10/18.
//

#include "AnalysisEngine.h"

AnalysisEngine::AnalysisEngine(): curr_vehicles(map<string, VehicleStats>()),
                                  total_stats(map<string, AnalysisStats>()),
                                  road_length(0.0), speed_limit(0.0), day_count(0),
                                  n_vehicles(0), n_parking_spots(0),
                                  speeding_tickets(vector<VehicleStats>()),
                                  today_stats(map<string, DailyStats>()),
                                  activity_logs(queue<string>())
{
    logger = Logger<SimTime, AnalysisLog>("Analysis Engine", INFO, "logs_baseline", false);
    log_file = "logs_baseline";
    data_file = "data_baseline";
    speeding_file = "speeding_baseline";
}

AnalysisEngine::AnalysisEngine(string log_file1): curr_vehicles(map<string, VehicleStats>()),
                                                 total_stats(map<string, AnalysisStats>()),
                                                 road_length(0.0), speed_limit(0.0), day_count(0),
                                                 n_vehicles(0), n_parking_spots(0),
                                                 speeding_tickets(vector<VehicleStats>()),
                                                 today_stats(map<string, DailyStats>()),
                                                 activity_logs(queue<string>())
{
    logger = Logger<SimTime, AnalysisLog>("Analysis Engine", INFO, log_file1, false);
    log_file = log_file1;
    data_file = "data_" + log_file1.substr(5, log_file1.length() - 5);
    speeding_file = "speeding_" + log_file1.substr(5, log_file1.length() - 5);
}

void AnalysisEngine::run(Vehicles &vehicles_dict)
{
    SimTime sim_time = init_time_date();

    // console log start of Analysis Engine
    char pwd[100];
    getcwd(pwd, sizeof(pwd));
    cout << setw(20) << "[*****SYSTEM*****]" << real_formatted_time_now() << " Analysis Engine Started and logging to: "
         << pwd << dir_slash << "logs" << dir_slash << log_file << endl;

    // log start of Analysis Engine
    logger.info(sim_time, AnalysisLog( "NOTICE", "Analysis Log", "Started Analysis Engine" ));

    import_vehicles(vehicles_dict);
    read_log();
    process_log(vehicles_dict, sim_time);

    cout << setw(20) << "[*****SYSTEM*****]" << real_formatted_time_now() << " Analysis Engine finished." << endl;
}

void AnalysisEngine::import_vehicles(Vehicles &vehicles_dict)
{
    auto iter = vehicles_dict.get_vehicles_dict()->begin();
    auto iter_end = vehicles_dict.get_vehicles_dict()->end();

    for (; iter != iter_end; ++iter) {
        // add keys for stats maps
        total_stats.insert(pair<string, AnalysisStats>((*iter).first, AnalysisStats((*iter).first)));
        today_stats.insert(pair<string, DailyStats>((*iter).first, DailyStats((*iter).first)));
        n_vehicles++;
    }
}

void AnalysisEngine::read_log()
{
    ifstream fin;
    stringstream filename;
    filename << "logs/" << log_file;
    fin.open(filename.str());

    if (!fin.good()) {
        cout << "<" << real_formatted_time_now() << "> [*****FILE ERROR*****] Failed to open log file." << endl;
        return;
    }

    string tmp;
    for (int i = 0; i < 7; i++)
        getline(fin, tmp, DELIMITER); // skip to road length

    getline(fin, tmp, '=');
    getline(fin, tmp, DELIMITER);
    road_length = stof(tmp);
    getline(fin, tmp, '=');
    getline(fin, tmp, DELIMITER);
    speed_limit = stof(tmp);
    getline(fin, tmp, '=');
    getline(fin, tmp);
    n_parking_spots = (unsigned) stod(tmp);

    while (!fin.eof()) {
        getline(fin, tmp);
        size_t pos;
        pos = tmp.find(DELIMITER);
        pos = tmp.find(DELIMITER, pos + 1);
        pos = tmp.find(DELIMITER, pos + 1);
        size_t end_pos = tmp.find(DELIMITER, pos + 1);
        if (tmp.substr(pos + 1, end_pos - pos - 1) != "Vehicle Log")
            continue;
        activity_logs.push(tmp);
    }

    fin.close();
}

void AnalysisEngine::process_log(Vehicles& vehicles, SimTime &sim_time)
{
    string tmp;
    SimTime prev_time;
    size_t start_pos;
    size_t end_pos;

    // write header to data file and speeding file
    ofstream fout_data, fout_speeding;
    string filename = "data/" + data_file;
    string filename_speeding = "data/" + speeding_file;
    fout_data.open(filename, ios::out | ios::app);
    fout_speeding.open(filename_speeding, ios::out | ios::app);

    if (!fout_data.good())
        console_log("FILE ERROR", "Failed to open data output file.");
    if (!fout_speeding.good())
        console_log("FILE ERROR", "Failed to open speeding output file.");

    fout_data << "day" << DELIMITER << "date" << DELIMITER << "vehicle_type"
         << DELIMITER << "volume" << DELIMITER << "speed_mean" << endl;
    fout_data.close();
    fout_speeding << "day" << DELIMITER << "vehicle_type" << DELIMITER << "registration" << DELIMITER
                  << "speed" << DELIMITER << "arrival_time" << DELIMITER
                  << "departure_time" << endl;
    fout_speeding.close();

    // process log
    if (!activity_logs.empty()) {
        stringstream msg;
        msg << "Analysing day " << (day_count+1) << ".";
        console_log("SYSTEM", msg.str());
    }
    while (!activity_logs.empty()) {
        tmp = activity_logs.front();
        activity_logs.pop();

        start_pos = tmp.find(DELIMITER);
        SimTime curr_time(tmp.substr(0, start_pos)); // time
        // initialise first date of simulation
        if (prev_time.compare_date(SimTime()) == 0) {
            auto iter = today_stats.begin();
            auto iter_end = today_stats.end();

            for (; iter != iter_end; iter++)
                (*iter).second.date = curr_time;
        }
        // test if the simulation has gone to the next day
        if (curr_time.compare_date(prev_time) > 0
            && prev_time.compare(SimTime()) != 0) {
            end_day(sim_time);
            // console log
            stringstream msg;
            msg << "Analysing day " << (day_count+1) << ".";
            console_log("SYSTEM", msg.str());
        }
        prev_time = curr_time;

        end_pos = tmp.find(DELIMITER, ++start_pos);
        start_pos = end_pos; // skip module
        end_pos = tmp.find(DELIMITER, ++start_pos);
        start_pos = end_pos; // skip log level
        end_pos = tmp.find(DELIMITER, ++start_pos);
        start_pos = end_pos; // skip log type

        end_pos = tmp.find(DELIMITER, ++start_pos);
        EVENT_TYPE evt_type = event_type(tmp.substr(start_pos, end_pos - start_pos));
        start_pos = end_pos;

        VehicleStats veh_stats;
        end_pos = tmp.find(DELIMITER, ++start_pos);
        veh_stats.veh_name = tmp.substr(start_pos, end_pos - start_pos); // vehicle name
        start_pos = end_pos;

        auto iter = vehicles.get_vehicles_dict()->find(veh_stats.veh_name);
        if (iter != vehicles.get_vehicles_dict()->end()) {
            veh_stats.permit_speeding_flag = ((*iter).second.speed_weight == 0);
        }

        end_pos = tmp.find(DELIMITER, ++start_pos);
        veh_stats.registration_id = tmp.substr(start_pos, end_pos - start_pos); // vehicle registration id
        start_pos = end_pos;

        if (evt_type == ARRIVAL) {
            veh_stats.arrival_time = curr_time; // arrival time

            end_pos = tmp.find(DELIMITER, ++start_pos);
            veh_stats.arrival_speed = stod(tmp.substr(start_pos, end_pos - start_pos)); // arrival speed
            // add to list of current vehicles on road
            curr_vehicles.insert(pair<string, VehicleStats>(veh_stats.registration_id, veh_stats));

            // add arrival_speed and increment volume in today_stats
            auto iter1 = today_stats.find(veh_stats.veh_name);
            if (iter1 != today_stats.end()) {
                (*iter1).second.speed.push_back(veh_stats.arrival_speed);
                (*iter1).second.volume++;
            }
            // add arrival_speed to total_stats
            auto iter2 = total_stats.find(veh_stats.veh_name);
            if (iter2 != total_stats.end())
                (*iter2).second.speed_dist.push_back(veh_stats.arrival_speed);

        } else if (evt_type == DEPART_SIDE_ROAD) {
            curr_vehicles.erase(veh_stats.registration_id);
        } else if (evt_type == DEPART_END_ROAD) {
            auto iter = curr_vehicles.find(veh_stats.registration_id);
            if (iter != curr_vehicles.end()) {
                (*iter).second.departure_time = curr_time;
                // check speeding
                double avg_speed = road_length / curr_time.diff((*iter).second.arrival_time) * 3600;
                if (avg_speed > speed_limit) {
                    (*iter).second.avg_speed = avg_speed;
                    add_speeding((*iter).second);
                }
            }
            curr_vehicles.erase(veh_stats.registration_id);
        }
    }

    end_day(sim_time);
}

void AnalysisEngine::add_speeding(VehicleStats &veh_stats)
{
    speeding_tickets.push_back(veh_stats);

    // console log
    stringstream msg_console;
    msg_console << "Speeding detected: " << veh_stats.veh_name << DELIMITER
        << veh_stats.registration_id << DELIMITER << veh_stats.avg_speed << " kmh";
    console_log("ALERT", msg_console.str());
    // log
    stringstream msg;
    msg << veh_stats.veh_name << DELIMITER << veh_stats.registration_id
        << DELIMITER << fixed << setprecision(2) << veh_stats.avg_speed
        << DELIMITER << veh_stats.arrival_time.formatted_time_date()
        << DELIMITER << veh_stats.departure_time.formatted_time_date();
    if (veh_stats.permit_speeding_flag)
        logger.warning(veh_stats.departure_time, AnalysisLog( "SPEEDING", "Analysis Log", msg.str() ));
    else logger.critical(veh_stats.departure_time, AnalysisLog( "SPEEDING", "Analysis Log", msg.str() ));
}

void AnalysisEngine::end_day(SimTime &sim_time)
{
    day_count++;
    ofstream fout_data, fout_speeding;
    string filename = "data/" + data_file;
    string filename_speeding = "data/" + speeding_file;
    fout_data.open(filename, ios::out | ios::app);
    fout_speeding.open(filename_speeding, ios::out | ios::app);

    if (!fout_data.good()) {
        stringstream msg;
        msg << "Failed to open output file. Data of day " <<(day_count) << " not recorded.";
        console_log("FILE ERROR", msg.str());
    }
    if (!fout_speeding.good()) {
        stringstream msg;
        msg << "Failed to open output file. Speeding tickets of day " <<(day_count) << " not recorded.";
        console_log("FILE ERROR", msg.str());
    }

    auto iter = today_stats.begin();
    auto iter_end = today_stats.end();

    while (iter != iter_end) {
        // calculate mean of arrival speed
        double speed_mean = mean<double>((*iter).second.speed);
        (*iter).second.speed.clear(); // reset speed

        // update volume data
        auto iter_total = total_stats.find((*iter).first);
        if (iter_total != total_stats.end()) {
            (*iter_total).second.volume_dist.push_back( (*iter).second.volume );

            // write to data file
            fout_data << day_count << DELIMITER << iter->second.date.formatted_date() << DELIMITER
                 << (*iter).second.veh_name << DELIMITER << (*iter).second.volume
                 << DELIMITER << fixed << setprecision(2) << speed_mean << endl;
            (*iter).second.volume = 0; // reset volume
        }

        // update date
        (*iter).second.date.next_day();
        iter++;
    }

    // report speeding
    while (!speeding_tickets.empty()) {
        VehicleStats veh_stats = speeding_tickets[0]; // extract the first ticket

        // write ticket to file
        stringstream msg;
        msg << day_count << DELIMITER << veh_stats.veh_name
            << DELIMITER << veh_stats.registration_id
            << DELIMITER << fixed << setprecision(2) << veh_stats.avg_speed
            << DELIMITER << veh_stats.arrival_time.formatted_time_date()
            << DELIMITER << veh_stats.departure_time.formatted_time_date();
        fout_speeding << msg.str() << endl;

        // delete ticket
        speeding_tickets.erase(speeding_tickets.begin());
    }
    fout_speeding.close();
    fout_data.close();

    // log
    stringstream msg;
    msg << "Analysis day " << day_count << " finished";
    logger.info(sim_time, AnalysisLog( "NOTICE", "Analysis Log", msg.str() ));
    sim_time.next_day();
}

void AnalysisEngine::generate_stats_baseline()
{
    ofstream fout;
    string filename = "data/" + stats_file;
    fout.open(filename, ios::out);

    fout << n_vehicles << " " << road_length << " "
         << speed_limit << " " << n_parking_spots << endl;

    auto iter = total_stats.begin();
    auto iter_end = total_stats.end();
    for (; iter != iter_end; iter++) {
        fout << (*iter).first << ":"
             << fixed << setprecision(2) << mean<unsigned long>(iter->second.volume_dist) << ":"
             << fixed << setprecision(2) << std_dev<unsigned long>(iter->second.volume_dist) << ":"
             << fixed << setprecision(2) << mean<double>(iter->second.speed_dist) << ":"
             << fixed << setprecision(2) << std_dev<double>(iter->second.speed_dist) << ":" << endl;
    }
    fout.close();
}