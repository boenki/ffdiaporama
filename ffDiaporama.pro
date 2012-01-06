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
    UI_DIR      += ../build$$TARGET
    ICON        = img/logo.png

    INCLUDEPATH += . \
                   ../build$$TARGET \
                   /usr/include/ffmpeg/                                # Specific for Fedora

    LIBS        += -lavformat -lavcodec -lavutil -lswscale -lSDL
}

win32 {
    DESTDIR     += ..\\winbuild
    OBJECTS_DIR += ..\\winbuild$$TARGET
    MOC_DIR     += ..\\winbuild$$TARGET
    UI_DIR      += ..\\winbuild$$TARGET

    RC_FILE     = ffDiaporama.rc

    INCLUDEPATH += . \
                   ..\\winbuild$$TARGET \
                   C:\\Qt\\ffmpeg-win32-dev\\include \                  #------ ffmpeg library path
                   C:\\Qt\\SDL-1.2.14\\include                          #------ SDL library path

    LIBS        +=  -L"C:\\Qt\\ffmpeg-win32-dev\\lib" \                 #------ ffmpeg library path
                   -L"C:\\Qt\\SDL-1.2.14\\lib" \                        #------ SDL library path
                   -lavformat -lavcodec -lavutil -lswscale -lSDL -lgdi32
}

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
    SubProjects/VariousWidgets/DlgCheckConfig.cpp \
    SubProjects/VariousClass/cSaveWindowPosition.cpp \
    SubProjects/VariousClass/cBaseApplicationConfig.cpp \
    SubProjects/VariousClass/cFilterTransformObject.cpp \
    SubProjects/VariousClass/cFilterCorrectObject.cpp \
    SubProjects/fmt_filters/fmt_filters.cpp \
    SubProjects/VariousClass/cLuLoImageCache.cpp \
    SubProjects/VariousClass/cDeviceModelDef.cpp \
    SubProjects/VariousClass/cSoundBlockList.cpp \
    SubProjects/VariousClass/cBaseMediaFile.cpp \
    SubProjects/VariousClass/_SDL_Support.cpp \
    SubProjects/VariousWidgets/_QCustomDialog.cpp \
    SubProjects/VariousWidgets/DlgManageDevices.cpp \
    SubProjects/VariousClass/QCustomFileInfoLabel.cpp \
    SubProjects/VariousWidgets/DlgffDPjrProperties.cpp \
    SubProjects/VariousClass/cBrushDefinition.cpp \
    SubProjects/VariousWidgets/DlgInfoFile.cpp \
    SubProjects/TAGLib/frames/urllinkframe.cpp \
    SubProjects/TAGLib/frames/unsynchronizedlyricsframe.cpp \
    SubProjects/TAGLib/frames/unknownframe.cpp \
    SubProjects/TAGLib/frames/uniquefileidentifierframe.cpp \
    SubProjects/TAGLib/frames/textidentificationframe.cpp \
    SubProjects/TAGLib/frames/relativevolumeframe.cpp \
    SubProjects/TAGLib/frames/privateframe.cpp \
    SubProjects/TAGLib/frames/popularimeterframe.cpp \
    SubProjects/TAGLib/frames/generalencapsulatedobjectframe.cpp \
    SubProjects/TAGLib/frames/commentsframe.cpp \
    SubProjects/TAGLib/xiphcomment.cpp \
    SubProjects/TAGLib/xingheader.cpp \
    SubProjects/TAGLib/vorbisproperties.cpp \
    SubProjects/TAGLib/vorbisfile.cpp \
    SubProjects/TAGLib/unicode.cpp \
    SubProjects/TAGLib/tstringlist.cpp \
    SubProjects/TAGLib/tstring.cpp \
    SubProjects/TAGLib/tmap.tcc \
    SubProjects/TAGLib/tlist.tcc \
    SubProjects/TAGLib/tfile.cpp \
    SubProjects/TAGLib/tdebug.cpp \
    SubProjects/TAGLib/tbytevectorlist.cpp \
    SubProjects/TAGLib/tbytevector.cpp \
    SubProjects/TAGLib/tagunion.cpp \
    SubProjects/TAGLib/tag.cpp \
    SubProjects/TAGLib/oggpageheader.cpp \
    SubProjects/TAGLib/oggpage.cpp \
    SubProjects/TAGLib/oggflacfile.cpp \
    SubProjects/TAGLib/oggfile.cpp \
    SubProjects/TAGLib/mpegproperties.cpp \
    SubProjects/TAGLib/mpegheader.cpp \
    SubProjects/TAGLib/mpegfile.cpp \
    SubProjects/TAGLib/id3v2tag.cpp \
    SubProjects/TAGLib/id3v2synchdata.cpp \
    SubProjects/TAGLib/id3v2header.cpp \
    SubProjects/TAGLib/id3v2framefactory.cpp \
    SubProjects/TAGLib/id3v2frame.cpp \
    SubProjects/TAGLib/id3v2footer.cpp \
    SubProjects/TAGLib/id3v2extendedheader.cpp \
    SubProjects/TAGLib/id3v1tag.cpp \
    SubProjects/TAGLib/id3v1genres.cpp \
    SubProjects/TAGLib/flacunknownmetadatablock.cpp \
    SubProjects/TAGLib/flacproperties.cpp \
    SubProjects/TAGLib/flacpicture.cpp \
    SubProjects/TAGLib/flacmetadatablock.cpp \
    SubProjects/TAGLib/flacfile.cpp \
    SubProjects/TAGLib/fileref.cpp \
    SubProjects/TAGLib/audioproperties.cpp \
    SubProjects/TAGLib/attachedpictureframe.cpp \
    SubProjects/TAGLib/apetag.cpp \
    SubProjects/TAGLib/apeitem.cpp \
    SubProjects/TAGLib/apefooter.cpp

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
    SubProjects/VariousWidgets/DlgCheckConfig.h \
    SubProjects/VariousClass/cSaveWindowPosition.h \
    SubProjects/VariousClass/cBaseApplicationConfig.h \
    SubProjects/VariousClass/cFilterTransformObject.h \
    SubProjects/VariousClass/cFilterCorrectObject.h \
    SubProjects/fmt_filters/fmt_filters.h \
    SubProjects/VariousClass/cLuLoImageCache.h \
    SubProjects/VariousClass/cDeviceModelDef.h \
    SubProjects/VariousClass/_GlobalDefines.h \
    SubProjects/VariousClass/cSoundBlockList.h \
    SubProjects/VariousClass/cBaseMediaFile.h \
    SubProjects/VariousClass/_SDL_Support.h \
    SubProjects/VariousWidgets/_QCustomDialog.h \
    SubProjects/VariousWidgets/DlgManageDevices.h \
    SubProjects/VariousClass/QCustomFileInfoLabel.h \
    SubProjects/VariousWidgets/DlgffDPjrProperties.h \
    SubProjects/VariousClass/cBrushDefinition.h \
    SubProjects/VariousWidgets/DlgInfoFile.h \
    SubProjects/TAGLib/frames/urllinkframe.h \
    SubProjects/TAGLib/frames/unsynchronizedlyricsframe.h \
    SubProjects/TAGLib/frames/unknownframe.h \
    SubProjects/TAGLib/frames/uniquefileidentifierframe.h \
    SubProjects/TAGLib/frames/textidentificationframe.h \
    SubProjects/TAGLib/frames/relativevolumeframe.h \
    SubProjects/TAGLib/frames/privateframe.h \
    SubProjects/TAGLib/frames/popularimeterframe.h \
    SubProjects/TAGLib/frames/generalencapsulatedobjectframe.h \
    SubProjects/TAGLib/frames/commentsframe.h \
    SubProjects/TAGLib/xiphcomment.h \
    SubProjects/TAGLib/xingheader.h \
    SubProjects/TAGLib/vorbisproperties.h \
    SubProjects/TAGLib/vorbisfile.h \
    SubProjects/TAGLib/unicode.h \
    SubProjects/TAGLib/tstringlist.h \
    SubProjects/TAGLib/tstring.h \
    SubProjects/TAGLib/tmap.h \
    SubProjects/TAGLib/tlist.h \
    SubProjects/TAGLib/tfile.h \
    SubProjects/TAGLib/tdebug.h \
    SubProjects/TAGLib/tbytevectorlist.h \
    SubProjects/TAGLib/tbytevector.h \
    SubProjects/TAGLib/tagunion.h \
    SubProjects/TAGLib/taglib_export.h \
    SubProjects/TAGLib/taglib_config.h \
    SubProjects/TAGLib/taglib.h \
    SubProjects/TAGLib/tag.h \
    SubProjects/TAGLib/oggpageheader.h \
    SubProjects/TAGLib/oggpage.h \
    SubProjects/TAGLib/oggflacfile.h \
    SubProjects/TAGLib/oggfile.h \
    SubProjects/TAGLib/mpegproperties.h \
    SubProjects/TAGLib/mpegheader.h \
    SubProjects/TAGLib/mpegfile.h \
    SubProjects/TAGLib/id3v2tag.h \
    SubProjects/TAGLib/id3v2synchdata.h \
    SubProjects/TAGLib/id3v2header.h \
    SubProjects/TAGLib/id3v2framefactory.h \
    SubProjects/TAGLib/id3v2frame.h \
    SubProjects/TAGLib/id3v2footer.h \
    SubProjects/TAGLib/id3v2extendedheader.h \
    SubProjects/TAGLib/id3v1tag.h \
    SubProjects/TAGLib/id3v1genres.h \
    SubProjects/TAGLib/flacunknownmetadatablock.h \
    SubProjects/TAGLib/flacproperties.h \
    SubProjects/TAGLib/flacpicture.h \
    SubProjects/TAGLib/flacmetadatablock.h \
    SubProjects/TAGLib/flacfile.h \
    SubProjects/TAGLib/fileref.h \
    SubProjects/TAGLib/config.h \
    SubProjects/TAGLib/audioproperties.h \
    SubProjects/TAGLib/attachedpictureframe.h \
    SubProjects/TAGLib/apetag.h \
    SubProjects/TAGLib/apeitem.h \
    SubProjects/TAGLib/apefooter.h

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
    SubProjects/VariousWidgets/DlgCheckConfig.ui \
    SubProjects/VariousWidgets/DlgManageDevices.ui \
    SubProjects/VariousWidgets/DlgffDPjrProperties.ui \
    SubProjects/VariousWidgets/DlgInfoFile.ui

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
    ffDiaporama-mime.xml \
    Devices.xml \
    SubProjects/TAGLib/NEWS \
    SubProjects/TAGLib/COPYING.LGPL \
    SubProjects/TAGLib/AUTHORS

TRANSLATIONS += locale/ffDiaporama_fr.ts \
    locale/ffDiaporama_it.ts \
    locale/ffDiaporama_de.ts \
    locale/ffDiaporama_nl.ts \
    locale/ffDiaporama_es.ts \
    locale/ffDiaporama_pt.ts \
    locale/ffDiaporama_ru.ts \
    locale/ffDiaporama_el.ts

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
    General.files       = ffDiaporama.xml Devices.xml BUILDVERSION.txt licence.rtf licences.txt authors.txt libx264-hq.ffpreset libx264-pq.ffpreset

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
