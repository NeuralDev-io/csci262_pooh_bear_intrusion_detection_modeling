/*********************************************************************************
* CSCI262 (Spring 2018) - Assignment 03
* Pooh Bear Intrusion Detection System ActivityEngine.cpp
* Purpose: Creates a traffic simulation engine based on reading of input from
*          Vehicles.txt and Stats.txt, as well as an argument for how many days
*          to simulate traffic.
*
* @version 0.6-dev
* @date 2018.10.06
*
* @authors Dinh Che (codeninja55) & Duong Le (daltonle)
* Emails andrew at codeninja55.me & duong.daltonle at gmail.com
* Students Dinh Che (5721970 | dbac496) & Duong Le (5560536 | ndl991)
*********************************************************************************/
#include "ActivityEngine.h"
#include <cassert>

ActivityEngine::ActivityEngine() : n_vehicles_monitored(0), n_parking_spots(0), road_length(0), speed_limit(0), simulate_days(0)
{
    // Set the last param to true if you want to output log to stdout
    log_file =  LOGS_FILENAME;
    logger = Logger<SimTime, ActivityLog>("Activity Engine", INFO, log_file, false);
    veh_logger = Logger<SimTime, VehicleLog>("Activity Engine", INFO, log_file, false);
    other_veh_logger = Logger<SimTime, GenericLog>("Activity Engine", INFO, log_file, false);
    mersenne_twister_engine.seed(SYSTEM_SEED);
}


/*
 * @brief: a constructor that sets the file name to log to.
 *
 * @param log_file: a string of the filename to be used. Must be consistent with Analysis Engine so it is easy to read.
 * */
ActivityEngine::ActivityEngine(string log_file) : n_vehicles_monitored(0), n_parking_spots(0), road_length(0),
                                                  speed_limit(0), simulate_days(0), log_file(log_file)
{
    logger = Logger<SimTime, ActivityLog>("Activity Engine", INFO, log_file, false);
    veh_logger = Logger<SimTime, VehicleLog>("Activity Engine", INFO, log_file, false);
    other_veh_logger = Logger<SimTime, GenericLog>("Activity Engine", INFO, log_file, false);
    mersenne_twister_engine.seed(SYSTEM_SEED);
}

void ActivityEngine::run(Vehicles &vehicles)
{
    SimTime sim_time = init_time_date();

    char pwd[100];
    getcwd(pwd, sizeof(pwd));
    stringstream console_msg;
    console_msg << "Activity Engine Started and logging to: " << pwd << dir_slash << "logs" << dir_slash << log_file;
    console_log("SYSTEM", console_msg.str());

    // log for the number of Days specified at the initial running of Traffic
    stringstream msg;
    msg << "Started Activity Engine" << DELIMITER << "Number of days=" << simulate_days << DELIMITER
        << "Road length=" << road_length << DELIMITER << "Speed limit=" << speed_limit << DELIMITER << "Parking spots="
        << n_parking_spots;
    logger.info(sim_time, ActivityLog( "NOTICE", "Activity Log", msg.str() ));

    // run the simulation for the first day before moving the sim_time to next_day()
    start_generating_discrete_events(sim_time, vehicles);

    for (int i = 0; i < simulate_days; i++) {
        msg.str(string());
        msg << "Generating Discrete Events for " << sim_time.formatted_date();
        logger.info(sim_time, ActivityLog("NOTICE", "Activity Log", msg.str()));
        console_log("SYSTEM", msg.str());
        simulate_events();
        sim_time.next_day();
        start_generating_discrete_events(sim_time, vehicles);
    }

    console_log("SYSTEM", "Activity Engine Completed.");
}

