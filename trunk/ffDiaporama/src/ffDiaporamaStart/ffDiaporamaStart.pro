#-------------------------------------------------
# THIS APPLICATION IS FOR WINDOWS ONLY
#-------------------------------------------------

win32 {
    TEMPLATE    = app
    SOURCES     += main.cpp
    TARGET      = ffDiaporamaStart
    CONFIG      -= app_bundle

    CONFIG(release, debug|release) {
        DESTDIR     += ..\\..\\..\\winbuild
        OBJECTS_DIR += ..\\..\\..\\winbuild\\src\\$$TARGET
        MOC_DIR     += ..\\..\\..\\winbuild\\src\\$$TARGET
        UI_DIR      += ..\\..\\..\\winbuild\\src\\$$TARGET
    }
    CONFIG(debug, debug|release) {
        DESTDIR     += ..\\..\\..\\windebugbuild
        OBJECTS_DIR += ..\\..\\..\\windebugbuild\\src\\$$TARGET
        MOC_DIR     += ..\\..\\..\\windebugbuild\\src\\$$TARGET
        UI_DIR      += ..\\..\\..\\windebugbuild\\src\\$$TARGET
    }
}
