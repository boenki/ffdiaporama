#-------------------------------------------------
#
# Project created by QtCreator 2011-11-20T09:14:58
#
#-------------------------------------------------

TARGET = ffDiaporamaStart
#CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += main.cpp

unix {
    DESTDIR     += ../../../build/ffDiaporamaStart/
    OBJECTS_DIR += ../../../build/ffDiaporamaStart/
    MOC_DIR     += ../../../build/ffDiaporamaStart/
    UI_DIR      += ../../../build/ffDiaporamaStart/
}
win32 {
    DESTDIR     += ../../../winbuild/ffDiaporamaStart/
    OBJECTS_DIR += ../../../winbuild/ffDiaporamaStart/
    MOC_DIR     += ../../../winbuild/ffDiaporamaStart/
    UI_DIR      += ../../../winbuild/ffDiaporamaStart/
}