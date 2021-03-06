/*********************************************************************************
* CSCI262 (Spring 2018) - Assignment 03
* Pooh Bear Intrusion Detection System Logger.cpp
* Purpose: Header file for Logger package class for a logging system.
*          Based heavily on:
 *         https://github.com/python/cpython/blob/3.7/Lib/logging/__init__.py
*
* @version 0.9-dev
* @date 2018.10.07
*
* @authors Dinh Che (codeninja55) & Duong Le (daltonle)
* Emails: andrew at codeninja55.me & duong.daltonle at gmail.com
* StudentsS Dinh Che (5721970 | dbac496) & Duong Le (5560536 | ndl991)
*********************************************************************************/

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#ifndef POOH_BEAR_INTRUSION_DETECTION_SYSTEM_LOGGER_H
#define POOH_BEAR_INTRUSION_DETECTION_SYSTEM_LOGGER_H

#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string>
#include <map>
#include <queue>
#include <time.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include "Utils.h"
using namespace std;

/* NOTE: Defining the implementation of the class in the header file because of
 *       https://stackoverflow.com/questions/8752837/undefined-reference-to-template-class-constructor
 * */

/*
 * NOTSET: No level has been set.
 * INFO: Confirmation that things are working as expected.
 * DEBUG: Detailed information, typically of interest only when diagnosing problems.
 * WARNING: An indication that something unexpected happened, or indicative of some problem in the near future
 *          (e.g. ‘disk space low’). The software is still working as expected.
 * ERROR: Due to a more serious problem, the software has not been able to perform some function.
 * CRITICAL: A serious error, indicating that the program itself may be unable to continue running.
 * */
enum LEVEL { NOTSET = 0, INFO = 10, DEBUG = 20, WARNING = 30, ERROR = 40, CRITICAL = 50 };
typedef enum LEVEL LEVEL;

typedef map<string, string>::iterator Config_Iter;
typedef map<string, string> Config_Dict;

static char DELIMITER = ',';

/*
 * @brief: a template Logger class with two template type arguments.
 *
 * @param T: a time type.
 * @param S: a log structure type with a requirement to overload the ostream << operator.
 * */
template<typename T, typename S>
class Logger {
    public:
        Logger();  // default constructor
        explicit Logger(string logger_name);
        Logger(string logger_name, string filename);
        Logger(string logger_name, LEVEL level, string filename);
        Logger(string logger_name, LEVEL level, string filename, bool std_out);
        Logger(const Logger &other);  // copy constructor
        Logger& operator=(const Logger &rhs);
        bool operator==(const Logger &rhs) const;
        bool operator!=(const Logger &rhs) const;

        void set_level(LEVEL level);
        void info(T &time, S log_struct);
        void debug(T &time, S log_struct);
        void warning(T &time, S log_struct);
        void error(T &time, S log_struct);
        void critical(T &time, S log_struct);
        const Config_Dict &get_config() const;
        string &get(string);

    private:
        void _log(LEVEL level, T &time, S log_struct);
        void _add_record(LEVEL level, T &time, S &log_struct);
        bool _is_file_exists(const string& name);  // test if the file for logging already exists to use trunc or append
        string _level_to_name(LEVEL);  // function to convert enum to string
        LEVEL _name_to_level(string&);  // function to convert string to enum
        Config_Dict config;  // a dictionary of configuration values with key and string values
        stringstream filename_ss;  // stringstream to add prefix to filenames
        const string logs_dir = "logs/";
};

/*
 * TODO:
 * There are some things to consider improving:
 * [] Ensure the level functions (i.e. info(), debug(), etc.) check the set level of the Logger to log only at the appropriate level.
 * [] Add multi threading to use a different thread than the  main program to ensure you do not affect the running of the main program.
 * [] Add additional information for the debug level such as:
 *     - Function caller
 *     - Line in the code
 *     - Thread id
 *     - Error messages
 *     - Exception messages
 * [] Check to see if the key is in config dictionary and return appropriate string.
 */

/* @brief Default constructor for a Logger with default values for the config dictionary. */
template<typename T, typename S>
Logger<T, S>::Logger()
{
    config.insert(pair<string, string>("LOGGER", "Default Logger"));
    config.insert(pair<string, string>("LEVEL", _level_to_name(WARNING)));
    config.insert(pair<string, string>("FILENAME", "false"));
    // TODO: consider what to do if you don't want to print to stdout
    config.insert(pair<string, string>("STDOUT", "true"));
}

