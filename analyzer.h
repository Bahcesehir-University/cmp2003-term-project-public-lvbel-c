#ifndef ANALYZER_H
#define ANALYZER_H

#include <string>
#include <vector>
#include <unordered_map>

struct ZoneCount {
    std::string zone;
    long long count;
};

struct SlotCount {
    std::string zone;
    int hour;
    long long count;
};

class TripAnalyzer {
public:
    TripAnalyzer();
    void ingestFile(const std::string& csvPath);
    std::vector<ZoneCount> topZones(int k = 10) const;
    std::vector<SlotCount> topBusySlots(int k = 10) const;

private:
    std::unordered_map<std::string, long long> pickupZoneTripCounts;
    std::unordered_map<std::string, std::vector<long long>> zoneHourlyTripCounts;
    bool analyzeLineContent(const std::string &rawLine, std::string &targetZone, int &targetHour);
    static int retrieveHourPart(const std::string &fullDate);
};

#endif




