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

unix {
    DESTDIR     += ../build
    OBJECTS_DIR += ../build$$TARGET
    MOC_DIR     += ../build$$TARGET
    ICON        = img/logo.png

    INCLUDEPATH += /usr/include/ffmpeg/    \             # Specific for Fedora
                   ./SubProjects/VariousClass  \
                   ./SubProjects/fmt_filters

    LIBS        += -lavformat -lavcodec -lavutil -lswscale -lSDL
}

win32 {
    DESTDIR     += ..\\winbuild
    OBJECTS_DIR += ..\\winbuild$$TARGET
    MOC_DIR     += ..\\winbuild$$TARGET

    RC_FILE     = ffDiaporama.rc

    INCLUDEPATH += . \
                   C:\\Qt\\ffmpeg-win32-dev\\include \                  #------ ffmpeg library path
                   C:\\Qt\\SDL-1.2.14\\include \                        #------ SDL library path
                   .\\SubProjects\\VariousClass \
                   .\\SubProjects\\fmt_filters

    LIBS        +=  -L"C:\\Qt\\ffmpeg-win32-dev\\lib" \                  #------ ffmpeg library path
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
    DlgCheckConfig.cpp \
    SubProjects/VariousClass/cSaveWindowPosition.cpp \
    SubProjects/VariousClass/cBaseApplicationConfig.cpp \
    SubProjects/VariousClass/cFilterTransformObject.cpp \
    SubProjects/VariousClass/cFilterCorrectObject.cpp \
    SubProjects/fmt_filters/fmt_filters.cpp \
    SubProjects/VariousClass/cLuLoImageCache.cpp

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
    DlgCheckConfig.h \
    SubProjects/VariousClass/cSaveWindowPosition.h \
    SubProjects/VariousClass/cBaseApplicationConfig.h \
    SubProjects/VariousClass/cFilterTransformObject.h \
    SubProjects/VariousClass/cFilterCorrectObject.h \
    SubProjects/fmt_filters/fmt_filters.h \
    SubProjects/VariousClass/cLuLoImageCache.h

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
    DlgCheckConfig.ui

OTHER_FILES += \
    ffDiaporama.xml \
    TODO-LIST.txt \
    ffDiaporama.rc \
    BUILDVERSION.txt \
    ffDiaporama.SPEC \
    ffDiaporama.desktop \
    Packaging/Readme.txt \
    Packaging/MakeFedoraRPM.bash \
    Packaging/ffDiaporama-InoSetup.iss \
    Packaging/ffDiaporama.SPEC \
    Packaging/MakeWindowsSetup.cmd \
    Packaging/WikiToWEBPHP.py \
    Packaging/MakeUbuntuDEB.bash \
    Packaging/debian/watch \
    Packaging/debian/rules \
    Packaging/debian/ffdiaporama.1 \
    Packaging/debian/docs \
    Packaging/debian/copyright \
    Packaging/debian/control \
    Packaging/debian/compat \
    Packaging/debian/changelog \
    Packaging/debian/source/format \
    libx264-pq.ffpreset \
    licences.txt \
    licences.rtf \
    authors.txt \
    ffDiaporama \
    ffDiaporama.url \
    ffDiaporamaopt.desktop \
    ffDiaporamalocal.desktop \
    ffDiaporama-mime.xml

TRANSLATIONS += locale/locale_fr.ts \
    locale/locale_it.ts \
    locale/locale_de.ts \
    locale/locale_nl.ts \
    locale/locale_es.ts \
    locale/locale_pt.ts \
    locale/locale_ru.ts \
    locale/locale_el.ts

#install
unix {
    !exists($$INSTALL_ROOT$$DEST_DIR) {
        DEST_DIR = /usr
    }

    message("Install to : $$INSTALL_ROOT$$DEST_DIR")

    TARGET.path         = $$INSTALL_ROOT$$DEST_DIR/bin
    TARGET.files        = ../build/$$TARGET
    translation.path    = $$INSTALL_ROOT$$DEST_DIR/share/$$APPFOLDER/locale
    translation.files   = locale/*.qm
    background.path     = $$INSTALL_ROOT$$DEST_DIR/share/$$APPFOLDER/background
    background.files    = background/*.*
    img.path            = $$INSTALL_ROOT$$DEST_DIR/share/$$APPFOLDER/img
    img.files           = img/*.*
    tr_img.path         = $$INSTALL_ROOT$$DEST_DIR/share/$$APPFOLDER/transitions-img
    tr_img.files        = transitions-img/*.*
    luma.path           = $$INSTALL_ROOT$$DEST_DIR/share/$$APPFOLDER/luma
    luma.files          = luma/*.*
    luma_Bar.path       = $$INSTALL_ROOT$$DEST_DIR/share/$$APPFOLDER/luma/Bar
    luma_Bar.files      = luma/Bar/*.*
    luma_Box.path       = $$INSTALL_ROOT$$DEST_DIR/share/$$APPFOLDER/luma/Box
    luma_Box.files      = luma/Box/*.*
    luma_Center.path    = $$INSTALL_ROOT$$DEST_DIR/share/$$APPFOLDER/luma/Center
    luma_Center.files   = luma/Center/*.*
    luma_Checker.path   = $$INSTALL_ROOT$$DEST_DIR/share/$$APPFOLDER/luma/Checker
    luma_Checker.files  = luma/Checker/*.*
    luma_Clock.path     = $$INSTALL_ROOT$$DEST_DIR/share/$$APPFOLDER/luma/Clock
    luma_Clock.files    = luma/Clock/*.*
    luma_Snake.path     = $$INSTALL_ROOT$$DEST_DIR/share/$$APPFOLDER/luma/Snake
    luma_Snake.files    = luma/Snake/*.*
    General.path        = $$INSTALL_ROOT$$DEST_DIR/share/$$APPFOLDER
    General.files       = ffDiaporama.xml BUILDVERSION.txt licence.rtf licences.txt authors.txt libx264-hq.ffpreset libx264-pq.ffpreset

    mimefile.path       = /usr/share/mime/packages
    mimefile.files      = ffDiaporama-mime.xml

    exists(/usr/share/icons/gnome/32x32/mimetypes) {
        iconfile.path       = /usr/share/icons/gnome/32x32/mimetypes
        iconfile.files      = img/application-ffDiaporama.png
    }

    exists(/usr/share/icons/default.kde4/32x32/mimetypes) {
        iconfile.path       = /usr/share/icons/default.kde4/32x32/mimetypes
        iconfile.files      = img/application-ffDiaporama.png
    }

    contains(INSTALL_ROOT,/opt) {
        desktop.path    = /usr/share/applications
        desktop.files   = ffDiaporamaopt.desktop
    }

    contains(INSTALL_ROOT,/usr/local) {
        desktop.path    = /usr/share/applications
        desktop.files   = ffDiaporamalocal.desktop
    }

    !contains(INSTALL_ROOT,/usr/local) : !contains(INSTALL_ROOT,/opt) {

        desktop.path    = /usr/share/applications
        desktop.files   = ffDiaporama.desktop
    }

    INSTALLS 		+= TARGET translation background img tr_img luma_Bar luma_Box luma_Center luma_Checker luma_Clock luma_Snake luma General desktop mimefile iconfile
}
