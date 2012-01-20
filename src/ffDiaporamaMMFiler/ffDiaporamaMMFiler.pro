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
    ../sharedfiles/DlgInfoFile.cpp

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
    ../sharedfiles/DlgInfoFile.h


FORMS    += mainwindow.ui \
    DlgApplicationSettings.ui \
    DlgAbout.ui \
    ../sharedfiles/DlgCheckConfig.ui \
    ../sharedfiles/DlgManageDevices.ui \
    ../sharedfiles/DlgInfoFile.ui



