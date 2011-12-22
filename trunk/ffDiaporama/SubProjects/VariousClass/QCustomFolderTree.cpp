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

#include <QtDebug>
#include <QtGui/QApplication>
#include <QFileInfoList>
#include <QDir>
#include <QFile>
#include <QIODevice>
#include <QTextStream>
#include <QProcess>

#if defined(Q_OS_WIN)
    #include <windows.h>
    #include <QSettings>
    #include <QPixmapCache>
#endif
#ifndef SHGFI_ADDOVERLAYS
    #define SHGFI_ADDOVERLAYS 0x000000020
#endif

#include "QCustomFolderTree.h"
#include "cBaseApplicationConfig.h"

#define TAG "<to expand>"

// Define icon images
#define DEV_CDROM       "/devices/drive-optical.png"
#define DEV_HDD         "/devices/drive-harddisk.png"
#define DEV_USB         "/devices/drive-removable-media.png"
#define DEV_USER        "/places/user-home.png"
#define FOLDER_STD      "/places/folder.png"

#define DEBUGMODE

QString IconsPath="";

QString GetTextSize(qlonglong Size) {
    Size=Size/1024;

    QString UnitStr="";
    int     Unit   =0;

    while ((Size>1024*1024)&&(Unit<2)) {
        Unit++;
        Size=Size/1024;
    }
    switch (Unit) {
        case 0 : UnitStr=QApplication::translate("QCustomFolderTree","Mb","Unit Mb");   break;
        case 1 : UnitStr=QApplication::translate("QCustomFolderTree","Gb","Unit Gb");   break;
        case 2 : UnitStr=QApplication::translate("QCustomFolderTree","Tb","Unit Tb");   break;
    }
    if (double(Size)/double(1024)>0.1) return QString("%1").arg(double(Size)/double(1024),8,'f',1).trimmed()+UnitStr;
    else return "<0.1"+UnitStr;
}

#if defined(Q_OS_WIN)

// qt_fromWinHBITMAP From Qmmander Filemanager / Copyright (C) Alex Skoruppa 2009 (See:http://qmmander.googlecode.com/svn-history/r93/trunk/winfileinfo.cpp)
QImage qt_fromWinHBITMAP(HDC hdc, HBITMAP bitmap, int w, int h) {
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
        qDebug()<<"qt_fromWinHBITMAP(), failed to get bitmap bits";
    }
    qFree(data);

    return image;
}

// convertHIconToPixmap From Qmmander Filemanager / Copyright (C) Alex Skoruppa 2009 (See:http://qmmander.googlecode.com/svn-history/r93/trunk/winfileinfo.cpp)
QPixmap convertHIconToPixmap( const HICON icon) {
    bool foundAlpha = false;
    HDC screenDevice = GetDC(0);
    HDC hdc = CreateCompatibleDC(screenDevice);
    ReleaseDC(0, screenDevice);

    ICONINFO iconinfo;
    bool result = GetIconInfo(icon, &iconinfo); //x and y Hotspot describes the icon center
    if (!result)
        qDebug()<<"convertHIconToPixmap(), failed to GetIconInfo()";

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

// GetIconForFileOrDir adapted by domledom From Qmmander Filemanager / Copyright (C) Alex Skoruppa 2009 (See:http://qmmander.googlecode.com/svn-history/r93/trunk/winfileinfo.cpp)
QIcon GetIconForFileOrDir(QString FileName,int IconIndex) {
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
                qDebug()<<"QCustomFolderTree::getWinIcon() no small icon found";
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
            qDebug()<<"QCustomFolderTree::getWinIcon() no large icon found";
        }
        DestroyIcon(info.hIcon);
    }
    return RetIcon;
}
#endif

//*********************************************************************************************************

