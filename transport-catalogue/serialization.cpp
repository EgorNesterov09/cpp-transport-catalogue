
#include "serialization.h"
#include <fstream>

using namespace std;

void Serializer::Serialize(std::filesystem::path file, transport_catalogue::TransportCatalogue& catalog, map_renderer::MapRendererSettings renderer_settings, transport_router::TransportRouterSettings router_settings) {
    ofstream out(file, ios::binary);
    
    transport_catalogue_serialize::TransportCatalogue ser_t_cat;
       
    int i = 0;
    for (const auto& [_, stop] : catalog.GetStops()) {
        transport_catalogue_serialize::Stop ser_stop;
        ser_stop.set_name(stop->name); 
        ser_stop.set_lat(stop->latitude); 
        ser_stop.set_lng(stop->longitude);
        
        ser_t_cat.add_stops();
        *ser_t_cat.mutable_stops(i) = ser_stop;
        ++i;
    }
    
    i = 0;
    for (const auto& bus: catalog.GetBuses()) {
        transport_catalogue_serialize::Bus ser_bus;
        ser_bus.set_name(bus->name);
        for (const auto& stop : bus->stops) {
            ser_bus.add_stop(stop->name);
        }
        ser_bus.set_is_round(bus->is_ring);
        ser_bus.set_geo_dist(bus->geo_distance);
        ser_bus.set_road_dist(bus->road_distance);
        
        ser_t_cat.add_buses();
        *ser_t_cat.mutable_buses(i) = ser_bus;
        ++i;
    }
    
    i = 0;
    for (const auto& [pair, dist] : catalog.GetDistances()) {
        transport_catalogue_serialize::Distance ser_dist;
        ser_dist.set_stop_from((pair.first)->name);
        ser_dist.set_stop_to((pair.second)->name);
        ser_dist.set_distance(dist);
        
        ser_t_cat.add_distances();
        *ser_t_cat.mutable_distances(i) = ser_dist;
        ++i;
    }
    *ser_t_cat.mutable_render_settings() = SerializeRendererSettings(renderer_settings);
    *ser_t_cat.mutable_router_settings() = SerializeRouterSettings(router_settings);
    ser_t_cat.SerializeToOstream(&out);
}

transport_catalogue_serialize::Point Serializer::SerializePoint(const svg::Point &point) {
    transport_catalogue_serialize::Point s_point;
    s_point.set_x(point.x);
    s_point.set_y(point.y);
    return s_point;
}

transport_catalogue_serialize::RGB Serializer::SerializeRGB(const svg::Rgb &rgb_color) {
    transport_catalogue_serialize::RGB s_rgb;
    s_rgb.set_red(rgb_color.red);
    s_rgb.set_green(rgb_color.green);
    s_rgb.set_blue(rgb_color.blue);
    return s_rgb;
}

transport_catalogue_serialize::RGBA Serializer::SerializeRGBA(const svg::Rgba &rgba_color) {
    transport_catalogue_serialize::RGBA s_rgba;
    s_rgba.set_red(rgba_color.red);
    s_rgba.set_green(rgba_color.green);
    s_rgba.set_blue(rgba_color.blue);
    s_rgba.set_opacity(rgba_color.opacity);
    return s_rgba;
}

transport_catalogue_serialize::Color Serializer::SerializeColor(const svg::Color &color) {
    transport_catalogue_serialize::Color s_color;
    if(holds_alternative<std::string>(color)){
        s_color.set_string_color(std::get<std::string>(color));
    }
    if(holds_alternative<svg::Rgb>(color)){
        *s_color.mutable_rgb_color() = SerializeRGB(std::get<svg::Rgb>(color));
    }
    if(holds_alternative<svg::Rgba>(color)){
        *s_color.mutable_rgba_color() = SerializeRGBA(std::get<svg::Rgba>(color));
    }
    return s_color;
}

transport_catalogue_serialize::RendererSettings Serializer::SerializeRendererSettings(const map_renderer::MapRendererSettings &renderer_settings) {
    transport_catalogue_serialize::RendererSettings s_rend_set;
    s_rend_set.set_width(renderer_settings.width);
    s_rend_set.set_height(renderer_settings.height);
    s_rend_set.set_padding(renderer_settings.padding);
    s_rend_set.set_line_width(renderer_settings.line_width);
    s_rend_set.set_stop_radius(renderer_settings.stop_radius);
    s_rend_set.set_stop_label_font_size(renderer_settings.stop_label_font_size);
    s_rend_set.set_bus_label_font_size(renderer_settings.bus_label_font_size);
    *s_rend_set.mutable_bus_label_offset() = SerializePoint(renderer_settings.bus_label_offset);
    *s_rend_set.mutable_stop_label_offset() = SerializePoint(renderer_settings.stop_label_offset);
    *s_rend_set.mutable_underlayer_color() = SerializeColor(renderer_settings.underlayer_color);
    s_rend_set.set_underlayer_width(renderer_settings.underlayer_width);
    for(const auto & i : renderer_settings.color_palette){
        *s_rend_set.add_color_palette() = SerializeColor(i);
    }
    return s_rend_set;
}

