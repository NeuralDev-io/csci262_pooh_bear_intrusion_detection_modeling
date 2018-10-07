/*********************************************************************************
* CSCI262 (Spring 2018) - Assignment 03
* Pooh Bear Intrusion Detection System Logger.cpp
* Purpose: Header file for Logger package class for a logging system.
*          Based heavily on:
 *         https://github.com/python/cpython/blob/3.7/Lib/logging/__init__.py
*
* @version 0.2-dev
* @date 2018.10.07
*
* @authors Dinh Che (codeninja55) & Duong Le (daltonle)
* Emails: andrew at codeninja55.me & duong.daltonle at gmail.com
* StudentsS Dinh Che (5721970 | dbac496) & Duong Le (5560536 | ndl991)
*********************************************************************************/

#ifndef POOH_BEAR_INTRUSION_DETECTION_SYSTEM_LOGGER_H
#define POOH_BEAR_INTRUSION_DETECTION_SYSTEM_LOGGER_H

#include <string>
#include <map>
#include <time.h>
#include "Helper.h"
using namespace std;

enum LEVEL { NOTSET = 0, INFO = 10, DEBUG = 20, WARNING = 30, ERROR = 40, CRITICAL = 50 };
typedef enum LEVEL LEVEL;

typedef map<string, string>::iterator Config_Iter;
typedef map<string, string> Config_Dict;

class Logger {
public:
    Logger();  // default constructor
    Logger(string, string);  // default constructor with just filename
    Logger(string, LEVEL, string);
    Logger(string, LEVEL, string, bool);
    void set_level(LEVEL);
    void info(SimTime&, EVENT_TYPE, string);
    void debug(SimTime&, EVENT_TYPE, string);
    void warning(SimTime&, EVENT_TYPE, string);
    void error(SimTime&, EVENT_TYPE, string);
    void critical(SimTime&, EVENT_TYPE, string);
    const Config_Dict &get_config() const;
    string &get(string);
    void _log(LEVEL, SimTime&, EVENT_TYPE&, string);
private:
    string _level_to_name(LEVEL);
    LEVEL _name_to_level(string&);
    Config_Dict config;
};

#endif //POOH_BEAR_INTRUSION_DETECTION_SYSTEM_LOGGER_H
