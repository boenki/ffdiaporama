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
#include <QHeaderView>
#include <QTableWidgetItem>

#include "QCustomFolderTable.h"

//#define DEBUGMODE

#define FILETABLESTATE_FILETOCHEK   1
#define FileToCheckIcon             "MMFiler_img/player_time.png"

QCustomFolderTable::QCustomFolderTable(QWidget *parent):QTableWidget(parent) {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomFolderTable::QCustomFolderTable";
    #endif
}

//====================================================================================================================

QCustomFolderTable::~QCustomFolderTable() {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomFolderTable::~QCustomFolderTable";
    #endif
    // Clear MediaList
    while (!MediaList.isEmpty()>0) {
        cBaseMediaFile *Item=MediaList.takeLast();
        delete Item;
    }
}

//====================================================================================================================

void QCustomFolderTable::SetMode(cDriveList *TheDriveList,int Mode,int Filter) {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomFolderTable::SetMode";
    #endif
    DriveList    =TheDriveList;
    CurrentMode  =Mode;
    CurrentFilter=Filter;

    // Reset content
    clearContents();
    while (rowCount()>0)    removeRow(0);
    while (columnCount()>0) removeColumn(0);

    // Reset content
    clearContents();
    while (rowCount()>0) removeRow(0);
    // Clear MediaList
    while (!MediaList.isEmpty()>0) {
        cBaseMediaFile *Item=MediaList.takeLast();
        delete Item;
    }

    setRowCount(0);

    QString ColumnDef="";
    int     SizeColumn=1;

    switch (CurrentMode) {
        case DISPLAY_DATA :
            this->setSelectionBehavior(QAbstractItemView::SelectRows);
            setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
            horizontalHeader()->show();
            horizontalHeader()->setStretchLastSection(false);
            horizontalHeader()->setSortIndicatorShown(false);
            horizontalHeader()->setCascadingSectionResizes(false);
            horizontalHeader()->setClickable(false);
            horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
            horizontalHeader()->setMovable(false);
            horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
            verticalHeader()->hide();
            verticalHeader()->setStretchLastSection(false);
            verticalHeader()->setSortIndicatorShown(false);
            verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
            setShowGrid(false);
            switch (CurrentFilter) {
                //case OBJECTTYPE_FOLDER    :
                //case OBJECTTYPE_THUMBNAIL :
                case OBJECTTYPE_UNMANAGED :
                case OBJECTTYPE_MANAGED   :
                    ColumnDef=  QApplication::translate("QCustomFolderTable","File","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","File type","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","File size","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","File date","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Duration","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Image size","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Format","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Geometry","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Video Codec","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Frame rate","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Video bitrate","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Audio Codec","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Audio channels","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Audio bitrate","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Frequency","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Title","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Artist","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Album","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Year","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Track","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Genre","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Comment","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Composer","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Encoder","Column header");
                    break;
                case OBJECTTYPE_FFDFILE   :
                    ColumnDef=  QApplication::translate("QCustomFolderTable","File","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","File size","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","File date","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Duration","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Title","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Author","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Album","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Year","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Composer","Column header");
                    break;
                case OBJECTTYPE_IMAGEFILE :
                    ColumnDef=  QApplication::translate("QCustomFolderTable","File","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","File size","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","File date","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Image size","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Image format","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Image geometry","Column header");
                    break;
                case OBJECTTYPE_VIDEOFILE :
                    ColumnDef=  QApplication::translate("QCustomFolderTable","File","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","File size","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","File date","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Duration","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Image size","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Format","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Geometry","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Video Codec","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Frame rate","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Video bitrate","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Audio Codec","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Audio channels","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Audio bitrate","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Frequency","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Title","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Artist","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Album","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Year","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Track","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Genre","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Comment","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Composer","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Encoder","Column header");
                    break;
                case OBJECTTYPE_MUSICFILE :
                    ColumnDef=  QApplication::translate("QCustomFolderTable","File","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","File size","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","File date","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Duration","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Audio Codec","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Audio channels","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Audio bitrate","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Frequency","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Title","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Artist","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Album","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Year","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Track","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Genre","Column header");
                    break;
            }
            setColumnCount(ColumnDef.split(";").count());
            setHorizontalHeaderLabels(ColumnDef.split(";"));
            horizontalHeaderItem(SizeColumn)->setTextAlignment(Qt::AlignRight);  // Size to the right
            break;
    }
}

