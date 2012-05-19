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

#include "cCustomIcon.h"

//*****************************************************************************************************************************************

#ifdef Q_OS_WIN
    #include <QPixmapCache>
    #include <windows.h>
    #include <QSettings>
    #ifndef SHGFI_ADDOVERLAYS
        #define SHGFI_ADDOVERLAYS 0x000000020
    #endif

    // qt_fromWinHBITMAP From Qmmander Filemanager / Copyright (C) Alex Skoruppa 2009 (See:http://qmmander.googlecode.com/svn-history/r93/trunk/winfileinfo.cpp)
    QImage qt_fromWinHBITMAP(HDC hdc, HBITMAP bitmap, int w, int h) {
        ToLog(LOGMSG_DEBUGTRACE,"IN:qt_fromWinHBITMAP");

        BITMAPINFO bmi;
        memset(&bmi, 0, sizeof(bmi));
        bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth       = w;
        bmi.bmiHeader.biHeight      = -h;
        bmi.bmiHeader.biPlanes      = 1;
        bmi.bmiHeader.biBitCount    = 32;
        bmi.bmiHeader.biCompression = BI_RGB;
        bmi.bmiHeader.biSizeImage   = w * h * 4;

        QImage image(w, h, QImage::Format_ARGB32_Premultiplied);
        if (image.isNull())
            return image;

        // Get bitmap bits
        uchar *data = (uchar *) qMalloc(bmi.bmiHeader.biSizeImage);

        if (GetDIBits(hdc, bitmap, 0, h, data, &bmi, DIB_RGB_COLORS)) {
            // Create image and copy data into image.
            for (int y=0; y<h; ++y) {
                void *dest = (void *) image.scanLine(y);
                void *src = data + y * image.bytesPerLine();
                memcpy(dest, src, image.bytesPerLine());
            }
        } else {
            ToLog(LOGMSG_CRITICAL,"qt_fromWinHBITMAP(), failed to get bitmap bits");
        }
        qFree(data);

        return image;
    }

    //====================================================================================================================
    // convertHIconToPixmap From Qmmander Filemanager / Copyright (C) Alex Skoruppa 2009 (See:http://qmmander.googlecode.com/svn-history/r93/trunk/winfileinfo.cpp)
    QPixmap convertHIconToPixmap( const HICON icon) {
        ToLog(LOGMSG_DEBUGTRACE,"IN:convertHIconToPixmap");

        bool foundAlpha = false;
        HDC screenDevice = GetDC(0);
        HDC hdc = CreateCompatibleDC(screenDevice);
        ReleaseDC(0, screenDevice);

        ICONINFO iconinfo;
        bool result = GetIconInfo(icon, &iconinfo); //x and y Hotspot describes the icon center
        if (!result)
            ToLog(LOGMSG_CRITICAL,"convertHIconToPixmap(), failed to GetIconInfo()");

        int w = iconinfo.xHotspot * 2;
        int h = iconinfo.yHotspot * 2;

        BITMAPINFOHEADER bitmapInfo;
        bitmapInfo.biSize        = sizeof(BITMAPINFOHEADER);
        bitmapInfo.biWidth       = w;
        bitmapInfo.biHeight      = h;
        bitmapInfo.biPlanes      = 1;
        bitmapInfo.biBitCount    = 32;
        bitmapInfo.biCompression = BI_RGB;
        bitmapInfo.biSizeImage   = 0;
        bitmapInfo.biXPelsPerMeter = 0;
        bitmapInfo.biYPelsPerMeter = 0;
        bitmapInfo.biClrUsed       = 0;
        bitmapInfo.biClrImportant  = 0;
        DWORD* bits;

        HBITMAP winBitmap = CreateDIBSection(hdc, (BITMAPINFO*)&bitmapInfo, DIB_RGB_COLORS, (VOID**)&bits, NULL, 0);
        HGDIOBJ oldhdc = (HBITMAP)SelectObject(hdc, winBitmap);
        DrawIconEx( hdc, 0, 0, icon, iconinfo.xHotspot * 2, iconinfo.yHotspot * 2, 0, 0, DI_NORMAL);
        QImage image = qt_fromWinHBITMAP(hdc, winBitmap, w, h);

        for (int y = 0 ; y < h && !foundAlpha ; y++) {
            QRgb *scanLine= reinterpret_cast<QRgb *>(image.scanLine(y));
            for (int x = 0; x < w ; x++) {
                if (qAlpha(scanLine[x]) != 0) {
                    foundAlpha = true;
                    break;
                }
            }
        }
        if (!foundAlpha) {
            //If no alpha was found, we use the mask to set alpha values
            DrawIconEx( hdc, 0, 0, icon, w, h, 0, 0, DI_MASK);
            QImage mask = qt_fromWinHBITMAP(hdc, winBitmap, w, h);

            for (int y = 0 ; y < h ; y++){
                QRgb *scanlineImage = reinterpret_cast<QRgb *>(image.scanLine(y));
                QRgb *scanlineMask = mask.isNull() ? 0 : reinterpret_cast<QRgb *>(mask.scanLine(y));
                for (int x = 0; x < w ; x++){
                    if (scanlineMask && qRed(scanlineMask[x]) != 0)
                        scanlineImage[x] = 0; //mask out this pixel
                    else
                        scanlineImage[x] |= 0xff000000; // set the alpha channel to 255
                }
            }
        }
        //dispose resources created by iconinfo call
        DeleteObject(iconinfo.hbmMask);
        DeleteObject(iconinfo.hbmColor);

        SelectObject(hdc, oldhdc); //restore state
        DeleteObject(winBitmap);
        DeleteDC(hdc);
        return QPixmap::fromImage(image);
    }

    //====================================================================================================================
    // GetIconForFileOrDir adapted by domledom From Qmmander Filemanager / Copyright (C) Alex Skoruppa 2009 (See:http://qmmander.googlecode.com/svn-history/r93/trunk/winfileinfo.cpp)
    QIcon GetIconForFileOrDir(QString FileName,int IconIndex) {
        ToLog(LOGMSG_DEBUGTRACE,"IN:GetIconForFileOrDir");

        QIcon RetIcon;
        WCHAR WinFileName[256+1];

        MultiByteToWideChar(CP_ACP,0,FileName.toLocal8Bit(),-1,WinFileName,256+1);

        if (IconIndex!=0) {
            HICON Icon;
            if (ExtractIconEx(WinFileName,IconIndex,&Icon,NULL,1)>0) {
                RetIcon=convertHIconToPixmap(Icon);
                DeleteObject(Icon);
                return RetIcon;
            } else if (ExtractIconEx(WinFileName,IconIndex,NULL,&Icon,1)>0) {
                RetIcon=convertHIconToPixmap(Icon);
                DeleteObject(Icon);
                return RetIcon;
            }
        }

        SHFILEINFO  info;
        QString     key;
        QPixmap     pixmap;
        QString     fileExtension=QFileInfo(FileName).suffix().toUpper();
        fileExtension.prepend(QLatin1String("."));

        if (QFileInfo(FileName).isFile() && !QFileInfo(FileName).isExecutable() && !QFileInfo(FileName).isSymLink()) key=QLatin1String("qt_")+fileExtension;
        if (!key.isEmpty()) QPixmapCache::find(key, pixmap);

        if (!pixmap.isNull()) {
            RetIcon.addPixmap(pixmap);
            if (QPixmapCache::find(key+QLatin1Char('l'),pixmap)) RetIcon.addPixmap(pixmap);
            return RetIcon;
        }

        //Get the small icon
        if (SHGetFileInfo(WinFileName,0,&info,sizeof(SHFILEINFO),SHGFI_ICON|SHGFI_SMALLICON|SHGFI_SYSICONINDEX|SHGFI_ADDOVERLAYS)) {
            if (QFileInfo(FileName).isDir() && !QDir(FileName).isRoot()) {
                //using the unique icon index provided by windows save us from duplicate keys
                key = QString::fromLatin1("qt_dir_%1").arg(info.iIcon);
                QPixmapCache::find(key,pixmap);
                if (!pixmap.isNull()) {
                    RetIcon.addPixmap(pixmap);
                    if (QPixmapCache::find(key + QLatin1Char('l'), pixmap)) RetIcon.addPixmap(pixmap);
                    DestroyIcon(info.hIcon);
                    return RetIcon;
                }
            }
            if (pixmap.isNull()) {
                pixmap = convertHIconToPixmap(info.hIcon);
                if (!pixmap.isNull()) {
                    RetIcon.addPixmap(pixmap);
                    if (!key.isEmpty()) QPixmapCache::insert(key,pixmap);
                } else {
                    ToLog(LOGMSG_WARNING,"QCustomFolderTree::getWinIcon() no small icon found");
                }
            }
            DestroyIcon(info.hIcon);
        }
        //Get the big icon
        if (SHGetFileInfo(WinFileName,0,&info,sizeof(SHFILEINFO),SHGFI_ICON|SHGFI_LARGEICON|SHGFI_SYSICONINDEX|SHGFI_ADDOVERLAYS)) {
            if (QFileInfo(FileName).isDir() && !QFileInfo(FileName).isRoot()) {
                //using the unique icon index provided by windows save us from duplicate keys
                key=QString::fromLatin1("qt_dir_%1").arg(info.iIcon);
            }
            pixmap=convertHIconToPixmap(info.hIcon);
            if (!pixmap.isNull()) {
                RetIcon.addPixmap(pixmap);
                if (!key.isEmpty()) QPixmapCache::insert(key+QLatin1Char('l'),pixmap);
            } else {
                ToLog(LOGMSG_WARNING,"QCustomFolderTree::getWinIcon() no large icon found");
            }
            DestroyIcon(info.hIcon);
        }
        if (RetIcon.isNull()) ToLog(LOGMSG_CRITICAL,QString("Loading icon nbr %1 From %2 Error").arg(IconIndex).arg(FileName));
        return RetIcon;
    }

