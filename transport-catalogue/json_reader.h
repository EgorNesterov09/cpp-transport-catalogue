#pragma once
#include "json.h"
#include "map_renderer.h"
#include "transport_router.h"
/*
 * Здесь код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */

#include "json.h"
#include "transport_catalogue.h"
#include <iostream>

class JSONReader {
public:
    void LoadJSONinBase(std::istream& in);
    virtual void LoadBaseInTransportCatalogue() = 0;
    virtual json::Document DoStatRequests() = 0;
    map_renderer::MapRendererSettings LoadRender();
    transport_router::TransportRouterSettings LoadRouter();
protected:
    json::Document QueryBase_;
    std::string file_patch_;
};
