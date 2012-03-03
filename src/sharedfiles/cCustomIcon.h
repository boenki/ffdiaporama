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

#ifndef CCUSTOMICON_H
#define CCUSTOMICON_H

// Basic inclusions (common to all files)
#include "_GlobalDefines.h"
#include <QString>
#include <QImage>
#include <QIcon>

#define ICONBIG_MAXHEIGHT                   200

class cCustomIcon {
public:
    bool   IsIconNeeded;                   // If true, create thumbnails on open
    QImage Icon16,Icon32,Icon48,Icon100,IconBIG;   // Icons
    enum   IconSize {ICON16,ICON32,ICON48,ICON100,ICONBIG};

                    cCustomIcon();

    virtual void    LoadIcons(cCustomIcon *CustomIcon);
    virtual void    LoadIcons(QString FileName);
    virtual void    LoadIcons(QImage *Image);
    virtual void    LoadIcons(QIcon Icon);
    virtual void    LoadIconsFromIMG(QString FileName);
    virtual void    LoadIconsFromLinux(QString LinuxPath,QString FileName);
    virtual QImage  *GetIcon(IconSize Size);
    virtual QIcon   GetIcon();
};

#ifdef Q_OS_WIN
    QImage  qt_fromWinHBITMAP(HDC hdc, HBITMAP bitmap, int w, int h);
    QPixmap convertHIconToPixmap( const HICON icon);
    QIcon   GetIconForFileOrDir(QString FileName,int IconIndex);
#endif

#endif // CCUSTOMICON_H