cHardDriveDescriptor::cHardDriveDescriptor(QString ThePath,QString Alias) {
    Label       ="";
    Size        =0;
    Used        =0;
    Avail       =0;
    IsReadOnly  =false;
    if (ThePath=="") {
        Path    ="";
        Device  =Alias;
        Alias   ="";
    } else {
        Path    =ThePath;
        Device  ="";
        Label   =Alias;
    }

    if (Alias==QApplication::translate("QCustomFolderTree","Personal folder"))
    #if defined(Q_OS_WIN)
        IconDrive=GetIconForFileOrDir(QString(getenv("SystemRoot"))+QString("\\system32\\SHELL32.dll"),126);
    #elif defined(Q_OS_UNIX) && !defined(Q_OS_MACX)
    {
        IconDrive=QIcon(IconsPath+"16x16"+DEV_USER);
        IconDrive.addFile(IconsPath+"48x48"+DEV_USER);
        IconDrive.addFile(IconsPath+"128x128"+DEV_USER);
    }
    #endif

    // Adjust path depending on Operating System
    #if defined(Q_OS_WIN)
        Path.replace("/","\\");

        WCHAR       Drive[256+1];
        WCHAR       VolumeName[256+1];
        WCHAR       SysName[256+1];
        DWORD       SerialNumber;
        DWORD       MaxComponent;
        DWORD       FileSysFlag;

        MultiByteToWideChar(CP_ACP,0,Path.toLocal8Bit(),-1,Drive,256+1);
        switch (GetDriveType(Drive)) {
            case DRIVE_CDROM     :  IconDrive=IconDrive=GetIconForFileOrDir(QString(getenv("SystemRoot"))+QString("\\system32\\SHELL32.dll"),-12);  IsReadOnly=true;    break;
            case DRIVE_REMOTE    :  IconDrive=IconDrive=GetIconForFileOrDir(QString(getenv("SystemRoot"))+QString("\\system32\\SHELL32.dll"),-10);                      break;
            case DRIVE_REMOVABLE :  IconDrive=GetIconForFileOrDir(QString(getenv("SystemRoot"))+QString("\\system32\\SHELL32.dll"),121);                               break;
            default              :  if (IconDrive.isNull()) IconDrive=GetIconForFileOrDir(QString(getenv("SystemRoot"))+QString("\\system32\\SHELL32.dll"),-8);         break;
        }
        if (GetVolumeInformation(Drive,VolumeName,sizeof(WCHAR)*(256+1),&SerialNumber,&MaxComponent,&FileSysFlag,SysName,sizeof(WCHAR)*(256+1))) {
            if (Label=="") {
                Label=Path;
                if (QString().fromWCharArray(VolumeName)!="") Label=Label+"["+QString().fromWCharArray(VolumeName)+"]";
            }
            _ULARGE_INTEGER Available,FullSize,TotalNumberOfFreeBytes;
            if (GetDiskFreeSpaceEx(Drive,&Available,&FullSize,&TotalNumberOfFreeBytes)) {
                Avail   =Available.QuadPart;
                Size    =FullSize.QuadPart;
                Used    =Size-TotalNumberOfFreeBytes.QuadPart;
            }
        } else {
            // Must be a CD/DVD ROM drive without disk
            if (Label!=QApplication::translate("QCustomFolderTree","Personal folder"))
                Label=Path+"["+QApplication::translate("QCustomFolderTree","Empty drive...")+"]";
        }

    #elif defined(Q_OS_UNIX) && !defined(Q_OS_MACX)
        bool        IsOk=true;
        QProcess    Process;
        Process.setProcessChannelMode(QProcess::MergedChannels);

        if (Path=="") Process.start("df "+Device); else Process.start("df "+Path);

        // use df to get information on drive (size/used/avail) and ensure drive is mounted
        if (!Process.waitForStarted()) {
            qDebug()<<"Impossible to execute df";
            IsOk=false;
        }
        if (IsOk && !Process.waitForFinished()) {
            Process.kill();
            qDebug()<<"Error during mount df";
            IsOk=false;
        }
        if (IsOk && (Process.exitStatus()<0)) {
            qDebug()<<"mount return df";
            IsOk=false;
        }
        if (!IsOk) {
            Process.terminate();
            Process.close();
        } else {
            QString Part=QString().fromLocal8Bit(Process.readAllStandardOutput());
            Process.terminate();
            Process.close();

            // First line is the header => then pass it :
            Part=Part.mid(Part.indexOf("\n")+QString("\n").length());

            // Second line contains information we whant like Device | Size | Used | Avail | Mount path
            // If Mount path = asked path then drive is mounted !

            if (Part.indexOf(" ")!=-1) {
                Device=Part.left(Part.indexOf(" "));
                Part  =Part.mid(Part.indexOf(" ")).trimmed();

                // Next is size
                if (Part.indexOf(" ")!=-1) {
                    Size=Part.left(Part.indexOf(" ")).toLongLong();
                    Part=Part.mid(Part.indexOf(" ")).trimmed();

                    // Next is used
                    if (Part.indexOf(" ")!=-1) {
                        Used=Part.left(Part.indexOf(" ")).toLongLong();
                        Part=Part.mid(Part.indexOf(" ")).trimmed();

                        // Next is avail
                        Avail=Part.left(Part.indexOf(" ")).toLongLong();
                        Part=Part.mid(Part.indexOf(" ")).trimmed();

                        // Next is Pct Use

                        // and finaly : the mounted path
                        if (Path=="") Path =Part.mid(Part.indexOf(" ")).trimmed();
                    }
                }
            }
        }

        // Get drive type
        if ((Path!="")&&(Device!="")) {
            if (Device.startsWith("/dev/sr") || Device.startsWith("/dev/scd")) {
                IconDrive   =QIcon(IconsPath+"16x16"+DEV_CDROM);
                IconDrive.addFile(IconsPath+"48x48"+DEV_CDROM);
                IconDrive.addFile(IconsPath+"128x128"+DEV_CDROM);
                IsReadOnly  =true;
            } else {
                // use dmesg to get drive type

                QProcess    Process;
                bool        IsOk=true;

                Process.setProcessChannelMode(QProcess::MergedChannels);
                Process.start("dmesg");
                if (!Process.waitForStarted()) {
                    qDebug()<<"Impossible to execute dmesg";
                    IsOk=false;
                }
                if (IsOk && !Process.waitForFinished()) {
                    Process.kill();
                    qDebug()<<"Error during mount dmesg";
                    IsOk=false;
                }
                if (IsOk && (Process.exitStatus()<0)) {
                    qDebug()<<"mount return dmesg";
                    IsOk=false;
                }
                if (!IsOk) {
                    Process.terminate();
                    Process.close();
                } else {
                    QString DmesgInfo=QString().fromLocal8Bit(Process.readAllStandardOutput());
                    Process.terminate();
                    Process.close();

                    // line we search is like "[dev without number] Attached"
                    QString ToFind=Device.mid(QString("/dev/").length());
                    if ((ToFind[ToFind.length()-1]>='0')&&(ToFind[ToFind.length()-1]<='9')) ToFind=ToFind.left(ToFind.length()-1);
                    ToFind="["+ToFind+"] Attached";

                    // Parse all line in Dmesg to try find line containing "[dev without number] Attached"
                    QString DriveTypeStr;
                    QString DmesgLine;
                    while (DmesgInfo!="") {
                        if (DmesgInfo.indexOf("\n")!=-1) {
                            DmesgLine=DmesgInfo.left(DmesgInfo.indexOf("\n"));
                            DmesgInfo=DmesgInfo.mid(DmesgInfo.indexOf("\n")+QString("\n").length());
                        } else {
                            DmesgLine=DmesgInfo;
                            DmesgInfo="";
                        }
                        if (DmesgLine.indexOf(ToFind)!=-1) DriveTypeStr=DmesgLine.mid(DmesgLine.indexOf(ToFind)+ToFind.length()+1);
                    }
                    if (DriveTypeStr=="SCSI removable disk") {
                        IconDrive=QIcon(IconsPath+"16x16"+DEV_USB);
                        IconDrive.addFile(IconsPath+"48x48"+DEV_USB);
                        IconDrive.addFile(IconsPath+"128x128"+DEV_USB);
                    }
                }
            }

            if (!Path.endsWith(QDir::separator())) Path=Path+QDir::separator();
            if (IconDrive.isNull()) {
                IconDrive   =QIcon(IconsPath+"16x16"+DEV_HDD);
                IconDrive.addFile(IconsPath+"48x48"+DEV_HDD);
                IconDrive.addFile(IconsPath+"128x128"+DEV_HDD);
            }
        }

        Path.replace("\\","/");
        if (Alias!="") Label=Alias; else if ((Path.length()>2)&&(Path.mid(1).indexOf("/")!=-1)) {
            Label=Path.mid(1).mid(Path.mid(1).indexOf("/")+1);
            if (Label.endsWith("/")) Label=Label.left(Label.indexOf("/"));
        }
    #endif

    // Check if there is an autorun.inf, a desktop.ini or folder.jpg
    if (Path!="") {
        QFileInfoList Directorys=QDir(Path).entryInfoList(QDir::Files|QDir::Hidden);
        for (int j=0;j<Directorys.count();j++) {
            if (Directorys[j].fileName().toLower()=="autorun.inf") {
                QString FileName=Path+Directorys[j].fileName();
                QFile   FileIO(FileName);
                if (FileIO.open(QIODevice::ReadOnly|QIODevice::Text)) {
                    QTextStream FileST(&FileIO);
                    while (!FileST.atEnd()) {
                        QString Line=FileST.readLine();
                        Line.trimmed();
                        if ((Line.toUpper().startsWith("ICON"))&&(Line.indexOf("=")!=-1)) {
                            Line=Line.mid(Line.indexOf("=")+1).trimmed();
                            if (Line.toLower().endsWith(".jpg") || Line.toLower().endsWith(".png") || Line.toLower().endsWith(".ico")) IconDrive=QIcon(AdjustDirForOS(Path+Line));
                            #if defined(Q_OS_WIN)
                            else IconDrive=GetIconForFileOrDir(AdjustDirForOS(Path+Line),0);
                            #endif
                        }
                    }
                    FileIO.close();
                }
            } else if (Directorys[j].fileName().toLower()=="desktop.ini") {
                QString FileName=Path;
                if (!FileName.endsWith(QDir::separator())) FileName=FileName+QDir::separator();
                FileName=FileName+Directorys[j].fileName();
                QFile   FileIO(FileName);
                QString IconFile ="";
                #if defined(Q_OS_WIN)
                int     IconIndex=0;
                #endif
                if (FileIO.open(QIODevice::ReadOnly/*|QIODevice::Text*/)) {
                    // Sometimes this kind of files have incorrect line terminator : nor \r\n nor \n
                    QTextStream FileST(&FileIO);
                    QString     AllInfo=FileST.readAll();
                    QString     Line="";
                    while (AllInfo!="") {
                        int j=0;
                        while ((j<AllInfo.length())&&((AllInfo[j]>=char(32))||(AllInfo[j]==9))) j++;
                        if (j<AllInfo.length()) {
                            Line=AllInfo.left(j);
                            while ((j<AllInfo.length())&&(AllInfo[j]<=char(32))) j++;
                            if (j<AllInfo.length()) AllInfo=AllInfo.mid(j); else AllInfo="";
                        } else {
                            Line=AllInfo;
                            AllInfo="";
                        }
                        #if defined(Q_OS_WIN)
                        if ((Line.toUpper().startsWith("ICONINDEX"))&&(Line.indexOf("=")!=-1)) {
                            IconIndex=Line.mid(Line.indexOf("=")+1).toInt();
                        } else
                        #endif
                        if ((Line.toUpper().startsWith("ICONFILE"))&&(Line.indexOf("=")!=-1)) {
                            Line=Line.mid(Line.indexOf("=")+1).trimmed();
                            // Replace all variables like %systemroot%
                            while (Line.indexOf("%")!=-1) {
                                QString Var=Line.mid(Line.indexOf("%")+1);  Var=Var.left(Var.indexOf("%"));
                                QString Value=getenv(Var.toLocal8Bit());
                                Line.replace("%"+Var+"%",Value,Qt::CaseInsensitive);
                            }
                            if (QFileInfo(Line).isRelative()) IconFile=AdjustDirForOS(Path+(Path.endsWith(QDir::separator())?QString(""):QDir::separator())+Line);
                                else IconFile=AdjustDirForOS(QFileInfo(Line).absoluteFilePath());
                        }
                    }
                    FileIO.close();
                }
                if (IconFile.toLower().endsWith(".jpg") || IconFile.toLower().endsWith(".png") || IconFile.toLower().endsWith(".ico")) IconDrive=QIcon(IconFile);
                #if defined(Q_OS_WIN)
                else IconDrive=GetIconForFileOrDir(IconFile,IconIndex);
                #endif
            } else if (Directorys[j].fileName().toLower()=="folder.jpg") {
                IconDrive=QIcon(Path+Directorys[j].fileName());
            }
        }
    }
}


