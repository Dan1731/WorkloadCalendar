#include "WorkUtilizationCalendar.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <iomanip>
#include <ctime>
#include <set>

// Global role map reference if needed across multiple functions
std::map<std::string, std::string> roleMap;

// Employee role helper
std::map<std::string, std::string> loadRoleMap(const std::string& filename);

// Labor availability helper
std::map<std::string, double> loadLaborGoals(const std::string& filename);

// Case sensitive helper
std::string toLower(const std::string& str) {
    std::string lower = str;
    for (char& c : lower) c = std::tolower(c);
    return lower;
}

// Color Coding
double getUtilization(MonthNode* m) {
    return ((m -> consultingEffort + m -> devEffort) / m -> availableHours) * 100.0; // calc utilization
}

void printFilteredUtilization(MonthNode* calendar, const std::string& filterType, const std::string& filterValue) {
    MonthNode* ptr = calendar;
    std::cout << "\nMonth   | Filtered Utilization (%)\n";
    std::cout << "--------|--------------------------\n";

    while (ptr) {
        double filteredEffort = 0.0;

        const std::vector<Task>& tasks =
            (filterType == "workspace" && filterValue == "Consulting") ? ptr->consultingTasks :
            (filterType == "workspace" && filterValue == "Development") ? ptr->devTasks :
            ptr->consultingTasks; // fallback to consulting

        for (const auto& task : tasks) {
            if ((filterType == "jobRole" && "" == filterValue) ||
                (filterType == "workspace" && task.workspace == filterValue)) {
                filteredEffort += task.hours;
            }
        }

        double filteredUtil = (filteredEffort / ptr->availableHours) * 100.0;
        filteredUtil = static_cast<int>(filteredUtil * 100 + 0.5) / 100.0;

        printf("%-8s| %7.2f%%\n", ptr->monthName.c_str(), filteredUtil);
        ptr = ptr->next;
    }
}

