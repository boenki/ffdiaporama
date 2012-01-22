TEMPLATE        = subdirs
CONFIG          += ordered
APPFOLDER       = ffDiaporama

# ffDiaporama main application
SUBDIRS         += src/ffDiaporama
TARGET          += ffDiaporama

# ffDiaporama MMFiler companion application
SUBDIRS         += src/ffDiaporamaMMFiler
TARGET          += ffDiaporamaMMFiler

# ffDiaporama start application (windows only)
win32:SUBDIRS   += src/ffDiaporamaStart

DESTDIR += ../build

isEmpty(PREFIX) {
    PREFIX = /usr
}

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
    message("Install to : $$INSTALL_ROOT$$PREFIX")

    TARGET.path          = $$INSTALL_ROOT$$PREFIX/bin
    TARGET.files         = ../build/ffDiaporama ../build/ffDiaporamaMMFiler
    PostInstall.path     = /
    PostInstall.extra    = chmod 755 $$INSTALL_ROOT$$PREFIX/bin/ffDiaporama & chmod 755 $$INSTALL_ROOT$$PREFIX/bin/ffDiaporamaMMFiler
    INSTALLS 		+= TARGET PostInstall

    translation.path    = $$INSTALL_ROOT$$PREFIX/share/$$APPFOLDER/locale
    translation.files   = locale/*.*
    INSTALLS 		+= translation

    background.path     = $$INSTALL_ROOT$$PREFIX/share/$$APPFOLDER/background
    background.files    = background/*.*
    INSTALLS 		+= background

    mediaic.path        = $$INSTALL_ROOT$$PREFIX/share/$$APPFOLDER/img/MediaIcons
    mediaic.files       = img/MediaIcons/*.*
    ic16x16.path        = $$INSTALL_ROOT$$PREFIX/share/$$APPFOLDER/img/MediaIcons/16x16
    ic16x16.files       = img/MediaIcons/16x16/*.*
    ic32x32.path        = $$INSTALL_ROOT$$PREFIX/share/$$APPFOLDER/img/MediaIcons/32x32
    ic32x32.files       = img/MediaIcons/32x32/*.*
    ic48x48.path        = $$INSTALL_ROOT$$PREFIX/share/$$APPFOLDER/img/MediaIcons/48x48
    ic48x48.files       = img/MediaIcons/48x48/*.*
    ic100x100.path      = $$INSTALL_ROOT$$PREFIX/share/$$APPFOLDER/img/MediaIcons/100x100
    ic100x100.files     = img/MediaIcons/100x100/*.*
    ic200x200.path      = $$INSTALL_ROOT$$PREFIX/share/$$APPFOLDER/img/MediaIcons/200x200
    ic200x200.files     = img/MediaIcons/200x200/*.*
    INSTALLS 		+= ic16x16 ic32x32 ic48x48 ic100x100 ic200x200 mediaic

    img.path            = $$INSTALL_ROOT$$PREFIX/share/$$APPFOLDER/img
    img.files           = img/*.*
    INSTALLS 		+= img

    tr_img.path         = $$INSTALL_ROOT$$PREFIX/share/$$APPFOLDER/transitions-img
    tr_img.files        = transitions-img/*.*
    INSTALLS 		+= tr_img

    luma.path           = $$INSTALL_ROOT$$PREFIX/share/$$APPFOLDER/luma
    luma.files          = luma/*.*
    luma_Bar.path       = $$INSTALL_ROOT$$PREFIX/share/$$APPFOLDER/luma/Bar
    luma_Bar.files      = luma/Bar/*.*
    luma_Box.path       = $$INSTALL_ROOT$$PREFIX/share/$$APPFOLDER/luma/Box
    luma_Box.files      = luma/Box/*.*
    luma_Center.path    = $$INSTALL_ROOT$$PREFIX/share/$$APPFOLDER/luma/Center
    luma_Center.files   = luma/Center/*.*
    luma_Checker.path   = $$INSTALL_ROOT$$PREFIX/share/$$APPFOLDER/luma/Checker
    luma_Checker.files  = luma/Checker/*.*
    luma_Clock.path     = $$INSTALL_ROOT$$PREFIX/share/$$APPFOLDER/luma/Clock
    luma_Clock.files    = luma/Clock/*.*
    luma_Snake.path     = $$INSTALL_ROOT$$PREFIX/share/$$APPFOLDER/luma/Snake
    luma_Snake.files    = luma/Snake/*.*
    INSTALLS 		+= luma_Bar luma_Box luma_Center luma_Checker luma_Clock luma_Snake luma

    General.path        = $$INSTALL_ROOT$$PREFIX/share/$$APPFOLDER
    General.files       = *.xml *.txt *.rtf *.ffpreset
    INSTALLS 		+= General

    mimefile.path       = $$INSTALL_ROOT$$PREFIX/share/mime/packages
    mimefile.files      = ffDiaporama-mime.xml
    INSTALLS 		+= mimefile

    # install icon files for GNOME systems
    exists(/usr/share/icons/gnome/32x32/mimetypes) {
        iconfile.path   = $$INSTALL_ROOT$$PREFIX/share/icons/gnome/32x32/mimetypes
        iconfile.files  = img/application-ffDiaporama.png img/application-ffDiaporamaMMFiler.png
    }
    # install icon files for KDE systems
    exists(/usr/share/icons/default.kde4/32x32/mimetypes) {
        iconfile.path   = $$INSTALL_ROOT$$PREFIX/share/icons/default.kde4/32x32/mimetypes
        iconfile.files  = img/application-ffDiaporama.png img/application-ffDiaporamaMMFiler.png
    }
    INSTALLS 		+= iconfile

    # install desktop files /opt version
    contains(PREFIX,/opt) {
        desktop.path    = $$INSTALL_ROOT$$PREFIX/share/applications
        desktop.files   = ffDiaporamaopt.desktop ffDiaporamaMMFileropt.desktop
    }
    # install PREFIX files /usr/local version
    contains(PREFIX,/usr/local) {
        desktop.path    = $$INSTALL_ROOT$$PREFIX/share/applications
        desktop.files   = ffDiaporamalocal.desktop ffDiaporamaMMFilerlocal.desktop
    }
    # install desktop files /usr version
    !contains(PREFIX,/usr/local) : !contains(PREFIX,/opt) {
        desktop.path    = $$INSTALL_ROOT$$PREFIX/share/applications
        desktop.files   = ffDiaporama.desktop ffDiaporamaMMFiler.desktop
    }
    INSTALLS 		+= desktop

}
