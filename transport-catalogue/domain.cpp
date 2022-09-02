#include "domain.h"

/*
 * В этом файле размещены классы/структуры, которые являются частью предметной области
 * (domain) приложения и не зависят от транспортного справочника. Например Автобусные
 * маршруты и Остановки.
 *
 * Их можно было бы разместить и в transport_catalogue.h, однако вынесение их в отдельный
 * заголовочный файл может оказаться полезным, при визуализации карты маршрутов:
 * визуализатор карты (map_renderer) не зависит от транспортного справочника.
 *
 */
Stop::Stop(const std::string& name_, long double latitude_, long double longitude_) : name(name_), latitude(latitude_), longitude(longitude_) {}

Bus::Bus(const std::string& name_, const std::vector<Stop*>& stops_, bool is_ring_, double geo_distance_, int road_distance_) : name(name_), stops(stops_), is_ring(is_ring_), geo_distance(geo_distance_), road_distance(road_distance_) {}
