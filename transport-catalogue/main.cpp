#include "json_builder.h"
#include "geo.h"
#include "json.h"
#include "json_reader.h"
#include "transport_catalogue.h"
#include "request_handler.h"
#include "transport_router.h"
#include "serialization.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string_view>

using namespace std;

void PrintUsage(std::ostream& stream = std::cerr) {
    stream << "Usage: transport_catalogue [make_base|process_requests]\n"sv;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        PrintUsage();
        return 1;
    }

    const std::string_view mode(argv[1]);

    if (mode == "make_base"sv) {

        // make base here
        transport_catalogue::TransportCatalogue t_cat;
        map_renderer::MapRenderer m_rend;
        transport_router::TransportRouter t_rout;
        request_handler::RequestHandler req_han;
        req_han.SetCatalogs(t_cat, m_rend, t_rout);
        req_han.LoadJSONinBase(cin);
        req_han.LoadBaseInTransportCatalogue();
        req_han.LoadRenderSettings();
        req_han.LoadRoutingSettings();
        Serializer::Serialize(req_han.GetPatch(), t_cat, m_rend.GetMapRendererSettings(), t_rout.GetTransportRouterSetting());
        
        
    } else if (mode == "process_requests"sv) {
         // process requests here
        
        request_handler::RequestHandler req_han;
        req_han.LoadJSONinBase(cin);
        auto deserializedDb = Serializer::Deserialize(req_han.GetPatch());
        transport_catalogue::TransportCatalogue t_cat = get<0>(deserializedDb);
        map_renderer::MapRenderer m_rend(get<1>(deserializedDb));
        transport_router::TransportRouter t_rout(get<2>(deserializedDb));
        req_han.SetCatalogs(t_cat, m_rend, t_rout);
        t_rout.BuildGraph(t_cat);
        json::Print(req_han.DoStatRequests(), cout);
       
    } else {
        PrintUsage();
        return 1;
    }
}