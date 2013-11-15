#-------------------------------------------------------------
# SYNTAXE IS :
#   QMAKE PREFIX=xxx ffDiaporama.pro
#       xxx could be /usr, /usr/local or /opt
#--------------------------------------------------------------

CONFIG += qt thread

greaterThan(QT_MAJOR_VERSION, 4): {
    # QT5 version
    QT += widgets concurrent help
} else: {
    # QT4 version
    CONFIG += help
}

QT          += core gui xml network svg sql
QMAKE_STRIP  = echo
APPFOLDER    = ffDiaporama
TARGET       = ffDiaporama
TEMPLATE     = app
DEFINES     += HAVE_CONFIG_H               # specific for TAGLib
DEFINES     += TAGLIB_STATIC               # specific for TAGLib

isEmpty(PREFIX) {
    PREFIX = /usr
}

#--------------------------------------------------------------
# DEFINES $$DESTDIR DIRECTORIES, COMMON INCLUDES AND COMMON LIBS
#--------------------------------------------------------------

DEFINES +=SHARE_DIR=\\\"$$PREFIX\\\"

unix {
    CFLAGS += -W"Missing debug information for"

    contains(DEFINES,Q_OS_SOLARIS) {

        HARDWARE_PLATFORM = $$system(uname -m)
        contains(HARDWARE_PLATFORM,x86_64) {
            DEFINES+=Q_OS_SOLARIS64
            message("Solaris x86_64 build")
        } else {
            DEFINES+=Q_OS_SOLARIS32
            message("Solaris x86 build")
        }
        message("Use ffmpeg in /opt/gnu/include")
        INCLUDEPATH += /opt/gnu/include
        DEFINES     += USELIBSWRESAMPLE
        LIBS        += -lswresample

    } else {

        HARDWARE_PLATFORM = $$system(uname -m)
        contains(HARDWARE_PLATFORM,x86_64) {
            DEFINES+=Q_OS_LINUX64
            message("Linux x86_64 build")
        } else {
            DEFINES+=Q_OS_LINUX32
            message("Linux x86 build")
        }

        exists(/opt/ffmpeg/include/libswresample/swresample.h) {         #------ conditionnaly includes from Sam Rog packages for Ubuntu
            message("Use SAM ROG Packages from /opt/ffmpeg")
            INCLUDEPATH += /opt/ffmpeg/include/
            LIBS        += -L"/opt/ffmpeg/lib"
            DEFINES += USELIBSWRESAMPLE
            LIBS    += -lswresample                                             #------ conditionnaly include libswresample
        } else:exists(/usr/include/ffmpeg/libswresample/swresample.h) {         #------ Specific for Fedora
            message("Use ffmpeg in /usr/include/ffmpeg")
            DEFINES += USELIBSWRESAMPLE
            INCLUDEPATH += /usr/include/ffmpeg/
            LIBS    += -lswresample                                             #------ conditionnaly include libswresample
        } else:exists(/usr/include/libswresample/swresample.h) {                #------ Specific for openSUSE
            message("Use ffmpeg in /usr/include")
            INCLUDEPATH += /usr/include/
            DEFINES += USELIBSWRESAMPLE
            LIBS    += -lswresample                                             #------ conditionnaly include libswresample
        } else:exists(/usr/include/libavresample/avresample.h) {
            message("Use libav 9 in /usr/include")
            DEFINES += USELIBAVRESAMPLE
            LIBS    += -lavresample                                             #------ conditionnaly include libavresample
            INCLUDEPATH += /usr/include/
        } else {
            message("Use libav 0.8+taglib in /usr/include")
            LIBS    += -ltag                                                    #------ TAGlib is used only with LIBAV 8
            DEFINES += USETAGLIB
            INCLUDEPATH += /usr/include/
        }

    }

    LIBS        += -lexiv2                                                  #------ Exiv2

} else:win32 {

    contains(QMAKE_HOST.arch,x86_64) {
        DEFINES+=Q_OS_WIN64
        message("x86_64 build")
        INCLUDEPATH += "../../../win_src/ffmpeg-2.1-win64-dev/include"
        LIBS        += -L"../../../win_src/ffmpeg-2.1-win64-dev/lib"
        LIBS        += -L"../../../win_src/SDL-1.2.15/lib/x64"
    } else {
        DEFINES+=Q_OS_WIN32
        message("x86 build")
        INCLUDEPATH += "../../../win_src/ffmpeg-2.1-win32-dev/include"
        LIBS        += -L"../../../win_src/ffmpeg-2.1-win32-dev/lib"
        LIBS        += -L"../../../win_src/SDL-1.2.15/lib/x86"
    }

    CONFIG      += sql                                                      #------ I don't know why, but windows version need sql module in config directive
    DEFINES     += USELIBSWRESAMPLE

    INCLUDEPATH += .                                                        #------ I don't know why, but windows need this !
    INCLUDEPATH += ../exiv2
    INCLUDEPATH += "../../../win_src/exiv2-0.23/msvc64/include"
    INCLUDEPATH += "../../../win_src/SDL-1.2.15/include"
    INCLUDEPATH += "../../../win_src/msinttypes"

    LIBS        += -lgdi32 -lkernel32 -luser32 -lshell32 -ladvapi32         #------ Windows GDI libs link
    LIBS        += -lswresample                                             #------ conditionnaly include libswresample

    SOURCES += \
        ../../../win_src/exiv2-0.23/src/xmpsidecar.cpp \
        ../../../win_src/exiv2-0.23/src/xmp.cpp \
        ../../../win_src/exiv2-0.23/src/version.cpp \
        ../../../win_src/exiv2-0.23/src/value.cpp \
        ../../../win_src/exiv2-0.23/src/utils.cpp \
        ../../../win_src/exiv2-0.23/src/types.cpp \
        ../../../win_src/exiv2-0.23/src/tiffvisitor.cpp \
        ../../../win_src/exiv2-0.23/src/tiffimage.cpp \
        ../../../win_src/exiv2-0.23/src/tiffcomposite.cpp \
        ../../../win_src/exiv2-0.23/src/tgaimage.cpp \
        ../../../win_src/exiv2-0.23/src/tags.cpp \
        ../../../win_src/exiv2-0.23/src/sonymn.cpp \
        ../../../win_src/exiv2-0.23/src/sigmamn.cpp \
        ../../../win_src/exiv2-0.23/src/samsungmn.cpp \
        ../../../win_src/exiv2-0.23/src/rw2image.cpp \
        ../../../win_src/exiv2-0.23/src/rafimage.cpp \
        ../../../win_src/exiv2-0.23/src/psdimage.cpp \
        ../../../win_src/exiv2-0.23/src/properties.cpp \
        ../../../win_src/exiv2-0.23/src/preview.cpp \
        ../../../win_src/exiv2-0.23/src/pngimage.cpp \
        ../../../win_src/exiv2-0.23/src/pngchunk.cpp \
        ../../../win_src/exiv2-0.23/src/pgfimage.cpp \
        ../../../win_src/exiv2-0.23/src/pentaxmn.cpp \
        ../../../win_src/exiv2-0.23/src/panasonicmn.cpp \
        ../../../win_src/exiv2-0.23/src/orfimage.cpp \
        ../../../win_src/exiv2-0.23/src/olympusmn.cpp \
        ../../../win_src/exiv2-0.23/src/nikonmn.cpp \
        ../../../win_src/exiv2-0.23/src/mrwimage.cpp \
        ../../../win_src/exiv2-0.23/src/minoltamn.cpp \
        ../../../win_src/exiv2-0.23/src/metadatum.cpp \
        ../../../win_src/exiv2-0.23/src/makernote.cpp \
        ../../../win_src/exiv2-0.23/src/localtime.c \
        ../../../win_src/exiv2-0.23/src/jpgimage.cpp \
        ../../../win_src/exiv2-0.23/src/jp2image.cpp \
        ../../../win_src/exiv2-0.23/src/iptc.cpp \
        ../../../win_src/exiv2-0.23/src/image.cpp \
        ../../../win_src/exiv2-0.23/src/gifimage.cpp \
        ../../../win_src/exiv2-0.23/src/getopt_win32.c \
        ../../../win_src/exiv2-0.23/src/futils.cpp \
        ../../../win_src/exiv2-0.23/src/fujimn.cpp \
        ../../../win_src/exiv2-0.23/src/exif.cpp \
        ../../../win_src/exiv2-0.23/src/error.cpp \
        ../../../win_src/exiv2-0.23/src/epsimage.cpp \
        ../../../win_src/exiv2-0.23/src/easyaccess.cpp \
        ../../../win_src/exiv2-0.23/src/datasets.cpp \
        ../../../win_src/exiv2-0.23/src/crwimage.cpp \
        ../../../win_src/exiv2-0.23/src/cr2image.cpp \
        ../../../win_src/exiv2-0.23/src/convert.cpp \
        ../../../win_src/exiv2-0.23/src/canonmn.cpp \
        ../../../win_src/exiv2-0.23/src/bmpimage.cpp \
        ../../../win_src/exiv2-0.23/src/basicio.cpp

    HEADERS += \
        ../../../win_src/exiv2-0.23/src/xmpsidecar.hpp \
        ../../../win_src/exiv2-0.23/src/xmp.hpp \
        ../../../win_src/exiv2-0.23/src/version.hpp \
        ../../../win_src/exiv2-0.23/src/value.hpp \
        ../../../win_src/exiv2-0.23/src/utils.hpp \
        ../../../win_src/exiv2-0.23/src/tzfile.h \
        ../../../win_src/exiv2-0.23/src/types.hpp \
        ../../../win_src/exiv2-0.23/src/timegm.h \
        ../../../win_src/exiv2-0.23/src/tiffvisitor_int.hpp \
        ../../../win_src/exiv2-0.23/src/tiffimage_int.hpp \
        ../../../win_src/exiv2-0.23/src/tiffimage.hpp \
        ../../../win_src/exiv2-0.23/src/tifffwd_int.hpp \
        ../../../win_src/exiv2-0.23/src/tiffcomposite_int.hpp \
        ../../../win_src/exiv2-0.23/src/tgaimage.hpp \
        ../../../win_src/exiv2-0.23/src/tags_int.hpp \
        ../../../win_src/exiv2-0.23/src/tags.hpp \
        ../../../win_src/exiv2-0.23/src/sonymn_int.hpp \
        ../../../win_src/exiv2-0.23/src/sigmamn_int.hpp \
        ../../../win_src/exiv2-0.23/src/samsungmn_int.hpp \
        ../../../win_src/exiv2-0.23/src/rw2image_int.hpp \
        ../../../win_src/exiv2-0.23/src/rw2image.hpp \
        ../../../win_src/exiv2-0.23/src/rcsid_int.hpp \
        ../../../win_src/exiv2-0.23/src/rafimage.hpp \
        ../../../win_src/exiv2-0.23/src/psdimage.hpp \
        ../../../win_src/exiv2-0.23/src/properties.hpp \
        ../../../win_src/exiv2-0.23/src/private.h \
        ../../../win_src/exiv2-0.23/src/preview.hpp \
        ../../../win_src/exiv2-0.23/src/pngimage.hpp \
        ../../../win_src/exiv2-0.23/src/pngchunk_int.hpp \
        ../../../win_src/exiv2-0.23/src/pgfimage.hpp \
        ../../../win_src/exiv2-0.23/src/pentaxmn_int.hpp \
        ../../../win_src/exiv2-0.23/src/panasonicmn_int.hpp \
        ../../../win_src/exiv2-0.23/src/orfimage_int.hpp \
        ../../../win_src/exiv2-0.23/src/orfimage.hpp \
        ../../../win_src/exiv2-0.23/src/olympusmn_int.hpp \
        ../../../win_src/exiv2-0.23/src/nikonmn_int.hpp \
        ../../../win_src/exiv2-0.23/src/mrwimage.hpp \
        ../../../win_src/exiv2-0.23/src/minoltamn_int.hpp \
        ../../../win_src/exiv2-0.23/src/metadatum.hpp \
        ../../../win_src/exiv2-0.23/src/metacopy.hpp \
        ../../../win_src/exiv2-0.23/src/makernote_int.hpp \
        ../../../win_src/exiv2-0.23/src/jpgimage.hpp \
        ../../../win_src/exiv2-0.23/src/jp2image.hpp \
        ../../../win_src/exiv2-0.23/src/iptc.hpp \
        ../../../win_src/exiv2-0.23/src/image.hpp \
        ../../../win_src/exiv2-0.23/src/i18n.h \
        ../../../win_src/exiv2-0.23/src/gifimage.hpp \
        ../../../win_src/exiv2-0.23/src/getopt_win32.h \
        ../../../win_src/exiv2-0.23/src/futils.hpp \
        ../../../win_src/exiv2-0.23/src/fujimn_int.hpp \
        ../../../win_src/exiv2-0.23/src/exiv2app.hpp \
        ../../../win_src/exiv2-0.23/src/exiv2.hpp \
        ../../../win_src/exiv2-0.23/src/exif.hpp \
        ../../../win_src/exiv2-0.23/src/error.hpp \
        ../../../win_src/exiv2-0.23/src/epsimage.hpp \
        ../../../win_src/exiv2-0.23/src/easyaccess.hpp \
        ../../../win_src/exiv2-0.23/src/datasets.hpp \
        ../../../win_src/exiv2-0.23/src/crwimage_int.hpp \
        ../../../win_src/exiv2-0.23/src/crwimage.hpp \
        ../../../win_src/exiv2-0.23/src/cr2image_int.hpp \
        ../../../win_src/exiv2-0.23/src/cr2image.hpp \
        ../../../win_src/exiv2-0.23/src/convert.hpp \
        ../../../win_src/exiv2-0.23/src/canonmn_int.hpp \
        ../../../win_src/exiv2-0.23/src/bmpimage.hpp \
        ../../../win_src/exiv2-0.23/src/basicio.hpp \
        ../exiv2/exv_msvc.h
}

