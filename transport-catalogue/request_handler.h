#pragma once
#include "transport_router.h"
#include "json.h"
#include "transport_catalogue.h"
#include "json_reader.h"
#include "json_builder.h"
#include <unordered_set>
#include <string>
/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 *
 * В качестве источника для идей предлагаем взглянуть на нашу версию обработчика запросов.
 * Вы можете реализовать обработку запросов способом, который удобнее вам.
 *
 * Если вы затрудняетесь выбрать, что можно было бы поместить в этот файл,
 * можете оставить его пустым.
 */

// Класс RequestHandler играет роль Фасада, упрощающего взаимодействие JSON reader-а
// с другими подсистемами приложения.
// См. паттерн проектирования Фасад: https://ru.wikipedia.org/wiki/Фасад_(шаблон_проектирования)
struct BusStat {
    double curvature;
    int route_length;
    int stop_count;
    int unique_stop_count;
 };

class RequestHandler: public JSONReader {
public:
    RequestHandler(TransportCatalogue& db, map_renderer::MapRenderer& renderer, transport_router::TransportRouter& router) : catalog_(db),  renderer_(renderer), router_(router){};
    
    void LoadBaseInTransportCatalogue() override;
    
    json::Document DoStatRequests() override;
    
    void LoadRoutingSettings();

    void LoadRenderSettings();
    // Возвращает информацию о маршруте (запрос Bus)
    BusStat GetBusStat(const std::string_view& bus_name) const;
    
    // Возвращает маршруты, проходящие через
    std::set<std::string> GetStopStat(const std::string_view& stop_name) const;
    
    std::vector<const Bus*> GetBuses() const;

private:
    // RequestHandler использует агрегацию объектов "Транспортный Справочник" и "Визуализатор Карты"
    TransportCatalogue& catalog_;
    map_renderer::MapRenderer& renderer_;
    transport_router::TransportRouter& router_;
};