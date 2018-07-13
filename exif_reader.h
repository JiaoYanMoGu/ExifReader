#ifndef EXIFREADER_H
#define EXIFREADER_H

#include <OpenImageIO/imageio.h>

#include <string>
#include <unordered_map>
#include <cmath>

#include "camera_specs.h"
#include "camera_information_prior.h"

class ExifReader
{
public:
    ExifReader();
    bool ExtractEXIFMetaData(
            const std::string& image_file,
            CameraInformationPrior* camera_intrinsics_prior) const;

private:
    inline bool IsValidFlocalLength(const double length) const
    {
        return std::isfinite(length) && length > 0;
    }


private:
    bool SetFocalLengthFrom35mmFilm(
            const OpenImageIO::ImageSpec& image_spec,
            CameraInformationPrior* camera_information_prior) const;

    bool SetFocalLengthFromExif(
        const OpenImageIO::ImageSpec& image_spec,
        CameraInformationPrior* camera_information_prior) const;

    bool SetFocalLengthFromSensorDatabase(
        const OpenImageIO::ImageSpec& image_spec,
        CameraInformationPrior* camera_information_prior) const;

    bool SetFocalLengthFromDefault(
        const OpenImageIO::ImageSpec& image_spec,
        CameraInformationPrior* camera_information_prior) const;

};

#endif // EXIFREADER_H
