/*********************************************************************************
* CSCI262 (Spring 2018) - Assignment 03
* Pooh Bear Intrusion Detection System ActivityEngine.cpp
* Purpose: Creates a traffic simulation engine based on reading of input from
*          Vehicles.txt and Stats.txt, as well as an argument for how many days
*          to simulate traffic.
*
* @version 0.5-dev
* @date 2018.10.06
*
* @authors Dinh Che (codeninja55) & Duong Le (daltonle)
* Emails andrew at codeninja55.me & duong.daltonle at gmail.com
* Students Dinh Che (5721970 | dbac496) & Duong Le (5560536 | ndl991)
*********************************************************************************/
#include "ActivityEngine.h"
#include <assert.h>

bool DEBUG_MODE = true;

ActivityEngine::ActivityEngine() : n_vehicles_monitored(0), n_parking_spots(0), road_length(0),
                                   speed_limit(0), simulate_days(0), time_seed(0)
{
    // Set the last param to true if you want to output log to stdout
    logger = Logger<SimTime, ActivityLog>("Activity Engine", INFO, "logs_baseline", !DEBUG_MODE);
    veh_logger = Logger<SimTime, VehicleLog>("Activity Engine", INFO, "logs_baseline", !DEBUG_MODE);

    time_seed = static_cast<unsigned long>(chrono::system_clock::now().time_since_epoch().count());
    default_engine.seed(0);
    linear_congruential_engine.seed(0);
    mersenne_twister_engine.seed(0);
}


/*
 * @brief:
 *
 * @param log_file:
 * */
ActivityEngine::ActivityEngine(string log_file)
{
    // Set the last param to true if you want to output log to stdout
    logger = Logger<SimTime, ActivityLog>("Activity Engine", INFO, log_file, !DEBUG_MODE);
    veh_logger = Logger<SimTime, VehicleLog>("Activity Engine", INFO, log_file, !DEBUG_MODE);

    time_seed = static_cast<unsigned long>(chrono::system_clock::now().time_since_epoch().count());
    default_engine.seed(0);
    linear_congruential_engine.seed(0);
    mersenne_twister_engine.seed(0);
}

void ActivityEngine::run(Vehicles &vehicles)
{
    SimTime sim_time = initialise_time();

    cout << "[*****SYSTEM*****] Activity Engine started: " << real_formatted_time_now() << "\n" << flush;

    // log for the number of Days specified at the initial running of Traffic
    stringstream msg;
    msg << "Started Activity Engine" << DELIMITER << "Number of days=" << simulate_days << DELIMITER
        << "Road length=" << road_length << DELIMITER << "Speed limit=" << speed_limit << DELIMITER << "Parking spots="
        << n_parking_spots;
    logger.info(sim_time, ActivityLog( "NOTICE", "Activity Log", msg.str() ));

    // TODO: for i... 0 -> N days
    generate_discrete_events(vehicles);
    simulate_events();

    // TODO: program should give some indication as to what is happening, without being verbose

    cout << "[*****SYSTEM*****] Activity Engine finished: " << real_formatted_time_now() << "\n" << flush;
}

