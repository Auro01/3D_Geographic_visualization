#ifndef MAPDATA_HPP
#define MAPDATA_HPP

#include <fstream>
#include <sstream>
#include <regex>
#include <vector>

struct MapDataPoint {
    double latitude;
    double longitude;
    double value;

    MapDataPoint(double lat, double lng, double val);
};

MapDataPoint::MapDataPoint(double lat, double lng, double val) : 
    latitude(lat), longitude(lng), value(val) { }

void loadFile(char * name, vector<MapDataPoint> & data) {
    ifstream file;
    smatch matches;
    string line;
    regex expr("(\\d+(?:\\.\\d*)?),(\\d+(?:\\.\\d*)?),(\\d+(?:\\.\\d*)?)");

    file.open(name);

    while(getline(file, line)) {
        if(regex_match(line,matches, expr)) {
            double lat = stod(string(matches[1].first, matches[1].second));
            double lng = stod(string(matches[2].first, matches[2].second));
            double val = stod(string(matches[3].first, matches[3].second));
            data.emplace_back(lat, lng, val);
        }
    }
}

#endif