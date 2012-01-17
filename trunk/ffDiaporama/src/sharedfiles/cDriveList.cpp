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
#endif

#include "cBaseApplicationConfig.h"
#include "cDriveList.h"

//#define DEBUGMODE

//*******************************************************************************************************************************************************

cDriveDesc::cDriveDesc(QString ThePath,QString Alias,cBaseApplicationConfig *ApplicationConfig) {
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

    if (Alias==QApplication::translate("QCustomFolderTree","Personal folder")) IconDrive=ApplicationConfig->DefaultUSERIcon.GetIcon(cCustomIcon::ICON16)->copy();

    // Adjust path depending on Operating System
    #if defined(Q_OS_WIN)
        Path.replace("/","\\");

        WCHAR       Drive[256+1];
        WCHAR       VolumeName[256+1];
        WCHAR       SysName[256+1];
        DWORD       SerialNumber;
        DWORD       MaxComponent;
        DWORD       FileSysFlag;

        QString PhysicalPath=Path;
        if ((PhysicalPath[1]==':')&&(PhysicalPath[2]=='\\')) PhysicalPath=PhysicalPath.left(3);
        MultiByteToWideChar(CP_ACP,0,PhysicalPath.toLocal8Bit(),-1,Drive,256+1);
        switch (GetDriveType(Drive)) {
            case DRIVE_CDROM     :  IconDrive=ApplicationConfig->DefaultCDROMIcon.GetIcon(cCustomIcon::ICON16)->copy();  IsReadOnly=true;       break;
            case DRIVE_REMOTE    :  IconDrive=ApplicationConfig->DefaultREMOTEIcon.GetIcon(cCustomIcon::ICON16)->copy();                        break;
            case DRIVE_REMOVABLE :  IconDrive=ApplicationConfig->DefaultREMOTEIcon.GetIcon(cCustomIcon::ICON16)->copy();                        break;
            default              :  if (IconDrive.isNull()) IconDrive=ApplicationConfig->DefaultHDDIcon.GetIcon(cCustomIcon::ICON16)->copy();   break;
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
                IconDrive   =ApplicationConfig->DefaultCDROMIcon.GetIcon(cCustomIcon::ICON16)->copy();
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
                    if (DriveTypeStr=="SCSI removable disk") IconDrive=ApplicationConfig->DefaultHDDIcon.GetIcon(cCustomIcon::ICON16)->copy();
                }
            }

            if (!Path.endsWith(QDir::separator())) Path=Path+QDir::separator();
            if (IconDrive.isNull()) IconDrive=ApplicationConfig->DefaultHDDIcon.GetIcon(cCustomIcon::ICON16)->copy();
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
                            if (Line.toLower().endsWith(".jpg") || Line.toLower().endsWith(".png") || Line.toLower().endsWith(".ico")) IconDrive=QIcon(AdjustDirForOS(Path+Line)).pixmap(16,16).toImage().copy();
                            #if defined(Q_OS_WIN)
                            else {
                                QIcon Ico(GetIconForFileOrDir(AdjustDirForOS(Path+Line),0));
                                IconDrive=Ico.pixmap(16,16).toImage();
                            }
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
                if (IconFile.toLower().endsWith(".jpg") || IconFile.toLower().endsWith(".png")) IconDrive=QImage(IconFile);
                else if (IconFile.toLower().endsWith(".ico")) {
                    QIcon Ico(IconFile);
                    if (!Ico.isNull()) IconDrive=Ico.pixmap(16,16).toImage();
                }

                #if defined(Q_OS_WIN)
                else {
                    QIcon Ico=GetIconForFileOrDir(IconFile,IconIndex);
                    if (!Ico.isNull()) IconDrive=Ico.pixmap(16,16).toImage();
                }
                #endif
            } else if (Directorys[j].fileName().toLower()=="folder.jpg") {
                IconDrive=QImage(Path+Directorys[j].fileName());
            }
        }
    }
}

//*******************************************************************************************************************************************************

cDriveList::cDriveList(cBaseApplicationConfig *TheApplicationConfig) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cDriveList::cDriveList";
    #endif
    ApplicationConfig=TheApplicationConfig;
}

//====================================================================================================================
// Private utility function to be use to populate ListList
bool cDriveList::SearchDrive(QString Path) {
    if (!Path.endsWith(QDir::separator())) Path=Path+QDir::separator();
    int i=0;
    while ((i<List.count())&&((List[i].Path!=Path)&&(List[i].Path+QDir::separator()!=Path))) i++;
    if ((i<List.count())&&((List[i].Path==Path)||(List[i].Path+QDir::separator()==Path))) {
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
        if (!SearchDrive(Settings.value("Personal").toString()))
            if (!SearchDrive(AdjustDirForOS(Settings.value("Personal").toString())))
                List.append(cDriveDesc(Settings.value("Personal").toString(),QApplication::translate("QCustomFolderTree","Personal folder"),ApplicationConfig));
        foreach(QFileInfo drive,QDir::drives()) if (!SearchDrive(AdjustDirForOS(drive.filePath()))) List.append(cDriveDesc(drive.filePath(),"",ApplicationConfig));

    #elif defined(Q_OS_UNIX) && !defined(Q_OS_MACX)

        if (!SearchDrive(QDir::homePath())) List.append(cDriveDesc(QDir::homePath(),QApplication::translate("QCustomFolderTree","Personal folder"),ApplicationConfig));
        if (!SearchDrive("/"))              List.append(cDriveDesc("/",QApplication::translate("QCustomFolderTree","System files"),ApplicationConfig));

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
                        cDriveDesc ToAppend("",Device,ApplicationConfig);
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
        for (int i=0;i<List.count();i++) if (FilePath==List[i].Path) RetIcon=QIcon(QPixmap().fromImage(List[i].IconDrive));
    }

    // If nothing found, use default closed folder icon
    if (RetIcon.isNull()) return ApplicationConfig->DefaultFOLDERIcon.GetIcon(); else return RetIcon;
}