/* @brief Constructor with just the logger name and default values for everything else. */
template<typename T, typename S>
Logger<T, S>::Logger(string logger_name)
{
    config.insert(pair<string, string>("LOGGER", logger_name));
    config.insert(pair<string, string>("LEVEL", _level_to_name(WARNING)));
    config.insert(pair<string, string>("FILENAME", "false"));
    config.insert(pair<string, string>("STDOUT", "true"));
}

/* @brief Constructor to allow setting of the logger name and filename */
template<typename T, typename S>
Logger<T, S>::Logger(string logger_name, string filename)
{
    filename_ss << logs_dir << filename;
    config.insert(pair<string, string>("LOGGER", logger_name));
    config.insert(pair<string, string>("LEVEL", _level_to_name(WARNING)));
    config.insert(pair<string, string>("FILENAME", filename_ss.str()));
    config.insert(pair<string, string>("STDOUT", "false"));
}

/* @brief Constructor to set the logger name and file name with a level. */
template<typename T, typename S>
Logger<T, S>::Logger(string logger_name, LEVEL level, string filename)
{
    filename_ss << logs_dir << filename;
    config.insert(pair<string, string>("LOGGER", logger_name));
    config.insert(pair<string, string>("LEVEL", _level_to_name(level)));
    config.insert(pair<string, string>("FILENAME", filename_ss.str()));
    config.insert(pair<string, string>("STDOUT", "false"));
}

/* @brief Constructor to set all values in the dictionary including whether to print to stdout. */
template<typename T, typename S>
Logger<T, S>::Logger(string logger_name, LEVEL level, string filename, bool std_out)
{
    filename_ss << logs_dir << filename;
    config.insert(pair<string, string>("LOGGER", logger_name));
    config.insert(pair<string, string>("LEVEL", _level_to_name(level)));
    config.insert(pair<string, string>("FILENAME", filename_ss.str()));
    config.insert(pair<string, string>("STDOUT", (std_out) ? "true" : "false"));
}

template<typename T, typename S>
Logger<T, S>& Logger<T, S>::operator=(const Logger<T, S> &rhs)
{
    // check for self assignment
    if (&rhs == this)
        return *this;

    filename_ss << rhs.filename_ss.str();
    config = rhs.config;
    return *this;
}

/* @brief: copy constructor. */
template<typename T, typename S>
Logger<T, S>::Logger(const Logger &other)
{
    filename_ss << other.filename_ss.str();
    config = other.config;
}

template<typename T, typename S>
bool Logger<T, S>::operator==(const Logger &rhs) const
{
    return config.begin() == rhs.config.begin();
}

template<typename T, typename S>
bool Logger<T, S>::operator!=(const Logger &rhs) const
{
    return !(rhs == *this);
}


/*
 * A specific log call with the INFO level.
 *
 * @param time: the SimTime struct of the event time.
 * @param ev_type: the enum EVENT_TYPE of the type of event.
 * @param msg: a c++ string of the message to log.
 * */
template<typename T, typename S>
void Logger<T, S>::info(T &time, S log_struct)
{
    // Check if info is allowed with the current level
    _log(INFO, time, log_struct);
}

/* TODO: Add additional debug info.
 * A specific log call with the DEBUG level.
 *
 * @param time: the SimTime struct of the event time.
 * @param ev_type: the enum EVENT_TYPE of the type of event.
 * @param msg: a c++ string of the message to log.
 * */
template<typename T, typename S>
void Logger<T, S>::debug(T &time, S log_struct)
{
    // Check if info is allowed with the current level
    _log(DEBUG, time, log_struct);
}

/*
 * A specific log call with the WARNING level.
 *
 * @param time: the SimTime struct of the event time.
 * @param ev_type: the enum EVENT_TYPE of the type of event.
 * @param msg: a c++ string of the message to log.
 * */
template<typename T, typename S>
void Logger<T, S>::warning(T &time, S log_struct)
{
    // Check if info is allowed with the current level
    _log(WARNING, time, log_struct);
}

/*
 * A specific log call with the ERROR level.
 *
 * @param time: the SimTime struct of the event time.
 * @param ev_type: the enum EVENT_TYPE of the type of event.
 * @param msg: a c++ string of the message to log.
 * */
template<typename T, typename S>
void Logger<T, S>::error(T &time, S log_struct)
{
    // Check if info is allowed with the current level
    _log(ERROR, time, log_struct);
}