#---- Libs for windows and linux
LIBS	    += -lSDL                                                        #------ SDL
LIBS        += -lavformat -lavcodec -lavutil -lswscale -lavfilter           #------ libav

#--------------------------------------------------------------
# PROJECT FILES
#--------------------------------------------------------------

# Ressource files
win32:RC_FILE    += ../../ffDiaporama.rc
unix:OTHER_FILES += ../../ffDiaporama.rc
RESOURCES        += ../../RSCffDiaporama.qrc

# Translation files
TRANSLATIONS += ../../locale/ffDiaporama_fr.ts \
    ../../locale/ffDiaporama_it.ts \
    ../../locale/ffDiaporama_de.ts \
    ../../locale/ffDiaporama_nl.ts \
    ../../locale/ffDiaporama_es.ts \
    ../../locale/ffDiaporama_pt.ts \
    ../../locale/ffDiaporama_ru.ts \
    ../../locale/ffDiaporama_el.ts \
    ../../locale/ffDiaporama_cz.ts \
    ../../locale/ffDiaporama_zh_tw.ts

OTHER_FILES += \
    $$TRANSLATIONS \
    ../../TODO-LIST.txt \          # Developpement file
    ../../BUILDVERSION.txt \                  # Developpement file
    ../../ffDiaporama-mime.xml \              # MIME definition of .ffd type
    ../../ffDiaporama.xml \                   # Default configuration options for ffDiaporama
    ../../Devices.xml \                       # Shared default configuration options for devices management
    ../../licences.txt \                      # Licence file
    ../../licences.rtf \                      # Licence file
    ../../authors.txt \                       # Authoring file for ffDiaporama
    ../../ffDiaporama.url \                   # URL file to be install on windows system
    ../../ffDiaporama.desktop \                 # Desktop (menu icon) entry for ffDiaporama installed in /usr
    ../../WIKI/src_en/wiki_en.qhp \
    ../../WIKI/src_en/wiki_en.qhcp \
    ../../WIKI/src_en/main.html \
    ../../WIKI/src_en/0043.html \
    ../../WIKI/src_en/0042.html \
    ../../WIKI/src_en/0041.html \
    ../../WIKI/src_en/0040.html \
    ../../WIKI/src_en/0039.html \
    ../../WIKI/src_en/0038.html \
    ../../WIKI/src_en/0037.html \
    ../../WIKI/src_en/0036.html \
    ../../WIKI/src_en/0035.html \
    ../../WIKI/src_en/0034.html \
    ../../WIKI/src_en/0033.html \
    ../../WIKI/src_en/0032.html \
    ../../WIKI/src_en/0031.html \
    ../../WIKI/src_en/0030.html \
    ../../WIKI/src_en/0024.html \
    ../../WIKI/src_en/0023.html \
    ../../WIKI/src_en/0022.html \
    ../../WIKI/src_en/0021.html \
    ../../WIKI/src_en/0020.html \
    ../../WIKI/src_en/0016.html \
    ../../WIKI/src_en/0015.html \
    ../../WIKI/src_en/0014.html \
    ../../WIKI/src_en/0011.html \
    ../../WIKI/src_en/0010.html \
    ../../WIKI/src_en/0101.html \
    ../../WIKI/src_en/0102.html \
    ../../WIKI/wiki.css \
    ../../WIKI/src_en/0012.html \
    ../../WIKI/src_en/003A.html \
    ../../WIKI/src_en/0044.html \
    ../../WIKI/src_en/0103.html \
    ../../WIKI/src_en/0104.html \
    ../../WIKI/src_en/0105.html \
    ../../WIKI/src_en/0106.html \
    ../../WIKI/src_en/0107.html \
    ../../WIKI/src_en/0108.html \
    ../../WIKI/src_en/0109.html \
    ../../WIKI/src_en/0110.html \
    ../../WIKI/src_en/0111.html \
    ../../WIKI/src_en/0112.html \
    ../../WIKI/src_en/0113.html \
    ../../WIKI/src_en/0114.html \
    ../../WIKI/src_en/0115.html \
    ../../WIKI/src_en/0116.html \
    ../../WIKI/src_en/0117.html \
    ../../WIKI/src_en/0118.html \
    ../../WIKI/src_en/0119.html \
    ../../WIKI/src_en/0120.html \
    ../../WIKI/src_en/0121.html \
    ../../WIKI/src_en/0122.html \
    ../../WIKI/UpdateTSFiles.bash \
    ../../WIKI/UpdateQMFiles.bash \
    ../../WIKI/readme.txt \
    ../../WIKI/UpdateTSFiles.cmd \
    ../../WIKI/UpdateQMFiles.cmd \
    ../../WIKI/PREPLANGUAGE.cmd \
    ../../WIKI/src_en/0013.html \
    ../../readme.txt \
    ../../translation.txt \
    ../../changelog-fr.txt \
    ../../changelog-en.txt

