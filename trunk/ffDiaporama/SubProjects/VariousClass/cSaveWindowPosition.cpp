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

#include "cSaveWindowPosition.h"

//====================================================================================================================

cSaveWindowPosition::cSaveWindowPosition(QString TheWindowName,bool &TheRestoreWindow,bool TheIsMainWindow) {
    RestoreWindow   =&TheRestoreWindow;
    WindowName      =TheWindowName;
    IsMainWindow    =TheIsMainWindow;
    WindowGeo       ="";
    MainWinSS       ="";

}

//***********************************************

void cSaveWindowPosition::ApplyToWindow(QWidget *Window) {
    if ((Window==NULL)||(*RestoreWindow==false)) return;

    // Restore window size and position
    if (WindowGeo!="") {
        QByteArray WinBA=QByteArray::fromHex(WindowGeo.toUtf8());
        Window->restoreGeometry(WinBA);
        if (IsMainWindow) {
            QByteArray MainWinBA=QByteArray::fromHex(MainWinSS.toUtf8());
            ((QMainWindow *)Window)->restoreState(MainWinBA);
        }
    }
}

//***********************************************

void cSaveWindowPosition::SaveWindowState(QWidget *Window) {
    if ((Window==NULL)||(*RestoreWindow==false)) return;
    // Save window size & position (if needed)
    QByteArray WinBA=QByteArray(Window->saveGeometry());
    WindowGeo=QString(WinBA.toHex());
    if (IsMainWindow) {
        QByteArray MainWinBA=QByteArray(((QMainWindow *)Window)->saveState());
        MainWinSS=QString(MainWinBA.toHex());
    }
}

//***********************************************

void cSaveWindowPosition::SaveToXML(QDomElement &domDocument) {
    QDomDocument    DomDocument;
    QDomElement     Element=DomDocument.createElement(WindowName);
    Element.setAttribute("saveGeometry",WindowGeo);
    if (IsMainWindow) Element.setAttribute("saveState",MainWinSS);
    domDocument.appendChild(Element);
}

//***********************************************

void cSaveWindowPosition::LoadFromXML(QDomElement domDocument) {
    if ((domDocument.elementsByTagName(WindowName).length()>0)&&(domDocument.elementsByTagName(WindowName).item(0).isElement()==true)) {
        QDomElement Element=domDocument.elementsByTagName(WindowName).item(0).toElement();
        if (Element.hasAttribute("saveGeometry"))               WindowGeo=Element.attribute("saveGeometry");
        if (IsMainWindow &&(Element.hasAttribute("saveState"))) MainWinSS=Element.attribute("saveState");
    }
}


