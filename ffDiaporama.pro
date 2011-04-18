#-------------------------------------------------
#
# Project created by QtCreator 2010-12-30T08:14:00
#
#-------------------------------------------------

QT          += core gui xml
QMAKE_STRIP = echo
PREFIX      = /usr

TARGETAPP   = ffDiaporama
TARGET      = $$TARGETAPP
TEMPLATE    = app
LIBS        += -lavformat -lavcodec -lavutil -lswscale -lSDL -lSDL_mixer
DESTDIR     += ../build/
OBJECTS_DIR += ../build/
MOC_DIR     += ../build/
UI_DIR      += ../build/

SOURCES += main.cpp\
        mainwindow.cpp \
    cvideofilewrapper.cpp \
    capplicationconfig.cpp \
    wgt_QVideoPlayer.cpp \
    wgt_QCustomRuller.cpp \
    cdiaporama.cpp \
    cimagefilewrapper.cpp \
    wgt_QCustomThumbnails.cpp \
    DlgImageProperties.cpp \
    DlgVideoProperties.cpp \
    wgt_QCustomScene.cpp \
    fmt_filters.cpp \
    wgt_QImageFilterTransform.cpp \
    wgt_QImageFilterCorrect.cpp \
    wgt_QBackgroundWidget.cpp \
    wgt_QBackgroundDefWidget.cpp \
    DlgProject.cpp \
    DlgApplicationSettings.cpp \
    DlgBackgroundProperties.cpp \
    wgt_QCompositionWidget.cpp \
    cCustomGraphicsRectItem.cpp \
    cCustomComboBox.cpp \
    wgt_QCustomBrush.cpp \
    DlgMusicProperties.cpp \
    DlgRenderVideo.cpp \
    DlgTransitionProperties.cpp \
    DlgAbout.cpp \
    cSoundDefinition.cpp

HEADERS  += mainwindow.h \
    cvideofilewrapper.h \
    capplicationconfig.h \
    wgt_QVideoPlayer.h \
    wgt_QCustomRuller.h \
    cdiaporama.h \
    cimagefilewrapper.h \
    wgt_QCustomThumbnails.h \
    DlgImageProperties.h \
    DlgVideoProperties.h \
    wgt_QCustomScene.h \
    fmt_filters.h \
    wgt_QImageFilterTransform.h \
    wgt_QImageFilterCorrect.h \
    wgt_QBackgroundWidget.h \
    wgt_QBackgroundDefWidget.h \
    DlgProject.h \
    DlgApplicationSettings.h \
    DlgBackgroundProperties.h \
    wgt_QCompositionWidget.h \
    cCustomGraphicsRectItem.h \
    cCustomComboBox.h \
    wgt_QCustomBrush.h \
    DlgMusicProperties.h \
    DlgRenderVideo.h \
    DlgTransitionProperties.h \
    DlgAbout.h \
    cSoundDefinition.h

FORMS    += mainwindow.ui \
    wgt_QVideoPlayer.ui \
    DlgImageProperties.ui \
    DlgVideoProperties.ui \
    wgt_QCustomScene.ui \
    wgt_QImageFilterTransform.ui \
    wgt_QImageFilterCorrect.ui \
    wgt_QBackgroundWidget.ui \
    wgt_QBackgroundDefWidget.ui \
    DlgProject.ui \
    DlgApplicationSettings.ui \
    DlgBackgroundProperties.ui \
    wgt_QCompositionWidget.ui \
    wgt_QCustomBrush.ui \
    DlgMusicProperties.ui \
    DlgRenderVideo.ui \
    DlgTransitionProperties.ui \
    DlgAbout.ui

OTHER_FILES += \
    ffDiaporama.xml \
    help/Rendering.ods \
    ../TODO-LIST.txt \
    ../licences.txt \
    ../licences.rtf \
    ../readme.txt

TRANSLATIONS += locale/locale_fr.ts

#install
unix {
    TARGET.path         = $$PREFIX/bin
    TARGET.files        = ../build/$$TARGET
    translation.path    = $$PREFIX/share/$$TARGETAPP/locale
    translation.files   = locale/*.qm
    background.path     = $$PREFIX/share/$$TARGETAPP/background
    background.files    = ../background/*.*
    icons.path          = $$PREFIX/share/$$TARGETAPP/icons
    icons.files         = icons/*.*
    tr_icons.path       = $$PREFIX/share/$$TARGETAPP/transitions-icons
    tr_icons.files      = transitions-icons/*.*
    luma.path           = $$PREFIX/share/$$TARGETAPP/luma
    luma.files          = luma/*.*
    luma_Bar.path       = $$PREFIX/share/$$TARGETAPP/luma/Bar
    luma_Bar.files      = luma/Bar/*.*
    luma_Box.path       = $$PREFIX/share/$$TARGETAPP/luma/Box
    luma_Box.files      = luma/Box/*.*
    luma_Center.path    = $$PREFIX/share/$$TARGETAPP/luma/Center
    luma_Center.files   = luma/Center/*.*
    luma_Checker.path   = $$PREFIX/share/$$TARGETAPP/luma/Checker
    luma_Checker.files  = luma/Checker/*.*
    luma_Clock.path     = $$PREFIX/share/$$TARGETAPP/luma/Clock
    luma_Clock.files    = luma/Clock/*.*
    luma_Snake.path     = $$PREFIX/share/$$TARGETAPP/luma/Snake
    luma_Snake.files    = luma/Snake/*.*
    documentation.path  = $$PREFIX/share/$$TARGETAPP/help
    documentation.files = help/*.*
    General.path        = $$PREFIX/share/$$TARGETAPP
    General.files       = ffDiaporama.xml ../licence.rtf ../licences.txt ../readme.txt
}

INSTALLS += TARGET translation background icons tr_icons luma luma_Bar luma_Box luma_Center luma_Checker luma_Clock luma_Snake documentation General
