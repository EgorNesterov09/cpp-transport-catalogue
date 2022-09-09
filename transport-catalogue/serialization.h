#pragma once
#include "json_reader.h"
#include "request_handler.h"
#include "transport_catalogue.h"
#include "transport_catalogue.pb.h"
#include "map_renderer.h"
#include "map_renderer.pb.h"
#include "graph.pb.h"
#include <filesystem>
#include <tuple>


class Serializer {
public:
    static void Serialize(std::filesystem::path file, transport_catalogue::TransportCatalogue& catalog, map_renderer::MapRendererSettings renderer_settings, transport_router::TransportRouterSettings router_settings);

    static std::tuple<transport_catalogue::TransportCatalogue, map_renderer::MapRendererSettings, transport_router::TransportRouterSettings> Deserialize (const std::filesystem::path& file);

private:
    static transport_catalogue_serialize::RGB SerializeRGB(const svg::Rgb &rgb_color);
    static transport_catalogue_serialize::RGBA SerializeRGBA(const svg::Rgba &rgba_color);
    static transport_catalogue_serialize::Color SerializeColor(const svg::Color &color);
    static transport_catalogue_serialize::Point SerializePoint(const svg::Point &point);
    static transport_catalogue_serialize::RendererSettings SerializeRendererSettings(const map_renderer::MapRendererSettings& renderer_settings);
    static transport_catalogue_serialize::RouterSettings SerializeRouterSettings(const transport_router::TransportRouterSettings& router_settings);
    
    static svg::Rgb DeserializeRGB(const transport_catalogue_serialize::RGB& s_rgb_color);
    static svg::Rgba DeserializeRGBA(const transport_catalogue_serialize::RGBA& s_rgba_color);
    static svg::Color DeserializeColor(const transport_catalogue_serialize::Color& s_color);
    static svg::Point DeserializePoint(const transport_catalogue_serialize::Point& s_point);
    static map_renderer::MapRendererSettings DeserializeRendererSettings(const transport_catalogue_serialize::RendererSettings& s_rend_set);
    static transport_router::TransportRouterSettings DeserializeRouterSettings(const transport_catalogue_serialize::RouterSettings& s_rout_set);
};