/// <summary> 
/// Span effort equally across months
/// </summary>
// Parse Celoxis date string 
std::tm parseDate(const std::string& dateStr) {
    std::tm tm = {};
    std::istringstream ss(dateStr);
    std::string day, month, year;

    if (!(ss >> day >> month >> year)) {
        std::cerr << "Error: Failed to parse date string: " << dateStr << "\n";
        throw std::invalid_argument("Invalid date format");
    }

    try {
        tm.tm_mday = std::stoi(day);
        tm.tm_year = std::stoi(year) - 1900;
    } catch (const std::exception& e) {
        std::cerr << "Invalid number in date string: " << dateStr << " (" << e.what() << ")\n";
        throw;
    }

    const char* months[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                             "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
    tm.tm_mon = -1;
    for (int i = 0; i < 12; ++i) {
        if (month == months[i]) {
            tm.tm_mon = i;
            break;
        }
    }

    if (tm.tm_mon == -1) {
        std::cerr << "Error: Unrecognized month in date string: " << dateStr << "\n";
        throw std::invalid_argument("Invalid month name");
    }

    return tm;
}

std::string getMonthAbbreviation(int monthIndex) {
    const char* months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
    return months[monthIndex % 12];
}

int countMonthSpan(const std::tm& start, const std::tm& end) {
    return (end.tm_year - start.tm_year) * 12 + (end.tm_mon - start.tm_mon) + 1;
}

    // Split string and distribute effort
    void taskEffortMonths(MonthNode* calendar, const CeloxisTask& task) {
        std::tm start = parseDate(task.start);
        std::tm end = parseDate(task.finish);
        int span = countMonthSpan(start, end);
    
        std::vector<ResourceSplit> splits = splitResources(task.resources);
        for (int i = 0; i < span; ++i) {
            int targetMonth = (start.tm_mon + i) % 12;
            MonthNode* current = calendar;
            while (current) {
                if (current->monthName == getMonthAbbreviation(targetMonth)) {
                    for (const auto& r : splits) {
                        double hoursPerMonth = 0.0;
    
                        if (r.unit == "h") {
                            hoursPerMonth = r.effort / span;
                        } else if (r.unit == "%") {
                            double avgMonthlyHours = ((5 * 8 * 52.0) / 12.0);  // approx 173.33 hrs/month
                            hoursPerMonth = (r.effort / 100.0) * avgMonthlyHours / span;
                        }
    
                        std::string role = (roleMap.count(r.name) > 0) ? roleMap.at(r.name) : "";
                        Task t(r.name, role, task.workspace, hoursPerMonth);
    
                        if (task.workspace == "Consulting") {
                            current->consultingEffort += hoursPerMonth;
                            current->consultingTasks.push_back(t);
                        } else if (task.workspace == "Development") {
                            current->devEffort += hoursPerMonth;
                            current->devTasks.push_back(t);
                        } else {
                            std::cerr << "Warning: Unknown workspace '" << task.workspace
                                      << "' for task '" << task.name << "'\n";
                        }
                    }
    
                    if (task.milestone == "Yes" && i == 0) {
                        std::cout << "Milestone marked for: " << current->monthName << " (" << task.name << ")\n";
                        current->milestoneCount++;
                    }
                    break;
                }
                current = current->next;
            }
        }
    }

MonthNode* createYearSkipList(int year) {
    std::string monthName[12] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" 
    };

    MonthNode* head = nullptr;
    MonthNode* prev = nullptr;

    for (const auto& name : monthName) {
        MonthNode* node = new MonthNode();
        node->monthName = name;
        node->availableHours = 0; // placeholder, will calculate next

        // Use actual business days for the given year/month
        std::tm t = {};
        t.tm_year = year - 1900;
        t.tm_mon = &name - monthName; // get index from name
        t.tm_mday = 1;

        int businessDays = 0;
        for (int d = 0; d < 31; ++d) {
            t.tm_mday = d + 1;
            if (mktime(&t) == -1) break;
            if (t.tm_mon != (&name - monthName)) break;
            if (t.tm_wday != 0 && t.tm_wday != 6) businessDays++;
        }

        node->availableHours = businessDays * 8;

        if (name == "Jan" || name == "Jun") {
            node->isReleaseMonth = true;
        }

        if (!head) head = node;
        else prev->next = node;
        prev = node;
    }

    return head;
}

std::vector<ResourceSplit> splitResources(const std::string& raw) {
    std::vector<ResourceSplit> result;
    std::string trimmed = raw;

    // Remove surrounding quotes
    if (!trimmed.empty() && trimmed.front() == '"' && trimmed.back() == '"') {
        trimmed = trimmed.substr(1, trimmed.size() - 2);
    }

    std::stringstream ss(trimmed);
    std::string token;
    while (std::getline(ss, token, ',')) {
        std::string entry = token;
        entry.erase(0, entry.find_first_not_of(" \t\r\n"));
        entry.erase(entry.find_last_not_of(" \t\r\n") + 1);

        size_t bracketStart = entry.find('[');
        size_t bracketEnd = entry.find(']');
        if (bracketStart == std::string::npos || bracketEnd == std::string::npos) continue;

        std::string name = entry.substr(0, bracketStart);
        name.erase(0, name.find_first_not_of(" \t\r\n"));
        name.erase(name.find_last_not_of(" \t\r\n") + 1);

        std::string valuePart = entry.substr(bracketStart + 1, bracketEnd - bracketStart - 1);
        std::string unit = valuePart.find('%') != std::string::npos ? "%" : "h";
        if (unit == "%") valuePart.erase(valuePart.find('%'), 1);

        try {
            double amount = std::stod(valuePart);
            result.push_back({ name, amount, unit });
        } catch (...) {
            std::cerr << "Warning: Failed to parse resource effort value: " << valuePart << std::endl;
        }
    }

    return result;
}