//====================================================================================================================

QCustomFolderTree::QCustomFolderTree(QWidget *parent):QTreeWidget(parent) {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomFolderTree::QCustomFolderTree";
    #endif
    ShowHidden  =false;
    ShowMntDrive=false;
    IconsPath   ="";
    if (QFileInfo("/usr/share/icons/gnome/16x16"+QString(DEV_CDROM)).exists()) IconsPath="/usr/share/icons/gnome/";
        else if (QFileInfo("/usr/share/icons/default.kde4/16x16"+QString(DEV_CDROM)).exists()) IconsPath="/usr/share/icons/default.kde4/";

    connect(this,SIGNAL(itemExpanded(QTreeWidgetItem *)),this,SLOT(s_itemExpanded(QTreeWidgetItem *)));
}

//====================================================================================================================
// Private utility function to be use to populate HardDriveList

void QCustomFolderTree::CreateHardDriveList() {
    #if defined(Q_OS_WIN)

        QSettings Settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders",QSettings::NativeFormat);
        HardDrive.append(cHardDriveDescriptor(Settings.value("Personal").toString(),QApplication::translate("QCustomFolderTree","Personal folder")));
        foreach(QFileInfo drive,QDir::drives()) HardDrive.append(cHardDriveDescriptor(drive.filePath(),""));

    #elif defined(Q_OS_UNIX) && !defined(Q_OS_MACX)

        HardDrive.append(cHardDriveDescriptor(QDir::homePath(),QApplication::translate("QCustomFolderTree","Personal folder")));
        HardDrive.append(cHardDriveDescriptor("/",QApplication::translate("QCustomFolderTree","System files")));

        // list mounted drives using mount command
        QProcess    Process;
        bool        IsOk=true;
        Process.setProcessChannelMode(QProcess::MergedChannels);

        Process.start("mount");
        if (!Process.waitForStarted()) {
            qDebug()<<"Impossible to execute mount";
            IsOk=false;
        }
        if (IsOk && !Process.waitForFinished()) {
            Process.kill();
            qDebug()<<"Error during mount process";
            IsOk=false;
        }
        if (IsOk && (Process.exitStatus()<0)) {
            qDebug()<<"mount return error";
            IsOk=false;
        }
        if (!IsOk) {
            Process.terminate();
            Process.close();
        } else {
            QString Info=QString().fromLocal8Bit(Process.readAllStandardOutput());
            Process.terminate();
            Process.close();

            QString InfoLine;
            while (Info!="") {
                if (Info.indexOf("\n")!=-1) {
                    InfoLine=Info.left(Info.indexOf("\n"));
                    Info    =Info.mid(Info.indexOf("\n")+1);
                } else {
                    InfoLine=Info;
                    Info    ="";
                }
                if (InfoLine.indexOf(" ")!=-1) {
                    QString Device=InfoLine.left(InfoLine.indexOf(" "));
                    if (Device.startsWith("/dev/")) {
                        cHardDriveDescriptor ToAppend("",Device);
                        if (ToAppend.Path!="/") HardDrive.append(ToAppend);
                    }
                }
            }
        }
    #endif
    foreach(cHardDriveDescriptor HDD,HardDrive)
        if ((!HDD.Path.startsWith("/mnt/"))||(ShowMntDrive))
            addTopLevelItem(CreateItem(HDD.Label,HDD.Path,HDD.IconDrive));
}

