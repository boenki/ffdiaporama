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
    DESTDIR     += ../build
    OBJECTS_DIR += ../build$$TARGET
    MOC_DIR     += ../build$$TARGET
}
win32 {
    DESTDIR     += ..\\..\\..\\winbuild
    OBJECTS_DIR += ..\\winbuild$$TARGET
    MOC_DIR     += ..\\winbuild$$TARGET
}
