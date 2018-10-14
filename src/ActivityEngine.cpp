/*********************************************************************************
* CSCI262 (Spring 2018) - Assignment 03
* Pooh Bear Intrusion Detection System ActivityEngine.cpp
* Purpose: Creates a traffic simulation engine based on reading of input from
*          Vehicles.txt and Stats.txt, as well as an argument for how many days
 *         to simulate traffic.
*
* @version 0.3-dev
* @date 2018.10.06
*
* @authors Dinh Che (codeninja55) & Duong Le (daltonle)
* Emails andrew at codeninja55.me & duong.daltonle at gmail.com
* Students Dinh Che (5721970 | dbac496) & Duong Le (5560536 | ndl991)
*********************************************************************************/
#include "ActivityEngine.h"

long double exit_occurrence(double rate_param, uniform_real_distribution<double> random, mt19937_64 mt_engine);

/*
 * Constructor to the activity engine to store stats value to be used to
 * create a statistical simulation model
 *
 * @param days: number of days to run the simulation
 * @param vehicles_monitored: number of vehicle types to be monitored
 * @param road_len: length of the road in km
 * @param speed_lim: speed limit in km/h
 * @param parking_spots: number of parking spaces available
 * */
ActivityEngine::ActivityEngine(uint days, uint vehicles_monitored, float road_len, float speed_lim, uint parking_spots)
{
    simulate_days = days;
    n_vehicles_monitored = vehicles_monitored;
    n_parking_spots = parking_spots;
    speed_limit = speed_lim;
    road_length = road_len;

    time_seed = static_cast<unsigned long>(chrono::system_clock::now().time_since_epoch().count());
    default_engine.seed(0);
    linear_congruential_engine.seed(0);
    mersenne_twister_engine.seed(0);
}


void ActivityEngine::run(Vehicles &vehicles)
{
    SimTime sim_time = time_now();

    cout << "Traffic Engine started: " << real_formatted_time_now() << "\n" << flush;
    // Set the last param to true if you want to output log to stdout
    logger = Logger<ActivityLog, SimTime>("Activity Engine", WARNING, "test.txt", true);
    veh_logger = Logger<VehicleLog, SimTime>("Activity Engine", WARNING, "test.txt", true);
    // log for the number of Days specified at the initial running of Traffic
    stringstream msg;
    msg << "Started Traffic Engine for number of days " << simulate_days;
    logger.info(sim_time, { "NOTICE", "Activity Log", msg.str() });

    generate_arrivals(vehicles);
    // TODO: time should be stepped in 1 minute blocks
    // simulate_events();

    // TODO: program should give some indication as to what is happening, without being verbose

    cout << "Activity Engine finished: " << real_formatted_time_now() << "\n" << flush;
}