# Source files
SOURCES +=  wgt_QVideoPlayer.cpp \
            mainwindow.cpp \
            main.cpp \
            cCustomSlideTable.cpp \
            DlgRenderVideo/DlgRenderVideo.cpp \
            DlgManageStyle/DlgManageStyle.cpp \
            DlgAbout/DlgAbout.cpp \
            DlgTransition/DlgTransitionProperties.cpp \
            DlgMusic/DlgMusicProperties.cpp \
            DlgBackground/DlgBackgroundProperties.cpp \
            DlgAppSettings/DlgManageDevices/DlgManageDevices.cpp \
            DlgAppSettings/DlgApplicationSettings.cpp \
            DlgImage/cImgInteractiveZone.cpp \
            DlgImage/DlgImageCorrection.cpp \
            DlgSlide/cCustomBlockTable.cpp \
            DlgSlide/DlgSlideProperties.cpp \
            DlgSlide/cInteractiveZone.cpp \
            DlgSlide/cCustomShotTable.cpp \
            DlgSlide/DlgRuler/DlgRulerDef.cpp \
            DlgSlide/DlgSlideDuration.cpp \
            DlgSlide/DlgImageComposer.cpp \
            DlgText/cCustomTextEdit.cpp \
            DlgText/DlgTextEdit.cpp \
            DlgCheckConfig/DlgCheckConfig.cpp \
            DlgInfoFile/DlgInfoFile.cpp \
            DlgffDPjrProperties/DlgffDPjrProperties.cpp \
            DlgManageFavorite/DlgManageFavorite.cpp \
            DlgWorkingTask/DlgWorkingTask.cpp \
            DlgTransition/DlgTransitionDuration.cpp \
            DlgFileExplorer/DlgFileExplorer.cpp \
            DlgChapter/DlgChapter.cpp \
            DlgAutoTitleSlide/DlgAutoTitleSlide.cpp \
            DlgExportProject/DlgExportProject.cpp \
            HelpPopup/HelpPopup.cpp \
            HelpPopup/HelpBrowser.cpp \
            ../engine/_GlobalDefines.cpp \
            ../engine/QCustomRuller.cpp \
            ../engine/cSaveWindowPosition.cpp \
            ../engine/cBaseApplicationConfig.cpp \
            ../engine/cLuLoImageCache.cpp \
            ../engine/cDeviceModelDef.cpp \
            ../engine/cSoundBlockList.cpp \
            ../engine/cBaseMediaFile.cpp \
            ../engine/_SDL_Support.cpp \
            ../engine/_ImageFilters.cpp \
            ../engine/cBrushDefinition.cpp \
            ../engine/cCustomIcon.cpp \
            ../engine/cDriveList.cpp \
            ../engine/_Transition.cpp \
            ../engine/_SpeedWave.cpp \
            ../engine/_Shape.cpp \
            ../engine/cTextFrame.cpp \
            ../engine/_EncodeVideo.cpp \
            ../engine/_StyleDefinitions.cpp \
            ../engine/_Diaporama.cpp \
            ../engine/_Variables.cpp \
            ../engine/_Model.cpp \
            ../engine/cDatabase.cpp \
            ../CustomCtrl/_QCustomDialog.cpp \
            ../CustomCtrl/cCColorComboBox.cpp \
            ../CustomCtrl/cCBrushComboBox.cpp \
            ../CustomCtrl/cCSpeedWaveComboBox.cpp \
            ../CustomCtrl/cCGrdOrientationComboBox.cpp \
            ../CustomCtrl/cCFramingComboBox.cpp \
            ../CustomCtrl/cCShapeComboBox.cpp \
            ../CustomCtrl/cBackgroundComboBox.cpp \
            ../CustomCtrl/QCustomFolderTable.cpp \
            ../CustomCtrl/QCustomHorizSplitter.cpp \
            ../CustomCtrl/QCustomFolderTree.cpp \
            ../CustomCtrl/cCTexteFrameComboBox.cpp \
            ../CustomCtrl/cThumbnailComboBox.cpp \
            ../CustomCtrl/cCustomTitleModelTable.cpp \
            ../CustomCtrl/_QCustomComboBox.cpp \
            HelpPopup/HelpContent.cpp

