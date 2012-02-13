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

    INCLUDEPATH += I:\\Qt\\ffmpeg-win32-dev\\include                    #------ ffmpeg library path
    INCLUDEPATH += I:\\Qt\\SDL-1.2.14\\include                          #------ SDL library path
    INCLUDEPATH += I:\\Qt\\ThirdLib\\include                            #------ Exiv2 and TAGlib library path
    INCLUDEPATH += .                                                    #------ I don't know why, but windows need this !

    LIBS        += -L"I:\\Qt\\ffmpeg-win32-dev\\lib"                    #------ ffmpeg library path
    LIBS        += -L"I:\\Qt\\SDL-1.2.14\\lib"                          #------ SDL library path
    LIBS        += -L"I:\\Qt\\ThirdLib\\lib"                            #------ Exiv2 and TAGlib library path
    LIBS        += -lgdi32 -lkernel32                                   #------ Windows GDI lib link
}

#---- Libs for windows and linux
LIBS        += -lavformat -lavcodec -lavutil -lswscale                  #------ ffmpeg lib link
unix:LIBS   += -lexiv2                                                  #------ Exiv2 [and TAGlib]Linux only]
LIBS        += -ltag                                                    #------ TAGlib
