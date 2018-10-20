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

AlertEngine::AlertEngine(string log_file, string data_file)
{
    log_file = log_file;
    alert_logger = Logger<SimTime, AlertLog>("Alert Engine", INFO, log_file, true);
}

void AlertEngine::run(Vehicles &vehicles, int &days)
{

}