//====================================================================================================================
// Private utility function to be use to know if a folder have child (depends on ShowHidden property)
//      FilePath : Path to check

bool QCustomFolderTree::IsFolderHaveChild(QString FilePath) {
    #ifdef DEBUGMODE
    qDebug() << "IN:QTreeWidgetItem::IsFolderHaveChild";
    #endif
    QFileInfoList List=QDir(FilePath).entryInfoList(QDir::Dirs|QDir::NoDotAndDotDot|(ShowHidden?QDir::Hidden:QDir::Dirs));
    return List.count()>0;
}

//====================================================================================================================
// Private utility function to be use to know if drive's folder is readonly (CD/DVD ROM)
//      FilePath : Path to check

bool QCustomFolderTree::IsReadOnlyDrive(QString Folder) {
    #ifdef DEBUGMODE
    qDebug() << "IN:QTreeWidgetItem::IsReadOnlyDrive";
    #endif
    bool IsReadOnly=false;
    for (int i=0;i<HardDrive.count();i++) if (Folder.startsWith(HardDrive[i].Path)) {
        IsReadOnly=HardDrive[i].IsReadOnly;
        break;
    }
    return IsReadOnly;
}

//====================================================================================================================
// Private utility function to create a new QTreeWidgetItem
//      Text     : Text to be shown
//      Icon     : Icon for the Item
//      FilePath : Path to get Icon

