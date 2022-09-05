#pragma once
#include <string>
#include <vector>

/*
 * В этом файле размесщены классы/структуры, которые являются частью предметной области (domain) приложения и не зависят от транспортного справочника. Например Автобусные маршруты и Остановки.
 * Их можно было бы разместить и в transport_catalogue.h, однако вынесение их в отдельный заголовочный файл может полезно, при визуализации карты маршрутов:
 * визуализатор карты (map_renderer) независим от транспортного справочника.
 */

struct Stop {
    Stop(const std::string& name_, long double latitude_, long double longitude_);
    std::string name;
    long double latitude;
    long double longitude;
};

struct Bus {
    Bus(const std::string& name_, const std::vector<Stop*>& stops_, bool is_ring_, double geo_distance_, int road_distance_);
    std::string name;
    std::vector<Stop*> stops;
    bool is_ring;
    double geo_distance = 0; 
    int road_distance = 0; 
};

enum class ActionType {
    WAIT,
    IN_BUS,
    EMPTY
};

struct TripAction {
    ActionType type;
    std::string name;
    int span_count;
    double time = 0;
};