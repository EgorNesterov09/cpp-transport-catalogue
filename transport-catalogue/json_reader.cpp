#include "json_reader.h"

/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */

#include <string>
#include <string_view>
#include <deque>
#include "geo.h"
#include "domain.h"
#include <algorithm>
#include <iostream>
#include<iomanip>
using namespace std;

void JSONReader::LoadJSONinBase(istream& in) {
    string t;
    string s;
    while (getline(in, t)) {
        if (t == ""s) {
            break;
        } 
        s += t;
        }
    istringstream strm(s);
    QueryBase_ = json::Load(strm);
}

svg::Color JsonToColor(const json::Node& nod) {
    svg::Color col;
    if (nod.IsString()) {
        col = {nod.AsString()};
    }
    else {
        if (nod.AsArray().size()==4) {
            col = svg::Rgba{nod.AsArray()[0].AsInt(), nod.AsArray()[1].AsInt(), nod.AsArray()[2].AsInt(), nod.AsArray()[3].AsDouble()};
        }
        else {
            col = svg::Rgb{nod.AsArray()[0].AsInt(), nod.AsArray()[1].AsInt(), nod.AsArray()[2].AsInt()};                
        }
    }
    return col;
}

transport_router::TransportRouterSettings JSONReader::LoadRouter() {
    json::Dict routing_settings = QueryBase_.GetRoot().AsMap().at("routing_settings"s).AsMap();
    transport_router::TransportRouterSettings result;
    result.bus_wait_time = routing_settings.at("bus_wait_time"s).AsInt();
    result.bus_velocity = routing_settings.at("bus_velocity"s).AsDouble();
    return result;
}

map_renderer::MapRendererSettings JSONReader::LoadRender() {
    json::Dict render_settings = QueryBase_.GetRoot().AsMap().at("render_settings"s).AsMap();
    vector<svg::Color> vec;
    for (auto& a: render_settings.at("color_palette"s).AsArray()) {
        vec.push_back(move(JsonToColor(a)));
    }

map_renderer::MapRendererSettings result;
result.width = render_settings.at("width"s).AsDouble(); 
result.height = render_settings.at("height"s).AsDouble(); 
result.padding = render_settings.at("padding"s).AsDouble();
result.line_width = render_settings.at("line_width"s).AsDouble(); 
result.stop_radius = render_settings.at("stop_radius"s).AsDouble(); 
result.bus_label_font_size = render_settings.at("bus_label_font_size"s).AsInt(); 
result.bus_label_offset = {render_settings.at("bus_label_offset"s).AsArray()[0].AsDouble(), render_settings.at("bus_label_offset"s).AsArray()[1].AsDouble()};
result.stop_label_font_size = render_settings.at("stop_label_font_size"s).AsInt(); 
result.stop_label_offset = {render_settings.at("stop_label_offset"s).AsArray()[0].AsDouble(), render_settings.at("stop_label_offset"s).AsArray()[1].AsDouble()};
result.underlayer_color = JsonToColor(render_settings.at("underlayer_color"s)); 
result.underlayer_width = render_settings.at("underlayer_width"s).AsDouble();
result.color_palette = move(vec);
return result;
}