QTreeWidgetItem *QCustomFolderTree::CreateItem(QString Text,QString FilePath,QIcon Icon) {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomFolderTree::CreateItem";
    #endif

    QTreeWidgetItem *Current=new QTreeWidgetItem();
    Current->setText(0,Text);
    Current->setIcon(0,Icon);
    if (IsFolderHaveChild(FilePath)) {
        QTreeWidgetItem *SubItem=new QTreeWidgetItem();
        SubItem->setText(0,TAG);                            // add a tag to sub item
        Current->addChild(SubItem);
    }
    return Current;
}

//====================================================================================================================
// Private utility function to get Icon corresponding to a FilePath
//      FilePath : Path to get Icon
//      IsDrive  : True if root Item

QIcon QCustomFolderTree::GetIcon(QString FilePath) {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomFolderTree::GetIcon";
    #endif

    if (!FilePath.endsWith(QDir::separator())) FilePath=FilePath+QDir::separator();

    #if defined(Q_OS_UNIX) && !defined(Q_OS_MACX)
    if (FilePath.startsWith("~")) FilePath=QDir::homePath()+FilePath.mid(2);
    #endif

    QIcon   RetIcon;

    if (RetIcon.isNull()) {
        // If not a root item but a standard item

        // Check if a folder.jpg file exist
        if (RetIcon.isNull()) {
            QFileInfoList Directorys=QDir(FilePath).entryInfoList(QDir::Files);
            for (int j=0;j<Directorys.count();j++) if (Directorys[j].fileName().toLower()=="folder.jpg") {
                QString FileName=FilePath+Directorys[j].fileName();
                RetIcon=QIcon(FileName);
            }
        }
    }

    // Check if there is an desktop.ini
    if (RetIcon.isNull()) {
        QFileInfoList Directorys=QDir(FilePath).entryInfoList(QDir::Files|QDir::Hidden);
        for (int j=0;j<Directorys.count();j++) if (Directorys[j].fileName().toLower()=="desktop.ini") {
            QString FileName=FilePath;
            if (!FileName.endsWith(QDir::separator())) FileName=FileName+QDir::separator();
            FileName=FileName+Directorys[j].fileName();
            QFile   FileIO(FileName);
            QString IconFile ="";
            #if defined(Q_OS_WIN)
            int     IconIndex=0;
            #endif
            if (FileIO.open(QIODevice::ReadOnly/*|QIODevice::Text*/)) {
                // Sometimes this kind of files have incorrect line terminator : nor \r\n nor \n
                QTextStream FileST(&FileIO);
                QString     AllInfo=FileST.readAll();
                QString     Line="";
                while (AllInfo!="") {
                    int j=0;
                    while ((j<AllInfo.length())&&((AllInfo[j]>=char(32))||(AllInfo[j]==9))) j++;
                    if (j<AllInfo.length()) {
                        Line=AllInfo.left(j);
                        while ((j<AllInfo.length())&&(AllInfo[j]<=char(32))) j++;
                        if (j<AllInfo.length()) AllInfo=AllInfo.mid(j); else AllInfo="";
                    } else {
                        Line=AllInfo;
                        AllInfo="";
                    }
                    #if defined(Q_OS_WIN)
                    if ((Line.toUpper().startsWith("ICONINDEX"))&&(Line.indexOf("=")!=-1)) {
                        IconIndex=Line.mid(Line.indexOf("=")+1).toInt();
                    } else
                    #endif
                    if ((Line.toUpper().startsWith("ICONFILE"))&&(Line.indexOf("=")!=-1)) {
                        Line=Line.mid(Line.indexOf("=")+1).trimmed();
                        // Replace all variables like %systemroot%
                        while (Line.indexOf("%")!=-1) {
                            QString Var=Line.mid(Line.indexOf("%")+1);  Var=Var.left(Var.indexOf("%"));
                            QString Value=getenv(Var.toLocal8Bit());
                            Line.replace("%"+Var+"%",Value,Qt::CaseInsensitive);
                        }
                        if (QFileInfo(Line).isRelative()) IconFile=AdjustDirForOS(FilePath+(FilePath.endsWith(QDir::separator())?QString(""):QDir::separator())+Line);
                            else IconFile=AdjustDirForOS(QFileInfo(Line).absoluteFilePath());
                    }
                }
                FileIO.close();
            }
            if (IconFile.toLower().endsWith(".jpg") || IconFile.toLower().endsWith(".png") || IconFile.toLower().endsWith(".ico")) RetIcon=QIcon(IconFile);
            #if defined(Q_OS_WIN)
            else RetIcon=GetIconForFileOrDir(IconFile,IconIndex);
            #endif
        }
    }

    if (RetIcon.isNull()) {
        if (!FilePath.endsWith(QDir::separator()))  FilePath=FilePath+QDir::separator();
        // If root item
        for (int i=0;i<HardDrive.count();i++) if (FilePath==HardDrive[i].Path) RetIcon=HardDrive[i].IconDrive;
    }

    // If nothing found, use default closed folder icon

    #if defined(Q_OS_WIN)
        if (RetIcon.isNull()) RetIcon=GetIconForFileOrDir(QString(getenv("SystemRoot"))+QString("\\system32\\SHELL32.dll"),-4);
    #elif defined(Q_OS_UNIX) && !defined(Q_OS_MACX)
        if (RetIcon.isNull()) {
            RetIcon=QIcon(IconsPath+"16x16"+FOLDER_STD);
            RetIcon.addFile(IconsPath+"48x48"+FOLDER_STD);
            RetIcon.addFile(IconsPath+"128x128"+FOLDER_STD);
        }
    #endif

    return RetIcon;
}

