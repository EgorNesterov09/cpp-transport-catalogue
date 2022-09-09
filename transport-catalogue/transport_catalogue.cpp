#include "transport_catalogue.h"

#include "geo.h"
#include <iostream>
#include <iomanip>
#include <string_view>
#include <set>
using namespace std;

namespace transport_catalogue {
void TransportCatalogue::AddStop(std::string stop, long double latitude,
long double longitude) {
    Stop a (stop, latitude, longitude);
    stops_.push_back(move(a));
    stops_hash_table_.insert({string_view(stops_.back().name), &stops_.back()});
    buses_to_stop_[&stops_.back()];
    
}

Stop* TransportCatalogue::FindStop(std::string_view stop) {
    return &*stops_hash_table_.at(stop);
}

void TransportCatalogue::AddBus(string bus, vector<Stop*> stops, bool is_ring, double geo_distance, int road_distance) {
    Bus b(bus, stops, is_ring, geo_distance, road_distance);
    buses_.push_back(move(b));
    buses_hash_table_.insert({string_view(buses_.back().name), &buses_.back()});
    for (auto& stop : stops) {
        buses_to_stop_[stop].insert(bus);
    }
}

Bus* TransportCatalogue::FindBus(std::string_view bus) const {
    return buses_hash_table_.at(bus);
}

bool TransportCatalogue::CheckBus(std::string_view bus) {
    if (buses_hash_table_.find(bus) == buses_hash_table_.end()) {
        return false;
    }
    return true;
}

bool TransportCatalogue::CheckStop(std::string_view stop) {
    if (stops_hash_table_.find(stop) == stops_hash_table_.end()) {
        return false;
    }
    return true;
}

void TransportCatalogue::GetBusInfo(Bus* bus) {
    double curvature = (*bus).road_distance / (*bus).geo_distance;
    int all_stops = (*bus).stops.size();
    set<Stop*> bu;
    for (auto a: (*bus).stops) {
        bu.insert(a);
    }
    int unique_stops = bu.size();

    cout << "Bus "s << (*bus).name << ": "s
    << all_stops << " stops on route, "s << unique_stops << " unique stops, "s << bus->road_distance << " route length, "s << curvature << " curvature"s << endl;
}

set<string> TransportCatalogue::FindBusesToStop(Stop* stop) {
    return buses_to_stop_.at(stop);
}

int TransportCatalogue::GetDistance(Stop* stop1, Stop* stop2) const {
    if (all_distance_.find(make_pair(stop1, stop2)) != all_distance_.end()) {
        return all_distance_.at({stop1, stop2});
    } else {
        return all_distance_.at({stop2, stop1});
    }
}

void TransportCatalogue::SetDistance(Stop* stop1, Stop* stop2, int distance) {
    all_distance_[{stop1, stop2}] = distance;
}

vector<const Bus*> TransportCatalogue::GetBuses() const {
    vector<const Bus*> result;
    for (const auto& [str, bus]: buses_hash_table_) {
        result.push_back(bus);
    }
    return result;
}

map<std::pair<Stop*, Stop*>, int> TransportCatalogue::GetDistances() {
    map<std::pair<Stop*, Stop*>, int> result;
    for (const auto& [key, value] : all_distance_) {
        result[key] = value;
    }
    return result;
}

size_t StopPairHasher::operator()(std::pair<Stop*, Stop*> stop) const {
        return hasher_(stop.first)*37 + hasher_(stop.second)*(37^2);
}

size_t StopHasher::operator()(Stop* stop) const {
        return hasher_(stop)*37;
}

std::unordered_map<std::string_view, Stop*> TransportCatalogue::GetStops() const {
    return stops_hash_table_;
}

} //namespace transport_catalogue 