void ActivityEngine::generate_arrivals(Vehicles &vehicles)
{
    const double day_min_interval = 1439.0F;
    auto iter = vehicles.get_vehicles_dict()->begin();
    auto iter_end = vehicles.get_vehicles_dict()->end();

    // @param min and max
    uniform_real_distribution<double> uniform_distribution(0, 1);

    for (; iter != iter_end; ++iter) {

        // TODO: Ensure the returned value from a normal distribution is not negative because of a large std. dev.
        // @param mean and std. dev
        normal_distribution<double> num_normal_distrib((*iter).second.num_mean, (*iter).second.num_stddev);
        normal_distribution<double> speed_normal_distrib((*iter).second.speed_mean, (*iter).second.speed_stddev);

        // Random number of X occurrences of vehicle over a day
        auto N = static_cast<uint>(lround(num_normal_distrib(default_engine)));
        // rate of occurrence
        const double arrival_time_interval = 23.0F;
        double lambda = N / (arrival_time_interval * 60.0F);  // converted to minutes i.e. 1 occurrence per minute

        // TODO: debug
        cout << "\nVehicle type: " << (*iter).first << " (" << N << ") [" << setprecision(6) << lambda << "]" << endl;

        double limit = arrival_time_interval * 60.0F;
        double timestamps;
        vector<double> timestamps_list;
        exponential_distribution<double> expovariate(lambda);

        // TODO: Add references (TAOCP 2 and https://preshing.com/20111007/how-to-generate-random-timings-for-a-poisson-process/)
        // TODO: Another reference (https://keystrokecountdown.com/articles/poisson/index.html)
        /*
         * Generate the next arrival delta interval using an exponential distribution and the poisson process.
         * The random deltas, being a continuous distribution, may exceed the total time limit of arrivals up to
         * 23:00. To overcome this, run a loop up to a max 1000 times and make a vector of timestamps.
         * If the timestamps do not exceed the limit, and it matches the N number of vehicles to arrive on road,
         * then return the values and continue.
         * */
        for (int j = 0; j < 1000; j++) {
            timestamps = 0;
            do {
                double delta = expovariate(mersenne_twister_engine);
                timestamps += delta;
                if (timestamps < limit)
                    timestamps_list.push_back(timestamps);
            } while (timestamps < limit);

            if (timestamps_list.size() == N)
                break;
            else
                timestamps_list.clear();
        }

        for (double &timestamp : timestamps_list) {

            VehicleStats veh_stats;
            veh_stats.veh_name = (*iter).first;
            veh_stats.registration_id = Vehicles::generate_registration((*iter).second.reg_format, default_engine);

            // make sure it is non-negative because not possible
            do {
                veh_stats.arrival_speed = speed_normal_distrib(default_engine);
            } while (veh_stats.arrival_speed < 0);

            // add event to queue
            SimTime event_time;
            event_time = time_now();

            event_time.tm_hour = static_cast<int>(lround(timestamp)) / 60;
            event_time.tm_min = (static_cast<int>(lround(timestamp))) % 60;
            // event_time.tm_sec = static_cast<int>(it) % 60;
            veh_stats.arrival_time = event_time;

            // set probability for other 3 events
            veh_stats.prob_parking = (!(*iter).second.parking_flag) ? 0 : 0.02;
            veh_stats.prob_end_exit = 0.5;

            veh_stats.prob_side_exit = 0.2;
            bernoulli_distribution side_exit_random(veh_stats.prob_side_exit);
            veh_stats.side_exit_flag = side_exit_random(default_engine);

            SimTime exit_time = time_now();
            if (veh_stats.side_exit_flag) {
                double exit_interval = day_min_interval - timestamp;
                double exit_timestamp = timestamp;  // can only exit after arrival time
                exponential_distribution<double> exit_expovariate(1 / exit_interval);

                while (exit_timestamp < limit) {
                    double test = exit_expovariate(mersenne_twister_engine);
                    if (exit_timestamp + test < limit) {
                        exit_timestamp += test;
                        break;
                    }
                }

                exit_time.tm_min = static_cast<int>(lround(exit_timestamp)) % 60;
                exit_time.tm_hour = static_cast<int>(lround(exit_timestamp)) / 60;
            }


            // Event arrival_event = { ARRIVAL, event_time, veh_stats };
            // event_q.push(arrival_event);

            // TODO: debug
            cout << "Arrival: " << event_time.formatted_time() << " " << veh_stats.registration_id << " ==> "
                 << "(" << veh_stats.arrival_speed << ") -> Exit: " << ((veh_stats.side_exit_flag) ? "T" : "F")
                 << " -> Exit timestamp: " << exit_time.formatted_time()
                 << endl;
        }
    }
}

void ActivityEngine::simulate_events()
{
    int t;
    for (t = 0; t <= 1439; t++) {
        Event ev = event_q.top();  // returns a reference to the top element

        if ((ev.time.tm_hour * 60 + ev.time.tm_min) == t) {

        }

    }

    /*while (!event_q.empty()) {
        Event ev = event_q.top();  // returns a reference to the top event but does not remove it
        switch(ev.ev_type) {
            case ARRIVAL:
                veh_logger.info(ev.time, { ARRIVAL, "Vehicle Log", ev.stats.veh_name, ev.stats.registration_id,
                                       ev.stats.arrival_speed });
                event_q.pop();  // returns void, but must remove from queue
                break;
        }
    }*/
}
