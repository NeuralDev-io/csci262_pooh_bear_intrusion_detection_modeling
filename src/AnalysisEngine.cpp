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
    logger = Logger<SimTime, AnalysisLog>("Analysis Engine", INFO, "logs_baseline", true);
    log_file = "logs_baseline";
    data_file = "data_baseline";
    stats_file = "stats_baseline";
}

AnalysisEngine::AnalysisEngine(string log_file1): curr_vehicles(map<string, VehicleStats>()),
                                                 total_stats(map<string, AnalysisStats>()),
                                                 road_length(0.0), speed_limit(0.0), day_count(0),
                                                 n_vehicles(0), n_parking_spots(0),
                                                 speeding_tickets(vector<VehicleStats>()),
                                                 today_stats(map<string, DailyStats>()),
                                                 activity_logs(queue<string>())
{
    logger = Logger<SimTime, AnalysisLog>("Analysis Engine", INFO, log_file1, true);
    log_file = log_file1;
    data_file = "data_" + log_file1.substr(5, log_file1.length() - 5);
    stats_file = "stats_" + log_file1.substr(5, log_file1.length() - 5);
}

void AnalysisEngine::run(Vehicles &vehicles_dict)
{
    SimTime sim_time = init_time_date();

    cout << "[*****SYSTEM*****] Analysis Engine started: " << real_formatted_time_now() << "\n" << flush;

    // log start of Analysis Engine
    stringstream msg;
    msg << "Started Analysis Engine";
    logger.info(sim_time, AnalysisLog( "NOTICE", "Analysis Log", msg.str() ));

    import_vehicles(vehicles_dict);
    read_log();
    process_log();

    cout << "[*****SYSTEM*****] Analysis Engine finished: " << real_formatted_time_now() << "\n" << flush;
}

void AnalysisEngine::import_vehicles(Vehicles &vehicles_dict)
{
    auto iter = vehicles_dict.get_vehicles_dict()->begin();
    auto iter_end = vehicles_dict.get_vehicles_dict()->end();

    for (; iter != iter_end; ++iter) {
        // initialise stats
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

void AnalysisEngine::process_log()
{
    string tmp;
    SimTime prev_time;
    size_t start_pos;
    size_t end_pos;

    while (!activity_logs.empty()) {
        tmp = activity_logs.front();
        activity_logs.pop();

        start_pos = tmp.find(DELIMITER);
        SimTime curr_time(tmp.substr(0, start_pos)); // time
        // test if the simulation has gone to the next day
        if (curr_time.compare_date(prev_time) > 0
            && prev_time.compare(SimTime()) != 0) {
            end_day();
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
                veh_stats.departure_time = curr_time;
                // check speeding
                double avg_speed = road_length / curr_time.diff(veh_stats.arrival_time) * 3600;
                if (avg_speed > speed_limit) {
                    veh_stats.avg_speed = avg_speed;
                    add_speeding(veh_stats);
                }
            }
            curr_vehicles.erase(veh_stats.registration_id);
        }
    }

    end_analysis();
}

void AnalysisEngine::add_speeding(VehicleStats &veh_stats)
{
    speeding_tickets.push_back(veh_stats);

    // log warning
    stringstream msg;
    msg << veh_stats.veh_name << DELIMITER << veh_stats.registration_id
        << DELIMITER << fixed << setprecision(2) << veh_stats.avg_speed
        << DELIMITER << veh_stats.arrival_time.formatted_time_date()
        << DELIMITER << veh_stats.departure_time.formatted_time_date();
    logger.warning(veh_stats.departure_time, AnalysisLog( "SPEEDING", "Analysis Log", msg.str() ));
}

void AnalysisEngine::end_day()
{
    ofstream fout;
    string filename = "data/" + data_file;
    fout.open(filename, ios::out | ios::app);

    if (!fout.good()) {
        cout << "[*****FILE ERROR*****] " << "<" << real_formatted_time_now()
             << "> Failed to open output file. Data of day " << (day_count+1) << " not recorded." << endl;
    }

    fout << "Day " << (day_count+1) << ": " << today_stats.begin()->second.date.formatted_time_date() << endl;
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
            fout << "\t" << (*iter).second.veh_name << DELIMITER << "Volume=" << (*iter).second.volume
                 << DELIMITER << "Speed mean=" << fixed << setprecision(2) << speed_mean << endl;
            (*iter).second.volume = 0; // reset volume
        }

        // update date
        (*iter).second.date.next_day();
        iter++;
    }
    day_count++;

    // report speeding
    if (!speeding_tickets.empty())
        fout << "\tSPEEDING TICKETS:" << endl;
    while (!speeding_tickets.empty()) {
        VehicleStats veh_stats = speeding_tickets[0]; // extract the first ticket

        // write ticket to file
        stringstream msg;
        msg << "\t" << veh_stats.veh_name << DELIMITER << veh_stats.registration_id
            << DELIMITER << fixed << setprecision(2) << veh_stats.avg_speed
            << DELIMITER << veh_stats.arrival_time.formatted_time_date()
            << DELIMITER << veh_stats.departure_time.formatted_time_date();
        fout << msg.str() << endl;

        // delete ticket
        speeding_tickets.erase(speeding_tickets.begin());
    }
    fout.close();

    // log
    SimTime sim_time = init_time_date();
    stringstream msg;
    msg << "Analysis day " << day_count << " finished";
    logger.info(sim_time, AnalysisLog( "NOTICE", "Analysis Log", msg.str() ));
}

void AnalysisEngine::end_analysis()
{
    end_day(); // end the current day

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
             << fixed << setprecision(2) << std_dev<double>(iter->second.speed_dist) << endl;
    }
    fout.close();
}