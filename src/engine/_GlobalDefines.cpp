/* ======================================================================
    This file is part of ffDiaporama
    ffDiaporama is a tools to make diaporama as video
    Copyright (C) 2011-2013 Dominique Levray <levray.dominique@bbox.fr>

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

// Basic inclusions (common to all files)
#include <iostream>
#include <QDir>
#include "_GlobalDefines.h"
#ifdef Q_OS_WIN
#include <windows.h>
#endif

QString CurrentAppName;                         // Application name (including devel, beta, ...)
QString CurrentAppVersion;                      // Application version read from BUILDVERSION.txt
double  ScreenFontAdjust=1;                     // System Font adjustement
int     SCALINGTEXTFACTOR=700;                  // 700 instead of 400 (ffD 1.0/1.1/1.2) to keep similar display from plaintext to richtext

//======================================================================
// Internal log defines and functions
//====================================================================

int         LogMsgLevel=LOGMSG_INFORMATION;     // Level from wich debug message was print to stdout
QStringList EventList;                          // Internal event queue
QObject     *EventReceiver=NULL;                // Windows wich receive event

#ifdef Q_OS_WIN

    bool IsWindowsXP=false;

    #include <windows.h>
    #include <winbase.h>
    #include <stdio.h>

    #if QT_VERSION<0x050000

        // set low fragmentation heap to remove memory error
        // from http://social.msdn.microsoft.com/forums/en-US/vclanguage/thread/7eec66a1-07b5-47aa-816d-7c1d7f7be274
        // NOTE: To enable the low-fragmentation heap when running under a debugger, set the _NO_DEBUG_HEAP environment variable to 1.
        void SetLFHeap() {

            // Re-attach stdio if application was started from a console
            BOOL (WINAPI *pAttachConsole)(DWORD dwProcessId);
            pAttachConsole = (BOOL (WINAPI*)(DWORD))
            GetProcAddress(LoadLibraryA("kernel32.dll"), "AttachConsole");

            if (pAttachConsole != NULL && pAttachConsole(((DWORD)-1))) {
               if (_fileno(stdout) < 0) freopen("CONOUT$","wb",stdout);
               if (_fileno(stderr) < 0) freopen("CONOUT$","wb",stderr);
               if (_fileno(stdin) < 0)  freopen("CONIN$","rb",stdin);
               std::ios::sync_with_stdio(); // Fix C++
            }

            // Check Windows System Version
            if (QSysInfo().WindowsVersion<0x0030) {     // prior to Windows XP

                ToLog(LOGMSG_CRITICAL,"Sorry but this application can't work on this system");
                exit(1);

            } else if (QSysInfo().WindowsVersion==0x0030) {    // If Windows XP

                IsWindowsXP=true;

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
                            ToLog(LOGMSG_INFORMATION,"DLLMain's Request for Low Fragmentation Heap for the Process Heap Successful");
                        } else {
                            ToLog(LOGMSG_WARNING,"DLLMain's Request for Low Fragmentation Heap for the Process Heap Unsuccessful.  Will Run with the Standard Heap Allocators");
                        }
                        #if _MSC_VER >= 1300
                        // no way to get the pointer to the CRT heap in VC 6.0 (_crtheap)
                        if(heapSetInfo((HANDLE)_get_heap_handle(), HeapCompatibilityInformation, &heapFlags, sizeof(ULONG))) {
                            ToLog(LOGMSG_INFORMATION,"DLLMain's Request for Low Fragmentation for the CRT Heap Successful");
                        } else {
                            ToLog(LOGMSG_WARNING,"DLLMain's Request for Low Fragmentation for the CRT Heap Unsuccessful.  Will Run with the Standard Heap Allocators");
                        }
                        #endif
                    } else {
                        ToLog(LOGMSG_WARNING,"DllMain unable to GetProcAddress for HeapSetInformation");
                    }
                } else {
                    ToLog(LOGMSG_WARNING,"DllMain unable to GetModuleHandle(kernel32.dll)");
                }
                // Only try to set the heap once.  If it fails, live with it.
            }
            // If > Windows/XP : nothing to do !
        }
    #endif

#endif

//====================================================================================================================

void PostEvent(int EventType,QString EventParam) {
    EventList.append(QString("%1###;###%2").arg(EventType).arg(EventParam));
    if (EventReceiver!=NULL) QApplication::postEvent(EventReceiver,new QEvent(BaseAppEvent));
}

//====================================================================================================================

double GetDoubleValue(QDomElement CorrectElement,QString Name) {
    QString sValue=CorrectElement.attribute(Name);
    bool    IsOk=true;
    double  dValue=sValue.toDouble(&IsOk);
    if (!IsOk) {
        for (int i=0;i<sValue.length();i++) if (sValue[i]==',') sValue[i]='.';
        //sValue=sValue.replace(",",".");
        dValue=sValue.toDouble(&IsOk);
    }
    return dValue;
}

double GetDoubleValue(QString sValue) {
    bool    IsOk=true;
    double  dValue=sValue.toDouble(&IsOk);
    if (!IsOk) {
        sValue=sValue.replace(",",".");
        dValue=sValue.toDouble(&IsOk);
    }
    return dValue;
}

//====================================================================================================================

QString UpInitials(QString Source) {
    for (int i=0;i<Source.length();i++)
        if ((i==0)||(Source.at(i-1)==' ')) Source[i]=Source.at(i).toUpper();
    return Source;
}

//====================================================================================================================

QString FormatLongDate(QDate EventDate) {
    return UpInitials(EventDate.toString(Qt::DefaultLocaleLongDate));
}

//====================================================================================================================

//functions used to adjust folder name depending on operating system
QString AdjustDirForOS(QString Dir) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:AdjustDirForOS");

    return QDir::toNativeSeparators(Dir);

    /*
    #ifdef Q_OS_WIN
    Dir.replace("/","\\");
    bool DoubleSlashBegin=Dir.startsWith("\\\\");
    Dir.replace("\\\\","\\");
    if (DoubleSlashBegin) Dir="\\"+Dir;
    #elif defined(Q_OS_LINUX) || defined(Q_OS_SOLARIS)
    Dir.replace("\\","/");
    #endif
    return Dir;
    */
}