# Header files
HEADERS  += wgt_QVideoPlayer.h \
            cCustomSlideTable.h \
            mainwindow.h \
            DlgRenderVideo/DlgRenderVideo.h \
            DlgManageStyle/DlgManageStyle.h \
            DlgAbout/DlgAbout.h \
            DlgTransition/DlgTransitionProperties.h \
            DlgMusic/DlgMusicProperties.h \
            DlgBackground/DlgBackgroundProperties.h \
            DlgAppSettings/DlgManageDevices/DlgManageDevices.h \
            DlgAppSettings/DlgApplicationSettings.h \
            DlgImage/cImgInteractiveZone.h \
            DlgImage/DlgImageCorrection.h \
            DlgSlide/DlgSlideProperties.h \
            DlgSlide/cCustomBlockTable.h \
            DlgSlide/cInteractiveZone.h \
            DlgSlide/cCustomShotTable.h \
            DlgSlide/DlgRuler/DlgRulerDef.h \
            DlgSlide/DlgSlideDuration.h \
            DlgSlide/DlgImageComposer.h \
            DlgText/cCustomTextEdit.h \
            DlgText/DlgTextEdit.h \
            DlgCheckConfig/DlgCheckConfig.h \
            DlgInfoFile/DlgInfoFile.h \
            DlgffDPjrProperties/DlgffDPjrProperties.h \
            DlgManageFavorite/DlgManageFavorite.h \
            DlgWorkingTask/DlgWorkingTask.h \
            DlgTransition/DlgTransitionDuration.h \
            DlgFileExplorer/DlgFileExplorer.h \
            DlgChapter/DlgChapter.h \
            DlgAutoTitleSlide/DlgAutoTitleSlide.h \
            DlgExportProject/DlgExportProject.h \
            HelpPopup/HelpPopup.h \
            HelpPopup/HelpBrowser.h \
            ../engine/QCustomRuller.h \
            ../engine/cSaveWindowPosition.h \
            ../engine/cBaseApplicationConfig.h \
            ../engine/cLuLoImageCache.h \
            ../engine/cDeviceModelDef.h \
            ../engine/_GlobalDefines.h \
            ../engine/cSoundBlockList.h \
            ../engine/cBaseMediaFile.h \
            ../engine/_SDL_Support.h \
            ../engine/_ImageFilters.h \
            ../engine/cBrushDefinition.h \
            ../engine/cCustomIcon.h \
            ../engine/cDriveList.h \
            ../engine/_Transition.h \
            ../engine/_SpeedWave.h \
            ../engine/_Shape.h \
            ../engine/cTextFrame.h \
            ../engine/_EncodeVideo.h \
            ../engine/_StyleDefinitions.h \
            ../engine/_Diaporama.h \
            ../engine/_Variables.h \
            ../engine/_Model.h \
            ../engine/cDatabase.h \
            ../CustomCtrl/_QCustomDialog.h \
            ../CustomCtrl/cCColorComboBox.h \
            ../CustomCtrl/cCBrushComboBox.h \
            ../CustomCtrl/cCSpeedWaveComboBox.h \
            ../CustomCtrl/cCGrdOrientationComboBox.h \
            ../CustomCtrl/cCFramingComboBox.h \
            ../CustomCtrl/cCShapeComboBox.h \
            ../CustomCtrl/cBackgroundComboBox.h \
            ../CustomCtrl/QCustomFolderTable.h \
            ../CustomCtrl/QCustomHorizSplitter.h \
            ../CustomCtrl/QCustomFolderTree.h \
            ../CustomCtrl/cCTexteFrameComboBox.h \
            ../CustomCtrl/cThumbnailComboBox.h \
            ../CustomCtrl/cCustomTitleModelTable.h \
            ../CustomCtrl/_QCustomComboBox.h \
            HelpPopup/HelpContent.h