void ActivityEngine::start_generating_discrete_events(SimTime &sim_time, Vehicles &vehicles)
{
    int i;
    auto iter = vehicles.get_vehicles_dict()->begin();
    auto iter_end = vehicles.get_vehicles_dict()->end();

    for (; iter != iter_end; ++iter) {
        // @param mean and std. dev
        normal_distribution<double> num_normal_distrib((*iter).second.num_mean, (*iter).second.num_stddev);
        normal_distribution<double> speed_normal_distrib((*iter).second.speed_mean, (*iter).second.speed_stddev);

        // Random number of n occurrences of vehicle over a day
        auto n_arrivals = 0;
        while (n_arrivals <=0)
            n_arrivals = static_cast<int>(lround(num_normal_distrib(mersenne_twister_engine)));
        // rate of occurrence
        double lambda = n_arrivals / T_ARRIVAL_LIMIT;  // converted to seconds i.e. N occurrence per X seconds

        simtime_t tmp_ts_arrivals;
        vector<simtime_t> ts_arrivals_ls;
        exponential_distribution<simtime_t> expovariate(lambda);

        // cout << "\n" << (*iter).first << "(" << setprecision(6) << lambda << ")" << "[" << n_arrivals << "]" << endl;

        /* ARRIVALS */
        /*
         * Generate the next arrival delta interval using an exponential distribution and the poisson process.
         * The random deltas, being a continuous distribution, may exceed the total time T_ARRIVAL_LIMIT of arrivals
         * up to 23:00. To overcome this, run a loop up to a max 100 times and make a vector of timestamps.
         * If the timestamps do not exceed the T_ARRIVAL_LIMIT, and it matches the N number of vehicles to arrive
         * on road, then return the values and continue.
         * */
        for (i = 0; i < 1000; i++) {
            tmp_ts_arrivals = 0;
            do {
                double delta = expovariate(mersenne_twister_engine);
                tmp_ts_arrivals += delta;
                if (tmp_ts_arrivals < T_ARRIVAL_LIMIT)
                    ts_arrivals_ls.push_back(tmp_ts_arrivals);
            } while (tmp_ts_arrivals < T_ARRIVAL_LIMIT);

            if (ts_arrivals_ls.size() == n_arrivals)
                break;
            else
                ts_arrivals_ls.clear();
        }

        // with a list of timestamps for this vehicle type, create vehicle stats
        for (simtime_t &ts_arrival : ts_arrivals_ls) {

            auto *veh = new VehicleStats();
            veh->veh_name = (*iter).first;
            veh->registration_id = Vehicles::generate_registration((*iter).second.reg_format, mersenne_twister_engine);
            veh->permit_speeding_flag = ((*iter).second.speed_weight == 0);

            /* Event ==> ARRIVAL */
            // make sure it is non-negative because not possible
            do {
                veh->arrival_speed = speed_normal_distrib(mersenne_twister_engine);
            } while (veh->arrival_speed < 0);

            // add arrival event to queue
            SimTime arrival_time(sim_time);  // initializes the date to the right day of processing
            arrival_time.mktime(ts_arrival);  // make a time based on the timestamp
            veh->arrival_time = arrival_time;
            veh->arrival_timestamp = ts_arrival;

            Event arrival_event(ARRIVAL, arrival_time, veh);
            future_event_list.push(arrival_event);

            // random binomial distribution true or false based on the probability of the event occurring
            bernoulli_distribution depart_side_random(DEPART_SIDE_PROBABILITY);
            veh->depart_side_flag = depart_side_random(mersenne_twister_engine);  // bernoulli implem returns bool

            // calculate estimate depart time
            veh->estimated_travel_delta = estimate_departure_delta(*veh, veh->arrival_speed);
            veh->estimated_depart_timestamp = estimate_departure_time(*veh, ts_arrival);

            process_parking_events(sim_time, (*iter).second.parking_flag, veh);
            // process_departure_events(sim_time, (*iter).second, veh_stats);

            // TODO: debug
            cout << fixed << setprecision(2) << "Arrival <" << veh->arrival_time.tm_timestamp << " secs>: "
                 << veh->arrival_time.formatted_time() << " ==> " << veh->veh_name << " "
                 << veh->registration_id << " (" << veh->arrival_speed << " kmh)" << endl;

            if (veh->n_parking > 0) {
                cout << "Parking: ";
                for (i = 0; i < veh->n_parking; i++) {
                    SimTime parking_time;
                    parking_time.mktime(veh->ts_parking_ls[i]);
                    cout << "[ " << parking_time.formatted_time() <<" | " << veh->ts_parking_ls[i] << " ] >> ";
                }

                cout << "\nParking durations: ";
                for (i = 0; i < veh->n_parking; i++) {
                    cout << "[ " << veh->ts_parking_duration[i]
                         << " ] >> ";
                }

                cout << "\nParking move: ";
                for (i = 0; i < veh->ts_vehicle_move_ls.size(); i++) {
                    SimTime veh_move_time;
                    veh_move_time.mktime(veh->ts_vehicle_move_ls[i]);
                    cout << "[ " << veh_move_time.formatted_time() << " | " << veh->ts_vehicle_move_ls[i]
                         << " ] >> ";
                }
                cout << endl;
            }

            if (veh->depart_side_flag)
                cout << "\nSide Exit: " << veh->departure_time.formatted_time() << " ==> " << veh->registration_id << endl;

            /*cout << "Departure <"<< veh->departure_time.tm_timestamp << " secs>: "
                 << veh->departure_time.formatted_time() << " ==> "
                 << veh->registration_id << " [Est. Delta: " << veh->estimated_travel_delta << "] ("
                 << veh->avg_speed << " kmh)" << endl;*/
        }
    }
}

