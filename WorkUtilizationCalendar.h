#ifndef WORK_UTILIZATION_CALENDAR_H
#define WORK_UTILIZATION_CALENDAR_H

#include <string>
#include <vector>
#include <iostream>
#include <map>

struct MonthNode;

int runCalendarCLI(int year, MonthNode* calendar, const std::map<std::string, std::string>& roleMap);

enum Priority { LOW, MED, HIGH}; // Priority labels for tasks

/// <summary>
/// Parsing Resources
/// </summary>
struct CeloxisTask {
    std::string name;
    std::string start;
    std::string finish;
    std::string resources;
    std::string workspace;
    std::string milestone;
};

/// <summary>
/// Parsing Resources
/// </summary>
struct ResourceSplit {
    std::string name;
    double effort;
    std::string unit;
};

/// <summary>
/// push_back contructor
/// </summary>
struct Task {
    std::string name;
    std::string jobRole;
    std::string workspace;
    double hours;

    Task(const std::string& n, const std::string& role, const std::string& ws, double h)
        : name(n), jobRole(role), workspace(ws), hours(h) {}
};


struct MonthNode {
    std::string monthName;
    double consultingEffort;
    double devEffort;
    double availableHours; // will reference business days - PTO

    std::vector<Task> consultingTasks;
    std::vector<Task> devTasks;
    std::vector<Task> deferredTasks; 

    /// <summary>
    /// Indicators for critical events
    /// </summary>

    int milestoneCount = 0;
    bool isReleaseMonth = false;

 

    /// <summary>
    /// data struc pointers
    /// </summary>

    MonthNode* next = NULL;
    MonthNode* down = NULL;

};


/// <summary>
/// Filter declarations
/// </summary>
void printFilteredUtilization(MonthNode* calendar, const std::string& filterType, const std::string& filterValue);

MonthNode* createYearSkipList(int year);

std::vector<ResourceSplit> splitResources(const std::string& raw);
std::vector<CeloxisTask> readCSV(const std::string& filename);
std::map<std::string, std::string> loadRoleMap(const std::string& filename);
std::map<std::string, double> loadLaborGoals(const std::string& filename);


double getUtilization(MonthNode* m);
void taskEffortMonths(MonthNode* calendar, const CeloxisTask & task);

int runCalendarCLI(int year, MonthNode* calendar,
    const std::map<std::string, std::string>& roleMap,
    const std::map<std::string, double>& directLaborGoals);

void filterAndPrintCalendar(MonthNode* calendar, const std::string& workspace, const std::string& role, int year, const std::map<std::string, std::string>& roleMap);

#endif