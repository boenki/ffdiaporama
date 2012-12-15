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
        DEFINES         += DEBUG_MODE
    }

    INCLUDEPATH += /usr/include/ffmpeg/                                     # Specific for Fedora
    INCLUDEPATH += /usr/include/SDL
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
        DEFINES         += DEBUG_MODE
        INCLUDEPATH     += "/F/Dev/ffDiaporama/trunk/msysenv/32bit/include"
        INCLUDEPATH     += "/F/Dev/ffDiaporama/trunk/msysenv/32bit/sse2/include"
        LIBS            += -L"/F/Dev/ffDiaporama/trunk/msysenv/32bit/lib"
        LIBS            += -L"/F/Dev/ffDiaporama/trunk/msysenv/32bit/sse2/lib"
    }

    INCLUDEPATH += .                                                        #------ I don't know why, but windows need this !
    LIBS        += -lgdi32 -lkernel32 -luser32 -lshell32 -ladvapi32         #------ Windows GDI libs link
    INCLUDEPATH += "/C/Program Files (x86)/Microsoft Visual Studio 10.0/VC/include"
    #LIBS        += -lavresample                                             #------ LibAV swresample
}

#---- Libs for windows and linux
LIBS        += -ltag                                                        #------ TAGlib
LIBS	    += -lSDL                                                        #------ SDL
LIBS        += -lexiv2                                                      #------ Exiv2
LIBS        += -lqimageblitz                                                #------ QImageBlitz
LIBS        += -lavformat -lavcodec -lavutil -lswscale -lavfilter           #------ libav lib link