void ActivityEngine::process_parking_events(SimTime &sim_time, bool parking_flag, VehicleStats *veh)
{
    int i;
    /* Event ==> PARKING_START */
    // randomly generate the number of times a person will park and use the  poisson process to generate
    // inter-arrival times between parking events.
    if (parking_flag) {
        binomial_distribution<int> parking_n_random((int) AVG_PARKING_N, PARKING_PROBABILITY);

        int tmp_n_parking = 0;
        while (tmp_n_parking < 2)
            tmp_n_parking = parking_n_random(mersenne_twister_engine);
        veh->n_parking = tmp_n_parking - 1;

        simtime_t ts_parking;
        double mean = 1 / AVG_PARKING_EVENT;
        // double mean = tmp_n_parking / (T_DAY_LIMIT - veh->arrival_timestamp);
        exponential_distribution<simtime_t> parking_expovariate(mean);
        for (i = 0; i < 100; i++) {
            ts_parking = veh->arrival_timestamp;
            do {
                double delta = parking_expovariate(mersenne_twister_engine);
                ts_parking += delta;
                if (ts_parking < T_DAY_LIMIT) veh->ts_parking_ls.push_back(ts_parking);
                if (veh->ts_parking_ls.size() == tmp_n_parking) break;
            } while (ts_parking < T_DAY_LIMIT);

            if (veh->ts_parking_ls.size() == tmp_n_parking)
                break;
            else
                veh->ts_parking_ls.clear();
        }

        for (i = 0; i < veh->n_parking; i++) {
            // add the parking start event to the FEL.
            SimTime parking_time(sim_time);
            parking_time.mktime(veh->ts_parking_ls[i]);
            Event parking_event(PARKING_START, parking_time, veh);
            future_event_list.push(parking_event);

            /* PARKING DURATIONS */
            exponential_distribution<simtime_t> duration_expovariate(1 / AVG_PARKING_DURATION);

            simtime_t ts_vehicle_move = 0;
            for (int j = 0; j < 100; j++) {
                ts_vehicle_move = veh->ts_parking_ls[i];
                do {
                    double veh_move_delta = duration_expovariate(mersenne_twister_engine);
                    ts_vehicle_move += veh_move_delta;
                    if (ts_vehicle_move < veh->ts_parking_ls[i+1])
                        break;
                } while (ts_vehicle_move < veh->ts_parking_ls[i+1]);
                if (ts_vehicle_move < veh->ts_parking_ls[i+1]) break;
            }

            veh->ts_parking_duration.push_back(ts_vehicle_move - veh->ts_parking_ls[i]);
            veh->ts_vehicle_move_ls.push_back(ts_vehicle_move);

            // add the vehicle move event to the FEL
            SimTime veh_move_time(parking_time);
            veh_move_time.mktime(ts_vehicle_move);
            Event vehicle_move_event(VEHICLE_MOVE, veh_move_time, veh);
            future_event_list.push(vehicle_move_event);
        }
    }
}