//====================================================================================================================
// Public utility function to get Path from an Item
//      Item     : Item to get Path from
//      TreeMode : if true, don't make alias interpretation

QString QCustomFolderTree::GetFolderPath(const QTreeWidgetItem *Item,bool TreeMode) {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomFolderTree::GetFolderPath";
    #endif

    if (!Item) return "";

    QString FilePath;

    // Parse Item to Compose path
    while (Item!=NULL) {
        // if root item
        if (!Item->parent()) {
            QString RootStr=Item->text(0);
            // For windows we display drive like D:\[Label], then cut after [ if exist
            if (Item->text(0).indexOf("[")!=-1) RootStr=Item->text(0).left(Item->text(0).indexOf("["));
            if (!TreeMode) {
                // Search if text is a registered alias, then replace text with path
                for (int i=0;i<HardDrive.count();i++) if (HardDrive[i].Label==RootStr) {
                    #if defined(Q_OS_UNIX) && !defined(Q_OS_MACX)
                    if (RootStr==QApplication::translate("QCustomFolderTree","Personal folder")) RootStr="~"; else
                    #endif
                    RootStr=HardDrive[i].Path;
                }
            }
            if (!RootStr.endsWith(QDir::separator())) RootStr=RootStr+QDir::separator();
            FilePath=RootStr+FilePath;

            Item=NULL;
        } else {
            FilePath=Item->text(0)+((Item->text(0)!=QDir().separator())&&(FilePath!=QString(""))?QString(QDir().separator()):QString(""))+FilePath;
            Item=Item->parent();
        }
    }
    return FilePath;
}

//====================================================================================================================
// Public utility function to get Path from from the current selected Item

QString QCustomFolderTree::GetCurrentFolderPath() {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomFolderTree::GetCurrentFolderPath";
    #endif

    return GetFolderPath(currentItem(),false);
}

//====================================================================================================================
// we use this signal function instead of overloaded itemExpanded function because we need to modify item and
// overloaded function use const !

void QCustomFolderTree::s_itemExpanded(QTreeWidgetItem *item) {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomFolderTree::expandItem";
    #endif

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    // Expand node if necessary
    if ((item->childCount()==1)&&(item->child(0)->text(0)==TAG)) {

        setUpdatesEnabled(false);

        QTreeWidgetItem     *CurItem=item;
        QString             Folder=GetFolderPath(CurItem,false);
        QFileInfoList       Directorys;
        int                 i,k;

        // remove tag to sub item
        QTreeWidgetItem *SubItem=CurItem->child(0);
        CurItem->removeChild(SubItem);
        delete SubItem;

        Directorys=QDir(Folder).entryInfoList(QDir::Dirs|QDir::NoDotAndDotDot|(ShowHidden?QDir::Hidden:QDir::Dirs));
        for (k=0;k<Directorys.count();k++)
            if (Directorys[k].isDir())
                CurItem->addChild(CreateItem(Directorys[k].fileName(),Directorys[k].absoluteFilePath(),GetIcon(Directorys[k].absoluteFilePath())));

        // Unselect previous selected item and then select new one
        QList<QTreeWidgetItem *>    List  =selectedItems();
        bool                        IsFind=false;

        for (i=0;i<List.count();i++)
            if (List[i]==CurItem) IsFind=true; else List[i]->setSelected(false);

        if (!IsFind) setCurrentItem(CurItem);

        setUpdatesEnabled(true);

    }

    QApplication::restoreOverrideCursor();
}

//====================================================================================================================

