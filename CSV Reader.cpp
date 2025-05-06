#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "WorkUtilizationCalendar.h"

std::vector<CeloxisTask> readCSV(const std::string& filename) {
    std::ifstream file(filename);
    std::vector<CeloxisTask> tasks;
    if (!file.is_open()) {
        std::cerr << "File must be opened. Try again!\n";
        return tasks;
    }

    std::string line;
    std::getline(file, line); // skip header

    while (std::getline(file, line)) {
        std::vector<std::string> fields;
        std::stringstream ss;
        bool inQuotes = false;

        for (size_t i = 0; i < line.size(); ++i) {
            char c = line[i];
            if (c == '"') {
                inQuotes = !inQuotes;
            } else if (c == ',' && !inQuotes) {
                fields.push_back(ss.str());
                ss.str(""); ss.clear();
            } else {
                ss << c;
            }
        }
        fields.push_back(ss.str()); // last field

        if (fields.size() < 6) {
            continue;
        }

        CeloxisTask t;
        t.name      = fields[0];
        t.start     = fields[1];
        t.finish    = fields[2];
        t.resources = fields[3];
        t.workspace = fields[4];
        t.milestone = fields[5];
        tasks.push_back(t);
    }

    file.close();
    return tasks;
}