void filterAndPrintCalendar(MonthNode* calendar,const std::string& workspace,const std::string& role,int year,const std::map<std::string, std::string>& roleMap, const std::map<std::string, double>& directLaborGoals) {
    std::cout << "\nMonth   | Filtered Utilization (%)\n";
    std::cout << "--------|--------------------------\n";

    MonthNode* ptr = calendar;
    while (ptr) {
        std::set<std::string> people;
        double total = 0.0;

        // Combine tasks based on workspace filter
        std::vector<Task> relevantTasks;
        if (toLower(workspace) == "development") {
        relevantTasks = ptr->devTasks;
        } else if (toLower(workspace) == "consulting") {
        relevantTasks = ptr->consultingTasks;
        } else if (toLower(workspace) == "all") {
        relevantTasks = ptr->consultingTasks;
        relevantTasks.insert(relevantTasks.end(), ptr->devTasks.begin(), ptr->devTasks.end());
        }

        // Accumulate hours if role matches
        for (const Task& task : relevantTasks) {
            bool matches = true;
            if (toLower(role) != "all") {
                auto it = roleMap.find(task.name);
                std::string mappedRole = (it != roleMap.end()) ? it->second : "";
                if (toLower(mappedRole) != toLower(role))
                    matches = false;
        }

        if (matches) {
        total += task.hours;
        people.insert(task.name);
        }
    }

    // Calculate available hours based on labor goal for each person
    double adjustedAvailableHours = 0.0;
    for (const auto& person : people) {
        double goal = 1.0; // default to 100%
        auto it = directLaborGoals.find(person);
        if (it != directLaborGoals.end()) {
            goal = it->second / 100.0;
        }
        adjustedAvailableHours += ptr->availableHours * goal;
    }

    double utilization = (adjustedAvailableHours == 0.0) ? 0.0 : (total / adjustedAvailableHours) * 100.0;
    utilization = static_cast<int>(utilization * 100 + 0.5) / 100.0;

    std::string icon;
    if (ptr->milestoneCount > 0) icon += " [M]";
    if (ptr->isReleaseMonth) icon += " [R]";
    if (utilization > 90.0)       icon += " [HIGH]";
    else if (utilization >= 70.0) icon += " [OK]";
    else                          icon += " [LOW]";

    printf("%-8s| %7.2f%% %s\n", ptr->monthName.c_str(), utilization, icon.c_str());
    ptr = ptr->next;
    }
}


int runCalendarCLI(int year, MonthNode* calendar,
    const std::map<std::string, std::string>& roleMap,
    const std::map<std::string, double>& directLaborGoals) {
std::string workspace, role;

std::cout << "\n==== Workload Utilization Menu ====\n";
std::cout << "Enter workspace (or 'All'): ";
std::getline(std::cin, workspace);
std::cout << "Enter job role (or 'All'): ";
std::getline(std::cin, role);

filterAndPrintCalendar(calendar, workspace, role, year, roleMap, directLaborGoals);
return 0;
}

std::map<std::string, std::string> loadRoleMap(const std::string& filename) {
    std::map<std::string, std::string> roleMap;
    std::ifstream file(filename);
    std::string line;

    std::getline(file, line); // skip header

    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string name, role;

        std::getline(ss, name, ',');
        std::getline(ss, role, ',');

        // Trim
        name.erase(0, name.find_first_not_of(" \t\r\n"));
        name.erase(name.find_last_not_of(" \t\r\n") + 1);
        role.erase(0, role.find_first_not_of(" \t\r\n"));
        role.erase(role.find_last_not_of(" \t\r\n") + 1);

        if (!name.empty() && !role.empty())
            roleMap[name] = role;
    }

    return roleMap;
}

std::map<std::string, double> loadLaborGoals(const std::string& filename) {
    std::map<std::string, double> goals;
    std::ifstream file(filename);
    std::string line;

    std::getline(file, line); // skip header
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string name, role, commercialGoal, devGoal;
        std::getline(ss, name, ',');
        std::getline(ss, role, ',');
        std::getline(ss, commercialGoal, ','); // assuming this is the % to use

        name.erase(0, name.find_first_not_of(" \t\r\n"));
        name.erase(name.find_last_not_of(" \t\r\n") + 1);

        double goal = 1.0; // default to 100%
        try {
            goal = std::stod(commercialGoal);
        } catch (...) {}

        goals[name] = goal;
    }
    return goals;
}