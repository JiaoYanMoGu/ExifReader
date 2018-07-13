#include "camera_database.h"
#include <algorithm>


const camera_specs_t CameraDatabase::specs_ = InitializeCameraSpecs();
CameraDatabase::CameraDatabase() {}


bool CameraDatabase::QuerySensorWidth(const std::string& make, const std::string& model, double* sensor_width)
{
    std::string cleaned_make = make;
    std::string cleaned_model = model;

    StringReplace(cleaned_make, " ", "");
    StringReplace(cleaned_model, " ", "");
    StringReplace(cleaned_make, "-", "");
    StringReplace(cleaned_model, "-", "");
    StringToLower(&cleaned_make);
    StringToLower(&cleaned_model);

    cleaned_model = StringReplace(cleaned_model, cleaned_make, "");

    // Check if cleaned_make exists in database: Test whether EXIF string is
    // substring of database entry and vice versa.
    size_t spec_matches = 0;
    for (const auto& make_elem : specs_) {
      if (StringContains(cleaned_make, make_elem.first) ||
          StringContains(make_elem.first, cleaned_make)) {
        for (const auto& model_elem : make_elem.second) {
          if (StringContains(cleaned_model, model_elem.first) ||
              StringContains(model_elem.first, cleaned_model)) {
            *sensor_width = model_elem.second;
            if (cleaned_model == model_elem.first) {
              // Model exactly matches, return immediately.
              return true;
            }
            spec_matches += 1;
            if (spec_matches > 1) {
              break;
            }
          }
        }
      }
    }

    // Only return unique results, if model does not exactly match.
    return spec_matches == 1;

}


std::string CameraDatabase::StringReplace(const std::string& str, const std::string& old_str,
                          const std::string& new_str) {
  if (old_str.empty()) {
    return str;
  }
  size_t position = 0;
  std::string mod_str = str;
  while ((position = mod_str.find(old_str, position)) != std::string::npos) {
    mod_str.replace(position, old_str.size(), new_str);
    position += new_str.size();
  }
  return mod_str;
}


void CameraDatabase::StringToLower(std::string* str) {
  std::transform(str->begin(), str->end(), str->begin(), ::tolower);
}

bool CameraDatabase::StringContains(const std::string& str, const std::string& sub_str) {
  return str.find(sub_str) != std::string::npos;
}

