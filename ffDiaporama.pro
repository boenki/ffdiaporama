# this file is to be used by qmake for Windows & Linux
TEMPLATE        = subdirs

# ffDiaporama main application
SUBDIRS         += src/ffDiaporama
TARGET          += ffDiaporama

OTHER_FILES += TODO-LIST.txt \          # Developpement file
    BUILDVERSION.txt \                  # Developpement file
    ffDiaporama-mime.xml \              # MIME definition of .ffd type
    ffDiaporama.xml \                   # Default configuration options for ffDiaporama
    Devices.xml \                       # Shared default configuration options for devices management
    licences.txt \                      # Licence file
    licences.rtf \                      # Licence file
    authors.txt \                       # Authoring file for ffDiaporama
    ffDiaporama.url \                   # URL file to be install on windows system
    ffDiaporama.desktop                 # Desktop (menu icon) entry for ffDiaporama installed in /usr

CONFIG(release, debug|release) {
    DESTDIR += ../build
}

CONFIG(debug, debug|release) {
    DESTDIR +=../debugbuild
    DEFINES +=DEBUG_MODE
}

OBJECTS_DIR     += $$DESTDIR
MOC_DIR         += $$DESTDIR
RCC_DIR         += $$DESTDIR
UI_DIR          += $$DESTDIR
UI_HEADERS_DIR  += $$DESTDIR
UI_SOURCES_DIR  += $$DESTDIR
INCLUDEPATH     += $$DESTDIR