void ActivityEngine::generate_discrete_events(Vehicles &vehicles)
{
    auto iter = vehicles.get_vehicles_dict()->begin();
    auto iter_end = vehicles.get_vehicles_dict()->end();
    int i;

    // TODO: debug
    cout << "T_arrival_limit: " << fixed << setprecision(2) << T_arrival_limit << endl;
    cout << "T_limit: " << fixed << setprecision(2) << T_day_limit << endl;

    for (; iter != iter_end; ++iter) {

        // @param mean and std. dev
        normal_distribution<double> num_normal_distrib((*iter).second.num_mean, (*iter).second.num_stddev);
        normal_distribution<double> speed_normal_distrib((*iter).second.speed_mean, (*iter).second.speed_stddev);

        // Random number of X occurrences of vehicle over a day
        auto N_arrival = static_cast<unsigned int>(lround(num_normal_distrib(default_engine)));
        // rate of occurrence
        double lambda = N_arrival / T_arrival_limit;  // converted to seconds i.e. 1 occurrence per X seconds

        // TODO: debug
        cout << "\nVehicle type: " << (*iter).first << " (" << N_arrival << ") [" << fixed << setprecision(6) << lambda << "]" << endl;

        simtime_t ts_arrivals;
        vector<simtime_t> ts_arrivals_ls;
        exponential_distribution<simtime_t> expovariate(lambda);

        // TODO: Add references (TAOCP 2 and https://preshing.com/20111007/how-to-generate-random-timings-for-a-poisson-process/)
        // TODO: Another reference (https://keystrokecountdown.com/articles/poisson/index.html)

        /* ARRIVALS */
        /*
         * Generate the next arrival delta interval using an exponential distribution and the poisson process.
         * The random deltas, being a continuous distribution, may exceed the total time t_limit of arrivals up to
         * 23:00. To overcome this, run a loop up to a max 1000 times and make a vector of timestamps.
         * If the timestamps do not exceed the t_limit, and it matches the N number of vehicles to arrive on road,
         * then return the values and continue.
         * */
        for (i = 0; i < 1000; i++) {
            ts_arrivals = 0;
            do {
                double delta = expovariate(mersenne_twister_engine);
                ts_arrivals += delta;
                if (ts_arrivals < T_day_limit)
                    ts_arrivals_ls.push_back(ts_arrivals);
            } while (ts_arrivals < T_day_limit);

            if (ts_arrivals_ls.size() == N_arrival)
                break;
            else
                ts_arrivals_ls.clear();
        }

        // with a list of timestamps for this vehicle type, create vehicle stats
        for (simtime_t &t_arrival : ts_arrivals_ls) {

            VehicleStats veh_stats;
            veh_stats.veh_name = (*iter).first;
            veh_stats.registration_id = Vehicles::generate_registration((*iter).second.reg_format, default_engine);

            // make sure it is non-negative because not possible
            do {
                veh_stats.arrival_speed = speed_normal_distrib(default_engine);
            } while (veh_stats.arrival_speed < 0);

            // add arrival event to queue
            SimTime arrival_time = initialise_time();
            arrival_time.mktime(t_arrival);
            veh_stats.arrival_time = arrival_time;

            /* PARKING_START */
            veh_stats.prob_parking = (!(*iter).second.parking_flag) ? 0 : 0.2;
            // randomly generate the number of times a person will park and use the  poisson process to generate
            // inter-arrival times between parking events.
            if (veh_stats.prob_parking != 0) {
                binomial_distribution<int> parking_n_random(n_parking_spots, veh_stats.prob_parking);
                veh_stats.n_parking = parking_n_random(default_engine);

                simtime_t ts_parking;
                exponential_distribution<simtime_t> parking_expovariate(veh_stats.n_parking/(T_limit - t_arrival));
                for (i = 0; i < 100; i++) {
                    ts_parking = t_arrival;
                    do {
                        double delta = parking_expovariate(mersenne_twister_engine);
                        ts_parking += delta;
                        if (ts_parking < T_limit)
                            veh_stats.ts_parking_ls.push_back(ts_parking);
                    } while (ts_parking < T_limit);

                    if (veh_stats.ts_parking_ls.size() == veh_stats.n_parking)
                        break;
                    else
                        veh_stats.ts_parking_ls.clear();
                }

                for (i = 0; i < veh_stats.ts_parking_ls.size(); i++) {
                    // add the event durations
                    // normal_distribution<simtime_t> duration_distribution(
                    //         (veh_stats.ts_parking_ls[i] - veh_stats.ts_parking_ls[i+1]), 1);

                    // add the parking start event to the FEL.
                    SimTime parking_time(arrival_time);
                    parking_time.mktime(veh_stats.ts_parking_ls[i]);
                    Event parking_event({PARKING_START, parking_time, veh_stats});
                    future_event_list.push(parking_event);

                    // parking durations
                    double upper_bound = (i < veh_stats.ts_parking_ls.size() - 1) ?
                                         ( veh_stats.ts_parking_ls[i+1] - veh_stats.ts_parking_ls[i] ) :
                                         ( T_limit - veh_stats.ts_parking_ls[i] );

                    uniform_real_distribution<simtime_t> duration_distribution(0.0, upper_bound);
                    double parking_delta = duration_distribution(default_engine);
                    veh_stats.ts_parking_duration.push_back(parking_delta);
                    veh_stats.ts_parking_times.emplace_back(veh_stats.ts_parking_ls[i], parking_delta);

                    // add the vehicle move event to the FEL
                    SimTime veh_move_time(parking_time);
                    veh_move_time.mktime(parking_delta + veh_stats.ts_parking_ls[i]);
                    Event vehicle_move_event({VEHICLE_MOVE, veh_move_time, veh_stats});
                    future_event_list.push(vehicle_move_event);
                }
            }

            /* DEPART_SIDE_ROAD */
            // random binomial distribution true or false based on the probability of the event occurring
            veh_stats.prob_side_exit = 0.2;
            bernoulli_distribution side_exit_random(veh_stats.prob_side_exit);
            veh_stats.side_exit_flag = side_exit_random(default_engine);  // bernoulli_distribution implem returns a bool

            /*
             * If the vehicle is due to exit via a side road, generate an exit timestamp.
             * To do this, make a biased uniform distribution that only gives small values.
             * To run this through the exponential distribution, it will always do a ln of a value extremely
             * close to 1 and then divide that by a very small lambda. This means the vehicle will be biased
             * to exit the road very soon.
             * */
            SimTime side_exit_time(arrival_time);
            if (veh_stats.side_exit_flag) {
                double exit_interval = (T_limit + (60*60)) - t_arrival;  // vehicles can exit after 24:00 so add an hour
                simtime_t ts_side_exit = t_arrival;  // can only exit after arrival time

                while (ts_side_exit < (T_limit + (60*60))) {
                    long double test = biased_expovariate(1/exit_interval, 0, 0.02F);
                    if (ts_side_exit + test < (T_limit + (60*60))) {
                        ts_side_exit += test;
                        break;
                    }
                }

                side_exit_time.mktime(ts_side_exit);
                Event exit_event({DEPART_SIDE_ROAD, side_exit_time, veh_stats} );
                future_event_list.push(exit_event);
            }

            /* DEPART_END */ TODO:


            Event arrival_event = { ARRIVAL, arrival_time, veh_stats };
            future_event_list.push(arrival_event);


            // TODO: debug
            // cout << "Arrival <" << t_arrival << " secs>:" << arrival_time.formatted_time() << " ==> " << veh_stats.registration_id << " "
            //      << "(" << veh_stats.arrival_speed << " kmh)" << endl;
            //
            // if (veh_stats.n_parking > 0) {
            //     cout << "Parking: ";
            //     for (i = 0; i < veh_stats.ts_parking_ls.size(); i++)
            //         cout << veh_stats.ts_parking_ls[i] << " >> ";
            //     cout << endl;
            //
            //     cout << "Parking durations: ";
            //     for (simtime_t &t :  veh_stats.ts_parking_duration)
            //         cout << t << " >> ";
            //     cout << endl;
            // }
            //
            // if (veh_stats.side_exit_flag)
            //     cout << "Side Exit: " << side_exit_time.formatted_time() << " ==> " << veh_stats.registration_id << endl;
        }
    }
}

