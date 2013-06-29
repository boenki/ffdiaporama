#-------------------------------------------------------------
# SYNTAXE IS :
#   QMAKE PREFIX=xxx ffDiaporama.pro
#       xxx could be /usr, /usr/local or /opt
#--------------------------------------------------------------

greaterThan(QT_MAJOR_VERSION, 4): {
    # QT5 version
    QT += widgets concurrent
} else: {
    # QT4 version
    CONFIG += thread
}

QT          += core gui xml network svg
QMAKE_STRIP  = echo
APPFOLDER    = ffDiaporama
TARGET       = ffDiaporama
TEMPLATE     = app
DEFINES     += HAVE_CONFIG_H               # specific for TAGLib
DEFINES     += TAGLIB_STATIC               # specific for TAGLib

unix {
    HARDWARE_PLATFORM = $$system(uname -i)
    contains( HARDWARE_PLATFORM, x86_64 ) {
        DEFINES+=Q_OS_LINUX64
    } else {
        DEFINES+=Q_OS_LINUX32
    }
}

isEmpty(PREFIX) {
    PREFIX = /usr
}

#--------------------------------------------------------------
# DEFINES $$DESTDIR DIRECTORIES, COMMON INCLUDES AND COMMON LIBS
#--------------------------------------------------------------

DEFINES +=SHARE_DIR=\\\"$$PREFIX\\\"

unix {

    INCLUDEPATH += /usr/include/ffmpeg/                                     # Specific for Fedora

    exists(/usr/include/libavresample/avresample.h) {
        DEFINES += USELIBAVRESAMPLE
        LIBS    += -lavresample                                             #------ conditionnaly include libavresample
    } else:exists(/usr/include/ffmpeg/libswresample/swresample.h) {         #------ Specific for Fedora
        DEFINES += USELIBSWRESAMPLE
        LIBS    += -lswresample                                             #------ conditionnaly include libswresample
    } else:exists(/usr/include/libswresample/swresample.h) {
        DEFINES += USELIBSWRESAMPLE
        LIBS    += -lswresample                                             #------ conditionnaly include libswresample
    } else {
        LIBS    += -ltag                                                    #------ TAGlib is used only with LIBAV_08
    }

} else:win32 {

    CONFIG(debug, debug|release) {
        INCLUDEPATH += "..\\..\\..\\Win64\\include"
        LIBS        += -L"..\\..\\..\\Win64\\lib"
    }

    INCLUDEPATH += .                                                        #------ I don't know why, but windows need this !
    LIBS        += -lgdi32 -lkernel32 -luser32 -lshell32 -ladvapi32         #------ Windows GDI libs link

    DEFINES += USELIBSWRESAMPLE
    LIBS    += -lswresample                                             #------ conditionnaly include libswresample
}

#---- Libs for windows and linux
LIBS	    += -lSDL                                                        #------ SDL
LIBS        += -lexiv2                                                      #------ Exiv2
LIBS        += -lavformat -lavcodec -lavutil -lswscale -lavfilter           #------ libav

#--------------------------------------------------------------
# PROJECT FILES
#--------------------------------------------------------------

# Ressource files
win32:RC_FILE    += ../../ffDiaporama.rc
unix:OTHER_FILES += ../../ffDiaporama.rc
RESOURCES        += ../../RSCffDiaporama.qrc

# Translation files
TRANSLATIONS += ../../locale/ffDiaporama_fr.ts \
    ../../locale/ffDiaporama_it.ts \
    ../../locale/ffDiaporama_de.ts \
    ../../locale/ffDiaporama_nl.ts \
    ../../locale/ffDiaporama_es.ts \
    ../../locale/ffDiaporama_pt.ts \
    ../../locale/ffDiaporama_ru.ts \
    ../../locale/ffDiaporama_el.ts \
    ../../locale/ffDiaporama_cz.ts \
    ../../locale/ffDiaporama_zh_tw.ts

OTHER_FILES += ../../TODO-LIST.txt \          # Developpement file
    ../../BUILDVERSION.txt \                  # Developpement file
    ../../ffDiaporama-mime.xml \              # MIME definition of .ffd type
    ../../ffDiaporama.xml \                   # Default configuration options for ffDiaporama
    ../../Devices.xml \                       # Shared default configuration options for devices management
    ../../licences.txt \                      # Licence file
    ../../licences.rtf \                      # Licence file
    ../../authors.txt \                       # Authoring file for ffDiaporama
    ../../ffDiaporama.url \                   # URL file to be install on windows system
    ../../ffDiaporama.desktop                 # Desktop (menu icon) entry for ffDiaporama installed in /usr

