QT           += core gui xml network
QMAKE_STRIP  = echo
TARGET       = sharedfiles
TEMPLATE     = lib

#--------------------------------------------------------------
# DEFINES BUILD DIRECTORIES, COMMON INCLUDES AND COMMON LIBS
#--------------------------------------------------------------
include(../common.pri)
#--------------------------------------------------------------

SOURCES +=  ../sharedfiles/DlgCheckConfig.cpp \
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
    ../sharedfiles/DlgInfoFile.cpp

HEADERS  += ../sharedfiles/DlgCheckConfig.h \
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
    ../sharedfiles/DlgInfoFile.h


FORMS    += ../sharedfiles/DlgCheckConfig.ui \
    ../sharedfiles/DlgManageDevices.ui \
    ../sharedfiles/DlgffDPjrProperties.ui \
    ../sharedfiles/DlgInfoFile.ui
