#include <iostream>
#include <vector>
#include <map>
#include "WorkUtilizationCalendar.h"

// You should have implemented this somewhere (or replace with manual CSV reading)
std::vector<CeloxisTask> readCSV(const std::string& filename);  // forward declare if needed

int main() {
    std::vector<CeloxisTask> tasks = readCSV("mock_tasks.csv");
    std::map<std::string, std::string> roleMap = loadRoleMap("UsersInfo.CSV");
    std::map<std::string, double> directLaborGoals = loadLaborGoals("UsersInfo.CSV"); // labor availability

    int year;
    std::cout << "Enter year: ";
    std::cin >> year;
    std::cin.ignore();

    MonthNode* calendar = createYearSkipList(year);
    for (const auto& task : tasks) {
        taskEffortMonths(calendar, task);  // This uses the global roleMap
    }

    runCalendarCLI(year, calendar, roleMap, directLaborGoals);

    return 0;
}