/*
 * A specific log call with the CRITICAL level.
 *
 * @param time: the SimTime struct of the event time.
 * @param ev_type: the enum EVENT_TYPE of the type of event.
 * @param msg: a c++ string of the message to log.
 * */
template<typename T, typename S>
void Logger<T, S>::critical(T &time, S log_struct)
{
    // Check if info is allowed with the current level
    _log(CRITICAL, time, log_struct);
}

/*
 * A private internal generalised function to log a record to to stdout or a log file.
 *
 * @param level: the level to log.
 * @param time: the SimTime struct to log the time of the event.
 * @param ev_time: the enum EVENT_TYPE to log.
 * @param msg: the c++ string message to log.
 * */
template<typename T, typename S>
void Logger<T, S>::_log(LEVEL level, T &time, S log_struct)
{
    if (get("STDOUT") == "true") {
        cout << time.formatted_time_date() << DELIMITER << get("LOGGER")
             << DELIMITER << _level_to_name(level) << DELIMITER << log_struct << endl;
    } else if (level > 30) {  // level is either ERROR or CRITICAL
        cout << "[*****IMPORTANT*****] " << time.formatted_time_date() << '\t' << get("LOGGER")
             << '\t' << _level_to_name(level) << '\t' << log_struct << endl;
    }

    if (get("FILENAME") != "false")
        _add_record(level, time, log_struct);
}

template<typename T, typename S>
void Logger<T, S>::_add_record(LEVEL level, T &time, S &log_struct)
{
    ofstream fout;
    fout.open(get("FILENAME").c_str(), ios::out | ios::app);

    if (!fout.good())
        cout << "<" << real_formatted_time_now() << "> [*****FILE ERROR*****] Failed to open log file " << get("FILENAME") << endl;

    fout << time.formatted_time_date() << DELIMITER << get("LOGGER") << DELIMITER
    << _level_to_name(level) << DELIMITER << log_struct << endl;

    fout.close();
}

/*
 * Allow setting or changing the level of the logger after initialisation.
 *
 * @param level: the LEVEL enum to change to.
 * */
template<typename T, typename S>
void Logger<T, S>::set_level(LEVEL level)
{
    auto iter = config.find("LEVEL");
    if (iter != config.end())
        (*iter).second = _level_to_name(level);
}

/*
 * Public getter method to access the private config dictionary.
 *
 * @return: the map<string, string> config dictionary.
 * */
template<typename T, typename S>
const Config_Dict &Logger<T, S>::get_config() const { return Logger::config; }

/* TODO: No check if key is actually in the dictionary.
 * Generalised method to access the dictionary value by the passed in string key.
 *
 * @param key: a c++ string of the key to access.
 * @return: a c++ string of the value stored in the config dictionary.
 * */
template<typename T, typename S>
string &Logger<T, S>::get(string key)
{
    auto iter = config.find(key);
    if (iter != config.end())
        return (*iter).second;
    return *new string("[*****LOGGER ERROR*****] No key in config");
}

template<typename T, typename S>
bool Logger<T, S>::_is_file_exists(const string& name)
{
    struct stat buffer{};
    return (stat (name.c_str(), &buffer) == 0);
}

/*
 * Simple function to change the string LEVEL to an enum LEVEL.
 *
 * @param level_str: a c++ string corresponding to the enum LEVEL.
 * @return: a LEVEL enum corresponding to the string parameter.
 * */
template<typename T, typename S>
LEVEL Logger<T, S>::_name_to_level(string &level_str)
{
    if (level_str == "NOTSET")
        return NOTSET;
    else if (level_str == "INFO")
        return INFO;
    else if (level_str == "DEBUG")
        return DEBUG;
    else if (level_str == "ERROR")
        return ERROR;
    else if (level_str == "CRITICAL")
        return CRITICAL;
    else
        return WARNING;
}

/*
 * A simple function to change the enum LEVEL to a string corresponding.
 *
 * @param level: the enum LEVEL.
 * @return: the string corresponding to each enum LEVEL.
 * */
template<typename T, typename S>
string Logger<T, S>::_level_to_name(LEVEL level)
{
    switch (level) {
        case NOTSET:
            return "NOTSET";
        case INFO:
            return "INFO";
        case DEBUG:
            return "DEBUG";
        case WARNING:
            return "WARNING";
        case ERROR:
            return "ERROR";
        case CRITICAL:
            return "CRITICAL";
        default:
            return "WARNING";
    }
}

#endif //POOH_BEAR_INTRUSION_DETECTION_SYSTEM_LOGGER_H

#pragma clang diagnostic pop