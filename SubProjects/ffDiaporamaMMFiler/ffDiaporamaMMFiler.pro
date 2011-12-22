QT          += core gui xml
CONFIG      += console
CONFIG      += thread
QMAKE_STRIP  = echo
APPFOLDER    = ffDiaporama
TARGET       = ffDiaporamaMMFiler

SOURCES += main.cpp\
        mainwindow.cpp \
    ../VariousClass/cSaveWindowPosition.cpp \
    ../VariousClass/cBaseApplicationConfig.cpp \
    ../VariousClass/QCustomFolderTree.cpp \
    ../VariousClass/QCustomFolderTable.cpp

HEADERS  += mainwindow.h \
    ../VariousClass/cSaveWindowPosition.h \
    ../VariousClass/cBaseApplicationConfig.h \
    ../VariousClass/QCustomFolderTree.h \
    ../VariousClass/QCustomFolderTable.h

FORMS    += mainwindow.ui

unix {
    DESTDIR     += ../../../build
    OBJECTS_DIR += ../build$$TARGET
    MOC_DIR     += ../build$$TARGET
    INCLUDEPATH += . \
                   ../VariousClass \
}
win32 {
    DESTDIR     += ..\..\..\winbuild
    OBJECTS_DIR += ..\winbuild$$TARGET
    MOC_DIR     += ..\winbuild$$TARGET
    LIBS        += -lgdi32
    INCLUDEPATH += . \
                   ..\\VariousClass \
}

TRANSLATIONS += locale/locale_fr.ts #\
    #locale/locale_it.ts \
    #locale/locale_de.ts \
    #locale/locale_nl.ts \
    #locale/locale_es.ts \
    #locale/locale_pt.ts \
    #locale/locale_ru.ts \
    #locale/locale_el.ts

OTHER_FILES += ffDiaporamaMMFiler.xml \
    authors-ffDiaporamaMMFiler.txt \
    ffDiaporamaMMFiler.desktop \
    ffDiaporamaMMFileropt.desktop \
    ffDiaporamaMMFilerlocal.desktop \
    licence.rtf \
    licences.txt

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
    img.path            = $$INSTALL_ROOT$$DEST_DIR/share/$$APPFOLDER/img
    img.files           = img/*.*
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