//====================================================================================================================

bool PreviousBreak=true;

#ifdef Q_OS_WIN
std::string toAscii(QString tab) {
    char buffer[2048];
    CharToOemA(tab.toLocal8Bit().constData(), buffer);
    std::string str(buffer);
    return str;
}
#endif

void ToLog(int MessageType,QString Message,QString Source,bool AddBreak) {
    if ((MessageType>=LogMsgLevel)&&(PreviousBreak)) {
        QString DateTime=QTime::currentTime().toString("hh:mm:ss.zzz");
        #ifdef Q_OS_WIN
        if (Message.endsWith("\n")) Message=Message.left(Message.length()-QString("\n").length());
        if (Message.endsWith(char(10))) Message=Message.left(Message.length()-QString(char(10)).length());
        if (Message.endsWith(char(13))) Message=Message.left(Message.length()-QString(char(13)).length());
        if (Message.endsWith(char(10))) Message=Message.left(Message.length()-QString(char(10)).length());
        #endif
        QString MSG="";
        if ((Message!="LIBAV: No accelerated colorspace conversion found from yuv422p to rgb24.")&&
            (Message!="LIBAV: Increasing reorder buffer to 1")&&
            (!Message.startsWith("LIBAV: Reference"))&&
            (!Message.startsWith("LIBAV: error while decoding MB"))&&
            (!Message.startsWith("LIBAV: left block unavailable for requested"))&&
            (!Message.startsWith("LIBAV: max_analyze_duration reached"))&&
            (!(Message.startsWith("LIBAV: mode:") && Message.contains("parity:") && (Message.contains("auto_enable:")||Message.contains("deint:"))))&&
            (!(Message.startsWith("LIBAV: w:") && Message.contains("h:") && Message.contains("pixfmt:")))
           ) {
            switch (MessageType) {
                case LOGMSG_DEBUGTRACE:    MSG=QString("["+DateTime+":DEBUG]\t"     +Message+(AddBreak?"\n":""));  break;
                case LOGMSG_INFORMATION:   MSG=QString("["+DateTime+":INFO]\t"      +Message+(AddBreak?"\n":""));  break;
                case LOGMSG_WARNING:       MSG=QString("["+DateTime+":WARNING]\t"   +Message+(AddBreak?"\n":""));  break;
                case LOGMSG_CRITICAL:      MSG=QString("["+DateTime+":ERROR]\t"     +Message+(AddBreak?"\n":""));  break;
            }
        }
        if (!MSG.isEmpty()) {
            if ((MessageType!=LOGMSG_DEBUGTRACE)&&(EventReceiver!=NULL)) PostEvent(EVENT_GeneralLogChanged,QString("%1###:###%2###:###%3").arg((int)MessageType).arg(Message).arg(Source));
            #ifdef Q_OS_WIN
            std::cout << toAscii(MSG) << std::flush;
            if (MSG.endsWith("\n")) MSG=MSG.left(MSG.indexOf("\n"));
            #else
            std::cout << MSG.toLocal8Bit().constData() << std::flush;
            #endif
        }
        PreviousBreak=((AddBreak)||(Message.endsWith("\n")));
    } else if (MessageType>=LogMsgLevel) {
        #ifdef Q_OS_WIN
        std::cout << toAscii(Message) << std::flush;
        #else
        std::cout << Message.toLocal8Bit().constData() << std::flush;
        #endif
    }
}

