#pragma once
#include "transport_router.h"
#include "json.h"
#include "transport_catalogue.h"
#include "json_reader.h"
#include "json_builder.h"
#include <unordered_set>
#include <string>
#include <filesystem>



/*
 * Здесь код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 * Класс RequestHandler играет роль Фасада, упрощающего взаимодействие JSON reader-а с другими подсистемами приложения.
 * См. паттерн проектирования Фасад: https://ru.wikipedia.org/wiki/Фасад_(шаблон_проектирования)
 */

namespace request_handler {
struct BusStat {
    double curvature;
    int route_length;
    int stop_count;
    int unique_stop_count;
 };

class RequestHandler: public JSONReader {
public:
    RequestHandler() = default;
    //RequestHandler(transport_catalogue::TransportCatalogue& db, map_renderer::MapRenderer& renderer, transport_router::TransportRouter& router) : catalog_(db),  renderer_(renderer), router_(router){};
    
    void SetCatalogs(transport_catalogue::TransportCatalogue& db, map_renderer::MapRenderer& renderer, transport_router::TransportRouter& router);
    
    void LoadBaseInTransportCatalogue() override;
    
    json::Document DoStatRequests() override;
    
    void LoadRoutingSettings();

    void LoadRenderSettings();
    
    BusStat GetBusStat(const std::string_view& bus_name) const;
    
    std::set<std::string> GetStopStat(const std::string_view& stop_name) const;
    
    std::vector<const Bus*> GetBuses() const;

    std::filesystem::path GetPatch();
private:
    // RequestHandler использует агрегацию объектов "Транспортный Справочник" и "Визуализатор Карты"
    transport_catalogue::TransportCatalogue* catalog_;
    map_renderer::MapRenderer* renderer_;
    transport_router::TransportRouter* router_;
};
} //namespace request_handler 