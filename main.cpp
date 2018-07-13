#include <iostream>
#include "exif_reader.h"
#include <OpenImageIO/imageio.h>
#include <OpenImageIO/imagebuf.h>

using namespace std;


int main(int argc, char *argv[])
{
    if(argc < 2 )
    {
        cout<<"Input Image Names"<<endl;
        return 0;
    }
    CameraInformationPrior p;
    ExifReader reader;
    reader.ExtractEXIFMetaData(argv[1],&p);
    cout<<"Print Exif Parameters: "<<'\n';
    cout<<"ImageWidth: "<<p.image_width<<'\n';
    cout<<"ImageHeight: "<<p.image_height<<'\n';
    cout<<"FocalLength: "<<p.focal_length.is_set<<" "<<p.focal_length.value[0]<<'\n';
    cout<<"PrincipalPoint: "<<p.principal_point.is_set<<" "<<p.principal_point.value[0]<<" "
       <<p.principal_point.value[1]<<'\n';
    cout<<"Position: "<<p.position.is_set<<" "<<p.position.value[0]<<" "
       <<p.position.value[1]<<" "<<p.position.value[2]<<'\n';
    cout<<"Orientation: "<<p.orientation.is_set<<" "<<p.orientation.value[0]<<" "
       <<p.orientation.value[1]<<" "<<p.orientation.value[2]<<" "<<p.orientation.value[3]<<'\n';

    cout<<"Latitude: "<<p.latitude.is_set<<" "<<p.latitude.value[0]<<'\n';
    cout<<"Longitude: "<<p.longitude.is_set<<" "<<p.longitude.value[0]<<'\n';
    cout<<"Altitude: "<<p.altitude.is_set<<" "<<p.altitude.value[0]<<'\n';

    return 0;
}
