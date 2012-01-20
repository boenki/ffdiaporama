#-------------------------------------------------------------
# SYNTAXE IS :
#   QMAKE INSTALL_ROOT=xxx ffDiaporama.pro
#
#   REMARKS :
#       - On some systems, use QMAKE-QT4 instead of QMAKE
#       - INSTALL_ROOT could only be one of this 3 values:
#            /usr
#            /usr/local
#            /opt
#       - if INSTALL_ROOT not defined, then use /usr
#--------------------------------------------------------------

QT           += core gui xml network
CONFIG       += console
QMAKE_STRIP  = echo
APPFOLDER    = ffDiaporama
TARGET       = ffDiaporama
TEMPLATE     = app
DEFINES      += HAVE_CONFIG_H               # specific for TAGLib
DEFINES      += TAGLIB_STATIC               # specific for TAGLib

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


