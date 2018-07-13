#ifndef CAMERASPECS_H
#define CAMERASPECS_H

#include <string>
#include <unordered_map>
#include <vector>


// { make1 : ({ model1 : sensor-width in mm }, ...), ... }

typedef std::vector<std::pair<std::string, float>> camera_make_specs_t;
typedef std::unordered_map<std::string,camera_make_specs_t> camera_specs_t;

camera_specs_t InitializeCameraSpecs();

#endif // CAMERASPECS_H
