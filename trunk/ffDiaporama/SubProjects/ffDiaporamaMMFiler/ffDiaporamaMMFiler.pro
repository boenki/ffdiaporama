QT          += core gui xml
CONFIG      += console
CONFIG      += thread
QMAKE_STRIP  = echo
APPFOLDER    = ffDiaporama
TARGET       = ffDiaporamaMMFiler
TEMPLATE     = app

DEFINES      += HAVE_CONFIG_H               # specific for TAGLib
DEFINES      += TAGLIB_STATIC               # specific for TAGLib

SOURCES += main.cpp\
        mainwindow.cpp \
    DlgApplicationSettings.cpp \
    cApplicationConfig.cpp \
    DlgAbout.cpp \
    ../VariousClass/cSaveWindowPosition.cpp \
    ../VariousClass/cBaseApplicationConfig.cpp \
    ../VariousClass/QCustomFolderTree.cpp \
    ../VariousClass/QCustomFolderTable.cpp \
    ../VariousClass/cDriveList.cpp \
    ../VariousClass/cDeviceModelDef.cpp \
    ../VariousClass/cBaseMediaFile.cpp \
    ../VariousClass/cLuLoImageCache.cpp \
    ../VariousClass/cFilterTransformObject.cpp \
    ../VariousClass/QCustomHorizSplitter.cpp \
    ../VariousClass/QCustomFileInfoLabel.cpp \
    ../VariousClass/cSoundBlockList.cpp \
    ../fmt_filters/fmt_filters.cpp \
    ../VariousWidgets/DlgCheckConfig.cpp \
    ../VariousWidgets/_QCustomDialog.cpp \
    ../VariousWidgets/DlgManageDevices.cpp \
    ../VariousWidgets/DlgInfoFile.cpp \
    ../TAGLib/tstringlist.cpp \
    ../TAGLib/tstring.cpp \
    ../TAGLib/tmap.tcc \
    ../TAGLib/tlist.tcc \
    ../TAGLib/tfile.cpp \
    ../TAGLib/tbytevectorlist.cpp \
    ../TAGLib/tbytevector.cpp \
    ../TAGLib/tag.cpp \
    ../TAGLib/mpegproperties.cpp \
    ../TAGLib/mpegheader.cpp \
    ../TAGLib/mpegfile.cpp \
    ../TAGLib/id3v2tag.cpp \
    ../TAGLib/id3v2header.cpp \
    ../TAGLib/id3v2framefactory.cpp \
    ../TAGLib/id3v2frame.cpp \
    ../TAGLib/flacproperties.cpp \
    ../TAGLib/flacpicture.cpp \
    ../TAGLib/flacmetadatablock.cpp \
    ../TAGLib/flacfile.cpp \
    ../TAGLib/fileref.cpp \
    ../TAGLib/audioproperties.cpp \
    ../TAGLib/unicode.cpp \
    ../TAGLib/tdebug.cpp \
    ../TAGLib/xingheader.cpp \
    ../TAGLib/tagunion.cpp \
    ../TAGLib/id3v1tag.cpp \
    ../TAGLib/apefooter.cpp \
    ../TAGLib/apetag.cpp \
    ../TAGLib/apeitem.cpp \
    ../TAGLib/id3v2extendedheader.cpp \
    ../TAGLib/id3v2footer.cpp \
    ../TAGLib/id3v2synchdata.cpp \
    ../TAGLib/id3v1genres.cpp \
    ../TAGLib/frames/textidentificationframe.cpp \
    ../TAGLib/frames/commentsframe.cpp \
    ../TAGLib/attachedpictureframe.cpp \
    ../TAGLib/frames/relativevolumeframe.cpp \
    ../TAGLib/frames/uniquefileidentifierframe.cpp \
    ../TAGLib/frames/unknownframe.cpp \
    ../TAGLib/frames/generalencapsulatedobjectframe.cpp \
    ../TAGLib/frames/urllinkframe.cpp \
    ../TAGLib/frames/unsynchronizedlyricsframe.cpp \
    ../TAGLib/frames/popularimeterframe.cpp \
    ../TAGLib/frames/privateframe.cpp \
    ../TAGLib/xiphcomment.cpp \
    ../TAGLib/flacunknownmetadatablock.cpp \
    ../TAGLib/vorbisfile.cpp \
    ../TAGLib/oggflacfile.cpp \
    ../TAGLib/oggfile.cpp \
    ../TAGLib/vorbisproperties.cpp \
    ../TAGLib/oggpageheader.cpp \
    ../TAGLib/oggpage.cpp \
    ../TAGLib/mp4file.cpp \
    ../TAGLib/mp4atom.cpp \
    ../TAGLib/mp4tag.cpp \
    ../TAGLib/mp4properties.cpp \
    ../TAGLib/mp4item.cpp \
    ../TAGLib/mp4coverart.cpp \
    ../TAGLib/asffile.cpp \
    ../TAGLib/asftag.cpp \
    ../TAGLib/asfproperties.cpp \
    ../TAGLib/asfpicture.cpp \
    ../TAGLib/asfattribute.cpp

