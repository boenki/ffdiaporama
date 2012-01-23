#-------------------------------------------------------------
# SYNTAXE IS :
#   QMAKE PREFIX=xxx ffDiaporama.pro
#       xxx could be /usr, /usr/local or /opt
#--------------------------------------------------------------

QT           += core gui xml network
CONFIG       += console
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

win32:RC_FILE = ffDiaporama.rc

LIBS		+=-lSDL                                                     # specific for SDL

OTHER_FILES += ffDiaporama.rc

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
            wgt_QCustomRuller.cpp \
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
            ../sharedfiles/DlgCheckConfig.cpp \
            ../sharedfiles/cSaveWindowPosition.cpp \
            ../sharedfiles/cBaseApplicationConfig.cpp \
            ../sharedfiles/cFilterTransformObject.cpp \
            ../sharedfiles/cFilterCorrectObject.cpp \
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
            ../sharedfiles/DlgInfoFile.cpp

HEADERS  += _GlobalDefines.h \
            _SoundDefinitions.h \
            _ImagesDefinitions.h \
            _ApplicationDefinitions.h \
            _Diaporama.h \
            wgt_QVideoPlayer.h \
            wgt_QCustomRuller.h \
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
            ../sharedfiles/DlgCheckConfig.h \
            ../sharedfiles/cSaveWindowPosition.h \
            ../sharedfiles/cBaseApplicationConfig.h \
            ../sharedfiles/cFilterTransformObject.h \
            ../sharedfiles/cFilterCorrectObject.h \
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
    #PostInstall.path     = /
    #PostInstall.extra    = chmod 755 $$PREFIX/bin/ffDiaporama & chmod 755 $$PREFIX/bin/ffDiaporamaMMFiler
    INSTALLS 		+= TARGET #PostInstall

    translation.path    = $$PREFIX/share/$$APPFOLDER/locale
    translation.files   = ../../locale/ffDiaporama_* \
                          ../../locale/qt_*
    INSTALLS 		+= translation

    background.path     = $$PREFIX/share/$$APPFOLDER/background
    background.files    = ../../background/*.*
    INSTALLS 		+= background

    mediaic.path        = $$PREFIX/share/$$APPFOLDER/img/MediaIcons
    mediaic.files       = ../../img/MediaIcons/*.*
    ic16x16.path        = $$PREFIX/share/$$APPFOLDER/img/MediaIcons/16x16
    ic16x16.files       = ../../img/MediaIcons/16x16/*.*
    ic32x32.path        = $$PREFIX/share/$$APPFOLDER/img/MediaIcons/32x32
    ic32x32.files       = ../../img/MediaIcons/32x32/*.*
    ic48x48.path        = $$PREFIX/share/$$APPFOLDER/img/MediaIcons/48x48
    ic48x48.files       = ../../img/MediaIcons/48x48/*.*
    ic100x100.path      = $$PREFIX/share/$$APPFOLDER/img/MediaIcons/100x100
    ic100x100.files     = ../../img/MediaIcons/100x100/*.*
    ic200x200.path      = $$PREFIX/share/$$APPFOLDER/img/MediaIcons/200x200
    ic200x200.files     = ../../img/MediaIcons/200x200/*.*
    INSTALLS 		+= ic16x16 ic32x32 ic48x48 ic100x100 ic200x200 mediaic

    img.path            = $$PREFIX/share/$$APPFOLDER/img
    img.files           = ../../img/*.*
    INSTALLS 		+= img

    tr_img.path         = $$PREFIX/share/$$APPFOLDER/transitions-img
    tr_img.files        = ../../transitions-img/*.*
    INSTALLS 		+= tr_img

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
    exists(/usr/share/icons/gnome/32x32/mimetypes) {
        iconfile.path   = $$PREFIX/share/icons/gnome/32x32/mimetypes
        iconfile.files  = ../../img/application-ffDiaporama.png
    }
    # install icon files for KDE systems
    exists(/usr/share/icons/default.kde4/32x32/mimetypes) {
        iconfile.path   = $$PREFIX/share/icons/default.kde4/32x32/mimetypes
        iconfile.files  = ../../img/application-ffDiaporama.png
    }
    INSTALLS 		+= iconfile

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
