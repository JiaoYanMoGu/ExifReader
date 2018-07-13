#ifndef CAMERAINTRINSICSPRIOR_H
#define CAMERAINTRINSICSPRIOR_H

#include <string>

template <int N>
class MetaData{
public:
    bool is_set = false;
    double value[N] = {0.0};
};


struct CameraInformationPrior
{
    int image_width = 0;
    int image_height = 0;

    MetaData<1> focal_length;
    MetaData<2> principal_point;

    //Position
    MetaData<3> position;
    MetaData<3> orientation;

    //gps information
    MetaData<1> latitude;
    MetaData<1> longitude;
    MetaData<1> altitude;
};

#endif // CAMERAINTRINSICSPRIOR_H
