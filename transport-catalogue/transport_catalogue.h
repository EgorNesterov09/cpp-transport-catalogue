#pragma once

/*
 * Здесь код транспортного справочника
 */
#include "domain.h"
#include <string>
#include <string_view>
#include <vector>
#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <functional>

class StopPairHasher {
public:
    size_t operator()(std::pair<Stop*, Stop*> stop) const;
private:
    std::hash<const void*> hasher_;
};

class StopHasher {
public:
    size_t operator()(Stop* stop) const;
private:
    std::hash<Stop*> hasher_;
};

class TransportCatalogue {
public:
    
    void AddStop(std::string stop, long double latitude,
    long double longitude);
    bool CheckStop(std::string_view stop);
    Stop* FindStop(std::string_view stop);
    std::unordered_map<std::string_view, Stop*> GetStops() const;
    
    std::vector<const Bus*> GetBuses() const;
    void AddBus(std::string bus, std::vector<Stop*> stops, bool is_ring, double geo_distance, int road_distance);
    Bus* FindBus(std::string_view bus);
    bool CheckBus(std::string_view bus);
    void GetBusInfo(Bus* bus);    
    std::set<std::string> FindBusesToStop(Stop* stop);
    
    void SetDistance(Stop* stop1, Stop* stop2, int distance);
    int GetDistance(Stop* stop1, Stop* stop2) const;    
private:
    std::deque<Stop> stops_;
    std::unordered_map<std::string_view, Stop*> stops_hash_table_;
    std::deque<Bus> buses_;
    std::unordered_map<std::string_view, Bus*> buses_hash_table_;
    std::unordered_map<Stop*,std::set<std::string>, StopHasher> buses_to_stop_;
    std::unordered_map<std::pair<Stop*, Stop*>, int, StopPairHasher> all_distance_;    
};