#endif

//*****************************************************************************************************************************************

cCustomIcon::cCustomIcon() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomIcon::cCustomIcon");
    IsIconNeeded=false;
}

void cCustomIcon::LoadIcons(QString FileName) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomIcon::LoadIcons as filename");

    IconBIG=QImage(FileName);
    if (IconBIG.isNull()) ToLog(LOGMSG_CRITICAL,QString("Loading %1 Error").arg(FileName)); else {
        if (IconBIG.width()>IconBIG.height()) {
            Icon16 =IconBIG.scaledToWidth(16,Qt::SmoothTransformation);
            Icon32 =IconBIG.scaledToWidth(32,Qt::SmoothTransformation);
            Icon48 =IconBIG.scaledToWidth(48,Qt::SmoothTransformation);
            Icon100=IconBIG.scaledToWidth(100,Qt::SmoothTransformation);
        } else {
            Icon16 =IconBIG.scaledToHeight(16,Qt::SmoothTransformation);
            Icon32 =IconBIG.scaledToHeight(32,Qt::SmoothTransformation);
            Icon48 =IconBIG.scaledToHeight(48,Qt::SmoothTransformation);
            Icon100=IconBIG.scaledToHeight(100,Qt::SmoothTransformation);
        }
        if (IconBIG.height()>ICONBIG_MAXHEIGHT) IconBIG=IconBIG.scaledToHeight(ICONBIG_MAXHEIGHT,Qt::SmoothTransformation);
    }
}

