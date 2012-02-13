TEMPLATE        = subdirs
//CONFIG          += ordered

# ffDiaporama main application
SUBDIRS         += src/ffDiaporama
TARGET          += ffDiaporama

# ffDiaporama MMFiler companion application
SUBDIRS         += src/ffDiaporamaMMFiler
TARGET          += ffDiaporamaMMFiler

# ffDiaporama start application (windows only)
win32:SUBDIRS   += src/ffDiaporamaStart

