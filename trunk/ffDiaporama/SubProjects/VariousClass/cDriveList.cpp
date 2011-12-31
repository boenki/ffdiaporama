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

#include <QtCore>
#include <QtDebug>
#include <QtGui/QApplication>
#include <QFileInfoList>
#include <QFileInfo>
#include <QDir>
#include <QFile>

#if defined(Q_OS_WIN)
    #include <windows.h>
    #include <QSettings>
    #include <QPixmapCache>
#endif
#ifndef SHGFI_ADDOVERLAYS
    #define SHGFI_ADDOVERLAYS 0x000000020
#endif

#include "cBaseApplicationConfig.h"
#include "cDriveList.h"

//#define DEBUGMODE

//====================================================================================================================

#if defined(Q_OS_WIN)

    // qt_fromWinHBITMAP From Qmmander Filemanager / Copyright (C) Alex Skoruppa 2009 (See:http://qmmander.googlecode.com/svn-history/r93/trunk/winfileinfo.cpp)
    QImage qt_fromWinHBITMAP(HDC hdc, HBITMAP bitmap, int w, int h) {
        #ifdef DEBUGMODE
        qDebug() << "IN:qt_fromWinHBITMAP";
        #endif
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

    //====================================================================================================================
    // convertHIconToPixmap From Qmmander Filemanager / Copyright (C) Alex Skoruppa 2009 (See:http://qmmander.googlecode.com/svn-history/r93/trunk/winfileinfo.cpp)
    QPixmap convertHIconToPixmap( const HICON icon) {
        #ifdef DEBUGMODE
        qDebug() << "IN:convertHIconToPixmap";
        #endif

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

    //====================================================================================================================
    // GetIconForFileOrDir adapted by domledom From Qmmander Filemanager / Copyright (C) Alex Skoruppa 2009 (See:http://qmmander.googlecode.com/svn-history/r93/trunk/winfileinfo.cpp)
    QIcon GetIconForFileOrDir(QString FileName,int IconIndex) {
        #ifdef DEBUGMODE
        qDebug() << "IN:GetIconForFileOrDir";
        #endif

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

//*******************************************************************************************************************************************************

cDriveDesc::cDriveDesc(QString ThePath,QString Alias) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cDriveDesc::cDriveDesc";
    #endif

    Flag        =2;         // New DriveDesc
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

    if (Alias==QApplication::translate("QCustomFolderTree","Personal folder")) IconDrive=DefaultUSERIcon;

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
            case DRIVE_CDROM     :  IconDrive=ParentList->DefaultCDROMIcon;  IsReadOnly=true;       break;
            case DRIVE_REMOTE    :  IconDrive=ParentList->DefaultREMOTEIcon;                        break;
            case DRIVE_REMOVABLE :  IconDrive=ParentList->DefaultREMOTEIcon;                        break;
            default              :  if (IconDrive.isNull()) IconDrive=ParentList->DefaultHDDIcon;   break;
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
                IconDrive   =DefaultCDROMIcon;
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
                    if (DriveTypeStr=="SCSI removable disk") IconDrive=DefaultHDDIcon;
                }
            }

            if (!Path.endsWith(QDir::separator())) Path=Path+QDir::separator();
            if (IconDrive.isNull()) IconDrive=DefaultHDDIcon;
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

//*******************************************************************************************************************************************************

cDriveList::cDriveList() {
    #ifdef DEBUGMODE
    qDebug() << "IN:cDriveList::cDriveList";
    #endif
}

//====================================================================================================================
// Private utility function to be use to populate ListList
bool cDriveList::SearchDrive(QString Path) {
    if (!Path.endsWith(QDir::separator())) Path=Path+QDir::separator();
    int i=0;
    while ((i<List.count())&&(List[i].Path!=Path)) i++;
    if ((i<List.count())&&(List[i].Path==Path)) {
        List[i].Flag=1;
        return true;
    } else return false;
}

//====================================================================================================================
// Utility function to be use to populate ListList

void cDriveList::UpdateDriveList() {
    #ifdef DEBUGMODE
    qDebug() << "IN:cDriveList::CreateListList";
    #endif

    for (int i=0;i<List.count();i++) List[i].Flag=0;

    #if defined(Q_OS_WIN)

        QSettings Settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders",QSettings::NativeFormat);
        if (!SearchDrive(Settings.value("Personal"))) List.append(cDriveDesc(Settings.value("Personal").toString(),QApplication::translate("QCustomFolderTree","Personal folder")));
        foreach(QFileInfo drive,QDir::drives()) if (!SearchDrive(drive.filePath())) List.append(cDriveDesc(drive.filePath(),""));

    #elif defined(Q_OS_UNIX) && !defined(Q_OS_MACX)

        if (!SearchDrive(QDir::homePath())) List.append(cDriveDesc(QDir::homePath(),QApplication::translate("QCustomFolderTree","Personal folder")));
        if (!SearchDrive("/"))              List.append(cDriveDesc("/",QApplication::translate("QCustomFolderTree","System files")));

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
                        cDriveDesc ToAppend("",Device);
                        if (ToAppend.Path!="/") if (!SearchDrive(ToAppend.Path)) List.append(ToAppend);
                    }
                }
            }
        }
    #endif
}

//====================================================================================================================
// Private utility function to get Icon corresponding to a FilePath
//      FilePath : Path to get Icon

QIcon cDriveList::GetFolderIcon(QString FilePath) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cDriveList::GetFolderIcon";
    #endif

    if (!FilePath.endsWith(QDir::separator())) FilePath=FilePath+QDir::separator();

    #if defined(Q_OS_UNIX) && !defined(Q_OS_MACX)
    if (FilePath.startsWith("~")) FilePath=QDir::homePath()+FilePath.mid(1);
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
        for (int i=0;i<List.count();i++) if (FilePath==List[i].Path) RetIcon=List[i].IconDrive;
    }

    // If nothing found, use default closed folder icon
    if (RetIcon.isNull()) return DefaultFOLDERIcon; else return RetIcon;
}
