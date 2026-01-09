#include "analyzer.h"
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <fstream>

using namespace std;

static int retrieveHourPart(const string &fullDate) {
    if (fullDate.empty()) return -1;
    size_t gapIndex = fullDate.find(' ');
    if (gapIndex == string::npos) return -1;
    string hourSegment = fullDate.substr(gapIndex + 1, 2);
    try {
        size_t conversionCount = 0;
        int resultHour = stoi(hourSegment, &conversionCount);
        return resultHour;
    } catch (...) {
        return -1;
    }
    if (gapIndex == string::npos || gapIndex + 2 >= fullDate.length()) return -1;
    
    int h = (fullDate[gapIndex + 1] - '0') * 10 + (fullDate[gapIndex + 2] - '0');
    return (h >= 0 && h <= 23) ? h : -1;
}

bool TripAnalyzer::analyzeLineContent(const string &rawLine, string &targetZone, int &targetHour) {
    if (rawLine.empty() || rawLine[0] == 'T') return false;

    size_t p1 = rawLine.find(',');
    if (p1 == string::npos) return false;

    size_t p2 = rawLine.find(',', p1 + 1);
    if (p2 == string::npos) return false;

    size_t p3 = rawLine.find(',', p2 + 1);
    if (p3 == string::npos) return false;

    size_t p4 = rawLine.find(',', p3 + 1);
    if (p4 == string::npos) return false;

    size_t p5 = rawLine.find(',', p4 + 1);
    if (p5 == string::npos) return false;

    targetZone = rawLine.substr(p1 + 1, p2 - p1 - 1);
    string dateVal = rawLine.substr(p3 + 1, p4 - p3 - 1);

    if (targetZone.empty() || dateVal.empty()) return false;

    targetHour = retrieveHourPart(dateVal);

    return (targetHour >= 0 && targetHour <= 23);
}

void TripAnalyzer::ingestFile(const string& csvPath) {
    ifstream file(csvPath);
    if (!file.is_open()) return;
    string line;
    if (!getline(file, line)) { file.close(); return; }
    string zone;
    int hour;
    while (getline(file, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (analyzeLineContent(line, zone, hour)) {
            pickupZoneTripCounts[zone]++;
            if (zoneHourlyTripCounts[zone].empty()) {
                zoneHourlyTripCounts[zone].assign(24, 0);
            }
            zoneHourlyTripCounts[zone][hour]++;
        }
    }
    file.close();
}
vector<ZoneCount> TripAnalyzer::topZones(int k) const {
    if (k <= 0 || pickupZoneTripCounts.empty()) return {};

    auto cmp = [](const ZoneCount& a, const ZoneCount& b) {
        if (a.count != b.count) return a.count > b.count;
        return a.zone < b.zone;
    };

    priority_queue<ZoneCount, vector<ZoneCount>, decltype(cmp)> minHeap(cmp);

    for (const auto& [zoneName, tripCount] : pickupZoneTripCounts) {
        minHeap.push({zoneName, tripCount});
        if (minHeap.size() > (size_t)k) {
            minHeap.pop();
        }
    }

    vector<ZoneCount> results;
    results.reserve(minHeap.size());
    while (!minHeap.empty()) {
        results.push_back(minHeap.top());
        minHeap.pop();
    }
    reverse(results.begin(), results.end());
    return results;
}

vector<SlotCount> TripAnalyzer::topBusySlots(int k) const {
    if (k <= 0 || zoneHourlyTripCounts.empty()) return {};

    auto cmp = [](const SlotCount& a, const SlotCount& b) {
        if (a.count != b.count) return a.count > b.count;
        if (a.zone != b.zone) return a.zone < b.zone;
        return a.hour < b.hour;
    };

    priority_queue<SlotCount, vector<SlotCount>, decltype(cmp)> minHeap(cmp);

    for (const auto& [zoneName, hourlyVec] : zoneHourlyTripCounts) {
        for (int h = 0; h < 24; ++h) {
            if (hourlyVec[h] > 0) {
                minHeap.push({zoneName, h, hourlyVec[h]});
                if (minHeap.size() > (size_t)k) {
                    minHeap.pop();
                }
            }
        }
    }

    vector<SlotCount> results;
    results.reserve(minHeap.size());
    while (!minHeap.empty()) {
        results.push_back(minHeap.top());
        minHeap.pop();
    }
    reverse(results.begin(), results.end());
    return results;
}



