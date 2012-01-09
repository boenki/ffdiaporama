TEMPLATE        = subdirs
CONFIG          += ordered
APPFOLDER       = ffDiaporama
SUBDIRS         += src/ffDiaporama          # ffDiaporama main application
win32:SUBDIRS   += src/ffDiaporamaStart     # ffDiaporama start application (windows only)
SUBDIRS         += src/ffDiaporamaMMFiler   # ffDiaporama MMFiler companion application

OTHER_FILES += TODO-LIST.txt \          # Developpement file
    BUILDVERSION.txt \                  # Developpement file
    ffDiaporama-mime.xml \              # MIME definition of .ffd type
    ffDiaporama.xml \                   # Default configuration options for ffDiaporama
    ffDiaporamaMMFiler.xml \            # Default configuration options for ffDiaporamaMMFiler
    Devices.xml \                       # Shared default configuration options for devices management
    libx264-hq.ffpreset \               # h264 custom preset for High Quality
    libx264-pq.ffpreset \               # h264 custom preset for Phone Quality
    licences.txt \                      # Licence file
    licences.rtf \                      # Licence file
    authors-ffDiaporama.txt \           # Authoring file for ffDiaporama
    authors-ffDiaporamaMMFiler.txt \    # Authoring file for ffDiaporamaMMFiler
    ffDiaporama.url \                   # URL file to be install on windows system
    ffDiaporama.desktop \               # Desktop (menu icon) entry for ffDiaporama installed in /usr
    ffDiaporamaopt.desktop \            # Desktop (menu icon) entry for ffDiaporama installed in /opt
    ffDiaporamalocal.desktop \          # Desktop (menu icon) entry for ffDiaporama installed in /usr/local
    ffDiaporamaMMFiler.desktop \        # Desktop (menu icon) entry for ffDiaporamaMMFiler installed in /usr
    ffDiaporamaMMFileropt.desktop \     # Desktop (menu icon) entry for ffDiaporamaMMFiler installed in /opt
    ffDiaporamaMMFilerlocal.desktop     # Desktop (menu icon) entry for ffDiaporamaMMFiler installed in /usr/local

# Installation on linux systems
unix {
    !exists($$INSTALL_ROOT$$DEST_DIR) {
        DEST_DIR = /usr
    }

    message("Install to : $$INSTALL_ROOT$$DEST_DIR")

    TARGET.path         = $$INSTALL_ROOT$$DEST_DIR/bin
    TARGET.files        = ../build/ffDiaporama ../build/ffDiaporamaMMFiler
    INSTALLS 		+= TARGET

    translation.path    = $$INSTALL_ROOT$$DEST_DIR/share/$$APPFOLDER/locale
    translation.files   = locale/*.*
    INSTALLS 		+= translation

    background.path     = $$INSTALL_ROOT$$DEST_DIR/share/$$APPFOLDER/background
    background.files    = background/*.*
    INSTALLS 		+= background

    img.path            = $$INSTALL_ROOT$$DEST_DIR/share/$$APPFOLDER/img
    img.files           = img/*.*
    INSTALLS 		+= img

    tr_img.path         = $$INSTALL_ROOT$$DEST_DIR/share/$$APPFOLDER/transitions-img
    tr_img.files        = transitions-img/*.*
    INSTALLS 		+= tr_img

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
    INSTALLS 		+= luma luma_Bar luma_Box luma_Center luma_Checker luma_Clock luma_Snake

    General.path        = $$INSTALL_ROOT$$DEST_DIR/share/$$APPFOLDER
    General.files       = *.xml *.txt *.rtf *.ffpreset
    INSTALLS 		+= General

    mimefile.path       = /usr/share/mime/packages
    mimefile.files      = ffDiaporama-mime.xml
    INSTALLS 		+= mimefile

    # install icon files for GNOME systems
    exists(/usr/share/icons/gnome/32x32/mimetypes) {
        iconfile.path   = /usr/share/icons/gnome/32x32/mimetypes
        iconfile.files  = img/application-ffDiaporama.png img/application-ffDiaporamaMMFiler.png
    }
    # install icon files for KDE systems
    exists(/usr/share/icons/default.kde4/32x32/mimetypes) {
        iconfile.path   = /usr/share/icons/default.kde4/32x32/mimetypes
        iconfile.files  = img/application-ffDiaporama.png img/application-ffDiaporamaMMFiler.png
    }
    INSTALLS 		+= iconfile

    # install desktop files /opt version
    contains(INSTALL_ROOT,/opt) {
        desktop.path    = /usr/share/applications
        desktop.files   = ffDiaporamaopt.desktop ffDiaporamaMMFileropt.desktop
    }
    # install desktop files /usr/local version
    contains(INSTALL_ROOT,/usr/local) {
        desktop.path    = /usr/share/applications
        desktop.files   = ffDiaporamalocal.desktop ffDiaporamaMMFilerlocal.desktop
    }
    # install desktop files /usr version
    !contains(INSTALL_ROOT,/usr/local) : !contains(INSTALL_ROOT,/opt) {
        desktop.path    = /usr/share/applications
        desktop.files   = ffDiaporama.desktop ffDiaporamaMMFiler.desktop
    }
    INSTALLS 		+= desktop
}