cHardDriveDescriptor *QCustomFolderTree::SearchRealDrive(QString Path) {
    #if defined(Q_OS_UNIX) && !defined(Q_OS_MACX)
    if (Path.startsWith("~")) Path=QDir::homePath()+Path.mid(2);
    #endif
    Path=QDir(Path).canonicalPath();
    if (!Path.endsWith(QDir::separator())) Path=Path+QDir::separator();
    for (int i=0;i<HardDrive.count();i++) if ((HardDrive[i].Path!=QString("/"))&&(Path.startsWith(HardDrive[i].Path))) return &HardDrive[i];
    for (int i=0;i<HardDrive.count();i++) if (HardDrive[i].Path==QString("/")) return &HardDrive[i];
    return NULL;
}

//====================================================================================================================

QString QCustomFolderTree::RealPathToTreePath(QString Path) {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomFolderTree::RealPathToTreePath";
    #endif

    for (int i=0;i<HardDrive.count();i++) if ((HardDrive[i].Path!=QString("/"))&&(Path.startsWith(HardDrive[i].Path))) {
        Path=HardDrive[i].Label+Path.mid(HardDrive[i].Path.length()-1);
        break;
    }
    return Path;
}

//====================================================================================================================

void QCustomFolderTree::SetSelectItemByPath(QString Path) {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomFolderTree::SetSelectedItemByPath";
    #endif

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    setUpdatesEnabled(false);

    QFileInfoList       Directorys;
    int                 i,j,k,NbrChild;
    QStringList         Folders;
    QString             CurrentFolder;
    QTreeWidgetItem     *Current=NULL;

    #if defined(Q_OS_UNIX) && !defined(Q_OS_MACX)
    if (Path.startsWith("~")) Path=QApplication::translate("QCustomFolderTree","Personal folder")+Path.mid(2);
    #endif

    // Create a list with each part of the wanted Path
    while (Path.contains(QDir::separator())) {
        Folders.append(Path.mid(0,Path.indexOf(QDir::separator())));
        Path=Path.mid(Path.indexOf(QDir::separator())+QString(QDir::separator()).length());
    }
    Folders.append(Path);

    #if defined(Q_OS_UNIX) && !defined(Q_OS_MACX)
    if ((Folders.count()>=1)&&(Folders[0]=="")) Folders[0]=QApplication::translate("QCustomFolderTree","System files");
    #endif

    // find node in the tree and expand it if it was not previously expanded
    i=0;
    Current=topLevelItem(0);

    while ((Current!=NULL)&&(i<Folders.count())) {
        if (i==0) {
            // Search in topitemlist : Note : Search in reverse order to give preference to drive instead of /mnt/drive or /media/drive
            j=topLevelItemCount()-1;
            while ((j>=0)&&(topLevelItem(j)->text(0)!=Folders[i])) j--;
            if ((j>=0)&&(topLevelItem(j)->text(0)==Folders[i])) Current=topLevelItem(j);
                else Current=NULL;
        } else {
            j=0;
            // Search in current item child list
            while ((j<Current->childCount())&&(Current->child(j)->text(0)!=Folders[i])) j++;
            if ((j<Current->childCount())&&(Current->child(j)->text(0)==Folders[i])) Current=Current->child(j);
                else Current=NULL;
        }
        if (Current) {
            CurrentFolder   =GetFolderPath(Current,false);
            NbrChild        =Current->childCount();
            if ((NbrChild==1)&&(Current->child(0)->text(0)==TAG)) { // If not initialize
                Current->removeChild(Current->child(0));      // remove tag to sub item
                QString RealPath=CurrentFolder;
                #if defined(Q_OS_UNIX) && !defined(Q_OS_MACX)
                if (RealPath.startsWith("~")) RealPath=QDir::homePath()+RealPath.mid(2);
                #endif
                Directorys=QDir(RealPath).entryInfoList(QDir::Dirs|QDir::NoDotAndDotDot|(ShowHidden?QDir::Hidden:QDir::Dirs));
                for (k=0;k<Directorys.count();k++) Current->addChild(CreateItem(Directorys[k].fileName(),Directorys[k].absoluteFilePath(),GetIcon(Directorys[k].absoluteFilePath())));
            }
            Current->setExpanded(true);
        } else Current=NULL; // Error ????
        i++;
    }

    // Unselect previous selected item and then select new one
    if (Current) {
        QList<QTreeWidgetItem *>    List=selectedItems();
        bool                        IsFind=false;
        for (i=0;i<List.count();i++) if (List[i]==Current) IsFind=true; else List[i]->setSelected(false);
        if (!IsFind) setCurrentItem(Current);
    }
    resizeColumnToContents(0);
    setUpdatesEnabled(true);

    QApplication::restoreOverrideCursor();
}

//====================================================================================================================

class cTreeItemDescriptor {
public:
    QTreeWidgetItem *Item;
    bool            IsFound;

    cTreeItemDescriptor(QTreeWidgetItem *TheItem) {
        Item=TheItem;
        IsFound=false;
    }
};

