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

OTHER_FILES += \
    ffDiaporama.rc

TRANSLATIONS += ../../locale/ffDiaporama_fr.ts \
    ../../locale/ffDiaporama_it.ts \
    ../../locale/ffDiaporama_de.ts \
    ../../locale/ffDiaporama_nl.ts \
    ../../locale/ffDiaporama_es.ts \
    ../../locale/ffDiaporama_pt.ts \
    ../../locale/ffDiaporama_ru.ts \
    ../../locale/ffDiaporama_el.ts

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
    ../sharedfiles/DlgInfoFile.cpp \
    ../TAGLib/frames/urllinkframe.cpp \
    ../TAGLib/frames/unsynchronizedlyricsframe.cpp \
    ../TAGLib/frames/unknownframe.cpp \
    ../TAGLib/frames/uniquefileidentifierframe.cpp \
    ../TAGLib/frames/textidentificationframe.cpp \
    ../TAGLib/frames/relativevolumeframe.cpp \
    ../TAGLib/frames/privateframe.cpp \
    ../TAGLib/frames/popularimeterframe.cpp \
    ../TAGLib/frames/generalencapsulatedobjectframe.cpp \
    ../TAGLib/frames/commentsframe.cpp \
    ../TAGLib/xiphcomment.cpp \
    ../TAGLib/xingheader.cpp \
    ../TAGLib/vorbisproperties.cpp \
    ../TAGLib/vorbisfile.cpp \
    ../TAGLib/unicode.cpp \
    ../TAGLib/tstringlist.cpp \
    ../TAGLib/tstring.cpp \
    ../TAGLib/tmap.tcc \
    ../TAGLib/tlist.tcc \
    ../TAGLib/tfile.cpp \
    ../TAGLib/tdebug.cpp \
    ../TAGLib/tbytevectorlist.cpp \
    ../TAGLib/tbytevector.cpp \
    ../TAGLib/tagunion.cpp \
    ../TAGLib/tag.cpp \
    ../TAGLib/oggpageheader.cpp \
    ../TAGLib/oggpage.cpp \
    ../TAGLib/oggflacfile.cpp \
    ../TAGLib/oggfile.cpp \
    ../TAGLib/mpegproperties.cpp \
    ../TAGLib/mpegheader.cpp \
    ../TAGLib/mpegfile.cpp \
    ../TAGLib/id3v2tag.cpp \
    ../TAGLib/id3v2synchdata.cpp \
    ../TAGLib/id3v2header.cpp \
    ../TAGLib/id3v2framefactory.cpp \
    ../TAGLib/id3v2frame.cpp \
    ../TAGLib/id3v2footer.cpp \
    ../TAGLib/id3v2extendedheader.cpp \
    ../TAGLib/id3v1tag.cpp \
    ../TAGLib/id3v1genres.cpp \
    ../TAGLib/flacunknownmetadatablock.cpp \
    ../TAGLib/flacproperties.cpp \
    ../TAGLib/flacpicture.cpp \
    ../TAGLib/flacmetadatablock.cpp \
    ../TAGLib/flacfile.cpp \
    ../TAGLib/fileref.cpp \
    ../TAGLib/audioproperties.cpp \
    ../TAGLib/attachedpictureframe.cpp \
    ../TAGLib/apetag.cpp \
    ../TAGLib/apeitem.cpp \
    ../TAGLib/apefooter.cpp \
    ../TAGLib/mp4tag.cpp \
    ../TAGLib/mp4properties.cpp \
    ../TAGLib/mp4item.cpp \
    ../TAGLib/mp4file.cpp \
    ../TAGLib/mp4coverart.cpp \
    ../TAGLib/mp4atom.cpp \
    ../TAGLib/asftag.cpp \
    ../TAGLib/asfproperties.cpp \
    ../TAGLib/asfpicture.cpp \
    ../TAGLib/asffile.cpp \
    ../TAGLib/asfattribute.cpp

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
    ../sharedfiles/DlgInfoFile.h \
    ../TAGLib/frames/urllinkframe.h \
    ../TAGLib/frames/unsynchronizedlyricsframe.h \
    ../TAGLib/frames/unknownframe.h \
    ../TAGLib/frames/uniquefileidentifierframe.h \
    ../TAGLib/frames/textidentificationframe.h \
    ../TAGLib/frames/relativevolumeframe.h \
    ../TAGLib/frames/privateframe.h \
    ../TAGLib/frames/popularimeterframe.h \
    ../TAGLib/frames/generalencapsulatedobjectframe.h \
    ../TAGLib/frames/commentsframe.h \
    ../TAGLib/xiphcomment.h \
    ../TAGLib/xingheader.h \
    ../TAGLib/vorbisproperties.h \
    ../TAGLib/vorbisfile.h \
    ../TAGLib/unicode.h \
    ../TAGLib/tstringlist.h \
    ../TAGLib/tstring.h \
    ../TAGLib/tmap.h \
    ../TAGLib/tlist.h \
    ../TAGLib/tfile.h \
    ../TAGLib/tdebug.h \
    ../TAGLib/tbytevectorlist.h \
    ../TAGLib/tbytevector.h \
    ../TAGLib/tagunion.h \
    ../TAGLib/taglib_export.h \
    ../TAGLib/taglib_config.h \
    ../TAGLib/taglib.h \
    ../TAGLib/tag.h \
    ../TAGLib/oggpageheader.h \
    ../TAGLib/oggpage.h \
    ../TAGLib/oggflacfile.h \
    ../TAGLib/oggfile.h \
    ../TAGLib/mpegproperties.h \
    ../TAGLib/mpegheader.h \
    ../TAGLib/mpegfile.h \
    ../TAGLib/id3v2tag.h \
    ../TAGLib/id3v2synchdata.h \
    ../TAGLib/id3v2header.h \
    ../TAGLib/id3v2framefactory.h \
    ../TAGLib/id3v2frame.h \
    ../TAGLib/id3v2footer.h \
    ../TAGLib/id3v2extendedheader.h \
    ../TAGLib/id3v1tag.h \
    ../TAGLib/id3v1genres.h \
    ../TAGLib/flacunknownmetadatablock.h \
    ../TAGLib/flacproperties.h \
    ../TAGLib/flacpicture.h \
    ../TAGLib/flacmetadatablock.h \
    ../TAGLib/flacfile.h \
    ../TAGLib/fileref.h \
    ../TAGLib/config.h \
    ../TAGLib/audioproperties.h \
    ../TAGLib/attachedpictureframe.h \
    ../TAGLib/apetag.h \
    ../TAGLib/apeitem.h \
    ../TAGLib/apefooter.h \
    ../TAGLib/mp4tag.h \
    ../TAGLib/mp4properties.h \
    ../TAGLib/mp4item.h \
    ../TAGLib/mp4file.h \
    ../TAGLib/mp4coverart.h \
    ../TAGLib/mp4atom.h \
    ../TAGLib/asftag.h \
    ../TAGLib/asfproperties.h \
    ../TAGLib/asfpicture.h \
    ../TAGLib/asffile.h \
    ../TAGLib/asfattribute.h

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


