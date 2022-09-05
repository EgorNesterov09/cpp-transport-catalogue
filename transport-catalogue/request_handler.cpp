#include "request_handler.h"
#include "map_renderer.h"
#include "geo.h"
#include <sstream>
#include <iostream>

using namespace std;
    // Возвращает информацию о маршруте (запрос Bus)
BusStat RequestHandler::GetBusStat(const string_view& bus_name) const {
    Bus* bus = catalog_.FindBus(bus_name);
    double curvature = (*bus).road_distance / (*bus).geo_distance;
    int all_stops = (*bus).stops.size();
    set<Stop*> bu;
    for (auto a: (*bus).stops) {
        bu.insert(a);
    }
    int unique_stops = bu.size();
    BusStat result;
    result.curvature = curvature;
    result.route_length = bus->road_distance;
    result.stop_count = all_stops;
    result.unique_stop_count = unique_stops;
    return result;
}

void RequestHandler::LoadBaseInTransportCatalogue() {
    json::Array base_requests = QueryBase_.GetRoot().AsMap().at("base_requests"s).AsArray();
    for (const auto& dict: base_requests) {
        if(dict.AsMap().at("type"s).AsString() == "Stop") {
            const string& stop = dict.AsMap().at("name"s).AsString();
            const double& latitude = dict.AsMap().at("latitude"s).AsDouble();
            const double& longitude = dict.AsMap().at("longitude"s).AsDouble();
            catalog_.AddStop(stop, latitude, longitude);
        }
    }
    for (const auto& dict: base_requests) {
        if(dict.AsMap().at("type"s).AsString() == "Stop") {
        const string& stop = dict.AsMap().at("name"s).AsString();
            for (const auto& [next_stop, dist]: dict.AsMap().at("road_distances"s).AsMap()) {
                catalog_.SetDistance(catalog_.FindStop(stop), catalog_.FindStop(next_stop), dist.AsDouble());
            }
        }
    }
    for (const auto& dict: base_requests) {
        if(dict.AsMap().at("type"s).AsString() == "Bus") {
            const string& name = dict.AsMap().at("name"s).AsString();
            bool is_ring = dict.AsMap().at("is_roundtrip"s).AsBool();
            vector<Stop*> vec;
            vector<string_view> vec1;
            double geo_distance = 0;
            int road_distance = 0;
            if (dict.AsMap().find("stops"s)!=dict.AsMap().end() && !dict.AsMap().at("stops"s).AsArray().empty()) {
            for (const auto& stop: dict.AsMap().at("stops"s).AsArray()) {
                vec.push_back(catalog_.FindStop(stop.AsString()));
                if (is_ring == false) {
                    vec1.push_back(string_view(stop.AsString()));
                }
            }
            if (is_ring == false) {    
                for (auto it = vec1.rbegin()+1; it != vec1.rend(); it++) {
                    vec.push_back(catalog_.FindStop(*it));
                }
            }
            for (size_t i = 0; i < vec.size()-1; i++) {
                geo_distance += ComputeDistance (
                geo::Coordinates((vec[i])->latitude, (vec[i])->longitude),
                geo::Coordinates((vec[i+1])->latitude, (vec[i+1])->longitude) 
                );
                road_distance += catalog_.GetDistance(vec[i], vec[i+1]);
            }
            }
            catalog_.AddBus(name, move(vec), is_ring, geo_distance, road_distance);
        }
    }
} 

