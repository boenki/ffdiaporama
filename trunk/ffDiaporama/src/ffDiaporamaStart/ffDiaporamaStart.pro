#-------------------------------------------------
# THIS APPLICATION IS FOR WINDOWS ONLY
#-------------------------------------------------

win32 {
	QT          -= core
	QT			-= gui
    TEMPLATE    = app
    TARGET      = ffDiaporamaStart
    CONFIG      -= app_bundle
    LIBS        += -lShell32

    HEADERS += \
        blitzcpu.h

    SOURCES += \
        blitzcpu.cpp \
        main.cpp

	# add console if debug mode
	CONFIG(debug, debug|release) {
		CONFIG += console
	}
     RC_FILE    += ffDiaporamaStart.rc
}
