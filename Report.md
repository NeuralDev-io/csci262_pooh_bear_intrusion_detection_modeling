# Pooh Bear Intrusion Detection System

CSCI262 System Security (Spring 2018) assignment 3 - intrusion detection system statistical modeling and simulation.

| Authors                         | Contact                                                 |
| ------------------------------- | ------------------------------------------------------- |
| Andrew (Dinh) Che (codeninja55) | [dbac496@uowmail.edu.au](mailto:dbac496@uowmail.edu.au) |
| Duong Le (daltonle)             | [ndl991@uowmail.edu.au](mailto:ndl991@uowmail.edu.au)   |

## Preface

### Overview

Pooh Bear Intrusion Detection System (IDS) is a honeypot event modeller and IDS/auditor system written in C++11. The system reads in vehicles information and traffic statistics and produce events consistent with this data. The collection of generated events forms the baseline data for the system, which are analysed to generate the baseline statistics. In the last phase, "live data" is compared to the baseline statistics to identify anomalies in the system.

### Files

#### Directory

Log files are stored in `dir/logs/` folder and data generated from the Analysis Engine is stored in `dir/data/`.

#### Suffices

Log files and data files generated from statistics of initial input are succeeded with a suffix `_baseline`.

Each time live data is inputed by a user, the log and data files generated from this process are followed by a positive integer incrementing from 1, for example, `data_1, logs_1`, `data_2, logs_2`,... 

## Initial input

Initial input including vehicles information and traffic statistics are read from files specified by user at the command line arguments. 

### Storing data

A struct `VehicleType` stores information and statistics associated with each vehicle type, including:

- Vehicle name
- Registration format
- Volume weight and speed weight
- The mean and standard deviation of the volume and speed.

Each time a new set of statistics is read, the data is stored in an instance of class `Vehicles`, which contains a `map` with the key being a `string` of vehicle name and the value being the `VehicleType` struct. This instance is referred to as a "vehicle dictionary" throughout the system and is used by both the Activity and Analysis engines.

### Data inconsistencies

When data inconsistencies are found within a file or between input files, the system reports the inconsistency and aborts. Below are the inconsistencies that were handled by the system:

- The number of vehicles being monitored is different to the number of vehicle statistics being listed.
- Vehicle names are inconsistent between vehicles file and statistics file.

## Logging system

A logging module is used consistently across the whole system. Following are 5 logging levels in the order of increasing severity.

| Level    | Description                                                  |
| -------- | ------------------------------------------------------------ |
| NOTSET   | No level has been set.                                       |
| INFO     | Confirmation that things are working as expected.            |
| DEBUG    | Detailed information, typically of interest only when diagnosing problems. |
| WARNING  | An indication that something unexpected happened, or indicative of some problem in the near future (e.g. ‘disk space low’). The software is still working as expected. |
| ERROR    | Due to a more serious problem, the software has not been able to perform some function. |
| CRITICAL | A serious error, indicating that the program itself may be unable to continue running. |

### Log format

Each log event has the following format

```
Time,Module,LEVEL,LogType,EVENT,AdditionalInformation
```

where:

- **Time** is a string of time with the format *\<dd-mm-yyyy hh:mm:ss\>*
- **Module** is the module that logged the event, e.g. "Analysis Engine", "Activity Engine", ...
- **LEVEL** is the log level
- **Log type** is the type of the log record, e.g. "Activity Log", "Vehicle Log", ...
- **EVENT** is the event type, e.g. "ARRIVAL", "SPEEDING", "DEPART_END_ROAD", ...
- **Additional information** is any information deemed necessary for the logged event, e.g. a message or the vehicle type, registration ID, ...

## Activity Engine

### Event generation

For each day and for each vehicle type, the Activity engine follows the folllowing process to generate discrete events of vehicles:

- **Generate arrival events**
  Using the normal distribution, the number of occurences for the vehicle type can be calculated, which in turn produces the rate of occurence. Using the rate of occurence, the activity engine can generate the next arrival delta interval using an exponential distribution and the Poisson process. 
- **Generate parking events**
  Something here
- **Generate departure events**
  Something here

After the events are generated and stored in an event queue, the activity engine processes this event queue and log events to `dir/logs/logs_suffix`.

### Errors

Errors may happen during the execution of the Activity engine. The table below lists the possible errors and the actions that will be performed by the system if they occur.

| Errors                                                       | Actions                                                      |
| :----------------------------------------------------------- | :----------------------------------------------------------- |
| The time of an arrival event exceeds the time limit of arrival events (23:00) | A vector of 1000 timestamps is created. If there are N vehicles arriving that day, the first N eligible timestamps will be chosen. |
|                                                              |                                                              |
| File errors (cannot open log file)                           | The system will reports the error to the console. If this error occurs, the system may still be able to run but data is likely to be lost. |

## Analysis Engine

### Storing data

The Analysis engine reads the event logs generated by the Activity engine and generate associated statistics. 

Each time a vehicle enters the road, a `VehicleStats` struct is created and added to a `map` (key: `string` of registration ID, value: `VehicleStats` containing vehicle information). The vehicle volume and speed distributions of that day are also updated. When the vehicle leaves the road, its struct is simply taken off the `map`.

The daily totals (volume and average speed for each vehicle type) are stored in `dir/data/data_suffix`, the speeding tickets are stored in `dir/data/speeding_suffix` and the generated overall statistics is stored in `dir/data/stats_suffix`. The `data` and `speeding` files are written with the CSV format with a header, while the `stats` file has the same format as the original inputed `stats` file.

All activities of the Analysis engine are also continued to be logged in `dir/logs/logs_suffix`.

### Possible anomalies



## Alert Engine

The Alert engine reads the `data` file associated with the "live data" and compare it to the baseline statistics (stored in `stats_baseline`). For each day, the Alert engine adds up the anomaly counters and compare them to the anomaly thresholds, then reports *"Volume/Speed anomaly detected"* or *"No volume/speed anomaly detected"* as appropirate.

Activities of the Alert engine are also continued to be logged in `dir/logs/logs_suffix`.