json::Document RequestHandler::DoStatRequests() {
    json::Array stat_requests = QueryBase_.GetRoot().AsMap().at("stat_requests"s).AsArray();
    json::Array result;
    for (const auto& request: stat_requests) {
        if (request.AsMap().at("type"s).AsString() == "Stop") {
            const string& stop = request.AsMap().at("name"s).AsString();
            if (catalog_.CheckStop(stop)) {
                int id = request.AsMap().at("id"s).AsInt();
                json::Array stops;
                for (auto a: catalog_.FindBusesToStop(catalog_.FindStop(stop))){
                    stops.push_back(json::Node(a));
                }
                result.push_back(json::Document{json::Builder{}
                                .StartDict()
                                    .Key("buses"s).Value(stops)
                                    .Key("request_id"s).Value(id)
                                .EndDict()
                            .Build()}.GetRoot()); 
            } else {
                int id = request.AsMap().at("id"s).AsInt();
                result.push_back(json::Document{json::Builder{}
                                .StartDict()
                                    .Key("error_message"s)
                                    .Value("not found"s)
                                    .Key("request_id"s)
                                    .Value(id)
                                .EndDict()
                            .Build()}.GetRoot()); 
            }            
        }
        else if (request.AsMap().at("type"s).AsString() == "Bus") {
            const string& bus = request.AsMap().at("name"s).AsString();
            if (catalog_.CheckBus(bus)) {
                int id = request.AsMap().at("id"s).AsInt();
                BusStat b_stat = GetBusStat(bus);
                result.push_back(json::Document{json::Builder{}
                                .StartDict()
                                    .Key("curvature"s)
                                    .Value(b_stat.curvature)
                                    .Key("request_id"s).Value(id)
                                    .Key("route_length"s)
                                    .Value(b_stat.route_length)
                                    .Key("stop_count"s)
                                    .Value(b_stat.stop_count)
                                    .Key("unique_stop_count"s)
                                    .Value(b_stat.unique_stop_count)
                                .EndDict()
                            .Build()}.GetRoot());
            } else {
                int id = request.AsMap().at("id"s).AsInt();
                result.push_back(json::Document{json::Builder{}
                                .StartDict()
                                    .Key("error_message"s).Value("not found"s)
                                    .Key("request_id"s).Value(id)
                                .EndDict()
                            .Build()}.GetRoot());
            }
        }
        else if (request.AsMap().at("type"s).AsString() == "Map") {
            int id = request.AsMap().at("id"s).AsInt();
            ostringstream ss;
            string s;
            renderer_.PrintRoad(GetBuses(), ss);
            result.push_back(json::Document{json::Builder{}
                                .StartDict()
                                    .Key("map"s).Value(ss.str())
                                    .Key("request_id"s).Value(id)
                                .EndDict()
                            .Build()}.GetRoot());
        }
        else if (request.AsMap().at("type"s).AsString() == "Route") {
            int id = request.AsMap().at("id"s).AsInt();
            const string& from = request.AsMap().at("from"s).AsString();
            const string& to = request.AsMap().at("to"s).AsString();
            auto route_answer = router_.GetRoute(from, to);
            if (!route_answer.empty()) {           
                json::Array array;
                double total_time = 0;
                for (const auto& answer: route_answer) {
                    total_time += answer.time;
                    if (answer.type == ActionType::WAIT) {
                        json::Node dict_node{json::Dict{{"stop_name"s, answer.name}, {"type"s, "Wait"s}, {"time"s, answer.time} }};
                        array.push_back(move(dict_node));
                    } else if (answer.type == ActionType::IN_BUS) {
                        json::Node dict_node{json::Dict{{"bus"s, answer.name}, {"span_count"s, answer.span_count}, {"time"s, answer.time}, {"type"s, "Bus"s} }};
                        array.push_back(move(dict_node));
                    }
                }
                result.push_back(json::Document{json::Builder{}
                            .StartDict()
                                .Key("items"s).Value(array)
                                .Key("request_id"s).Value(id)
                                .Key("total_time"s).Value(total_time)
                            .EndDict()
                        .Build()}.GetRoot());
            } else {
                result.push_back(json::Document{json::Builder{}
                                .StartDict()
                                    .Key("request_id"s).Value(id)
                                    .Key("error_message"s).Value("not found"s)
                                .EndDict()
                            .Build()}.GetRoot()); 
            }
        }
    }
    return json::Document(json::Builder{}.Value(result).Build());
}

void RequestHandler::LoadRenderSettings() {
    renderer_.SetMapRenderer(LoadRender()); 
 }

std::vector<const Bus*> RequestHandler::GetBuses() const {
    return catalog_.GetBuses();
}
                
void RequestHandler::LoadRoutingSettings() {
    router_.SetTransportRouter(LoadRouter());
    router_.BuildGraph(catalog_);
}