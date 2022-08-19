#define _USE_MATH_DEFINES
#include "geo.h"

#include <cmath>

namespace geo {
    
Coordinates::Coordinates(double lat_, double lng_) : lat(lat_), lng(lng_) {}
    
bool Coordinates::operator==(const Coordinates& other) const {
        return lat == other.lat && lng == other.lng;
    }
    
bool Coordinates::operator!=(const Coordinates& other) const {
        return !(*this == other);
    }    

double ComputeDistance(Coordinates from, Coordinates to) {
    using namespace std;
    if (from == to) {
        return 0;
    }
    const int earth_radius = 6371000;
    static const double dr = M_PI / 180.;
    return acos(sin(from.lat * dr) * sin(to.lat * dr)
                + cos(from.lat * dr) * cos(to.lat * dr) * cos(abs(from.lng - to.lng) * dr))
        * earth_radius;
}

}  // namespace geo