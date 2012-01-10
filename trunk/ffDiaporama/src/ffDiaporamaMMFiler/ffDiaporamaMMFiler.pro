QT          += core gui xml
CONFIG      += console
CONFIG      += thread
QMAKE_STRIP  = echo
APPFOLDER    = ffDiaporama
TARGET       = ffDiaporamaMMFiler
TEMPLATE     = app

DEFINES      += HAVE_CONFIG_H               # specific for TAGLib
DEFINES      += TAGLIB_STATIC               # specific for TAGLib

#--------------------------------------------------------------
# DEFINES BUILD DIRECTORIES, COMMON INCLUDES AND COMMON LIBS
#--------------------------------------------------------------
include(../common.pri)
#--------------------------------------------------------------

win32:RC_FILE = ffDiaporamaMMFiler.rc

OTHER_FILES += \
    ffDiaporamaMMFiler.rc

TRANSLATIONS += ../../locale/ffDiaporamaMMFiler_fr.ts \
    ../../locale/ffDiaporamaMMFiler_it.ts \
    ../../locale/ffDiaporamaMMFiler_de.ts \
    ../../locale/ffDiaporamaMMFiler_nl.ts \
    ../../locale/ffDiaporamaMMFiler_es.ts \
    ../../locale/ffDiaporamaMMFiler_pt.ts \
    ../../locale/ffDiaporamaMMFiler_ru.ts \
    ../../locale/ffDiaporamaMMFiler_el.ts \
    ../../locale/ffDiaporamaMMFiler_cz.ts

SOURCES += main.cpp\
        mainwindow.cpp \
    DlgApplicationSettings.cpp \
    cApplicationConfig.cpp \
    DlgAbout.cpp \
    ../sharedfiles/cSaveWindowPosition.cpp \
    ../sharedfiles/cBaseApplicationConfig.cpp \
    ../sharedfiles/QCustomFolderTree.cpp \
    ../sharedfiles/QCustomFolderTable.cpp \
    ../sharedfiles/cDriveList.cpp \
    ../sharedfiles/cDeviceModelDef.cpp \
    ../sharedfiles/cBaseMediaFile.cpp \
    ../sharedfiles/cLuLoImageCache.cpp \
    ../sharedfiles/cFilterTransformObject.cpp \
    ../sharedfiles/QCustomHorizSplitter.cpp \
    ../sharedfiles/QCustomFileInfoLabel.cpp \
    ../sharedfiles/cSoundBlockList.cpp \
    ../fmt_filters/fmt_filters.cpp \
    ../sharedfiles/DlgCheckConfig.cpp \
    ../sharedfiles/_QCustomDialog.cpp \
    ../sharedfiles/DlgManageDevices.cpp \
    ../sharedfiles/DlgInfoFile.cpp \
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
    ../sharedfiles/cSaveWindowPosition.h \
    ../sharedfiles/cBaseApplicationConfig.h \
    ../sharedfiles/QCustomFolderTree.h \
    ../sharedfiles/QCustomFolderTable.h \
    ../sharedfiles/cDriveList.h \
    ../sharedfiles/cDeviceModelDef.h \
    ../sharedfiles/cBaseMediaFile.h \
    ../sharedfiles/cLuLoImageCache.h \
    ../sharedfiles/cFilterTransformObject.h \
    ../sharedfiles/cSoundBlockList.h \
    ../sharedfiles/_GlobalDefines.h \
    ../sharedfiles/QCustomHorizSplitter.h \
    ../sharedfiles/QCustomFileInfoLabel.h \
    ../fmt_filters/fmt_filters.h \
    ../sharedfiles/DlgCheckConfig.h \
    ../sharedfiles/_QCustomDialog.h \
    ../sharedfiles/DlgManageDevices.h \
    ../sharedfiles/DlgInfoFile.h \
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
    ../TAGLib/asfattribute.h \
    ../fmt_filters/fmt_filters.h

FORMS    += mainwindow.ui \
    DlgApplicationSettings.ui \
    DlgAbout.ui \
    ../sharedfiles/DlgCheckConfig.ui \
    ../sharedfiles/DlgManageDevices.ui \
    ../sharedfiles/DlgInfoFile.ui



