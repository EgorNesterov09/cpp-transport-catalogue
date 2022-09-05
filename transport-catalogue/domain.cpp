#include "domain.h"

Stop::Stop(const std::string& name_, long double latitude_, long double longitude_) : name(name_), latitude(latitude_), longitude(longitude_) {}

Bus::Bus(const std::string& name_, const std::vector<Stop*>& stops_, bool is_ring_, double geo_distance_, int road_distance_) : name(name_), stops(stops_), is_ring(is_ring_), geo_distance(geo_distance_), road_distance(road_distance_) {}