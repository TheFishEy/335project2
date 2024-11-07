#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>

using namespace std;

int convertTimeToMinutes(const string& time) {
    int hours = stoi(time.substr(0, 2));
    int minutes = stoi(time.substr(3, 2));
    return hours * 60 + minutes;
}

string convertMinutesToTime(int minutes) {
    int hours = minutes / 60;
    int mins = minutes % 60;
    return (hours < 10 ? "0" : "") + to_string(hours) + ":" + (mins < 10 ? "0" : "") + to_string(mins);
}

bool readTestCase(ifstream& file, vector<vector<pair<int, int>>>& schedules, vector<pair<int, int>>& workingHours, int& meetingDuration) {
    string line;
    while (getline(file, line)) {
        if (!line.empty() && line[0] != '#') break;
    }
    if (line.empty()) return false;

    int numPeople;
    try {
        numPeople = stoi(line);
    } catch (...) {
        cout << "Error: Invalid number format." << endl;
        return false;
    }

    schedules.clear();
    workingHours.clear();
    schedules.resize(numPeople);
    workingHours.resize(numPeople);

    for (int i = 0; i < numPeople; ++i) {
        if (!getline(file, line) || line.empty()) return false;

        size_t pos = 0;
        while ((pos = line.find(',')) != string::npos) {
            string interval = line.substr(0, pos);
            size_t dashPos = interval.find('-');
            if (dashPos == string::npos || dashPos + 1 >= interval.size()) return false;
            string startTime = interval.substr(0, dashPos);
            string endTime = interval.substr(dashPos + 1);
            schedules[i].push_back({convertTimeToMinutes(startTime), convertTimeToMinutes(endTime)});
            line.erase(0, pos + 1);
        }
        if (!line.empty()) {
            size_t dashPos = line.find('-');
            string startTime = line.substr(0, dashPos);
            string endTime = line.substr(dashPos + 1);
            schedules[i].push_back({convertTimeToMinutes(startTime), convertTimeToMinutes(endTime)});
        }

        if (!getline(file, line) || line.size() < 5) return false;
        size_t dashPos = line.find('-');
        string activeStart = line.substr(0, dashPos);
        string activeEnd = line.substr(dashPos + 1);
        workingHours[i] = {convertTimeToMinutes(activeStart), convertTimeToMinutes(activeEnd)};
    }

    if (!getline(file, line) || line.empty()) return false;
    try {
        meetingDuration = stoi(line);
    } catch (...) {
        cout << "Error: Invalid meeting duration format." << endl;
        return false;
    }

    return true;
}

vector<pair<int, int>> findFreeTimes(const vector<vector<pair<int, int>>>& schedules, const vector<pair<int, int>>& workingHours, int meetingDuration) {
    vector<pair<int, int>> busyTimes;
    for (const auto& personSchedule : schedules) {
        busyTimes.insert(busyTimes.end(), personSchedule.begin(), personSchedule.end());
    }
    sort(busyTimes.begin(), busyTimes.end());

    vector<pair<int, int>> mergedBusyTimes;
    for (const auto& interval : busyTimes) {
        if (mergedBusyTimes.empty() || mergedBusyTimes.back().second < interval.first) {
            mergedBusyTimes.push_back(interval);
        } else {
            mergedBusyTimes.back().second = max(mergedBusyTimes.back().second, interval.second);
        }
    }

    int groupStart = workingHours[0].first;
    int groupEnd = workingHours[0].second;
    for (const auto& hours : workingHours) {
        groupStart = max(groupStart, hours.first);
        groupEnd = min(groupEnd, hours.second);
    }

    if (groupStart >= groupEnd) {
        return {};
    }

    vector<pair<int, int>> availableTimes;
    int start = groupStart;

    for (const auto& busy : mergedBusyTimes) {
        if (start + meetingDuration <= busy.first && start >= groupStart && busy.first <= groupEnd) {
            availableTimes.push_back({start, busy.first});
        }
        start = max(start, busy.second);
    }

    if (start + meetingDuration <= groupEnd) {
        availableTimes.push_back({start, groupEnd});
    }

    return availableTimes;
}

void writeResults(ofstream& file, const vector<pair<int, int>>& availableTimes, int testCaseNum) {
    file << "Test Case " << testCaseNum << ":\n";
    for (const auto& time : availableTimes) {
        file << "[" << convertMinutesToTime(time.first) << ", " << convertMinutesToTime(time.second) << "]\n";
    }
    file << "--------------------\n";
}

int main() {
    ifstream file("Input.txt");
    if (!file.is_open()) {
        cout << "Error: Could not open Input.txt" << endl;
        return 1;
    }

    ofstream output("Output.txt");
    if (!output.is_open()) {
        cout << "Error: Could not open Output.txt" << endl;
        return 1;
    }

    int testCaseNum = 1;
    while (true) {
        vector<vector<pair<int, int>>> schedules;
        vector<pair<int, int>> workingHours;
        int meetingDuration;

        if (!readTestCase(file, schedules, workingHours, meetingDuration)) {
            break;
        }

        vector<pair<int, int>> availableTimes = findFreeTimes(schedules, workingHours, meetingDuration);
        writeResults(output, availableTimes, testCaseNum);

        testCaseNum++;
    }

    file.close();
    output.close();
    return 0;
}
