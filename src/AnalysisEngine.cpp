//
// Created by lnduo on 17/10/18.
//

#include "AnalysisEngine.h"

AnalysisEngine::AnalysisEngine(): curr_vehicles(map<string, VehicleStats>()),
                                  stats(AnalysisStats()), road_length(0.0),
                                  speed_limit(0.0)
{
    logger = Logger<SimTime, AnalysisLog>("Analysis Engine", INFO, "logs_baseline", true);
}

AnalysisEngine::AnalysisEngine(string log_file): curr_vehicles(map<string, VehicleStats>()),
                                                 stats(AnalysisStats()), road_length(0.0),
                                                 speed_limit(0.0)
{
    logger = Logger<SimTime, AnalysisLog>("Analysis Engine", INFO, log_file, true);
}

void AnalysisEngine::run(Vehicles vehicles_dict, char* log_file)
{
    SimTime sim_time = time_now();

    cout << "[*****SYSTEM*****] Analysis Engine started: " << real_formatted_time_now() << "\n" << flush;

    // log start of Analysis Engine
    stringstream msg;
    msg << "Started Analysis Engine";
    logger.info(sim_time, AnalysisLog( "NOTICE", "Analysis Log", msg.str() ));

    import_vehicles(std::move(vehicles_dict));
    read_log(log_file);
    process_log();

    cout << "[*****SYSTEM*****] Analysis Engine finished: " << real_formatted_time_now() << "\n" << flush;
}

void AnalysisEngine::import_vehicles(Vehicles vehicles_dict)
{
    auto iter = vehicles_dict.get_vehicles_dict()->begin();
    auto iter_end = vehicles_dict.get_vehicles_dict()->end();

    for (; iter != iter_end; ++iter) {
        // initialise stats
        stats.day_volume.insert(pair<string, unsigned long>((*iter).first, 0));
        stats.total_volume.insert(pair<string, unsigned long>((*iter).first, 0));
        stats.speed.insert(pair<string, vector<double>>((*iter).first, vector<double>()));
    }
}

void AnalysisEngine::read_log(string log_file)
{
    ifstream fin;
    fin.open(log_file);

    if (!fin.good()) {
        cout << "<" << real_formatted_time_now() << "> [*****FILE ERROR*****] Failed to open log file." << endl;
        return;
    }

    string tmp;
    for (int i = 0; i < 9; i++) {
        getline(fin, tmp, ':'); // skip to road length
    }
    getline(fin, tmp, ':');
    road_length = stof(tmp);
    getline(fin, tmp, ':');
    getline(fin, tmp, ':');
    speed_limit = stof(tmp);

    while (!fin.eof()) {
        getline(fin, tmp);
        size_t pos;
        pos = tmp.find(':');
        pos = tmp.find(':', pos + 1);
        pos = tmp.find(':', pos + 1);
        size_t end_pos = tmp.find(':', pos + 1);
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

        start_pos = tmp.find(':');
        SimTime curr_time(tmp.substr(0, start_pos)); // time
        // test if the simulation has gone to the next day
        if (curr_time.compare_date(prev_time) > 0)
            end_day();
        prev_time = curr_time;

        end_pos = tmp.find(':', ++start_pos);
        start_pos = end_pos; // skip module
        end_pos = tmp.find(':', ++start_pos);
        start_pos = end_pos; // skip log level
        end_pos = tmp.find(':', ++start_pos);
        start_pos = end_pos; // skip log type

        end_pos = tmp.find(':', ++start_pos);
        EVENT_TYPE evt_type = event_type(tmp.substr(start_pos, end_pos - start_pos));
        start_pos = end_pos;

        VehicleStats veh_stats;
        end_pos = tmp.find(':', ++start_pos);
        veh_stats.veh_name = tmp.substr(start_pos, end_pos - start_pos); // vehicle name
        start_pos = end_pos;

        end_pos = tmp.find(':', ++start_pos);
        veh_stats.registration_id = tmp.substr(start_pos, end_pos - start_pos); // vehicle registration id
        start_pos = end_pos;

        if (evt_type == ARRIVAL) {
            veh_stats.arrival_time = curr_time;

            end_pos = tmp.find(':', ++start_pos);
            veh_stats.arrival_speed = stod(tmp.substr(start_pos, end_pos - start_pos));
            curr_vehicles.insert(pair<string, VehicleStats>(veh_stats.registration_id, veh_stats));

            auto iter1 = stats.speed.find(veh_stats.veh_name);
            if (iter1 != stats.speed.end())
                (*iter1).second.push_back(veh_stats.arrival_speed);
            auto iter2 = stats.day_volume.find(veh_stats.veh_name);
            if (iter2 != stats.day_volume.end())
                (*iter2).second++;
        }
        else if (evt_type == DEPART_SIDE_ROAD) {
            curr_vehicles.erase(veh_stats.registration_id);
        }
        else if (evt_type == DEPART_END_ROAD) {
            auto iter = curr_vehicles.find(veh_stats.registration_id);
            if (iter != curr_vehicles.end()) {
                veh_stats.departure_time = curr_time;
                // check speeding
                double avg_speed = road_length / (curr_time.diff(veh_stats.arrival_time));
                if (avg_speed > speed_limit)
                    add_speeding(veh_stats, avg_speed);
            }
        }
        else if (evt_type == PARKING_START) {

        }
        else if (evt_type == VEHICLE_MOVE) {

        }
        else if (evt_type == UNKNOWN) {

        }
    }

}

void add_speeding(VehicleStats veh_stats, double avg_speed)
{

}

void AnalysisEngine::end_day()
{

    stats.day_count++;
}

void AnalysisEngine::end_analysis()
{
    end_day();

}