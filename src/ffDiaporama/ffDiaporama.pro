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
RESOURCES        += ../../RSCffDiaporama.qrc

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
            _SoundDefinitions.cpp \
            _ImagesDefinitions.cpp \
            _Diaporama.cpp \
            wgt_QVideoPlayer.cpp \
            wgt_QCustomThumbnails.cpp \
            DlgApplicationSettings.cpp \
            DlgBackgroundProperties.cpp \
            cCustomGraphicsRectItem.cpp \
            cCustomComboBox.cpp \
            DlgMusicProperties.cpp \
            DlgRenderVideo.cpp \
            DlgTransitionProperties.cpp \
            DlgAbout.cpp \
            mainwindow.cpp \
            main.cpp \
            DlgSlideProperties.cpp \
            DlgImageCorrection.cpp \
            DlgVideoEdit.cpp \
            DlgTextEdit.cpp \
            cCustomTableWidget.cpp \
            DlgManageStyle.cpp \
            _StyleDefinitions.cpp \
            ../sharedfiles/_GlobalDefines.cpp \
            ../sharedfiles/QCustomRuller.cpp \
            ../sharedfiles/DlgCheckConfig.cpp \
            ../sharedfiles/cSaveWindowPosition.cpp \
            ../sharedfiles/cBaseApplicationConfig.cpp \
            ../sharedfiles/cFilterTransformObject.cpp \
            ../fmt_filters/fmt_filters.cpp \
            ../sharedfiles/cLuLoImageCache.cpp \
            ../sharedfiles/cDeviceModelDef.cpp \
            ../sharedfiles/cSoundBlockList.cpp \
            ../sharedfiles/cBaseMediaFile.cpp \
            ../sharedfiles/_SDL_Support.cpp \
            ../sharedfiles/_QCustomDialog.cpp \
            ../sharedfiles/DlgManageDevices.cpp \
            ../sharedfiles/QCustomFileInfoLabel.cpp \
            ../sharedfiles/DlgffDPjrProperties.cpp \
            ../sharedfiles/cBrushDefinition.cpp \
            ../sharedfiles/cCustomIcon.cpp \
            ../sharedfiles/DlgInfoFile.cpp

HEADERS  += \
            _SoundDefinitions.h \
            _ImagesDefinitions.h \
            _ApplicationDefinitions.h \
            _Diaporama.h \
            wgt_QVideoPlayer.h \
            wgt_QCustomThumbnails.h \
            DlgApplicationSettings.h \
            DlgBackgroundProperties.h \
            cCustomGraphicsRectItem.h \
            cCustomComboBox.h \
            DlgMusicProperties.h \
            DlgRenderVideo.h \
            DlgTransitionProperties.h \
            DlgAbout.h \
            mainwindow.h \
            DlgSlideProperties.h \
            DlgImageCorrection.h \
            DlgVideoEdit.h \
            DlgTextEdit.h \
            cCustomTableWidget.h \
            DlgManageStyle.h \
            _StyleDefinitions.h \
            ../sharedfiles/QCustomRuller.h \
            ../sharedfiles/DlgCheckConfig.h \
            ../sharedfiles/cSaveWindowPosition.h \
            ../sharedfiles/cBaseApplicationConfig.h \
            ../sharedfiles/cFilterTransformObject.h \
            ../fmt_filters/fmt_filters.h \
            ../sharedfiles/cLuLoImageCache.h \
            ../sharedfiles/cDeviceModelDef.h \
            ../sharedfiles/_GlobalDefines.h \
            ../sharedfiles/cSoundBlockList.h \
            ../sharedfiles/cBaseMediaFile.h \
            ../sharedfiles/_SDL_Support.h \
            ../sharedfiles/_QCustomDialog.h \
            ../sharedfiles/DlgManageDevices.h \
            ../sharedfiles/QCustomFileInfoLabel.h \
            ../sharedfiles/DlgffDPjrProperties.h \
            ../sharedfiles/cBrushDefinition.h \
            ../sharedfiles/cCustomIcon.h \
            ../sharedfiles/DlgInfoFile.h

FORMS    += mainwindow.ui \
            wgt_QVideoPlayer.ui \
            DlgApplicationSettings.ui \
            DlgBackgroundProperties.ui \
            DlgMusicProperties.ui \
            DlgRenderVideo.ui \
            DlgTransitionProperties.ui \
            DlgAbout.ui \
            DlgSlideProperties.ui \
            DlgImageCorrection.ui \
            DlgVideoEdit.ui \
            DlgTextEdit.ui \
            DlgManageStyle.ui \
            ../sharedfiles/DlgCheckConfig.ui \
            ../sharedfiles/DlgManageDevices.ui \
            ../sharedfiles/DlgffDPjrProperties.ui \
            ../sharedfiles/DlgInfoFile.ui


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

    # install icon files for GNOME systems
    #exists(/usr/share/icons/gnome/32x32/mimetypes) {
    #    iconfile.path   = $$PREFIX/share/icons/gnome/32x32/mimetypes
    #    iconfile.files  = ../../application-ffDiaporama.png
    #}
    # install icon files for KDE systems
    #exists(/usr/share/icons/default.kde4/32x32/mimetypes) {
    #    iconfile.path   = $$PREFIX/share/icons/default.kde4/32x32/mimetypes
    #    iconfile.files  = ../../application-ffDiaporama.png
    #}
    #INSTALLS 		+= iconfile

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
