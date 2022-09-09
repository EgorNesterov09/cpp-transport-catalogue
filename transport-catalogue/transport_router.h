#pragma once

#include "transport_catalogue.h"
#include "domain.h"
#include "router.h"

#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>

namespace transport_router {
    
struct TransportRouterSettings {
    int bus_wait_time;
    double bus_velocity;
};
    
class TransportRouter {
public:
    TransportRouter() = default;
    TransportRouter(TransportRouterSettings transp_rout) : transp_rout_(transp_rout) {};
    
    void SetTransportRouter(TransportRouterSettings transp_rout);
    TransportRouterSettings GetTransportRouterSetting();
    
    void BuildGraph(const transport_catalogue::TransportCatalogue& db);
    
    std::vector<TripAction> GetRoute(std::string_view from_stop, 
                                        std::string_view to_stop);
    
    void Clear();
    
private:    
    TransportRouterSettings transp_rout_;
    
    graph::DirectedWeightedGraph<double> graph_;
    std::unique_ptr<graph::Router<double>> router_;
    
    std::vector<Stop*> stops_;
    std::unordered_map<std::string_view, size_t> stops_ids_;
    
    std::unordered_map<graph::EdgeId, std::string_view> buses_names_;
    std::unordered_map<graph::EdgeId, int> stops_counts_;
    
    void AddEdges(const Bus* bus, const transport_catalogue::TransportCatalogue& db);
}; 
} //namespace transport_router