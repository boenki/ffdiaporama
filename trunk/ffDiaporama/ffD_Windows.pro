# this file is to be used by Qt Creator for Windows
TEMPLATE        = subdirs

# ffDiaporama main application
SUBDIRS         += src/ffDiaporama
TARGET          += ffDiaporama

OTHER_FILES += TODO-LIST.txt \          # Developpement file
    BUILDVERSION.txt \                  # Developpement file
    ffDiaporama-mime.xml \              # MIME definition of .ffd type
    ffDiaporama.xml \                   # Default configuration options for ffDiaporama
    ffDiaporamaMMFiler.xml \            # Default configuration options for ffDiaporamaMMFiler
    Devices.xml \                       # Shared default configuration options for devices management
    libx264-hq.ffpreset \               # h264 custom preset for High Quality
    libx264-pq.ffpreset \               # h264 custom preset for Phone Quality
    libx264-hq-10.ffpreset \            # h264 custom preset for High Quality - ffmpeg 0.10 version
    libx264-pq-10.ffpreset \            # h264 custom preset for Phone Quality - ffmpeg 0.10 version
    licences.txt \                      # Licence file
    licences.rtf \                      # Licence file
    authors.txt \                       # Authoring file for ffDiaporama
    ffDiaporama.url \                   # URL file to be install on windows system
    ffDiaporama.desktop                 # Desktop (menu icon) entry for ffDiaporama installed in /usr

