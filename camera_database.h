#ifndef CAMERADATABASE_H
#define CAMERADATABASE_H

#include <string>
#include "camera_specs.h"
class CameraDatabase
{
    //Database that contains sensor widths for many cameras, which is useful
    // to automatically extract the focal length if EXIF information is incomplete.
public:
    CameraDatabase();
    size_t NumEntries() const { return specs_.size(); }
    bool QuerySensorWidth(const std::string& make, const std::string& model,
                          double* sensor_width);

private:
    std::string StringReplace(const std::string& str, const std::string& old_str,
                              const std::string& new_str);
    void StringToLower(std::string* str);
    bool StringContains(const std::string& str, const std::string& sub_str);

private:
    static const camera_specs_t specs_;
};

#endif // CAMERADATABASE_H
