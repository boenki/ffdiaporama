#--------------------------------------------------
# COMMON PROJECT FILES TO DEFINE BUILD DIRECTORIES
# AND COMMON INCLUSIONS (INCLUDES AND LIBS)
#--------------------------------------------------

unix {
    INCLUDEPATH += /usr/include/ffmpeg/                                 # Specific for Fedora
    LIBS        += -lavformat -lavcodec -lavutil -lswscale
    CONFIG(release, debug|release) {
        DESTDIR     += ../../../build
        OBJECTS_DIR += ../../../build/src/$$TARGET
        MOC_DIR     += ../../../build/src/$$TARGET
        UI_DIR      += ../../../build/src/$$TARGET
    }
    CONFIG(debug, debug|release) {
        DESTDIR     += ../../../debugbuild
        OBJECTS_DIR += ../../../debugbuild/src/$$TARGET
        MOC_DIR     += ../../../debugbuild/src/$$TARGET
        UI_DIR      += ../../../debugbuild/src/$$TARGET
    }
}
win32 {
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

    INCLUDEPATH += C:\\Qt\\ffmpeg-win32-dev\\include                    #------ ffmpeg library path
    INCLUDEPATH += C:\\Qt\\SDL-1.2.14\\include                          #------ SDL library path
    INCLUDEPATH += .                                                    #------ I don't know why, but windows need this !

    LIBS        +=  -L"C:\\Qt\\ffmpeg-win32-dev\\lib"    \              #------ ffmpeg library path
                    -L"C:\\Qt\\SDL-1.2.14\\lib" \                       #------ SDL library path
                   -lavformat -lavcodec -lavutil -lswscale -lgdi32
}