# Forms files
FORMS    += mainwindow.ui \
            wgt_QVideoPlayer.ui \
            DlgRenderVideo/DlgRenderVideo.ui \
            DlgManageStyle/DlgManageStyle.ui \
            DlgAbout/DlgAbout.ui \
            DlgTransition/DlgTransitionProperties.ui \
            DlgMusic/DlgMusicProperties.ui \
            DlgBackground/DlgBackgroundProperties.ui \
            DlgAppSettings/DlgManageDevices/DlgManageDevices.ui \
            DlgAppSettings/DlgApplicationSettings.ui \
            DlgImage/DlgImageCorrection.ui \
            DlgSlide/DlgSlideProperties.ui \
            DlgSlide/DlgRuler/DlgRulerDef.ui \
            DlgSlide/DlgImageComposer.ui \
            DlgText/DlgTextEdit.ui \
            DlgCheckConfig/DlgCheckConfig.ui \
            DlgInfoFile/DlgInfoFile.ui \
            DlgffDPjrProperties/DlgffDPjrProperties.ui \ 
            DlgManageFavorite/DlgManageFavorite.ui \
            DlgWorkingTask/DlgWorkingTask.ui \
            DlgTransition/DlgTransitionDuration.ui \
            DlgSlide/DlgSlideDuration.ui \
            DlgFileExplorer/DlgFileExplorer.ui \
            DlgChapter/DlgChapter.ui \
            DlgAutoTitleSlide/DlgAutoTitleSlide.ui \
            DlgExportProject/DlgExportProject.ui \
            HelpPopup/HelpPopup.ui

