TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH+=/usr/include/OpenImageIO
SOURCES += main.cpp \
    camera_database.cpp \
    camera_specs.cpp \
    exif_reader.cpp

HEADERS += \
    camera_information_prior.h \
    exif_reader.h \
    camera_specs.h \
    camera_database.h


LIBS+=-lOpenImageIO
