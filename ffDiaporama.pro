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
unix {
    LIBS        += -lavformat -lavcodec -lavutil -lswscale -lSDL
    DESTDIR     += ../build/
    OBJECTS_DIR += ../build/
    MOC_DIR     += ../build/
    UI_DIR      += ../build/
    ICON        = icons/logo.png
    INCLUDEPATH += /usr/include/ffmpeg/  # Specific for Fedora
}
win32 {
    DESTDIR     += ../winbuild/
    OBJECTS_DIR += ../winbuild/
    MOC_DIR     += ../winbuild/
    UI_DIR      += ../winbuild/
    RC_FILE     = ffDiaporama.rc
    INCLUDEPATH += . \
                   C:\Qt\ffmpeg-git-c967860-win32-dev\include \        #------ ffmpeg library path
                   C:\Qt\SDL-1.2.14\include                            #------ SDL library path

    LIBS        += -L"C:\Qt\ffmpeg-git-c967860-win32-dev\lib" \        #------ ffmpeg library path
                   -L"C:\Qt\SDL-1.2.14\lib" \                          #------ SDL library path
                   -lavformat -lavcodec -lavutil -lswscale -lSDL
}

SOURCES +=  _ApplicationDefinitions.cpp \
            _SoundDefinitions.cpp \
            _ImagesDefinitions.cpp \
            _ImageFileWrapper.cpp \
            _VideoFileWrapper.cpp \
            _Diaporama.cpp \
            wgt_QVideoPlayer.cpp \
            wgt_QCustomRuller.cpp \
            wgt_QCustomThumbnails.cpp \
            DlgImageProperties.cpp \
            DlgVideoProperties.cpp \
            wgt_QCustomScene.cpp \
            fmt_filters.cpp \
            wgt_QImageFilterTransform.cpp \
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
            mainwindow.cpp \
            main.cpp \
    DlgSlideProperties.cpp \
    DlgImageCorrection.cpp \
    DlgImageTransformation.cpp \
    DlgVideoEdit.cpp

HEADERS  += _GlobalDefines.h \
            _SoundDefinitions.h \
            _ImagesDefinitions.h \
            _ApplicationDefinitions.h \
            _ImageFileWrapper.h \
            _VideoFileWrapper.h \
            _Diaporama.h \
            wgt_QVideoPlayer.h \
            wgt_QCustomRuller.h \
            wgt_QCustomThumbnails.h \
            DlgImageProperties.h \
            DlgVideoProperties.h \
            wgt_QCustomScene.h \
            fmt_filters.h \
            wgt_QImageFilterTransform.h \
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
            mainwindow.h \
    DlgSlideProperties.h \
    DlgImageCorrection.h \
    DlgImageTransformation.h \
    DlgVideoEdit.h

FORMS    += mainwindow.ui \
    wgt_QVideoPlayer.ui \
    DlgImageProperties.ui \
    DlgVideoProperties.ui \
    wgt_QCustomScene.ui \
    wgt_QImageFilterTransform.ui \
    wgt_QBackgroundDefWidget.ui \
    DlgProject.ui \
    DlgApplicationSettings.ui \
    DlgBackgroundProperties.ui \
    wgt_QCompositionWidget.ui \
    wgt_QCustomBrush.ui \
    DlgMusicProperties.ui \
    DlgRenderVideo.ui \
    DlgTransitionProperties.ui \
    DlgAbout.ui \
    DlgSlideProperties.ui \
    DlgImageCorrection.ui \
    DlgImageTransformation.ui \
    DlgVideoEdit.ui

OTHER_FILES += \
    ffDiaporama.xml \
    TODO-LIST.txt \
    licences.txt \
    licences.rtf \
    ffDiaporama.rc \
    BUILDVERSION.txt

TRANSLATIONS += locale/locale_fr.ts

#install
unix {
    TARGET.path         = $$PREFIX/bin
    TARGET.files        = ../build/$$TARGET
    translation.path    = $$PREFIX/share/$$TARGETAPP/locale
    translation.files   = locale/*.qm
    background.path     = $$PREFIX/share/$$TARGETAPP/background
    background.files    = background/*.*
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
    General.files       = ffDiaporama.xml BUILDVERSION.txt ../licence.rtf ../licences.txt libx264-hq.ffpreset
}

INSTALLS += TARGET translation background icons tr_icons luma luma_Bar luma_Box luma_Center luma_Checker luma_Clock luma_Snake documentation General
