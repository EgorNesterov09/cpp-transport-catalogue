#pragma once

namespace geo {

struct Coordinates {
    Coordinates(double lat_, double lng_);
    double lat; // Широта
    double lng; // Долгота
    bool operator==(const Coordinates& other) const;
    bool operator!=(const Coordinates& other) const;
};

double ComputeDistance(Coordinates from, Coordinates to);

}  // namespace geo