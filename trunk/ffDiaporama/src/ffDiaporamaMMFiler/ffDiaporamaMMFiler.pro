QT           += core gui xml
#win32:CONFIG += console
CONFIG       += thread
QMAKE_STRIP  = echo
APPFOLDER    = ffDiaporama
TARGET       = ffDiaporamaMMFiler
TEMPLATE     = app

DEFINES      += HAVE_CONFIG_H               # specific for TAGLib
DEFINES      += TAGLIB_STATIC               # specific for TAGLib

isEmpty(PREFIX) {
    PREFIX = /usr
}

#--------------------------------------------------------------
# DEFINES BUILD DIRECTORIES, COMMON INCLUDES AND COMMON LIBS
#--------------------------------------------------------------
include(../common.pri)
#--------------------------------------------------------------

# Ressource files
win32:RC_FILE    += ../../ffDiaporamaMMFiler.rc
unix:OTHER_FILES += ../../ffDiaporamaMMFiler.rc
RESOURCES        += ../../RSCffDiaporama.qrc

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
    ../sharedfiles/_GlobalDefines.cpp \
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
    ../sharedfiles/cCustomIcon.cpp \
    ../sharedfiles/DlgInfoFile.cpp \
    ../sharedfiles/QCustomGetFolder.cpp \
    cJobQueue.cpp \
    QCustomJobTable.cpp \
    DlgJobSettings.cpp \
    wgt_JobConvertImage.cpp \
    wgt_JobBase.cpp

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
    ../sharedfiles/cCustomIcon.h \
    ../sharedfiles/DlgManageDevices.h \
    ../sharedfiles/DlgInfoFile.h \
    ../sharedfiles/QCustomGetFolder.h \
    cJobQueue.h \
    QCustomJobTable.h \
    DlgJobSettings.h \
    wgt_JobConvertImage.h \
    wgt_JobBase.h


FORMS    += mainwindow.ui \
    DlgApplicationSettings.ui \
    DlgAbout.ui \
    ../sharedfiles/DlgCheckConfig.ui \
    ../sharedfiles/DlgManageDevices.ui \
    ../sharedfiles/DlgInfoFile.ui \
    ../sharedfiles/QCustomGetFolder.ui \
    DlgJobSettings.ui \
    wgt_JobConvertImage.ui

# Installation on linux systems
unix {
    message("Install to : $$PREFIX")

    TARGETF.path         = $$PREFIX/bin
    TARGETF.files        = $$DESTDIR/$$TARGET
    INSTALLS 		+= TARGETF

    translation.path    = $$PREFIX/share/$$APPFOLDER/locale
    translation.files   = ../../locale/ffDiaporamaMMFiler_*
    INSTALLS 		+= translation

    ico.path            = $$PREFIX/share/$$APPFOLDER
    ico.files           = ../../application-ffDiaporamaMMFiler.png \
                          ../../application-ffDiaporamaMMFiler.ico
    INSTALLS 		+= ico

    # install icon files for GNOME systems
    #exists(/usr/share/icons/gnome/32x32/mimetypes) {
    #    iconfile.path   = $$PREFIX/share/icons/gnome/32x32/mimetypes
    #    iconfile.files  = ../../application-ffDiaporamaMMFiler.png
    #}
    # install icon files for KDE systems
    #exists(/usr/share/icons/default.kde4/32x32/mimetypes) {
    #    iconfile.path   = $$PREFIX/share/icons/default.kde4/32x32/mimetypes
    #    iconfile.files  = ../../application-ffDiaporamaMMFiler.png
    #}
    #INSTALLS 		+= iconfile

    # install desktop files /opt version
    contains(PREFIX,/opt) {
        desktop.path    = $$PREFIX/share/applications
        desktop.files   = ../../ffDiaporamaMMFileropt.desktop
    }
    # install PREFIX files /usr/local version
    contains(PREFIX,/usr/local) {
        desktop.path    = $$PREFIX/share/applications
        desktop.files   = ../../ffDiaporamaMMFilerlocal.desktop
    }
    # install desktop files /usr version
    !contains(PREFIX,/usr/local) : !contains(PREFIX,/opt) {
        desktop.path    = $$PREFIX/share/applications
        desktop.files   = ../../ffDiaporamaMMFiler.desktop
    }
    INSTALLS 		+= desktop

}
