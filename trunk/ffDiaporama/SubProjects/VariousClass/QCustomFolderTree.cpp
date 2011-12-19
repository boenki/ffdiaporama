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
#endif

#include "QCustomFolderTree.h"
#include "cBaseApplicationConfig.h"

#define TAG "<to expand>"

// Define icon images
#define DEV_CDROM       "img/dev_cdrom.png"
#define DEV_HDD         "img/dev_hdd.png"
#define DEV_NETWORK     "img/dev_network.png"
#define DEV_USB         "img/dev_usb.png"
#define DEV_USER        "img/dev_user.png"
#define FOLDER_STD      "img/FolderClose.png"

#define DEBUGMODE

cHardDriveDescriptor::cHardDriveDescriptor(QString ThePath,QString Alias) {
    if (ThePath=="") {
        Path    ="";
        Device  =Alias;
        Alias   ="";
    } else {
        Path    =ThePath;
        Device  ="";
    }
    Label   ="";
    Size    =0;
    Used    =0;
    Avail   =0;

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
        GetVolumeInformation(Drive,VolumeName,sizeof(WCHAR)*(256+1),&SerialNumber,&MaxComponent,&FileSysFlag,SysName,sizeof(WCHAR)*(256+1));
        Label=QString().fromWCharArray(VolumeName);
        switch (GetDriveType(Drive)) {
            case DRIVE_CDROM     :  IconDrive=QIcon(DEV_CDROM);     break;
            case DRIVE_REMOTE    :  IconDrive=QIcon(DEV_NETWORK);   break;
            case DRIVE_REMOVABLE :  IconDrive=QIcon(DEV_USB);       break;
            default              :  IconDrive=QIcon(DEV_HDD);       break;
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

            if (Device.startsWith("/dev/sr") || Device.startsWith("/dev/scd"))  IconDrive=QIcon(DEV_CDROM); else {
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
                    if (DriveTypeStr=="SCSI removable disk") IconDrive=QIcon(DEV_USB);
                }
            }

            if (!Path.endsWith(QDir::separator())) Path=Path+QDir::separator();
            if (IconDrive.isNull()) IconDrive=QIcon(DEV_HDD);
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
                            IconDrive=QIcon(AdjustDirForOS(Path+Line));
                        }
                    }
                    FileIO.close();
                }
            } else if (Directorys[j].fileName().toLower()=="desktop.ini") {
                QString FileName=Path+Directorys[j].fileName();
                QFile   FileIO(FileName);
                if (FileIO.open(QIODevice::ReadOnly|QIODevice::Text)) {
                    QTextStream FileST(&FileIO);
                    while (!FileST.atEnd()) {
                        QString Line=FileST.readLine();
                        Line.trimmed();
                        if ((Line.toUpper().startsWith("ICONFILE"))&&(Line.indexOf("=")!=-1)) {
                            Line=Line.mid(Line.indexOf("=")+1).trimmed();
                            if (!Line.toLower().endsWith(".dll")&&!Line.toLower().endsWith(".exe")) IconDrive=QIcon(AdjustDirForOS(Path+Line));
                        }
                    }
                    FileIO.close();
                }
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
    ShowHidden=false;
    connect(this,SIGNAL(itemExpanded(QTreeWidgetItem *)),this,SLOT(s_itemExpanded(QTreeWidgetItem *)));
}

//====================================================================================================================
// Private utility function to be use to populate HardDriveList

