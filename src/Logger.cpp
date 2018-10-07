/*********************************************************************************
* CSCI262 (Spring 2018) - Assignment 03
* Pooh Bear Intrusion Detection System Logger.cpp
* Purpose: Implementation for Logger package class for a logging system.
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

#include <iostream>
#include <fstream>
#include "Logger.h"

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
 * */

/* @brief Default constructor for a Logger with default values for the config dictionary. */
Logger::Logger()
{
    config.insert(pair<string, string>("LOGGER", "Default Logger"));
    config.insert(pair<string, string>("LEVEL", _level_to_name(WARNING)));
    config.insert(pair<string, string>("FILENAME", "false"));
    // TODO: consider what to do if you don't want to print to stdout
    config.insert(pair<string, string>("STDOUT", "true"));
}

/* @brief Constructor with just the logger name and default values for everything else. */
Logger::Logger(string logger_name)
{
    config.insert(pair<string, string>("LOGGER", logger_name));
    config.insert(pair<string, string>("LEVEL", _level_to_name(WARNING)));
    config.insert(pair<string, string>("FILENAME", "false"));
    config.insert(pair<string, string>("STDOUT", "true"));
}

/* @brief Constructor to allow setting of the logger name and filename */
Logger::Logger(string logger_name, string filename)
{
    filename_ss << "logs/" << filename;
    config.insert(pair<string, string>("LOGGER", logger_name));
    config.insert(pair<string, string>("LEVEL", _level_to_name(WARNING)));
    config.insert(pair<string, string>("FILENAME", filename_ss.str()));
    config.insert(pair<string, string>("STDOUT", "false"));
}

/* @brief Constructor to set the logger name and file name with a level. */
Logger::Logger(string logger_name, LEVEL level, string filename)
{
    filename_ss << "logs/" << filename;
    config.insert(pair<string, string>("LOGGER", logger_name));
    config.insert(pair<string, string>("LEVEL", _level_to_name(level)));
    config.insert(pair<string, string>("FILENAME", filename_ss.str()));
    config.insert(pair<string, string>("STDOUT", "false"));
}

/* @brief Constructor to set all values in the dictionary including whether to print to stdout. */
Logger::Logger(string logger_name, LEVEL level, string filename, bool std_out)
{
    filename_ss << "logs/" << filename;
    config.insert(pair<string, string>("LOGGER", logger_name));
    config.insert(pair<string, string>("LEVEL", _level_to_name(level)));
    config.insert(pair<string, string>("FILENAME", filename_ss.str()));
    config.insert(pair<string, string>("STDOUT", (std_out) ? "true" : "false"));
}

/*
 * A specific log call with the INFO level.
 *
 * @param time: the SimTime struct of the event time.
 * @param ev_type: the enum EVENT_TYPE of the type of event.
 * @param msg: a c++ string of the message to log.
 * */
void Logger::info(SimTime &time, EVENT_TYPE ev_type, string msg)
{
    // Check if info is allowed with the current level
    _log(INFO, time, ev_type, msg);
}

/* TODO: Add additional debug info.
 * A specific log call with the DEBUG level.
 *
 * @param time: the SimTime struct of the event time.
 * @param ev_type: the enum EVENT_TYPE of the type of event.
 * @param msg: a c++ string of the message to log.
 * */
void Logger::debug(SimTime &time, EVENT_TYPE ev_type, string msg)
{
    // Check if info is allowed with the current level
    _log(DEBUG, time, ev_type, msg);
}

/*
 * A specific log call with the WARNING level.
 *
 * @param time: the SimTime struct of the event time.
 * @param ev_type: the enum EVENT_TYPE of the type of event.
 * @param msg: a c++ string of the message to log.
 * */
void Logger::warning(SimTime &time, EVENT_TYPE ev_type, string msg)
{
    // Check if info is allowed with the current level
    _log(WARNING, time, ev_type, msg);
}

/*
 * A specific log call with the ERROR level.
 *
 * @param time: the SimTime struct of the event time.
 * @param ev_type: the enum EVENT_TYPE of the type of event.
 * @param msg: a c++ string of the message to log.
 * */
void Logger::error(SimTime &time, EVENT_TYPE ev_type, string msg)
{
    // Check if info is allowed with the current level
    _log(ERROR, time, ev_type, msg);
}

/*
 * A specific log call with the CRITICAL level.
 *
 * @param time: the SimTime struct of the event time.
 * @param ev_type: the enum EVENT_TYPE of the type of event.
 * @param msg: a c++ string of the message to log.
 * */
void Logger::critical(SimTime &time, EVENT_TYPE ev_type, string msg)
{
    // Check if info is allowed with the current level
    _log(CRITICAL, time, ev_type, msg);
}

/*
 * A private internal generalised function to log a record to to stdout or a log file.
 *
 * @param level: the level to log.
 * @param time: the SimTime struct to log the time of the event.
 * @param ev_time: the enum EVENT_TYPE to log.
 * @param msg: the c++ string message to log.
 * */
void Logger::_log(LEVEL level, SimTime &time, EVENT_TYPE &ev_type, string msg)
{
    if (get("STDOUT") == "true") {
        cout << "<" << formatted_date(time) << " " << formatted_time(time) << ">:"
             << get("LOGGER") << ":" << _level_to_name(level) << ":"
             << event_name(ev_type) << ":" << msg << endl;
    }

    if (get("FILENAME") != "false") {
        ofstream fout(get("FILENAME").c_str(), ofstream::out | ofstream::app);
        if (!fout.good())
            cout << "[!!] Failed to open log file " << get("FILENAME");

        fout << "<" << formatted_date(time) << " " << formatted_time(time) << ">:"
             << get("LOGGER") << ":" << _level_to_name(level) << ":"
             << event_name(ev_type) << ":" << msg << endl;

        fout.close();
    }
}

/*
 * Allow setting or changing the level of the logger after initialisation.
 *
 * @param level: the LEVEL enum to change to.
 * */
void Logger::set_level(LEVEL level)
{
    Config_Iter iter = config.find("LEVEL");
    if (iter != config.end())
        (*iter).second = _level_to_name(level);
}

/*
 * Public getter method to access the private config dictionary.
 *
 * @return: the map<string, string> config dictionary.
 * */
const Config_Dict &Logger::get_config() const { return config; }

/* TODO: No check if key is actually in the dictionary.
 * Generalised method to access the dictionary value by the passed in string key.
 *
 * @param key: a c++ string of the key to access.
 * @return: a c++ string of the value stored in the config dictionary.
 * */
string &Logger::get(string key)
{
    Config_Iter iter = config.find(key);
    if (iter != config.end())
        return (*iter).second;
}

/*
 * Simple function to change the string LEVEL to an enum LEVEL.
 *
 * @param level_str: a c++ string corresponding to the enum LEVEL.
 * @return: a LEVEL enum corresponding to the string parameter.
 * */
LEVEL Logger::_name_to_level(string &level_str) {
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
string Logger::_level_to_name(LEVEL level) {
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