//====================================================================================================================

QString ito2a(int val) {
    QString Ret=QString("%1").arg(val);
    while (Ret.length()<2) Ret="0"+Ret;
    return Ret;
}

QString ito3a(int val) {
    QString Ret=QString("%1").arg(val);
    while (Ret.length()<3) Ret="0"+Ret;
    return Ret;
}

//====================================================================================================================

QString ffDText(ffDSection_ID SectionId,int TextId) {
    switch (TEXTID(SectionId,TextId)) {
        // Section Common Error Messages
        case TEXTID(ffDSection_CommonErrorMsg,0):           return QApplication::translate("CommonErrorMsg","Format not supported");
        case TEXTID(ffDSection_CommonErrorMsg,1):           return QApplication::translate("CommonErrorMsg","This application support only audio track with unsigned 8 bits or signed 16 bits sample format");
        case TEXTID(ffDSection_CommonErrorMsg,2):           return QApplication::translate("CommonErrorMsg","This application support only mono or stereo audio track");

        // Section Common information Messages
        case TEXTID(ffDSection_CommonInfoMsg,0):            return QApplication::translate("CommonInfoMsg","Select a file");
        case TEXTID(ffDSection_CommonInfoMsg,1):            return QApplication::translate("CommonInfoMsg","Error","Title of dialog box displaying an error");

        // Section DlgImageCorrection
        case TEXTID(ffDSection_DlgImageCorrection,0):       return QApplication::translate("DlgSlideProperties","Correct or reframe image","Action title in slide edit dialog + dialog title of image edit dialog");
        case TEXTID(ffDSection_DlgImageCorrection,1):       return QApplication::translate("DlgSlideProperties","Correct, reframe or cut video","Action title in slide edit dialog + dialog title of image edit dialog");
        case TEXTID(ffDSection_DlgImageCorrection,2):       return QApplication::translate("DlgSlideProperties","Define framing and correction of image","Tooltip");
        case TEXTID(ffDSection_DlgImageCorrection,3):       return QApplication::translate("DlgSlideProperties","Define framing and correction of image or cut video","Tooltip");

    }
    return "Message text not found!";
}