# Source files
SOURCES +=  wgt_QVideoPlayer.cpp \
            mainwindow.cpp \
            main.cpp \
            cCustomSlideTable.cpp \
            DlgRenderVideo/DlgRenderVideo.cpp \
            DlgManageStyle/DlgManageStyle.cpp \
            DlgAbout/DlgAbout.cpp \
            DlgTransition/DlgTransitionProperties.cpp \
            DlgMusic/DlgMusicProperties.cpp \
            DlgBackground/DlgBackgroundProperties.cpp \
            DlgAppSettings/DlgManageDevices/DlgManageDevices.cpp \
            DlgAppSettings/DlgApplicationSettings.cpp \
            DlgImage/cImgInteractiveZone.cpp \
            DlgImage/DlgImageCorrection.cpp \
            DlgSlide/cCustomBlockTable.cpp \
            DlgSlide/DlgSlideProperties.cpp \
            DlgSlide/cInteractiveZone.cpp \
            DlgSlide/cCustomShotTable.cpp \
            DlgSlide/DlgRuler/DlgRulerDef.cpp \
    	    DlgText/cCustomTextEdit.cpp \
            DlgText/DlgTextEdit.cpp \
            DlgCheckConfig/DlgCheckConfig.cpp \
            DlgInfoFile/DlgInfoFile.cpp \
            DlgffDPjrProperties/DlgffDPjrProperties.cpp \
            DlgManageFavorite/DlgManageFavorite.cpp \
            DlgWorkingTask/DlgWorkingTask.cpp \
            DlgTransition/DlgTransitionDuration.cpp \
            DlgSlide/DlgSlideDuration.cpp \
            DlgFileExplorer/DlgFileExplorer.cpp \
            ../engine/_GlobalDefines.cpp \
            ../engine/QCustomRuller.cpp \
            ../engine/cSaveWindowPosition.cpp \
            ../engine/cBaseApplicationConfig.cpp \
            ../engine/cLuLoImageCache.cpp \
            ../engine/cDeviceModelDef.cpp \
            ../engine/cSoundBlockList.cpp \
            ../engine/cBaseMediaFile.cpp \
            ../engine/_SDL_Support.cpp \
            ../engine/_ImageFilters.cpp \
            ../engine/cBrushDefinition.cpp \
            ../engine/cCustomIcon.cpp \
            ../engine/cDriveList.cpp \
            ../engine/_Transition.cpp \
            ../engine/_SpeedWave.cpp \
            ../engine/_Shape.cpp \
            ../engine/cTextFrame.cpp \
            ../engine/_EncodeVideo.cpp \
            ../engine/_StyleDefinitions.cpp \
            ../engine/_Diaporama.cpp \
            ../CustomCtrl/_QCustomDialog.cpp \
            ../CustomCtrl/cCColorComboBox.cpp \
            ../CustomCtrl/cCBrushComboBox.cpp \
            ../CustomCtrl/cCSpeedWaveComboBox.cpp \
            ../CustomCtrl/cCGrdOrientationComboBox.cpp \
            ../CustomCtrl/cCFramingComboBox.cpp \
            ../CustomCtrl/cCShapeComboBox.cpp \
            ../CustomCtrl/cBackgroundComboBox.cpp \
            ../CustomCtrl/QCustomFolderTable.cpp \
            ../CustomCtrl/QCustomHorizSplitter.cpp \
            ../CustomCtrl/QCustomFolderTree.cpp \
            ../CustomCtrl/cCTexteFrameComboBox.cpp

# Header files
HEADERS  += wgt_QVideoPlayer.h \
            cCustomSlideTable.h \
            mainwindow.h \
            DlgRenderVideo/DlgRenderVideo.h \
            DlgManageStyle/DlgManageStyle.h \
            DlgAbout/DlgAbout.h \
            DlgTransition/DlgTransitionProperties.h \
            DlgMusic/DlgMusicProperties.h \
            DlgBackground/DlgBackgroundProperties.h \
            DlgAppSettings/DlgManageDevices/DlgManageDevices.h \
            DlgAppSettings/DlgApplicationSettings.h \
            DlgImage/cImgInteractiveZone.h \
            DlgImage/DlgImageCorrection.h \
            DlgSlide/DlgSlideProperties.h \
            DlgSlide/cCustomBlockTable.h \
            DlgSlide/cInteractiveZone.h \
            DlgSlide/cCustomShotTable.h \
            DlgSlide/DlgRuler/DlgRulerDef.h \
            DlgText/cCustomTextEdit.h \
            DlgText/DlgTextEdit.h \
            DlgCheckConfig/DlgCheckConfig.h \
            DlgInfoFile/DlgInfoFile.h \
            DlgffDPjrProperties/DlgffDPjrProperties.h \
            DlgManageFavorite/DlgManageFavorite.h \
            DlgWorkingTask/DlgWorkingTask.h \
            DlgTransition/DlgTransitionDuration.h \
            DlgSlide/DlgSlideDuration.h \
            DlgFileExplorer/DlgFileExplorer.h \
            ../engine/QCustomRuller.h \
            ../engine/cSaveWindowPosition.h \
            ../engine/cBaseApplicationConfig.h \
            ../engine/cLuLoImageCache.h \
            ../engine/cDeviceModelDef.h \
            ../engine/_GlobalDefines.h \
            ../engine/cSoundBlockList.h \
            ../engine/cBaseMediaFile.h \
            ../engine/_SDL_Support.h \
            ../engine/_ImageFilters.h \
            ../engine/cBrushDefinition.h \
            ../engine/cCustomIcon.h \
            ../engine/cDriveList.h \
            ../engine/_Transition.h \
            ../engine/_SpeedWave.h \
            ../engine/_Shape.h \
            ../engine/cTextFrame.h \
            ../engine/_EncodeVideo.h \
            ../engine/_StyleDefinitions.h \
            ../engine/_Diaporama.h \
            ../CustomCtrl/_QCustomDialog.h \
            ../CustomCtrl/cCColorComboBox.h \
            ../CustomCtrl/cCBrushComboBox.h \
            ../CustomCtrl/cCSpeedWaveComboBox.h \
            ../CustomCtrl/cCGrdOrientationComboBox.h \
            ../CustomCtrl/cCFramingComboBox.h \
            ../CustomCtrl/cCShapeComboBox.h \
            ../CustomCtrl/cBackgroundComboBox.h \
            ../CustomCtrl/QCustomFolderTable.h \
            ../CustomCtrl/QCustomHorizSplitter.h \
            ../CustomCtrl/QCustomFolderTree.h \
            ../CustomCtrl/cCTexteFrameComboBox.h