HEADERS  += mainwindow.h \
    DlgApplicationSettings.h \
    cApplicationConfig.h \
    DlgAbout.h \
    ../VariousClass/cSaveWindowPosition.h \
    ../VariousClass/cBaseApplicationConfig.h \
    ../VariousClass/QCustomFolderTree.h \
    ../VariousClass/QCustomFolderTable.h \
    ../VariousClass/cDriveList.h \
    ../VariousClass/cDeviceModelDef.h \
    ../VariousClass/cBaseMediaFile.h \
    ../VariousClass/cLuLoImageCache.h \
    ../VariousClass/cFilterTransformObject.h \
    ../VariousClass/cSoundBlockList.h \
    ../VariousClass/_GlobalDefines.h \
    ../VariousClass/QCustomHorizSplitter.h \
    ../VariousClass/QCustomFileInfoLabel.h \
    ../fmt_filters/fmt_filters.h \
    ../VariousWidgets/DlgCheckConfig.h \
    ../VariousWidgets/_QCustomDialog.h \
    ../VariousWidgets/DlgManageDevices.h \
    ../VariousWidgets/DlgInfoFile.h \
    ../TAGLib/tstringlist.h \
    ../TAGLib/tstring.h \
    ../TAGLib/tmap.h \
    ../TAGLib/tlist.h \
    ../TAGLib/tfile.h \
    ../TAGLib/tbytevectorlist.h \
    ../TAGLib/tbytevector.h \
    ../TAGLib/taglib_export.h \
    ../TAGLib/taglib_config.h \
    ../TAGLib/taglib.h \
    ../TAGLib/tag.h \
    ../TAGLib/mpegproperties.h \
    ../TAGLib/mpegheader.h \
    ../TAGLib/mpegfile.h \
    ../TAGLib/id3v2tag.h \
    ../TAGLib/id3v2header.h \
    ../TAGLib/id3v2framefactory.h \
    ../TAGLib/id3v2frame.h \
    ../TAGLib/flacproperties.h \
    ../TAGLib/flacpicture.h \
    ../TAGLib/flacmetadatablock.h \
    ../TAGLib/flacfile.h \
    ../TAGLib/fileref.h \
    ../TAGLib/audioproperties.h \
    ../TAGLib/attachedpictureframe.h \
    ../TAGLib/unicode.h \
    ../TAGLib/tdebug.h \
    ../TAGLib/xingheader.h \
    ../TAGLib/tagunion.h \
    ../TAGLib/id3v1tag.h \
    ../TAGLib/apefooter.h \
    ../TAGLib/apetag.h \
    ../TAGLib/apeitem.h \
    ../TAGLib/id3v2extendedheader.h \
    ../TAGLib/id3v2footer.h \
    ../TAGLib/id3v2synchdata.h \
    ../TAGLib/id3v1genres.h \
    ../TAGLib/frames/textidentificationframe.h \
    ../TAGLib/frames/commentsframe.h \
    ../TAGLib/frames/relativevolumeframe.h \
    ../TAGLib/frames/uniquefileidentifierframe.h \
    ../TAGLib/frames/unknownframe.h \
    ../TAGLib/frames/generalencapsulatedobjectframe.h \
    ../TAGLib/frames/urllinkframe.h \
    ../TAGLib/frames/unsynchronizedlyricsframe.h \
    ../TAGLib/frames/popularimeterframe.h \
    ../TAGLib/frames/privateframe.h \
    ../TAGLib/xiphcomment.h \
    ../TAGLib/flacunknownmetadatablock.h \
    ../TAGLib/vorbisfile.h \
    ../TAGLib/oggflacfile.h \
    ../TAGLib/oggfile.h \
    ../TAGLib/vorbisproperties.h \
    ../TAGLib/oggpageheader.h \
    ../TAGLib/oggpage.h \
    ../TAGLib/mp4file.h \
    ../TAGLib/mp4atom.h \
    ../TAGLib/mp4tag.h \
    ../TAGLib/mp4properties.h \
    ../TAGLib/mp4item.h \
    ../TAGLib/mp4coverart.h \
    ../TAGLib/config.h \
    ../TAGLib/asffile.h \
    ../TAGLib/asftag.h \
    ../TAGLib/asfproperties.h \
    ../TAGLib/asfpicture.h \
    ../TAGLib/asfattribute.h

