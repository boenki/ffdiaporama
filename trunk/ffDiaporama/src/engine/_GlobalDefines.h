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

#ifndef _GLOBALDEFINES_H
#define _GLOBALDEFINES_H

/* ======================================================================
    THIS FILE MUST ABSOLUTELY BE REFERENCED AT FIRST IN ALL .h FILES OF
    THE PROJECT
   ====================================================================== */

//============================================
// Activate standard stdint macro
//============================================
#ifdef _STDINT_H
    #undef _STDINT_H            // Remove previous inclusion (if exist)
#endif

#define __STDC_CONSTANT_MACROS  // Activate macro for stdint
#include <stdint.h>             // Include stdint with macro activated

//============================================
// Minimum QT inclusions needed by all files
//============================================
#include <QtCore>
#include <QApplication>
#include <QtDebug>

#if QT_VERSION >= 0x050000
// Qt5 code
#else
// Qt4 code
#endif

// Global values
extern QString CurrentAppName;                                  // Application name (including devel, beta, ...)
extern QString CurrentAppVersion;                               // Application version read from BUILDVERSION.txt

// URL to link to help page
#define HELPFILE_DEF "http://ffdiaporama.tuxfamily.org/?page_id=%1&lang=%2"

//====================================================================
// For windows, windows.h and winbase.h must be included after QtCore
//====================================================================
#ifdef Q_OS_WIN
    #include <windows.h>
    #include <winbase.h>
    extern bool IsWindowsXP;    // True if OS is Windows/XP
#endif

//====================================================================
// Internal log defines and functions
//====================================================================

// Log level for message
#define LOGMSG_DEBUGTRACE                   1
#define LOGMSG_INFORMATION                  2
#define LOGMSG_WARNING                      3
#define LOGMSG_CRITICAL                     4

const QEvent::Type BaseAppEvent = (QEvent::Type)2000;   // The custom event will be send to EventReceiver (if EventReceiver not null)
#define EVENT_GeneralLogChanged             1           // General internal event code to display log message

extern int          LogMsgLevel;                        // Level from wich debug message was print to stdout
extern QStringList  EventList;                          // Internal event queue
extern QObject      *EventReceiver;                     // Windows wich receive event

#ifdef Q_OS_WIN
void    SetLFHeap();
#endif
void    PostEvent(int EventType,QString EventParam="");
void    ToLog(int MessageType,QString Message,QString Source="internal",bool AddBreak=true);
QString ito2a(int val);
QString ito3a(int val);

//====================================================================
// VARIOUS
//====================================================================

enum SELECTMODE {SELECTMODE_NONE, SELECTMODE_ONE, SELECTMODE_MULTIPLE};

//====================================================================

// Table of sentence that ffDText could return
enum ffDSection_ID {
    // Section definition
    ffDSection_CommonErrorMsg,
    ffDSection_CommonInfoMsg,
    ffDSection_DlgImageCorrection
};

#define TEXTID(Section,Id) (Section*1000+Id)

QString ffDText(ffDSection_ID SectionId,int TextId);

#endif // _GLOBALDEFINES_H