void ActivityEngine::process_departure_events(SimTime &sim_time, VehicleType &veh_type, VehicleStats *veh_stats)
{
    /*
     * If the vehicle is due to exit via a side road, generate an exit timestamp.
     * To do this, make a biased uniform distribution that only gives small values.
     * To run this through the exponential distribution, it will always do a ln of a value extremely
     * close to 1 and then divide that by a very small lambda. This means the vehicle will be biased
     * to exit the road very soon.
     * */
    if (veh_stats->depart_side_flag) {
        /* Event ==> DEPART_SIDE_ROAD */
        SimTime depart_side_time(sim_time);
        double exit_interval;
        simtime_t ts_depart_side = veh_stats->arrival_timestamp;  // can only exit after arrival time

        // if (veh->n_parking != 0) {
        //     double total_parking_duration = accumulate(next(veh->ts_parking_duration.begin()),
        //             veh->ts_parking_duration.end(), veh->ts_parking_duration[0]);
        //     exit_interval = T_DAY_LIMIT - ( total_parking_duration + veh->arrival_time.tm_timestamp);
        //     ts_depart_side += total_parking_duration;
        // } else {
        //     exit_interval = T_DAY_LIMIT - veh->arrival_timestamp;
        // }
        //
        // while (ts_depart_side < T_DAY_LIMIT) {
        //     long double test = biased_expovariate(1/exit_interval, 0, DEPART_SIDE_UPPER_BOUND);
        //     if (ts_depart_side + test < T_DAY_LIMIT) {
        //         ts_depart_side += test;
        //         break;
        //     }
        // }



        depart_side_time.mktime(ts_depart_side);
        veh_stats->departure_time = depart_side_time;
        veh_stats->departure_timestamp = ts_depart_side;
        Event exit_event(DEPART_SIDE_ROAD, depart_side_time, veh_stats);
        future_event_list.push(exit_event);
    } else {
        /* Event ==> DEPART_END */
        SimTime depart_end_time(sim_time);
        simtime_t ts_depart_end = 0;
        // estimate of when they would exit at end of road if they were going at arrival speeds
        double estimated_travel_delta = (road_length / veh_stats->arrival_speed) * 3600;
        // check if parking time needs to be added
        if (veh_stats->n_parking != 0) {
            simtime_t estimated_t_depart_end = veh_stats->arrival_time.tm_timestamp
                                               + accumulate(next(veh_stats->ts_parking_duration.begin()),
                                                            veh_stats->ts_parking_duration.end(),
                                                            veh_stats->ts_parking_duration[0])
                                               + estimated_travel_delta;
            // using the estimated departure time as a mean,with 2 standard deviation difference, randomly select
            // a time the vehicle will depart.
            normal_distribution<double> depart_end_random(estimated_t_depart_end, 2);
            // TODO: should probably check if this time is after 24:00
            ts_depart_end = depart_end_random(mersenne_twister_engine);
            // while (t_depart_end > T_DAY_LIMIT || t_depart_end < veh_stats->ts_parking_ls[veh_stats->n_parking - 1])
            //     ts_depart_end = depart_end_random(mersenne_twister_engine);
            depart_end_time.mktime(ts_depart_end);
        } else {
            // TODO: Method 1
            // simtime_t estimated_t_depart_end = veh_stats->arrival_time.tm_timestamp + estimated_travel_delta;
            // normal_distribution<double> depart_end_random(estimated_t_depart_end, 3);
            // ts_depart_end = depart_end_random(mersenne_twister_engine);
            // TODO: Method 2
            normal_distribution<double> depart_end_random(estimated_travel_delta, 3);
            ts_depart_end = depart_end_random(mersenne_twister_engine) + veh_stats->arrival_time.tm_timestamp;
            // TODO: Method 3
            // exponential_distribution<double> depart_end_expovariate(1/(T_DAY_LIMIT - veh_stats->arrival_time.tm_timestamp));
            // ts_depart_end = veh_stats->arrival_time.tm_timestamp + depart_end_expovariate(mersenne_twister_engine);
            depart_end_time.mktime(ts_depart_end);
        }

        veh_stats->estimated_travel_delta = estimated_travel_delta;
        veh_stats->departure_time = depart_end_time;
        veh_stats->avg_speed = (road_length * 3600) /
                               (veh_stats->departure_time.tm_timestamp - veh_stats->arrival_time.tm_timestamp);
        Event depart_end_event(DEPART_END_ROAD, depart_end_time, veh_stats);
        future_event_list.push(depart_end_event);
    }
}

