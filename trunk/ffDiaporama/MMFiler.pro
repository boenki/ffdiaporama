# this file is to be used by Qt Creator for Linux
TEMPLATE        = subdirs
//CONFIG          += ordered

# ffDiaporama MMFiler companion application
SUBDIRS         += src/ffDiaporamaMMFiler
TARGET          += ffDiaporamaMMFiler

OTHER_FILES += TODO-LIST.txt \          # Developpement file
    BUILDVERSION.txt \                  # Developpement file
    ffDiaporamaMMFiler.xml \            # Default configuration options for ffDiaporamaMMFiler
    Devices.xml \                       # Shared default configuration options for devices management
    libx264-hq.ffpreset \               # h264 custom preset for High Quality
    libx264-pq.ffpreset \               # h264 custom preset for Phone Quality
    libx264-lossless.ffpreset \         # h264 custom preset for lossless (no quality lost)
    licences.txt \                      # Licence file
    licences.rtf \                      # Licence file
    authors.txt \                       # Authoring file for ffDiaporama
    ffDiaporamaMMFiler.desktop \        # Desktop (menu icon) entry for ffDiaporamaMMFiler installed in /usr
    ffDiaporamaMMFileropt.desktop \     # Desktop (menu icon) entry for ffDiaporamaMMFiler installed in /opt
    ffDiaporamaMMFilerlocal.desktop     # Desktop (menu icon) entry for ffDiaporamaMMFiler installed in /usr/local