//====================================================================================================================

void cCustomIcon::LoadIcons(cCustomIcon *CustomIcon) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomIcon::LoadIcons as CustomIcon");

    Icon16 =CustomIcon->Icon16.copy();
    Icon32 =CustomIcon->Icon32.copy();
    Icon48 =CustomIcon->Icon48.copy();
    Icon100=CustomIcon->Icon100.copy();
    IconBIG=CustomIcon->IconBIG.copy();
}

//====================================================================================================================

void cCustomIcon::LoadIconsFromIMG(QString FileName) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomIcon::LoadIconsFromIMG");

    Icon16.load(":/img/MediaIcons/16x16/"+FileName);      if (Icon16.isNull())  ToLog(LOGMSG_CRITICAL,QString("Loading img/MediaIcons/16x16/%1 Error").arg(FileName));
    Icon32.load(":/img/MediaIcons/32x32/"+FileName);      if (Icon32.isNull())  ToLog(LOGMSG_CRITICAL,QString("Loading img/MediaIcons/32x32/%1 Error").arg(FileName));
    Icon48.load(":/img/MediaIcons/48x48/"+FileName);      if (Icon48.isNull())  ToLog(LOGMSG_CRITICAL,QString("Loading img/MediaIcons/48x48/%1 Error").arg(FileName));
    Icon100.load(":/img/MediaIcons/100x100/"+FileName);   if (Icon100.isNull()) ToLog(LOGMSG_CRITICAL,QString("Loading img/MediaIcons/100x100/%1 Error").arg(FileName));
    IconBIG.load(":/img/MediaIcons/200x200/"+FileName);   if (IconBIG.isNull()) ToLog(LOGMSG_CRITICAL,QString("Loading img/MediaIcons/200x200/%1 Error").arg(FileName));
}

