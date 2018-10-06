/*********************************************************************************
* CSCI262 (Spring 2018) - Assignment 03
* Pooh Bear Intrusion Detection System ActivityEngine.cpp
* Purpose: Creates a traffic simulation engine based on reading of input from
*          Vehicles.txt and Stats.txt, as well as an argument for how many days
 *         to simulate traffic.
*
* @version 0.1-dev 2018.10.06
*
* @authors Dinh Che (codeninja55) & Duong Le (daltonle)
* Emails andrew at codeninja55.me & duong.daltonle at gmail.com
* Students Dinh Che (5721970 | dbac496) & Duong Le (5560536 | ndl991)
*********************************************************************************/
#include <iostream>
#include "ActivityEngine.h"
#include "Helper.h"

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
}

void ActivityEngine::run()
{
    sim_time sim_start_time = time_now();
    cout << formatted_time(sim_start_time);
    Logger logger = Logger();
    // TODO: time should be stepped in 1 minute blocks
    // TODO: program should give some indication as to what is happening, without being verbose
    /*
     * TODO:
     * There are five types of events. The first is associated with vehicle generation, the other
     * four with existing vehicle activity.
     *   1. Vehicle arrival, always at the start of the road. This is only up to 23:00 on any given day.
     *   2. Vehicle departure via a side road. The vehicle is then out of the road system.
     *   3. Vehicle departure via the end of the road. The vehicle is then out of the road system.
     *   4. Vehicle parks or stops parking.
     *   5. Vehicle moves and possibly changes speed.
     *
     * -> You should think about appropriate probabilities for each of those activites.
     * -> The statistics associated with vehicle volume are tied to the vehicle arrival.
     * -> The statistics associated with vehicle speed are tied to the speed of the vehicle
     *    when it arrives on the road.
     * -> You only test breaches of speed limit using the average speed across the whole road based on the times,
     * */
}


