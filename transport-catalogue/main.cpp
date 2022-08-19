#include "json_builder.h"
#include<iomanip>
#include "geo.h"
#include "json.h"
#include "json_reader.h"
#include "transport_catalogue.h"
#include "request_handler.h"
#include "transport_router.h"
#include <iostream>


using namespace std;

ostream& operator << (ostream& out, Stop stop) {
    out << "Stop "s << stop.name << ": "s << std::fixed << std::setprecision(6) << stop.latitude << ", "s << std::fixed << std::setprecision(6) << stop.longitude;
    return out;
}
ostream& operator << (ostream& out, Bus bus) {
    out << "Bus "s << bus.name << ": "s; 
    for (auto stop : bus.stops) {
        out << *stop << ", ";
    }
    return out;
}
using namespace svg;

int main() {
    
    TransportCatalogue t_cat;
    map_renderer::MapRenderer m_rend;
    transport_router::TransportRouter t_rout;
    RequestHandler req_han (t_cat, m_rend, t_rout);
    req_han.LoadJSONinBase(cin);
    req_han.LoadBaseInTransportCatalogue();
    req_han.LoadRenderSettings();
    req_han.LoadRoutingSettings();
    //PrintValue(req_han.DoStatRequests(), cout); 
    json::Print(req_han.DoStatRequests(), cout); 

    return 0;
}