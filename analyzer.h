#ifndef ANALYZER_H
#define ANALYZER_H

#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

struct ZoneCount {
    string zone;
    long long count;
};

struct SlotCount {
    string zone;
    int hour;
    long long count;
};

class TripAnalyzer {
public:
    void ingestFile(const string& csvPath);
    
    vector<ZoneCount> topZones(int k = 10) const;
    vector<SlotCount> topBusySlots(int k = 10) const;

private:
    unordered_map<string, long long> pickupZoneTripCounts;
    unordered_map<string, vector<long long>> zoneHourlyTripCounts;
    bool analyzeLineContent(const string &rawLine, string &targetZone, int &targetHour);
};

#endif