void QCustomFolderTree::CreateHardDriveList() {
    #if defined(Q_OS_WIN)

        QSettings Settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders",QSettings::NativeFormat);
        HardDrive.append(new cHardDriveDescriptor(Settings.value("Personal").toString(),QApplication::translate("QCustomFolderSelector","Personal folder")));
        foreach(QFileInfo drive,QDir::drives()) HardDrive.append(cHardDriveDescriptor(drive.filePath(),drive.filePath());

    #elif defined(Q_OS_UNIX) && !defined(Q_OS_MACX)

        HardDrive.append(cHardDriveDescriptor(QDir::homePath(),QApplication::translate("QCustomFolderSelector","Personal folder")));
        HardDrive.append(cHardDriveDescriptor("/",QApplication::translate("QCustomFolderSelector","System files")));

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
   foreach(cHardDriveDescriptor HDD,HardDrive) addTopLevelItem(CreateItem(HDD.Label,HDD.Path,HDD.IconDrive));
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

QIcon QCustomFolderTree::GetIcon(QString FilePath,bool IsDrive) {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomFolderTree::GetIcon";
    #endif

    QIcon   RetIcon;

    if (!FilePath.endsWith(QDir::separator()))  FilePath=FilePath+QDir::separator();
    if (IsDrive) {

        // If root item
        for (int i=0;i<HardDrive.count();i++) if (FilePath==HardDrive[i].Path) RetIcon=HardDrive[i].IconDrive;

    } else {
        // If not a root item but a standard item

        // Check if a folder.jpg file exist
        if (RetIcon.isNull()) {
            QFileInfoList Directorys=QDir(FilePath).entryInfoList(QDir::Files);
            for (int j=0;j<Directorys.count();j++) if (Directorys[j].fileName().toLower()=="folder.jpg") {
                QString FileName=FilePath+Directorys[j].fileName();
                RetIcon=QIcon(FileName);
            }
        }

        // Check if there is an desktop.ini
        if (RetIcon.isNull()) {
            QFileInfoList Directorys=QDir(FilePath).entryInfoList(QDir::Files|QDir::Hidden);
            for (int j=0;j<Directorys.count();j++) if (Directorys[j].fileName().toLower()=="desktop.ini") {
                QString FileName=FilePath+Directorys[j].fileName();
                QFile   FileIO(FileName);
                if (FileIO.open(QIODevice::ReadOnly|QIODevice::Text)) {
                    QTextStream FileST(&FileIO);
                    while (!FileST.atEnd()) {
                        QString Line=FileST.readLine();
                        Line.trimmed();
                        if ((Line.toUpper().startsWith("ICONFILE"))&&(Line.indexOf("=")!=-1)) {
                            Line=Line.mid(Line.indexOf("=")+1).trimmed();
                            if (!Line.toLower().endsWith(".dll")&&!Line.toLower().endsWith(".exe")) RetIcon=QIcon(AdjustDirForOS(FilePath+Line));
                        }
                    }
                    FileIO.close();
                }
            }
        }
    }

    // If nothing found, use default closed folder icon
    if (RetIcon.isNull()) RetIcon=QIcon(FOLDER_STD);

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
                for (int i=0;i<HardDrive.count();i++) if (HardDrive[i].Label==RootStr) RootStr=HardDrive[i].Path;
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
        QString             Folder=GetFolderPath(CurItem,true);
        QFileInfoList       Directorys;
        int                 i,k;

        // remove tag to sub item
        QTreeWidgetItem *SubItem=CurItem->child(0);
        CurItem->removeChild(SubItem);
        delete SubItem;

        Directorys=QDir(Folder).entryInfoList(QDir::Dirs|QDir::NoDotAndDotDot|(ShowHidden?QDir::Hidden:QDir::Dirs));
        for (k=0;k<Directorys.count();k++)
            if (Directorys[k].isDir())
                CurItem->addChild(CreateItem(Directorys[k].fileName(),Directorys[k].absoluteFilePath(),GetIcon(Directorys[k].absoluteFilePath(),false)));

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
    Path.replace("/~",QApplication::translate("QCustomFolderTree","Personal folder"));
    #endif

    // Create a list with each part of the wanted Path
    while (Path.contains(QDir::separator())) {
        Folders.append(Path.mid(0,Path.indexOf(QDir::separator())));
        Path=Path.mid(Path.indexOf(QDir::separator())+QString(QDir::separator()).length());
    }
    Folders.append(Path);

    // find node in the tree and expand it if it was not previously expanded
    i=0;
    Current=topLevelItem(0);

    while ((Current!=NULL)&&(i<Folders.count())) {
        j=0;
        if (i==0) {
            // Search in topitemlist
            while ((j<topLevelItemCount())&&(topLevelItem(j)->text(0)!=Folders[i])) j++;
            if ((j<topLevelItemCount())&&(topLevelItem(j)->text(0)==Folders[i])) Current=topLevelItem(j);
                else Current=NULL;
        } else {
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
                Directorys=QDir(CurrentFolder).entryInfoList(QDir::Dirs|QDir::NoDotAndDotDot|(ShowHidden?QDir::Hidden:QDir::Dirs));
                for (k=0;k<Directorys.count();k++) Current->addChild(CreateItem(Directorys[k].fileName(),Directorys[k].absoluteFilePath(),GetIcon(Directorys[k].absoluteFilePath(),false)));
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

void QCustomFolderTree::RefreshItemByPath(QString Path) {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomFolderTree::SetSelectedItemByPath";
    #endif

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    QStringList         Folders;
    QString             PartPath=Path;
    QString             RealPath=Path;
    QTreeWidgetItem     *Current=NULL;
    QTreeWidgetItem     *SubItem=NULL;
    int                 i,j;

    #if defined(Q_OS_UNIX) && !defined(Q_OS_MACX)
    Path.replace("/~",QApplication::translate("QCustomFolderTree","Personal folder"));
    #endif

    for (i=0;i<HardDrive.count();i++) if (RealPath.startsWith(HardDrive[i].Label)) RealPath=HardDrive[i].Path+RealPath.mid(HardDrive[i].Label.length()+1);

    // Create a list with each part of the wanted Path
    while (PartPath.contains(QDir::separator())) {
        Folders.append(PartPath.mid(0,PartPath.indexOf(QDir::separator())));
        PartPath=PartPath.mid(PartPath.indexOf(QDir::separator())+QString(QDir::separator()).length());
    }
    Folders.append(PartPath);

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
        setUpdatesEnabled(false);

        // Set icon for this current item
        Current->setIcon(0,GetIcon(RealPath,Folders.count()==1));

//************************************ à revoir pour ne supprimer que ce qui a disparu et n'ajouter que ce qui est nouveau !
        // Delete all child
        while (Current->childCount()>0) {
            SubItem=Current->child(0);
            Current->removeChild(SubItem);
            delete SubItem;
        }

        // Add TAG
        SubItem=new QTreeWidgetItem();
        SubItem->setText(0,TAG);                            // add a tag to sub item
        Current->addChild(SubItem);

        // Now select and expand folder
        SetSelectItemByPath(Path);                          // this function call setUpdatesEnabled(true);
//************************************
    }

    QApplication::restoreOverrideCursor();
}
