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
    ICON        = img/logo.png
    INCLUDEPATH += /usr/include/ffmpeg/  # Specific for Fedora
}
win32 {
    DESTDIR     += ../winbuild/
    OBJECTS_DIR += ../winbuild/
    MOC_DIR     += ../winbuild/
    UI_DIR      += ../winbuild/
    RC_FILE     = ffDiaporama.rc
    INCLUDEPATH += . \
                   C:\\Qt\\ffmpeg-win32-dev\\include \                  #------ ffmpeg library path
                   C:\\Qt\\SDL-1.2.14\\include                          #------ SDL library path

    LIBS        += -L"C:\\Qt\\ffmpeg-win32-dev\\lib" \                  #------ ffmpeg library path
                   -L"C:\\Qt\\SDL-1.2.14\\lib" \                        #------ SDL library path
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
            fmt_filters.cpp \
            wgt_QBackgroundDefWidget.cpp \
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
    DlgVideoEdit.cpp \
    DlgTextEdit.cpp

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
            fmt_filters.h \
            wgt_QBackgroundDefWidget.h \
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
    DlgVideoEdit.h \
    DlgTextEdit.h

FORMS    += mainwindow.ui \
    wgt_QVideoPlayer.ui \
    wgt_QBackgroundDefWidget.ui \
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
    DlgVideoEdit.ui \
    DlgTextEdit.ui

OTHER_FILES += \
    ffDiaporama.xml \
    TODO-LIST.txt \
    licences.txt \
    licences.rtf \
    ffDiaporama.rc \
    BUILDVERSION.txt \
    ffDiaporama.SPEC \
    ffDiaporama.desktop \
    Packaging/Readme.txt \
    Packaging/MakeFedoraRPM.bash \
    Packaging/ffDiaporama-InoSetup.iss \
    Packaging/ffDiaporama.SPEC \
    Packaging/MakeWindowsSetup.cmd \
    Packaging/WikiToWEBPHP.py

TRANSLATIONS += locale/locale_fr.ts

#install
unix {
    TARGET.path         = $$PREFIX/bin
    TARGET.files        = ../build/$$TARGET
    translation.path    = $$PREFIX/share/$$TARGETAPP/locale
    translation.files   = locale/*.qm
    background.path     = $$PREFIX/share/$$TARGETAPP/background
    background.files    = background/*.*
    img.path          = $$PREFIX/share/$$TARGETAPP/img
    img.files         = img/*.*
    tr_img.path       = $$PREFIX/share/$$TARGETAPP/transitions-img
    tr_img.files      = transitions-img/*.*
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
    wiki.path           = $$PREFIX/share/$$TARGETAPP/WIKI
    wiki.files          = WIKI/*.*
    wiki_img.path       = $$PREFIX/share/$$TARGETAPP/WIKI_img
    wiki_img.files      = WIKI_img/*.*
    wiki_en.path        = $$PREFIX/share/$$TARGETAPP/WIKI/en
    wiki_en.files       = WIKI/en/*.*
    wiki_en_img.path    = $$PREFIX/share/$$TARGETAPP/WIKI/en/img
    wiki_en_img.files   = WIKI/en/img/*.*
    wiki_fr.path        = $$PREFIX/share/$$TARGETAPP/WIKI/fr
    wiki_fr.files       = WIKI/fr/*.*
    wiki_fr_img.path    = $$PREFIX/share/$$TARGETAPP/WIKI/fr/img
    wiki_fr_img.files   = WIKI/fr/img/*.*
    General.path        = $$PREFIX/share/$$TARGETAPP
    General.files       = ffDiaporama.xml BUILDVERSION.txt ../licence.rtf ../licences.txt libx264-hq.ffpreset ffDiaporama.desktop

    INSTALLS += TARGET General translation background img tr_img luma luma_Bar luma_Box luma_Center luma_Checker luma_Clock luma_Snake \
            wiki wiki_img wiki_en wiki_en_img wiki_fr wiki_fr_img
}