FORMS    += mainwindow.ui \
    DlgApplicationSettings.ui \
    DlgAbout.ui \
    ../VariousWidgets/DlgCheckConfig.ui \
    ../VariousWidgets/DlgManageDevices.ui \
    ../VariousWidgets/DlgInfoFile.ui

unix {
    DESTDIR     += ../../../build
    OBJECTS_DIR += ../build$$TARGET
    MOC_DIR     += ../build$$TARGET
    #UI_DIR      += ../build$$TARGET
    INCLUDEPATH += . \
                   ../build$$TARGET \
                   /usr/include/ffmpeg/                                 # Specific for Fedora

    LIBS        += -lavformat -lavcodec -lavutil -lswscale
}
win32 {
    DESTDIR     += ..\\..\\..\\winbuild
    OBJECTS_DIR += ..\\winbuild$$TARGET
    MOC_DIR     += ..\\winbuild$$TARGET
    #UI_DIR      += ..\\winbuild$$TARGET

    RC_FILE     = ffDiaporamaMMFiler.rc

    INCLUDEPATH += . \
                   ..\\winbuild$$TARGET \
                   C:\\Qt\\ffmpeg-win32-dev\\include \                  #------ ffmpeg library path
                   C:\\Qt\\SDL-1.2.14\\include \                        #------ SDL library path

    LIBS        +=  -L"C:\\Qt\\ffmpeg-win32-dev\\lib"    \              #------ ffmpeg library path
                   -lavformat -lavcodec -lavutil -lswscale -lgdi32
}

TRANSLATIONS += locale/ffDiaporamaMMFiler_fr.ts #\
    #locale/ffDiaporamaMMFiler_it.ts \
    #locale/ffDiaporamaMMFiler_de.ts \
    #locale/ffDiaporamaMMFiler_nl.ts \
    #locale/ffDiaporamaMMFiler_es.ts \
    #locale/ffDiaporamaMMFiler_pt.ts \
    #locale/ffDiaporamaMMFiler_ru.ts \
    #locale/ffDiaporamaMMFiler_el.ts

OTHER_FILES += ffDiaporamaMMFiler.xml \
    authors-ffDiaporamaMMFiler.txt \
    ffDiaporamaMMFiler.desktop \
    ffDiaporamaMMFileropt.desktop \
    ffDiaporamaMMFilerlocal.desktop \
    licence.rtf \
    licences.txt \
    ffDiaporamaMMFiler.rc \
    Devices.xml

#install
unix {
    !exists($$INSTALL_ROOT$$DEST_DIR) {
        DEST_DIR       = /usr
    }

    message("Install to : $$INSTALL_ROOT$$DEST_DIR")

    TARGET.path         = $$INSTALL_ROOT$$DEST_DIR/bin
    TARGET.files        = ../../../build/$$TARGET
    translation.path    = $$INSTALL_ROOT$$DEST_DIR/share/$$APPFOLDER/locale
    translation.files   = locale/*.qm
    img.path            = $$INSTALL_ROOT$$DEST_DIR/share/$$APPFOLDER/MMFiler_img
    img.files           = MMFiler_img/*.*
    General.path        = $$INSTALL_ROOT$$DEST_DIR/share/$$APPFOLDER
    General.files       = ffDiaporamaMMFiler.xml licence.rtf licences.txt authors-ffDiaporamaMMFiler.txt

    contains(INSTALL_ROOT,/opt) {
        desktop.path    = /usr/share/applications
        desktop.files   = ffDiaporamaMMFileropt.desktop
    }

    contains(INSTALL_ROOT,/usr/local) {
        desktop.path    = /usr/share/applications
        desktop.files   = ffDiaporamaMMFilerlocal.desktop
    }

    !contains(INSTALL_ROOT,/usr/local) : !contains(INSTALL_ROOT,/opt) {

        desktop.path    = /usr/share/applications
        desktop.files   = ffDiaporamaMMFiler.desktop
    }

    iconfile.path       = /usr/share/icons/gnome/32x32/mimetypes
    iconfile.files      = application-ffDiaporamaMMFiler.png

    INSTALLS 		+= TARGET translation img General desktop iconfile
}
