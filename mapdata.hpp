#ifndef MAPDATA_HPP
#define MAPDATA_HPP

struct MapDataPoint {
    double latitude;
    double longitude;
    double value;

    MapDataPoint();
};

MapDataPoint::MapDataPoint(double lat, double lng, double val) : 
    latitude(lat), longitude(lng), value(val)
{ }

#endif