# Forms files
FORMS    += mainwindow.ui \
            wgt_QVideoPlayer.ui \
            DlgRenderVideo/DlgRenderVideo.ui \
            DlgManageStyle/DlgManageStyle.ui \
            DlgAbout/DlgAbout.ui \
            DlgTransition/DlgTransitionProperties.ui \
            DlgMusic/DlgMusicProperties.ui \
            DlgBackground/DlgBackgroundProperties.ui \
            DlgAppSettings/DlgManageDevices/DlgManageDevices.ui \
            DlgAppSettings/DlgApplicationSettings.ui \
            DlgImage/DlgImageCorrection.ui \
            DlgSlide/DlgSlideProperties.ui \
            DlgSlide/DlgRuler/DlgRulerDef.ui \
            DlgText/DlgTextEdit.ui \
            DlgCheckConfig/DlgCheckConfig.ui \
            DlgInfoFile/DlgInfoFile.ui \
            DlgffDPjrProperties/DlgffDPjrProperties.ui \ 
            DlgManageFavorite/DlgManageFavorite.ui \
            DlgWorkingTask/DlgWorkingTask.ui \
            DlgTransition/DlgTransitionDuration.ui \
            DlgSlide/DlgSlideDuration.ui \
            DlgFileExplorer/DlgFileExplorer.ui

#--------------------------------------------------------------
# INSTALLATION
#--------------------------------------------------------------
message("Install to : $$PREFIX")

TARGET.path         = $$PREFIX/bin
TARGET.files        = $$TARGET
INSTALLS 	    += TARGET

ico.path            = $$PREFIX/share/icons/hicolor/32x32/apps
ico.files           = ../../ffdiaporama.png
INSTALLS 	    += ico

desktop.path        = $$PREFIX/share/applications
desktop.files       = ../../ffDiaporama.desktop
INSTALLS 	    += desktop

mimefile.path       = $$PREFIX/share/mime/packages
mimefile.files      = ../../ffDiaporama-mime.xml
INSTALLS 	    += mimefile

translation.path    = $$PREFIX/share/$$APPFOLDER/locale
translation.files   = ../../locale/ffDiaporama_* \
                      ../../locale/qt_*
INSTALLS 	    += translation

background.path     = $$PREFIX/share/$$APPFOLDER/background
background.files    = ../../background/*.*
INSTALLS 	    += background

luma.path           = $$PREFIX/share/$$APPFOLDER/luma
luma.files          = ../../luma/*.*
luma_Bar.path       = $$PREFIX/share/$$APPFOLDER/luma/Bar
luma_Bar.files      = ../../luma/Bar/*.*
luma_Box.path       = $$PREFIX/share/$$APPFOLDER/luma/Box
luma_Box.files      = ../../luma/Box/*.*
luma_Center.path    = $$PREFIX/share/$$APPFOLDER/luma/Center
luma_Center.files   = ../../luma/Center/*.*
luma_Checker.path   = $$PREFIX/share/$$APPFOLDER/luma/Checker
luma_Checker.files  = ../../luma/Checker/*.*
luma_Clock.path     = $$PREFIX/share/$$APPFOLDER/luma/Clock
luma_Clock.files    = ../../luma/Clock/*.*
luma_Snake.path     = $$PREFIX/share/$$APPFOLDER/luma/Snake
luma_Snake.files    = ../../luma/Snake/*.*
INSTALLS 	    += luma_Bar luma_Box luma_Center luma_Checker luma_Clock luma_Snake luma

General.path        = $$PREFIX/share/$$APPFOLDER
General.files       = ../../*.xml ../../*.txt ../../*.rtf
INSTALLS            += General
