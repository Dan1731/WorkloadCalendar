#include <iostream>
#include <vector>
#include <map>
#include "WorkUtilizationCalendar.h"

std::vector<CeloxisTask> readCSV(const std::string& filename);  

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
        taskEffortMonths(calendar, task);  // Global roleMap
    }

    runCalendarCLI(year, calendar, roleMap, directLaborGoals);

    return 0;
}