void QCustomFolderTree::RefreshItemByPath(QString Path,int Level) {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomFolderTree::SetSelectedItemByPath";
    #endif

    QString RealPath=Path;
    int     i,j;

    // Construct RealPath
    for (i=0;i<HardDrive.count();i++) if (RealPath.startsWith(HardDrive[i].Label)) {
        if (RealPath.startsWith(QDir::separator())) RealPath=HardDrive[i].Path+RealPath.mid(HardDrive[i].Label.length()+1); else {
            RealPath=HardDrive[i].Path+RealPath.mid(HardDrive[i].Label.length());
            if (RealPath.endsWith("//")) RealPath=RealPath.left(RealPath.length()-1);
            #if defined(Q_OS_UNIX) && !defined(Q_OS_MACX)
            if (RealPath.startsWith("//")) RealPath=RealPath.mid(1);
            #endif
        }
        break;
    }
    if ((Level==0)&&(IsReadOnlyDrive(RealPath))) return;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    QString             PartPath=Path;
    QStringList         Folders;
    QTreeWidgetItem     *Current=NULL;
    QTreeWidgetItem     *SubItem=NULL;

    // Adjust Path
    #if defined(Q_OS_UNIX) && !defined(Q_OS_MACX)
    Path.replace("~",QApplication::translate("QCustomFolderTree","Personal folder"));
    #endif

    // Create a list with each part of the wanted Path
    while (PartPath.contains(QDir::separator())) {
        Folders.append(PartPath.mid(0,PartPath.indexOf(QDir::separator())));
        PartPath=PartPath.mid(PartPath.indexOf(QDir::separator())+QString(QDir::separator()).length());
    }
    if (PartPath!="") Folders.append(PartPath);

    // Now we can search corresponding item in the tree
    i=0;
    while ((i<this->topLevelItemCount())&&(topLevelItem(i)->text(0)!=Folders[0])) i++;
    if ((i<this->topLevelItemCount())&&(topLevelItem(i)->text(0)==Folders[0])) {
        // We have found the toplevel, now down the tree
        Current=topLevelItem(i);
        j=1;
        while ((Current)&&(j<Folders.count())) {
            i=0;
            while ((i<Current->childCount())&&(Current->child(i)->text(0)!=Folders[j])) i++;
            if ((i<Current->childCount())&&(Current->child(i)->text(0)==Folders[j])) {
                Current=Current->child(i);
                j++;
            } else Current=NULL;
        }
    }

    // if item found
    if (Current) {
        if (Level==0) setUpdatesEnabled(false);

        // Set icon for this current item
        Current->setIcon(0,GetIcon(RealPath));

        // Check if havechild status have change
        if (IsFolderHaveChild(RealPath)) {
            // if folder now have child
            if (Current->childCount()==0) {
                QTreeWidgetItem *SubItem=new QTreeWidgetItem();
                SubItem->setText(0,TAG);                            // add a tag to sub item
                Current->addChild(SubItem);
            }
        } else {
            // if folder no longer have child
            while (Current->childCount()>0) {
                SubItem=Current->child(0);
                Current->removeChild(SubItem);
                delete SubItem;
            }
        }

        // Construct a list with actual known folders
        QList<cTreeItemDescriptor>  CurrentList;
        for (i=0;i<Current->childCount();i++) CurrentList.append(cTreeItemDescriptor(Current->child(i)));

        // Construct a second list for real folders
        QFileInfoList   Directorys=QDir(RealPath).entryInfoList(QDir::Dirs|QDir::NoDotAndDotDot|(ShowHidden?QDir::Hidden:QDir::Dirs));
        QStringList     FolderToAdd;
        bool            isFound;
        for (i=0;i<Directorys.count();i++) {
            isFound=false;
            // for each item search if it is present in current list
            for (j=0;j<CurrentList.count();j++) if (Directorys[i].fileName()==CurrentList[j].Item->text(0)) {
                CurrentList[j].IsFound=true;
                isFound=true;
                break;
            }
            // if folder not found add it to to FolderToAdd List
            if (!isFound) FolderToAdd.append(Directorys[i].fileName());
        }

        // Now we have 2 lists to work with
        for (i=0;i<CurrentList.count();i++) {
            // update all previously existing folder
            if (CurrentList[i].IsFound) {
                QString ChildPath=Path;
                if (!ChildPath.endsWith(QDir::separator())) ChildPath=ChildPath+QDir::separator();
                ChildPath=ChildPath+CurrentList[i].Item->text(0);
                RefreshItemByPath(ChildPath,Level+1);
            } else {

                // remove existing folder which no longer exist
                SubItem=CurrentList[i].Item;
                Current->removeChild(SubItem);
                delete SubItem;
            }
        }
        // add new folder
        QString ChildPath=QDir().absoluteFilePath(RealPath);
        if (!ChildPath.endsWith(QDir::separator())) ChildPath=ChildPath+QDir::separator();
        for (i=0;i<FolderToAdd.count();i++) {
            int ItemBefore=0;
            while ((ItemBefore<Current->childCount())&&(FolderToAdd[i]>=Current->child(ItemBefore)->text(0))) ItemBefore++;
            Current->insertChild(ItemBefore,CreateItem(FolderToAdd[i],ChildPath+FolderToAdd[i],GetIcon(ChildPath+FolderToAdd[i])));
        }

        if (Level==0) setUpdatesEnabled(true);
    }

    QApplication::restoreOverrideCursor();
}
