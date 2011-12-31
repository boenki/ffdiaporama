/* ======================================================================
    This file is part of ffDiaporama
    ffDiaporama is a tools to make diaporama as video
    Copyright (C) 2011 Dominique Levray <levray.dominique@bbox.fr>

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

    //====================================================================
    // For windows, windows.h and winbase.h must be included after QtCore
    //====================================================================
    #if defined(Q_OS_WIN32) || defined(Q_OS_WIN64)
    #include <windows.h>
    #include <winbase.h>
    #endif

#endif // _GLOBALDEFINES_H
