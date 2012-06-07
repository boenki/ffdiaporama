#-------------------------------------------------------------
# SYNTAXE IS :
#   QMAKE PREFIX=xxx ffDiaporama.pro
#       xxx could be /usr, /usr/local or /opt
#--------------------------------------------------------------

QT           += core gui xml network
#win32:CONFIG += console
CONFIG       += thread
QMAKE_STRIP  = echo
APPFOLDER    = ffDiaporama
TARGET       = ffDiaporama
TEMPLATE     = app
DEFINES      += HAVE_CONFIG_H               # specific for TAGLib
DEFINES      += TAGLIB_STATIC               # specific for TAGLib

isEmpty(PREFIX) {
    PREFIX = /usr
}

#--------------------------------------------------------------
# DEFINES BUILD DIRECTORIES, COMMON INCLUDES AND COMMON LIBS
#--------------------------------------------------------------
include(../common.pri)
#--------------------------------------------------------------

LIBS		+=-lSDL                                                     # specific for SDL

# Ressource files
win32:RC_FILE    += ../../ffDiaporama.rc
unix:OTHER_FILES += ../../ffDiaporama.rc
RESOURCES        += \
    ../../RSCffDiaporama.qrc

TRANSLATIONS += ../../locale/ffDiaporama_fr.ts \
    ../../locale/ffDiaporama_it.ts \
    ../../locale/ffDiaporama_de.ts \
    ../../locale/ffDiaporama_nl.ts \
    ../../locale/ffDiaporama_es.ts \
    ../../locale/ffDiaporama_pt.ts \
    ../../locale/ffDiaporama_ru.ts \
    ../../locale/ffDiaporama_el.ts \
    ../../locale/ffDiaporama_cz.ts

SOURCES +=  _ApplicationDefinitions.cpp \
            _ImagesDefinitions.cpp \
            _Diaporama.cpp \
            wgt_QVideoPlayer.cpp \
            cCustomComboBox.cpp \
            mainwindow.cpp \
            main.cpp \
            cCustomSlideTable.cpp \
            _StyleDefinitions.cpp \
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
            DlgVideo/DlgVideoEdit.cpp \
            DlgCheckConfig/DlgCheckConfig.cpp \
            DlgInfoFile/DlgInfoFile.cpp \
            DlgffDPjrProperties/DlgffDPjrProperties.cpp \
            ../engine/_GlobalDefines.cpp \
            ../engine/QCustomRuller.cpp \
            ../engine/cSaveWindowPosition.cpp \
            ../engine/cBaseApplicationConfig.cpp \
            ../engine/cFilterTransformObject.cpp \
            ../engine/cLuLoImageCache.cpp \
            ../engine/cDeviceModelDef.cpp \
            ../engine/cSoundBlockList.cpp \
            ../engine/cBaseMediaFile.cpp \
            ../engine/_SDL_Support.cpp \
            ../engine/_QCustomDialog.cpp \
            ../engine/QCustomFileInfoLabel.cpp \
            ../engine/cBrushDefinition.cpp \
            ../engine/cCustomIcon.cpp \
            ../fmt_filters/fmt_filters.cpp \ 
    ../engine/QCustomHorizSplitter.cpp \
    ../engine/QCustomFolderTree.cpp \
    ../engine/QCustomFolderTable.cpp \
    ../engine/cDriveList.cpp \
    DlgManageFavorite/DlgManageFavorite.cpp

HEADERS  += \
            _ImagesDefinitions.h \
            _ApplicationDefinitions.h \
            _Diaporama.h \
            _StyleDefinitions.h \
            wgt_QVideoPlayer.h \
            cCustomComboBox.h \
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
            DlgVideo/DlgVideoEdit.h \
            DlgCheckConfig/DlgCheckConfig.h \
            DlgInfoFile/DlgInfoFile.h \
            DlgffDPjrProperties/DlgffDPjrProperties.h \
            ../engine/QCustomRuller.h \
            ../engine/cSaveWindowPosition.h \
            ../engine/cBaseApplicationConfig.h \
            ../engine/cFilterTransformObject.h \
            ../engine/cLuLoImageCache.h \
            ../engine/cDeviceModelDef.h \
            ../engine/_GlobalDefines.h \
            ../engine/cSoundBlockList.h \
            ../engine/cBaseMediaFile.h \
            ../engine/_SDL_Support.h \
            ../engine/_QCustomDialog.h \
            ../engine/QCustomFileInfoLabel.h \
            ../engine/cBrushDefinition.h \
            ../engine/cCustomIcon.h \
            ../fmt_filters/fmt_filters.h \ 
    ../engine/QCustomHorizSplitter.h \
    ../engine/QCustomFolderTree.h \
    ../engine/QCustomFolderTable.h \
    ../engine/cDriveList.h \
    DlgManageFavorite/DlgManageFavorite.h

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
            DlgVideo/DlgVideoEdit.ui \
            DlgCheckConfig/DlgCheckConfig.ui \
            DlgInfoFile/DlgInfoFile.ui \
            DlgffDPjrProperties/DlgffDPjrProperties.ui \ 
    DlgManageFavorite/DlgManageFavorite.ui


# Installation on linux systems
unix {
    message("Install to : $$PREFIX")

    TARGET.path          = $$PREFIX/bin
    TARGET.files         = $$DESTDIR/$$TARGET
    INSTALLS 		+= TARGET

    translation.path    = $$PREFIX/share/$$APPFOLDER/locale
    translation.files   = ../../locale/ffDiaporama_* \
                          ../../locale/qt_*
    INSTALLS 		+= translation

    ico.path            = $$PREFIX/share/$$APPFOLDER
    ico.files           = ../../application-ffDiaporama.png \
                          ../../application-ffDiaporama.ico
    INSTALLS 		+= ico

    background.path     = $$PREFIX/share/$$APPFOLDER/background
    background.files    = ../../background/*.*
    INSTALLS 		+= background

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
    INSTALLS 		+= luma_Bar luma_Box luma_Center luma_Checker luma_Clock luma_Snake luma

    General.path        = $$PREFIX/share/$$APPFOLDER
    General.files       = ../../*.xml ../../*.txt ../../*.rtf ../../*.ffpreset
    INSTALLS 		+= General

    mimefile.path       = $$PREFIX/share/mime/packages
    mimefile.files      = ../../ffDiaporama-mime.xml
    INSTALLS 		+= mimefile

    # install desktop files /opt version
    contains(PREFIX,/opt) {
        desktop.path    = $$PREFIX/share/applications
        desktop.files   = ../../ffDiaporamaopt.desktop
    }
    # install PREFIX files /usr/local version
    contains(PREFIX,/usr/local) {
        desktop.path    = $$PREFIX/share/applications
        desktop.files   = ../../ffDiaporamalocal.desktop
    }
    # install desktop files /usr version
    !contains(PREFIX,/usr/local) : !contains(PREFIX,/opt) {
        desktop.path    = $$PREFIX/share/applications
        desktop.files   = ../../ffDiaporama.desktop
    }
    INSTALLS 		+= desktop

}
