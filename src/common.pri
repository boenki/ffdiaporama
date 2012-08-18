#--------------------------------------------------
# COMMON PROJECT FILES TO DEFINE BUILD DIRECTORIES
# AND COMMON INCLUSIONS (INCLUDES AND LIBS)
#--------------------------------------------------

DEFINES +=SHARE_DIR=\\\"$$PREFIX\\\"

unix {
    CONFIG(release, debug|release) {
        DESTDIR         += ../../../build
        OBJECTS_DIR     += ../../../build/src/$$TARGET
        MOC_DIR         += ../../../build/src/$$TARGET
        RCC_DIR         += ../../../build/src/$$TARGET
        UI_DIR          += ../../../build/src/$$TARGET
        UI_HEADERS_DIR  += ../../../build/src/$$TARGET
        UI_SOURCES_DIR  += ../../../build/src/$$TARGET
    }
    CONFIG(debug, debug|release) {
        DESTDIR         += ../../../debugbuild
        OBJECTS_DIR     += ../../../debugbuild/src/$$TARGET
        MOC_DIR         += ../../../debugbuild/src/$$TARGET
        RCC_DIR         += ../../../debugbuild/src/$$TARGET
        UI_DIR          += ../../../debugbuild/src/$$TARGET
        UI_HEADERS_DIR  += ../../../debugbuild/src/$$TARGET
        UI_SOURCES_DIR  += ../../../debugbuild/src/$$TARGET
        INCLUDEPATH     += ../../../debugbuild/src/$$TARGET
    }

    INCLUDEPATH += /usr/include/ffmpeg/                                 # Specific for Fedora
    INCLUDEPATH += /usr/include/SDL
    LIBS        += -lexiv2                                              #------ Exiv2
}

win32 {
    CONFIG(release, debug|release) {
        DESTDIR         += ..\\..\\..\\winbuild
        OBJECTS_DIR     += ..\\..\\..\\winbuild\\src\\$$TARGET
        MOC_DIR         += ..\\..\\..\\winbuild\\src\\$$TARGET
        RCC_DIR         += ..\\..\\..\\winbuild\\src\\$$TARGET
        UI_DIR          += ..\\..\\..\\winbuild\\src\\$$TARGET
        UI_HEADERS_DIR  += ..\\..\\..\\winbuild\\src\\$$TARGET
        UI_SOURCES_DIR  += ..\\..\\..\\winbuild\\src\\$$TARGET
    }
    CONFIG(debug, debug|release) {
        DESTDIR         += ..\\..\\..\\windebugbuild
        OBJECTS_DIR     += ..\\..\\..\\windebugbuild\\src\\$$TARGET
        MOC_DIR         += ..\\..\\..\\windebugbuild\\src\\$$TARGET
        RCC_DIR         += ..\\..\\..\\windebugbuild\\src\\$$TARGET
        UI_DIR          += ..\\..\\..\\windebugbuild\\src\\$$TARGET
        UI_HEADERS_DIR  += ..\\..\\..\\windebugbuild\\src\\$$TARGET
        UI_SOURCES_DIR  += ..\\..\\..\\windebugbuild\\src\\$$TARGET
    }

    INCLUDEPATH += C:\\Qt\\libav-win32\\usr\\include                        #------ libav library path
    #INCLUDEPATH += C:\\Qt\\ffmpeg-win32-dev\\include                        #------ libav library path
    INCLUDEPATH += C:\\Qt\\SDL-1.2.15\\include\\SDL                         #------ SDL library path
    INCLUDEPATH += C:\\Qt\\ThirdLib\\include                                #------ TAGlib and EXIV2 library path
    INCLUDEPATH += .                                                        #------ I don't know why, but windows need this !

    LIBS        += -L"C:\\Qt\\libav-win32\\usr\\bin"                        #------ libav library path
    #LIBS        += -L"C:\\Qt\\ffmpeg-win32-dev\\lib"                        #------ libav library path
    LIBS        += -L"C:\\Qt\\SDL-1.2.15\\lib"                              #------ SDL library path
    LIBS        += -L"C:\\Qt\\ThirdLib\\lib"                                #------ library path (dynamic version) of third party libs (exiv2lib and taglib)
    LIBS        += -lgdi32 -lkernel32 -luser32 -lshell32 -ladvapi32         #------ Windows GDI libs link
    LIBS        += -llibexiv2                                               #------ Exiv2 (dynamic version, so not -llibexpat -lxmpsdk -lzlib1)
}

#---- Libs for windows and linux
LIBS        += -ltag                                                        #------ TAGlib
LIBS	    += -lSDL                                                        #------ SDL
LIBS        += -lavformat -lavcodec -lavutil -lswscale -lavfilter           #------ libav lib link