//====================================================================================================================

void cCustomIcon::LoadIconsFromLinux(QString LinuxPath,QString FileName) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomIcon::LoadIconsFromLinux");

    Icon16.load(LinuxPath+"16x16/"+FileName);           if (Icon16.isNull())  ToLog(LOGMSG_CRITICAL,QString("Loading %116x16/%2 Error").arg(LinuxPath).arg(FileName));
    Icon32.load(LinuxPath+"32x32/"+FileName);           if (Icon32.isNull())  ToLog(LOGMSG_CRITICAL,QString("Loading %132x32/%2 Error").arg(LinuxPath).arg(FileName));
    Icon48.load(LinuxPath+"48x48/"+FileName);           if (Icon48.isNull())  ToLog(LOGMSG_CRITICAL,QString("Loading %148x48/%2 Error").arg(LinuxPath).arg(FileName));
    Icon100=QImage(LinuxPath+"128x128/"+FileName);      if (Icon100.isNull()) ToLog(LOGMSG_CRITICAL,QString("Loading %1128x128/%2 Error").arg(LinuxPath).arg(FileName));
    if (!Icon100.isNull()) IconBIG=Icon100.copy();

    if (!Icon100.isNull()) Icon100=Icon100.scaledToHeight(100,Qt::SmoothTransformation);
    if (!IconBIG.isNull()) IconBIG=IconBIG.scaledToHeight(200,Qt::SmoothTransformation);
}

//====================================================================================================================

void cCustomIcon::LoadIcons(QImage *Image) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomIcon::LoadIcons as QImage");

    if (Image->width()>Image->height()) {
        Icon16 =Image->scaledToWidth(16,Qt::SmoothTransformation);
        Icon32 =Image->scaledToWidth(32,Qt::SmoothTransformation);
        Icon48 =Image->scaledToWidth(48,Qt::SmoothTransformation);
        Icon100=Image->scaledToWidth(100,Qt::SmoothTransformation);
    } else {
        Icon16 =Image->scaledToHeight(16,Qt::SmoothTransformation);
        Icon32 =Image->scaledToHeight(32,Qt::SmoothTransformation);
        Icon48 =Image->scaledToHeight(48,Qt::SmoothTransformation);
        Icon100=Image->scaledToHeight(100,Qt::SmoothTransformation);
    }
    if (Image->height()<=ICONBIG_MAXHEIGHT) IconBIG=Image->copy();
        else IconBIG=Image->scaledToHeight(ICONBIG_MAXHEIGHT);
}

//====================================================================================================================

void cCustomIcon::LoadIcons(QIcon Icon) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomIcon::LoadIcons as QIcon");

    Icon16 =Icon.pixmap(16,16).toImage();
    Icon32 =Icon.pixmap(32,32).toImage();
    Icon48 =Icon.pixmap(48,48).toImage();
    Icon100=Icon.pixmap(100,100).toImage();
    if ((Icon100.height()<100)&&(Icon100.width()<100)) {
        if (Icon100.height()>Icon100.width()) Icon100=Icon100.scaledToHeight(100,Qt::SmoothTransformation);
            else Icon100=Icon100.scaledToWidth(100,Qt::SmoothTransformation);
    }
    IconBIG=Icon.pixmap(ICONBIG_MAXHEIGHT,ICONBIG_MAXHEIGHT).toImage();
}

//====================================================================================================================

QIcon cCustomIcon::GetIcon() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomIcon::GetIcon");

    QIcon Ret=QIcon(QPixmap().fromImage(Icon16));
    Ret.addPixmap(QPixmap().fromImage(Icon32));
    Ret.addPixmap(QPixmap().fromImage(Icon48));
    Ret.addPixmap(QPixmap().fromImage(Icon100));
    Ret.addPixmap(QPixmap().fromImage(IconBIG));
    return Ret;
}

//====================================================================================================================

QImage  *cCustomIcon::GetIcon(IconSize Size) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomIcon::GetIcon");

    switch (Size) {
        case ICON16:  return &Icon16;
        case ICON32:  return &Icon32;
        case ICON48:  return &Icon48;
        case ICON100: return &Icon100;
        case ICONBIG: return &IconBIG;
        default:      return &Icon16;
    }
}
