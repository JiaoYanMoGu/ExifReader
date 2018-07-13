#include "exif_reader.h"
#include "camera_database.h"

#include <cmath>
#include <string>
#include <algorithm>

#include <OpenImageIO/imageio.h>
#include <OpenImageIO/imagebuf.h>


#include <iostream>


ExifReader::ExifReader() {}

bool ExifReader::ExtractEXIFMetaData(
        const std::string& image_file,
        CameraInformationPrior* camera_informations_prior) const
{
    OpenImageIO::ImageBuf image(image_file);
    OpenImageIO::ImageSpec image_spec(image.spec());

    camera_informations_prior->image_width = image_spec.width;
    camera_informations_prior->image_height = image_spec.height;


    //Set principal point
    camera_informations_prior->principal_point.is_set = true;
    camera_informations_prior->principal_point.value[0] =
            image_spec.width / 2.0;
    camera_informations_prior->principal_point.value[1] =
            image_spec.height / 2.0;


    if(!SetFocalLengthFrom35mmFilm(image_spec,camera_informations_prior)&&
       !SetFocalLengthFromExif(image_spec,camera_informations_prior)&&
       !SetFocalLengthFromSensorDatabase(image_spec,camera_informations_prior)){
        return true;
    }
    camera_informations_prior->focal_length.is_set  = true;

    //Set GPS latitude
    const OpenImageIO::ImageIOParameter* latitude =
            image_spec.find_attribute("GPS:Latitude");
    if(latitude != nullptr){
        camera_informations_prior->latitude.is_set = true;
        const float* latitude_val =
            reinterpret_cast<const float*>(latitude->data());
        camera_informations_prior->latitude.value[0] =
            latitude_val[0] + latitude_val[1] / 60.0 + latitude_val[2] / 3600.0;
        // Adjust the sign of the latitude depending on if the coordinates given
        // were north or south.
        const std::string north_or_south =
            image_spec.get_string_attribute("GPS:LatitudeRef");
        if (north_or_south == "S") {
          camera_informations_prior->longitude.value[0] *= -1.0;
        }
    }

    // Set GPS longitude.
    const OpenImageIO::ImageIOParameter* longitude =
        image_spec.find_attribute("GPS:Longitude");
    if (longitude != nullptr) {
      camera_informations_prior->longitude.is_set = true;
      const float* longitude_val =
          reinterpret_cast<const float*>(longitude->data());
      camera_informations_prior->longitude.value[0] =
          longitude_val[0] + longitude_val[1] / 60.0 + longitude_val[2] / 3600.0;

      // Adjust the sign of the longitude depending on if the coordinates given
      // were east or west.
      const std::string east_or_west =
          image_spec.get_string_attribute("GPS:LongitudeRef");
      if (east_or_west == "W") {
        camera_informations_prior->longitude.value[0] *= -1.0;
      }
    }


    // Set GSP altitude.
    const OpenImageIO::ImageIOParameter* altitude =
        image_spec.find_attribute("GPS:Altitude");
    if (altitude != nullptr) {
      camera_informations_prior->altitude.is_set = true;
      camera_informations_prior->altitude.value[0] =
          image_spec.get_float_attribute("GPS:Altitude");
    }
    return true;
}


bool ExifReader::SetFocalLengthFrom35mmFilm(
        const OpenImageIO::ImageSpec &image_spec,
        CameraInformationPrior *camera_information_prior) const
{
    const float exif_focal_length35mm = image_spec.get_float_attribute("Exif:FocalLengthIn35mmFilm");
    if(exif_focal_length35mm < 1e-2){
        return false;
    }
    const int max_image_size = std::max(image_spec.width,image_spec.height);
    const float focal_length = exif_focal_length35mm / 35.0 * max_image_size;
    camera_information_prior->focal_length.value[0] = focal_length;
    return IsValidFlocalLength(focal_length);
}


//Theia Way1
//Use exif f/mm and FocalPlaneXResolution

bool ExifReader::SetFocalLengthFromExif(
    const OpenImageIO::ImageSpec& image_spec,
    CameraInformationPrior* camera_information_prior) const
{
    static const float kMinFocalLength = 1e-2;

    const float exif_focal_length =
      image_spec.get_float_attribute("Exif:FocalLength", kMinFocalLength);
    const float focal_plane_x_resolution =
        image_spec.get_float_attribute("Exif:FocalPlaneXResolution");
    const float focal_plane_y_resolution =
        image_spec.get_float_attribute("Exif:FocalPlaneYResolution");
    const int focal_plane_resolution_unit =
        image_spec.get_int_attribute("Exif:FocalPlaneResolutionUnit");

    if(exif_focal_length <= kMinFocalLength || focal_plane_x_resolution <= 0.0 ||
            focal_plane_y_resolution <=0.0){
        return false;
    }

    double ccd_resolution_units = 1.0;

    switch (focal_plane_resolution_unit) {
    case 2:
       //Convert inches to mm
        ccd_resolution_units = 25.4;
        break;
    case 3:
        //Convert centimeters to mm
        ccd_resolution_units = 10.0;
        break;
    case 4:
        //Already in mm
        break;
    case 5:
        //Convert micrometers to mm
        ccd_resolution_units = 1.0 / 1000.0;
        break;
    default:
        return false;
        break;
    }

    //Get the ccd dimensions in mm
    const int exif_width = image_spec.get_int_attribute("Exif:PixelXDimension");
    const int exif_height = image_spec.get_int_attribute("Exif:PixelYDimension");

    const double ccd_width =
        exif_width / (focal_plane_x_resolution / ccd_resolution_units);
    const double ccd_height =
        exif_height / (focal_plane_y_resolution / ccd_resolution_units);

    const double focal_length_x =
        exif_focal_length * image_spec.width / ccd_width;
    const double focal_length_y =
        exif_focal_length * image_spec.height / ccd_height;

    const double focal_length = (focal_length_x + focal_length_y) / 2.0;
    camera_information_prior->focal_length.value[0]=focal_length;

    return IsValidFlocalLength(focal_length);

}

//Theia Way2:
//Use Datbase sensor width and f/mm
bool ExifReader::SetFocalLengthFromSensorDatabase(
    const OpenImageIO::ImageSpec& image_spec,
    CameraInformationPrior* camera_information_prior) const
{
    const int max_image_size = std::max(image_spec.width,image_spec.height);
    const float exif_focal_length = image_spec.get_float_attribute("Exif::FocalLength");

    const std::string camera_make = image_spec.get_string_attribute("Make");
    const std::string camera_model = image_spec.get_string_attribute("Model");

    //First,try to look up just the model
    CameraDatabase database;
    double sensor_width = 0.0;
    double focal_length = 0.0;

    if(database.QuerySensorWidth(camera_make,camera_model,&sensor_width)){
        focal_length= exif_focal_length / sensor_width*max_image_size;
        camera_information_prior->focal_length.value[0] = focal_length;
        return IsValidFlocalLength(focal_length);
    }
    return false;
}



bool ExifReader::SetFocalLengthFromDefault(
    const OpenImageIO::ImageSpec& image_spec,
    CameraInformationPrior* camera_information_prior) const
{
    const int max_image_size = std::max(image_spec.width,image_spec.height);
    float focal_length = max_image_size * 1.25;
    camera_information_prior->focal_length.value[0] = focal_length;
    return IsValidFlocalLength(focal_length);
}
