@echo off
call C:\QtSDK\Desktop\Qt\4.7.3\mingw\bin\qtenv2.bat
y:
cd y:\ffDiaporama\ffDiaporama
rd /Q /S y:\ffDiaporama\winbuild
qmake.exe ffDiaporama.pro -r -spec win32-g++ "CONFIG+=release"
mingw32-make
mingw32-make clean
c:
cd \
rd /Q /S c:\ffDiaporama
md c:\ffDiaporama
cd \ffDiaporama
copy y:\ffDiaporama\winbuild\ffDiaporama.exe
copy C:\Qt\ffmpeg-win32-shared\bin\avcodec-53.dll
copy C:\Qt\ffmpeg-win32-shared\bin\avformat-53.dll
copy C:\Qt\ffmpeg-win32-shared\bin\avutil-51.dll
copy C:\Qt\ffmpeg-win32-shared\bin\swscale-0.dll
copy C:\QtSDK\Desktop\Qt\4.7.3\mingw\bin\libgcc_s_dw2-1.dll
copy C:\QtSDK\Desktop\Qt\4.7.3\mingw\bin\mingwm10.dll
copy C:\QtSDK\Desktop\Qt\4.7.3\mingw\bin\QtCore4.dll
copy C:\QtSDK\Desktop\Qt\4.7.3\mingw\bin\QtGui4.dll
copy C:\QtSDK\Desktop\Qt\4.7.3\mingw\bin\QtXml4.dll
copy C:\Qt\SDL-1.2.14\bin\SDL.dll
md imageformats
copy C:\QtSDK\Desktop\Qt\4.7.3\mingw\plugins\imageformats\qgif4.dll imageformats
copy C:\QtSDK\Desktop\Qt\4.7.3\mingw\plugins\imageformats\qmng4.dll imageformats 
copy C:\QtSDK\Desktop\Qt\4.7.3\mingw\plugins\imageformats\qico4.dll imageformats
copy C:\QtSDK\Desktop\Qt\4.7.3\mingw\plugins\imageformats\qsvg4.dll imageformats
copy C:\QtSDK\Desktop\Qt\4.7.3\mingw\plugins\imageformats\qjpeg4.dll imageformats
copy C:\QtSDK\Desktop\Qt\4.7.3\mingw\plugins\imageformats\qtiff4.dll imageformats
copy y:\ffDiaporama\ffDiaporama\BUILDVERSION.txt
copy y:\ffDiaporama\ffDiaporama\ffDiaporama.xml
copy y:\ffDiaporama\ffDiaporama\libx264-hq.ffpreset
copy y:\ffDiaporama\fmt_filters.README
copy y:\ffDiaporama\fmt_filters.VERSION
copy y:\ffDiaporama\licence.rtf
copy y:\ffDiaporama\licences.txt

md background
xcopy /s /e Y:\ffDiaporama\ffDiaporama\background background
md img
xcopy /s /e Y:\ffDiaporama\ffDiaporama\img img
md locale
xcopy /s /e Y:\ffDiaporama\ffDiaporama\locale locale
md luma
xcopy /s /e Y:\ffDiaporama\ffDiaporama\luma luma
md transitions-img
xcopy /s /e Y:\ffDiaporama\ffDiaporama\transitions-img transitions-img

md exiv2
xcopy /s /e c:\Qt\exiv2 exiv2
md ffmpeg
xcopy /s /e c:\Qt\ffmpeg-win32-static ffmpeg

"C:\Program Files\Inno Setup 5\Compil32.exe" /cc Y:\ffDiaporama\ffDiaporama\Packaging\ffDiaporama-InoSetup.iss