void ActivityEngine::simulate_events()
{
    while (!future_event_list.empty()) {
        Event ev = future_event_list.top();  // returns a reference to the top event but does not remove it

        // TODO: check if time is after limit. Then discard
        // if (ev.time.tm_timestamp > T_DAY_LIMIT) {
        //     future_event_list.pop();
        //     continue;
        // }

        switch(ev.ev_type) {
            case ARRIVAL:
                veh_logger.info(ev.time, VehicleLog(ARRIVAL, "Vehicle Log", ev.stats->veh_name, ev.stats->registration_id,
                        ev.stats->arrival_speed ));
                break;
            case DEPART_SIDE_ROAD:
                other_veh_logger.info(ev.time, GenericLog(DEPART_SIDE_ROAD, "Vehicle Log", ev.stats->veh_name,
                        ev.stats->registration_id));
                break;
            case PARKING_START:
                other_veh_logger.info(ev.time, GenericLog(PARKING_START, "Vehicle Log", ev.stats->veh_name,
                        ev.stats->registration_id));
                break;
            case VEHICLE_MOVE:
                other_veh_logger.info(ev.time, GenericLog(VEHICLE_MOVE, "Vehicle Log", ev.stats->veh_name,
                        ev.stats->registration_id));
                break;
            case DEPART_END_ROAD:
                if (ev.stats->avg_speed > speed_limit && !ev.stats->permit_speeding_flag) {
                    veh_logger.critical(ev.time, VehicleLog(DEPART_END_ROAD, "Vehicle Log", ev.stats->veh_name,
                                                        ev.stats->registration_id, ev.stats->avg_speed));
                } else if (ev.stats->avg_speed > speed_limit && ev.stats->permit_speeding_flag) {
                    veh_logger.warning(ev.time, VehicleLog(DEPART_END_ROAD, "Vehicle Log", ev.stats->veh_name,
                                                           ev.stats->registration_id, ev.stats->avg_speed));
                } else {
                    veh_logger.info(ev.time, VehicleLog(DEPART_END_ROAD, "Vehicle Log", ev.stats->veh_name,
                                                        ev.stats->registration_id, ev.stats->avg_speed));
                }
                break;
            case UNKNOWN: break;
        }
        future_event_list.pop();  // returns void, but must remove from queue
    }
}

double ActivityEngine::estimate_departure_delta(VehicleStats &veh, double speed) {
    // estimate of when they would exit at end of road if they were going at arrival speeds
    return (road_length / speed) * 3600;
}

double ActivityEngine::estimate_departure_time(VehicleStats &veh, simtime_t start_timestamp)
{
    double depart_avg = 0;
    int variance = (veh.depart_side_flag) ? 10 : 5;

    // run the random estimation of exit times a few iterations and get their average
    normal_distribution<double> est_depart_random(veh.estimated_travel_delta, variance);
    for (int i = 0; i <= 10; i++) {
        double test = 0;
        do {
            test = est_depart_random(mersenne_twister_engine);
        } while (test < 0);

        depart_avg += test;
    }
    return start_timestamp + (depart_avg / 10);
}

/*
 * @brief: a biased function to return the variate from an exponential distribution with a lower and upper
 * bound that does not necessarily need to be [0, 1].
 *
 * @param rate_param: the rate of occurrence for the event.
 * @param lower_bound: the lower bound of the bias, must be >= 0.
 * @param upper_bound: the upper bound of the bias, must be <= 1.0.
 *
 * @return: the y value of the exponential distribution which is the likely next interval occurrencee
 *          of the event defined by the rate parameter or lambda.
 * */
long double ActivityEngine::biased_expovariate(double rate_param, double lower_bound, double upper_bound)
{
    assert (lower_bound >= 0);
    assert (upper_bound <= 1.0F);
    uniform_real_distribution<double> biased_distribution(lower_bound, upper_bound);
    long double rand_val = biased_distribution(mersenne_twister_engine);
    while (rand_val == 1.0F)  // check if the random value is 1 to avoid passing 0 to logb()
        rand_val = biased_distribution(mersenne_twister_engine);
    return -logl(1.0F - rand_val) / rate_param;
}

/*
 * Function to the activity engine to set stats value to be used to
 * create a statistical simulation model
 *
 * @param days: number of days to run the simulation
 * @param vehicles_monitored: number of vehicle types to be monitored
 * @param road_len: length of the road in km
 * @param speed_lim: speed limit in km/h
 * @param parking_spots: number of parking spaces available
 * */
void ActivityEngine::set_statistics(unsigned days, unsigned vehicles_monitored, float road_len,
                                    float speed_lim, unsigned parking_spots)
{
    simulate_days = days;
    n_vehicles_monitored = vehicles_monitored;
    n_parking_spots = parking_spots;
    speed_limit = speed_lim;
    road_length = road_len;
}
