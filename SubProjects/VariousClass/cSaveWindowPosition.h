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

#ifndef CSAVEWINDOWPOSITION_H
#define CSAVEWINDOWPOSITION_H

//============================================
// Class to handle window size & position
//============================================

#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QWidget>
#include <QString>
#include <QMainWindow>

class cSaveWindowPosition {
public:
    QString     WindowName;     // Name of the Window
    bool        *RestoreWindow; // Link to RestoreWindow boolean variable
    bool        IsMainWindow;   // true if window is a QDockWidget
    QString     WindowGeo;      // Array for saveGeometry (All windows)
    QString     MainWinSS;      // Array for saveState (QMainWindow only)

    cSaveWindowPosition(QString WindowName,bool &RestoreWindow,bool IsMainWindow);
    void    ApplyToWindow(QWidget *Window);
    void    SaveWindowState(QWidget *Window);
    void    SaveToXML(QDomElement &domDocument);
    void    LoadFromXML(QDomElement domDocument);
};

#endif  // CSAVEWINDOWPOSITION_H
