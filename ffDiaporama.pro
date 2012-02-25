# this file is to be used by qmake for Windows & Linux
TEMPLATE        = subdirs
//CONFIG          += ordered

# ffDiaporama main application
SUBDIRS         += src/ffDiaporama
TARGET          += ffDiaporama

# ffDiaporama start application (windows only)
win32:SUBDIRS   += src/ffDiaporamaStart

OTHER_FILES += TODO-LIST.txt \          # Developpement file
    BUILDVERSION.txt \                  # Developpement file
    ffDiaporama-mime.xml \              # MIME definition of .ffd type
    ffDiaporama.xml \                   # Default configuration options for ffDiaporama
    Devices.xml \                       # Shared default configuration options for devices management
    libx264-hq.ffpreset \               # h264 custom preset for High Quality
    libx264-pq.ffpreset \               # h264 custom preset for Phone Quality
    licences.txt \                      # Licence file
    licences.rtf \                      # Licence file
    authors.txt \                       # Authoring file for ffDiaporama
    ffDiaporama.url \                   # URL file to be install on windows system
    ffDiaporama.desktop \               # Desktop (menu icon) entry for ffDiaporama installed in /usr
    ffDiaporamaopt.desktop \            # Desktop (menu icon) entry for ffDiaporama installed in /opt
    ffDiaporamalocal.desktop            # Desktop (menu icon) entry for ffDiaporama installed in /usr/local