//====================================================================================================================

QTableWidgetItem *QCustomFolderTable::CreateItem(QString ItemText,int Alignment,QBrush Background) {
    QTableWidgetItem *Item=new QTableWidgetItem(ItemText);
    Item->setTextAlignment(Alignment);
    Item->setBackground(Background);
    return Item;
}

void QCustomFolderTable::AppendToMedialList(cBaseMediaFile *MediaObject) {
    QTableWidgetItem    *Item;
    int                 Row,Col;

    MediaList.append(MediaObject);
    if (MediaObject->ObjectType==OBJECTTYPE_FOLDER) MediaObject->Icon=DriveList->GetFolderIcon(MediaObject->FileName);

    MediaObject->GetFullInformationFromFile();

    Row=rowCount();
    insertRow(Row);
    verticalHeader()->setResizeMode(Row,QHeaderView::Fixed);
    setRowHeight(Row,18);

    QColor Background;
    if ((Row & 0x01)==0x01) Background=Qt::white; else Background=QColor(0xE0,0xE0,0xE0);

    for (Col=0;Col<columnCount();Col++) {
        QString ColName=horizontalHeaderItem(Col)->text();
        if (ColName==QApplication::translate("QCustomFolderTable","File","Column header")) {
            setItem(Row,Col,Item=CreateItem(MediaObject->ShortName,Qt::AlignLeft|Qt::AlignVCenter,Background));
            Item->setData(Qt::UserRole,QVariant(int(FILETABLESTATE_FILETOCHEK)));
            Item->setIcon(MediaObject->Icon);
        } else if (ColName==QApplication::translate("QCustomFolderTable","File type","Column header"))      setItem(Row,Col,CreateItem(MediaObject->GetTypeText(),Qt::AlignLeft|Qt::AlignVCenter,Background));
        else if (ColName==QApplication::translate("QCustomFolderTable","File size","Column header"))        setItem(Row,Col,CreateItem(MediaObject->FileSizeText,Qt::AlignRight|Qt::AlignVCenter,Background));
        else if (ColName==QApplication::translate("QCustomFolderTable","File date","Column header"))        setItem(Row,Col,CreateItem(MediaObject->ModifDateTimeText,Qt::AlignLeft|Qt::AlignVCenter,Background));
        else if (ColName==QApplication::translate("QCustomFolderTable","Duration","Column header"))         setItem(Row,Col,CreateItem(MediaObject->GetInformationValue("Duration"),Qt::AlignLeft|Qt::AlignVCenter,Background));
        else if (ColName==QApplication::translate("QCustomFolderTable","Image size","Column header"))       setItem(Row,Col,CreateItem(MediaObject->GetImageSizeStr(cBaseMediaFile::SIZEONLY),Qt::AlignCenter|Qt::AlignVCenter,Background));
        else if (ColName==QApplication::translate("QCustomFolderTable","Format","Column header"))           setItem(Row,Col,CreateItem(MediaObject->GetImageSizeStr(cBaseMediaFile::FMTONLY),Qt::AlignCenter|Qt::AlignVCenter,Background));
        else if (ColName==QApplication::translate("QCustomFolderTable","Geometry","Column header"))         setItem(Row,Col,CreateItem(MediaObject->GetImageSizeStr(cBaseMediaFile::GEOONLY),Qt::AlignCenter|Qt::AlignVCenter,Background));
        else if (ColName==QApplication::translate("QCustomFolderTable","Video Codec","Column header"))      setItem(Row,Col,CreateItem(MediaObject->GetInformationValue("Video codec"),Qt::AlignCenter|Qt::AlignVCenter,Background));
        else if (ColName==QApplication::translate("QCustomFolderTable","Frame rate","Column header"))       setItem(Row,Col,CreateItem(MediaObject->GetInformationValue("Frame rate"),Qt::AlignCenter|Qt::AlignVCenter,Background));
        else if (ColName==QApplication::translate("QCustomFolderTable","Video bitrate","Column header"))    setItem(Row,Col,CreateItem(MediaObject->GetInformationValue("Video bitrate"),Qt::AlignCenter|Qt::AlignVCenter,Background));
        else if (ColName==QApplication::translate("QCustomFolderTable","Audio Codec","Column header"))      setItem(Row,Col,CreateItem(MediaObject->GetInformationValue("Audio codec"),Qt::AlignCenter|Qt::AlignVCenter,Background));
        else if (ColName==QApplication::translate("QCustomFolderTable","Audio channels","Column header"))   setItem(Row,Col,CreateItem(MediaObject->GetInformationValue("Audio channels"),Qt::AlignCenter|Qt::AlignVCenter,Background));
        else if (ColName==QApplication::translate("QCustomFolderTable","Audio bitrate","Column header"))    setItem(Row,Col,CreateItem(MediaObject->GetInformationValue("Audio bitrate"),Qt::AlignCenter|Qt::AlignVCenter,Background));
        else if (ColName==QApplication::translate("QCustomFolderTable","Frequency","Column header"))        setItem(Row,Col,CreateItem(MediaObject->GetInformationValue("Audio frequency"),Qt::AlignCenter|Qt::AlignVCenter,Background));
        else if (ColName==QApplication::translate("QCustomFolderTable","Title","Column header"))            setItem(Row,Col,CreateItem(MediaObject->GetInformationValue("title"),Qt::AlignLeft|Qt::AlignVCenter,Background));
        else if (ColName==QApplication::translate("QCustomFolderTable","Artist","Column header"))           setItem(Row,Col,CreateItem(MediaObject->GetInformationValue("artist"),Qt::AlignLeft|Qt::AlignVCenter,Background));
        else if (ColName==QApplication::translate("QCustomFolderTable","Album","Column header"))            setItem(Row,Col,CreateItem(MediaObject->GetInformationValue("album"),Qt::AlignLeft|Qt::AlignVCenter,Background));
        else if (ColName==QApplication::translate("QCustomFolderTable","Year","Column header"))             setItem(Row,Col,CreateItem(MediaObject->GetInformationValue("date"),Qt::AlignCenter|Qt::AlignVCenter,Background));
        else if (ColName==QApplication::translate("QCustomFolderTable","Track","Column header"))            setItem(Row,Col,CreateItem(MediaObject->GetInformationValue("track"),Qt::AlignRight|Qt::AlignVCenter,Background));
        else if (ColName==QApplication::translate("QCustomFolderTable","Genre","Column header"))            setItem(Row,Col,CreateItem(MediaObject->GetInformationValue("genre"),Qt::AlignLeft|Qt::AlignVCenter,Background));
        else if (ColName==QApplication::translate("QCustomFolderTable","Comment","Column header"))          setItem(Row,Col,CreateItem(MediaObject->GetInformationValue("comment"),Qt::AlignLeft|Qt::AlignVCenter,Background));
        else if (ColName==QApplication::translate("QCustomFolderTable","Composer","Column header"))         setItem(Row,Col,CreateItem(MediaObject->GetInformationValue("composer"),Qt::AlignLeft|Qt::AlignVCenter,Background));
        else if (ColName==QApplication::translate("QCustomFolderTable","Encoder","Column header"))          setItem(Row,Col,CreateItem(MediaObject->GetInformationValue("encoder"),Qt::AlignLeft|Qt::AlignVCenter,Background));
    }
}