#--------------------------------------------------------------
# INSTALLATION
#--------------------------------------------------------------
message("Install to : $$PREFIX")

TARGET.path         = $$PREFIX/bin
TARGET.files        = $$TARGET
INSTALLS 	    += TARGET

ico.path            = $$PREFIX/share/icons/hicolor/32x32/apps
ico.files           = ../../ffdiaporama.png
INSTALLS 	    += ico

desktop.path        = $$PREFIX/share/applications
desktop.files       = ../../ffDiaporama.desktop
INSTALLS 	    += desktop

mimefile.path       = $$PREFIX/share/mime/packages
mimefile.files      = ../../ffDiaporama-mime.xml
INSTALLS 	    += mimefile

translation.path    = $$PREFIX/share/$$APPFOLDER/locale
translation.files   = ../../locale/ffDiaporama_* \
                      ../../locale/wiki_* \
                      ../../locale/qt_*
INSTALLS 	    += translation

background.path     = $$PREFIX/share/$$APPFOLDER/background
background.files    = ../../background/*.*
INSTALLS 	    += background

clipart.path        = $$PREFIX/share/$$APPFOLDER/clipart
clipart.files       = ../../clipart/*.*
ffdclipart.path     = $$PREFIX/share/$$APPFOLDER/clipart/ffDiaporama
ffdclipart.files    = ../../clipart/ffDiaporama/*.*
INSTALLS 	    += ffdclipart clipart

model.path          = $$PREFIX/share/$$APPFOLDER/model
model.files         = ../../model/*.*
model_thumb.path    = $$PREFIX/share/$$APPFOLDER/model/Thumbnails
model_thumb.files   = ../../model/Thumbnails/*.*
model_tts.path      = $$PREFIX/share/$$APPFOLDER/model/Titles
model_tts.files     = ../../model/Titles/*.*
model_tts43.path    = $$PREFIX/share/$$APPFOLDER/model/Titles/4_3
model_tts43.files   = ../../model/Titles/4_3/*.*
model_tts169.path   = $$PREFIX/share/$$APPFOLDER/model/Titles/16_9
model_tts169.files  = ../../model/Titles/16_9/*.*
model_tts4017.path  = $$PREFIX/share/$$APPFOLDER/model/Titles/40_17
model_tts4017.files = ../../model/Titles/40_17/*.*
model_fat.path      = $$PREFIX/share/$$APPFOLDER/model/FanArts
model_fat.files     = ../../model/FanArts/*.*
INSTALLS 	    += model_thumb model_tts model_tts43 model_tts169 model_tts4017 model_fat model

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
INSTALLS 	    += luma_Bar luma_Box luma_Center luma_Checker luma_Clock luma_Snake luma

General.path        = $$PREFIX/share/$$APPFOLDER
General.files       = ../../*.xml ../../*.txt ../../*.rtf
INSTALLS            += General
