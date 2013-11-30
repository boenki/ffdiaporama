# this file is to be used by qmake for Windows & Linux
TEMPLATE        = subdirs

# ffDiaporama main application
SUBDIRS         += src/ffDiaporama
TARGET          += ffDiaporama

RESOURCES += \
    src/RSCffDiaporama.qrc \
    ../../../../../../home/dominique/Dev/ffdiaporama/trunk/ffDiaporama/src/ffDiaporama/RSCffDiaporama.qrc \
    src/ffDiaporama/MainWindow/RSCffDiaporama.qrc \
    src/ffDiaporama/wgt_QMultimediaBrowser/RSCffDiaporama.qrc \
    src/ffDiaporama/HelpPopup/RSCffDiaporama.qrc \
    src/ffDiaporama/DlgText/RSCffDiaporama.qrc \
    src/ffDiaporama/DlgSlide/RSCffDiaporama.qrc \
    src/ffDiaporama/DlgSlide/RSCffDiaporama.qrc \
    src/ffDiaporama/DlgImage/RSCffDiaporama.qrc \
    src/ffDiaporama/DlgBackground/RSCffDiaporama.qrc \
    src/ffDiaporama/DlgAppSettings/RSCffDiaporama.qrc \
    src/ffDiaporama/DlgAbout/RSCffDiaporama.qrc