//====================================================================================================================

int QCustomFolderTable::NbrFilesDisplayed() {
    switch (CurrentMode) {
        case DISPLAY_DATA : return rowCount();
    }
    return 0;
}

//====================================================================================================================

int QCustomFolderTable::FillListFolder(QString Path,cBaseApplicationConfig *ApplicationConfig,int Filter) {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomFolderTable::FillListFolder";
    #endif

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    setUpdatesEnabled(false);

    CurrentFilter=Filter;
    int NbrFiles =0;

    // Reset content
    clearContents();
    while (rowCount()>0) removeRow(0);
    // Clear MediaList
    while (!MediaList.isEmpty()>0) {
        cBaseMediaFile *Item=MediaList.takeLast();
        delete Item;
    }

    #if defined(Q_OS_UNIX) && !defined(Q_OS_MACX)
    if (Path.startsWith("~")) Path=QDir::homePath()+Path.mid(1);
    #endif

    if (!Path.endsWith(QDir::separator())) Path=Path+QDir::separator();

    // Scan files and add them to table
    QDir                Folder(Path);
    QFileInfoList       Files=Folder.entryInfoList(QDir::Dirs|QDir::AllDirs|QDir::Files|QDir::Hidden);
    cBaseMediaFile      *MediaObject=NULL;

    // if ShowFoldersFirst, the 1st step to display folders
    if (ShowFoldersFirst) foreach(QFileInfo File,Files) if ((File.fileName()!=".")&&(File.fileName()!="..")) {
        if ((ShowHidden || !((File.isHidden())||(File.fileName().startsWith(".")))) && File.isDir()) {
            MediaObject=new cFolder(ApplicationConfig);
            if (MediaObject) {
                if (!MediaObject->IsFilteredFile(CurrentFilter)) {
                    delete MediaObject;
                    MediaObject=NULL;
                }
                if ((MediaObject)&&(MediaObject->GetInformationFromFile(File.absoluteFilePath(),NULL,NULL))) {
                    AppendToMedialList(MediaObject);
                    MediaObject=NULL;
                } else {
                    delete MediaObject;
                    MediaObject=NULL;
                }
            }
        }
    }

    // Display all others
    foreach(QFileInfo File,Files) if ((File.fileName()!=".")&&(File.fileName()!="..")) {
        if (ShowHidden || !((File.isHidden())||(File.fileName().startsWith(".")))) {
            NbrFiles++;
            if (File.isDir()) {
                if (!ShowFoldersFirst) MediaObject=new cFolder(ApplicationConfig);
            } else if (ApplicationConfig->AllowImageExtension.contains(File.suffix().toLower())) {
                MediaObject=new cImageFile(ApplicationConfig);
                // Special case for folder Thumbnails
                if (File.fileName()=="folder.jpg") MediaObject->ObjectType=OBJECTTYPE_THUMBNAIL;
                else if (File.suffix().toLower()=="jpg") {  // Special case for video Thumbnails
                    // Search if a video with same name exist
                    for (int i=0;i<Files.count();i++)
                        if ((ApplicationConfig->AllowVideoExtension.contains(((QFileInfo)Files[i]).suffix().toLower()))&&
                            (((QFileInfo)Files[i]).completeBaseName()==File.completeBaseName())) MediaObject->ObjectType=OBJECTTYPE_THUMBNAIL;
                }
            } else if (ApplicationConfig->AllowVideoExtension.contains(File.suffix().toLower())) {
                MediaObject=new cVideoFile(false,ApplicationConfig);
                // Search if an image (jpg) with same name exist
                if (QFileInfo(File.absolutePath()+(File.absolutePath().endsWith(QDir::separator())?"":QString(QDir::separator()))+File.completeBaseName()+".jpg").exists()) {
                    MediaObject->Icon.addPixmap(QPixmap(File.absolutePath()+(File.absolutePath().endsWith(QDir::separator())?"":QString(QDir::separator()))+File.completeBaseName()+".jpg"));
                }
            } else if (ApplicationConfig->AllowMusicExtension.contains(File.suffix().toLower())) {
                MediaObject=new cVideoFile(true,ApplicationConfig);
            } else if (File.suffix().toLower()=="ffd") {
                MediaObject=new cffDProjectFile(ApplicationConfig);
            } else {
                MediaObject=new cUnmanagedFile(ApplicationConfig);
            }
            if (MediaObject) {
                if (!MediaObject->IsFilteredFile(CurrentFilter)) {
                    delete MediaObject;
                    MediaObject=NULL;
                }
                if ((MediaObject)&&(MediaObject->GetInformationFromFile(File.absoluteFilePath(),NULL,NULL))) {
                    AppendToMedialList(MediaObject);
                    MediaObject=NULL;
                } else {
                    delete MediaObject;
                    MediaObject=NULL;
                }
            }
        }
    }
    //resizeRowsToContents();
    //resizeColumnsToContents();

    setUpdatesEnabled(true);
    QApplication::restoreOverrideCursor();

    return NbrFiles;
}
