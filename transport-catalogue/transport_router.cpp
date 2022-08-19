#include "transport_router.h"
#include <iostream>

using namespace std;

namespace transport_router {

void transport_router::TransportRouter::SetTransportRouter(TransportRouterSettings transp_rout){
    transp_rout_ = transp_rout;
}

void TransportRouter::BuildGraph(const TransportCatalogue& db) {
    Clear();

    const auto& stops = db.GetStops();
    auto stops_count = stops.size();
    //stops_.resize(stops_count);
    graph_.VertexResize(stops_count);
    size_t i = 0;
    
    for (const auto& [stop_name, stop] : stops) {
        stops_.push_back(stop);
        stops_ids_[stop_name] = i++;
    }

    for (const auto& bus : db.GetBuses()) {
        AddEdges(bus, db);
    }

    router_ = std::make_unique<graph::Router<double>>(graph_);
}

void TransportRouter::AddEdges(const Bus* bus,
    const TransportCatalogue& db) {

    const auto& bus_stops = bus->stops;
    auto bus_stops_count = bus_stops.size();
    if (bus_stops_count == 0) {
        return;
    }

    double bus_speed = 
        transp_rout_.bus_velocity * 1000.0 / 60;

    for (size_t i = 0; i < bus_stops_count - 1; ++i) {
        for (size_t j = i + 1; j < bus_stops_count; ++j) {
            int distance = 0;
            double trip_time = 0;
            graph::VertexId from_vid = 
                stops_ids_.at(bus_stops[i]->name);
            graph::VertexId to_vid = 
                stops_ids_.at(bus_stops[j]->name);
             for (size_t s = i; s < j; ++s) {
                distance +=
                    db.GetDistance(bus_stops[s],
                                            bus_stops[s + 1]);
            }
            trip_time += (distance * 1.0 / bus_speed + transp_rout_.bus_wait_time);
            graph::Edge<double> edge = { from_vid , to_vid,
                                         trip_time};
            graph::EdgeId edge_id = graph_.AddEdge(edge);

            buses_names_[edge_id] = bus->name;
            stops_counts_[edge_id] = j - i;
        }
    }

    if (bus->is_ring) {
        return;
    }

    for (size_t i = bus_stops_count - 1; i > 0; --i) {
        for (size_t j = i - 1; j + 1 > 0; --j) {
            int distance = 0;
            double trip_time = 0;
            graph::VertexId from_vid = 
                stops_ids_.at(bus_stops[i]->name);
            graph::VertexId to_vid = 
                stops_ids_.at(bus_stops[j]->name);

            for (size_t s = i; s > j; --s) {
                distance +=
                    db.GetDistance(bus_stops[s],
                                            bus_stops[s - 1]);
            }
            trip_time += (distance * 1.0 / bus_speed + transp_rout_.bus_wait_time);
            graph::Edge<double> edge = { from_vid , to_vid,
                                        trip_time};
            graph::EdgeId edge_id = graph_.AddEdge(edge);

            buses_names_[edge_id] = bus->name;
            stops_counts_[edge_id] = i - j;
        }
    }
}
    
vector<TripAction> TransportRouter::GetRoute(string_view from_stop,
            string_view to_stop) {

    if (stops_ids_.count(from_stop) == 0 ||
        stops_ids_.count(to_stop) == 0) {
        return {};
    }

    auto from_id = stops_ids_.at(from_stop);
    auto to_id = stops_ids_.at(to_stop);

    vector<TripAction> result = {};
    optional<graph::Router<double>::RouteInfo> info = nullopt;
    if ((*router_.get()).BuildRoute(from_id, to_id).has_value()) {
        info = *router_.get()->BuildRoute(from_id, to_id);
    }

    TripAction trip_action;

    if (info.has_value()) {
        trip_action.type = ActionType::EMPTY;
        result.push_back(trip_action);   
        for (size_t i = 0; i < info.value().edges.size(); ++i) {
            trip_action.type = ActionType::WAIT;
            auto vid = 
                graph_.GetEdge(info.value().edges.at(i)).from;
            trip_action.name = stops_[vid]->name;
            trip_action.span_count = 0;
            trip_action.time = transp_rout_.bus_wait_time;
            result.push_back(trip_action);
            trip_action.type = ActionType::IN_BUS;
            auto eid = info.value().edges.at(i);
            trip_action.name = buses_names_.at(eid);
            trip_action.span_count = stops_counts_.at(eid);
            trip_action.time = graph_.GetEdge(eid).weight - transp_rout_.bus_wait_time;
            result.push_back(trip_action);          
        }
    } 
    return result;
}

void TransportRouter::Clear() {
    graph_.Clear(); 
    stops_.clear();
    stops_ids_.clear();
    buses_names_.clear();
    stops_counts_.clear();
}
    
} //namespace transport_router    