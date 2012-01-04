QT          += core gui xml
CONFIG      += console
CONFIG      += thread
QMAKE_STRIP  = echo
APPFOLDER    = ffDiaporama
TARGET       = ffDiaporamaMMFiler
TEMPLATE     = app

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
    ../VariousWidgets/DlgInfoFile.cpp

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
    ../VariousWidgets/DlgInfoFile.h

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
                   /usr/include/ffmpeg/    \                            # Specific for Fedora
                   ../VariousClass  \
                   ../fmt_filters

    LIBS        += -lavformat -lavcodec -lavutil -lswscale
}
win32 {
    DESTDIR     += ..\..\..\winbuild
    OBJECTS_DIR += ..\winbuild$$TARGET
    MOC_DIR     += ..\winbuild$$TARGET
    UI_DIR      += ..\winbuild$$TARGET

    RC_FILE     = ffDiaporamaMMFiler.rc

    INCLUDEPATH += . \
                   ..\\winbuild$$TARGET \
                   C:\\Qt\\ffmpeg-win32-dev\\include \                  #------ ffmpeg library path
                   C:\\Qt\\SDL-1.2.14\\include \                        #------ SDL library path
                   ..\\VariousClass \
                   ..\\fmt_filters

    LIBS        +=  -L"C:\\Qt\\ffmpeg-win32-dev\\lib" \                  #------ ffmpeg library path
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
