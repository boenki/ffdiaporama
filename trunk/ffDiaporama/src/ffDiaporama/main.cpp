/* ======================================================================
    This file is part of ffDiaporama
    ffDiaporama is a tools to make diaporama as video
    Copyright (C) 2011-2012 Dominique Levray <levray.dominique@bbox.fr>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
   ====================================================================== */

// Somethings needed by ffmpeg
#ifdef _STDINT_H
    #undef _STDINT_H            // Remove previous inclusion (if exist)
#endif
#define __STDC_CONSTANT_MACROS  // Activate macro for stdint
#include <stdint.h>             // Include stdint with macro activated

#include "mainwindow.h"
#include <QTranslator>

#ifdef Q_OS_WIN
    #include <windows.h>
    #include <winbase.h>

    // set low fragmentation heap to remove memory error
    // from http://social.msdn.microsoft.com/forums/en-US/vclanguage/thread/7eec66a1-07b5-47aa-816d-7c1d7f7be274
    // NOTE: To enable the low-fragmentation heap when running under a debugger, set the _NO_DEBUG_HEAP environment variable to 1.
    VOID SetLFHeap() {
        // Why would we have have to code it the hard way, that is by pulling the function out of the kernel32.dll?
        // VS 6.0 doesn't have the API defined in its headers.

        // Missing enum borrowed from: C:\Program Files\Microsoft Visual Studio 8\VC\PlatformSDK\Include\WinNT.h(8815)
        typedef enum _HEAP_INFORMATION_CLASS {
            HeapCompatibilityInformation
        } HEAP_INFORMATION_CLASS;

        // Function pointer prototype
        typedef BOOL (WINAPI *Function_HeapSetInformation) (HANDLE, HEAP_INFORMATION_CLASS, PVOID, SIZE_T);

        WCHAR WinFileName[256+1];
        MultiByteToWideChar(CP_ACP,0,QString("kernel32.dll").toLocal8Bit(),-1,WinFileName,256+1);
        HMODULE hKernel32 = GetModuleHandle(WinFileName);

        if(hKernel32) {
            Function_HeapSetInformation heapSetInfo;
            ULONG heapFlags = 2;  // LFH == 2
            HANDLE hProcessHeap = GetProcessHeap();
            heapSetInfo = (Function_HeapSetInformation)GetProcAddress(hKernel32, "HeapSetInformation");
            if (heapSetInfo) {
                if(heapSetInfo(hProcessHeap, HeapCompatibilityInformation, &heapFlags, sizeof(ULONG))) {
                    qDebug()<<"DLLMain's Request for Low Fragmentation Heap for the Process Heap Successful";
                } else {
                    qDebug()<<"DLLMain's Request for Low Fragmentation Heap for the Process Heap Unsuccessful.  Will Run with the Standard Heap Allocators";
                }
                #if _MSC_VER >= 1300
                // no way to get the pointer to the CRT heap in VC 6.0 (_crtheap)
                if(heapSetInfo((HANDLE)_get_heap_handle(), HeapCompatibilityInformation, &heapFlags, sizeof(ULONG))) {
                    qDebug()<<"DLLMain's Request for Low Fragmentation for the CRT Heap Successful";
                } else {
                    qDebug()<<"DLLMain's Request for Low Fragmentation for the CRT Heap Unsuccessful.  Will Run with the Standard Heap Allocators";
                }
                #endif
            } else {
                qDebug()<<"DllMain unable to GetProcAddress for HeapSetInformation";
            }
        } else {
            qDebug()<<"DllMain unable to GetModuleHandle(kernel32.dll)";
        }
        // Only try to set the heap once.  If it fails, live with it.
    }
#endif

int main(int argc, char *argv[]) {

    #ifdef Q_OS_WIN
    SetLFHeap();
    #endif

    SetWorkingPath(argv,APPLICATION_NAME,APPLICATION_NAME,CONFIGFILEEXT);
    #if defined(Q_OS_UNIX) && !defined(Q_OS_MACX)
        if (SearchRasterMode(APPLICATION_NAME,APPLICATION_NAME,CONFIGFILEEXT,CONFIGFILE_ROOTNAME)) QApplication::setGraphicsSystem("raster");
    #endif

    QApplication::setStyle("Cleanlooks");

    QApplication app(argc,argv);

    QString AutoLoad="";
    QString ForceLanguage="";

    // Parse parameters to find ForceLanguage and AutoLoad
    for (int i=1;i<argc;i++) {
        QString Param=QString(argv[i]).toLower();
        if (Param.startsWith("-lang=")) ForceLanguage=Param.mid(QString("-lang=").length());
        else AutoLoad=QString().fromLocal8Bit(argv[i]);
    }

    MainWindow w;
    w.InitWindow(ForceLanguage,&app);

    if (w.ApplicationConfig->RestoreWindow && w.ApplicationConfig->MainWinState) w.showMaximized(); else w.show();

    if (AutoLoad!="") {
        w.FileForIO=AutoLoad;
        w.DoOpenFileParam();
    }
    return app.exec();
}