transport_catalogue_serialize::RouterSettings Serializer::SerializeRouterSettings(const transport_router::TransportRouterSettings& router_settings) {
    transport_catalogue_serialize::RouterSettings s_rout_set;
    s_rout_set.set_wait_time(router_settings.bus_wait_time);
    s_rout_set.set_velocity(router_settings.bus_velocity);
    return s_rout_set;
}

tuple<transport_catalogue::TransportCatalogue, map_renderer::MapRendererSettings, transport_router::TransportRouterSettings> Serializer::Deserialize(const std::filesystem::path& file) {
    ifstream ifs(file, ios::binary);
    transport_catalogue_serialize::TransportCatalogue ser_t_cat;
    ser_t_cat.ParseFromIstream(&ifs);
    transport_catalogue::TransportCatalogue result;
    
    for(int i = 0; i < ser_t_cat.stops_size(); ++i){
        result.AddStop(ser_t_cat.stops(i).name(), ser_t_cat.stops(i).lat(), ser_t_cat.stops(i).lng());
    }
   
    for(int i = 0; i < ser_t_cat.buses_size(); ++i){
        std::vector<Stop*> stops;
        for (int j = 0; j < ser_t_cat.buses(i).stop_size(); ++j) {
            stops.push_back(result.FindStop(ser_t_cat.buses(i).stop(j)));
        }
        result.AddBus(ser_t_cat.buses(i).name(), move(stops), ser_t_cat.buses(i).is_round(), ser_t_cat.buses(i).geo_dist(), ser_t_cat.buses(i).road_dist());
    }
    
    for(int i = 0; i < ser_t_cat.distances_size(); ++i){
    
        result.SetDistance(result.FindStop(ser_t_cat.distances(i).stop_from()), result.FindStop(ser_t_cat.distances(i).stop_to()), ser_t_cat.distances(i).distance());
    }
    map_renderer::MapRendererSettings renderer_settings = DeserializeRendererSettings(ser_t_cat.render_settings());
    transport_router::TransportRouterSettings router_settings = DeserializeRouterSettings(ser_t_cat.router_settings());
    return make_tuple(move(result), renderer_settings, router_settings);
}

svg::Rgb Serializer::DeserializeRGB(const transport_catalogue_serialize::RGB &s_rgb_color) {
    svg::Rgb ds_rgb;
    ds_rgb.red = s_rgb_color.red();
    ds_rgb.green = s_rgb_color.green();
    ds_rgb.blue = s_rgb_color.blue();
    return ds_rgb;
}

svg::Rgba Serializer::DeserializeRGBA(const transport_catalogue_serialize::RGBA &s_rgba_color) {
    svg::Rgba ds_rgba;
    ds_rgba.red = s_rgba_color.red();
    ds_rgba.green = s_rgba_color.green();
    ds_rgba.blue = s_rgba_color.blue();
    ds_rgba.opacity = s_rgba_color.opacity();
    return ds_rgba;
}

svg::Color Serializer::DeserializeColor(const transport_catalogue_serialize::Color &s_color) {
    svg::Color ds_color;
    switch (s_color.variant_case()) {
        case transport_catalogue_serialize::Color::kStringColor:
            ds_color = s_color.string_color();
            break;
        case transport_catalogue_serialize::Color::kRGBColor:
            ds_color = DeserializeRGB(s_color.rgb_color());
            break;
        case transport_catalogue_serialize::Color::kRGBAColor:
            ds_color = DeserializeRGBA(s_color.rgba_color());
            break;
        case transport_catalogue_serialize::Color::VARIANT_NOT_SET:
            ds_color = svg::NoneColor;
            break;
    }
    return ds_color;
}

svg::Point Serializer::DeserializePoint(const transport_catalogue_serialize::Point &s_point) {
    return svg::Point({s_point.x(), s_point.y()});
}

map_renderer::MapRendererSettings Serializer::DeserializeRendererSettings(const transport_catalogue_serialize::RendererSettings &s_rend_set) {
    map_renderer::MapRendererSettings ds_rend_set;
    ds_rend_set.width = s_rend_set.width();
    ds_rend_set.height = s_rend_set.height();
    ds_rend_set.padding = s_rend_set.padding();
    ds_rend_set.line_width = s_rend_set.line_width();
    ds_rend_set.stop_radius = s_rend_set.stop_radius();
    ds_rend_set.stop_label_font_size = s_rend_set.stop_label_font_size();
    ds_rend_set.bus_label_font_size = s_rend_set.bus_label_font_size();
    ds_rend_set.stop_label_offset = DeserializePoint(s_rend_set.stop_label_offset());
    ds_rend_set.bus_label_offset = DeserializePoint(s_rend_set.bus_label_offset());
    ds_rend_set.underlayer_color = DeserializeColor(s_rend_set.underlayer_color());
    ds_rend_set.underlayer_width = s_rend_set.underlayer_width();
    for(int i = 0; i < s_rend_set.color_palette_size(); ++i) {
        ds_rend_set.color_palette.push_back(DeserializeColor(s_rend_set.color_palette(i)));
    }
    return ds_rend_set;
}

transport_router::TransportRouterSettings Serializer::DeserializeRouterSettings(const transport_catalogue_serialize::RouterSettings& s_rout_set) {
    transport_router::TransportRouterSettings ds_rout_set;
    ds_rout_set.bus_wait_time = s_rout_set.wait_time();
    ds_rout_set.bus_velocity = s_rout_set.velocity();
    return ds_rout_set;
}