void ActivityEngine::simulate_events()
{
    /*int t;
    for (t = 0; t <= 1439; t++) {
        Event ev = event_q.top();  // returns a reference to the top element

        if ((ev.time.tm_hour * 60 + ev.time.tm_min) == t) {

        }

    }*/

    while (!future_event_list.empty()) {
        Event ev = future_event_list.top();  // returns a reference to the top event but does not remove it
        switch(ev.ev_type) {
            case ARRIVAL:
                veh_logger.info(ev.time, VehicleLog( ARRIVAL, "Vehicle Log", ev.stats.veh_name, ev.stats.registration_id,
                        ev.stats.arrival_speed ));
                break;
            case DEPART_SIDE_ROAD:
                veh_logger.warning(ev.time, VehicleLog( DEPART_SIDE_ROAD, "Vehicle Log", ev.stats.veh_name,
                        ev.stats.registration_id, 0));
                break;
            case PARKING_START:
                veh_logger.info(ev.time, VehicleLog( PARKING_START, "Vehicle Log", ev.stats.veh_name,
                        ev.stats.registration_id, 0));
                break;
            case VEHICLE_MOVE:
                veh_logger.info(ev.time, VehicleLog( VEHICLE_MOVE, "Vehicle Log", ev.stats.veh_name,
                        ev.stats.registration_id, 0));
                break;
        }
        future_event_list.pop();  // returns void, but must remove from queue
    }

}

/*
 * @brief: a biased function to return the variate from an exponential distribution with a lower and upper
 * bound that does not necessarily need to be [0, 1].
 *
 * @param rate_param: the rate of occurrence for the event.
 * @param lower_bound: the lower bound of the bias, must be >= 0.
 * @param upper_bound: the upper bound of the bias, must be <= 1.0.
 *
 * @return: the y value of the exponential distribution which is the likely next interval occurence
 *          of the event defined by the rate parameter or lambda.
 * */
long double ActivityEngine::biased_expovariate(double rate_param, double lower_bound, double upper_bound) {
    // TODO: consider logging critical event to file
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
