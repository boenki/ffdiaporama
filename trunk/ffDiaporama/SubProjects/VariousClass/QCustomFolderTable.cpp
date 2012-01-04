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
#include <QHeaderView>
#include <QTableWidgetItem>

#include "QCustomFolderTable.h"
#include "QCustomFileInfoLabel.h"

//#define DEBUGMODE

#define FILETABLESTATE_FILETOCHEK   1
#define FileToCheckIcon             "MMFiler_img/player_time.png"

QCustomFolderTable::QCustomFolderTable(QWidget *parent):QTableWidget(parent) {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomFolderTable::QCustomFolderTable";
    #endif
    CurrentPath             ="";
    CurrentShowFolderNumber =0;
    CurrentShowFilesNumber  =0;
    CurrentShowFolderNumber =0;
    CurrentTotalFilesNumber =0;
    CurrentShowFolderSize   =0;
    CurrentTotalFolderSize  =0;
    CurrentShowDuration     =QTime(0,0,0,0);
    StopScanMediaList       =false;
    connect(this,SIGNAL(NeedResizeColumns()),this,SLOT(DoResizeColumns()));
    connect(this,SIGNAL(UpdateItemIcon(QTableWidgetItem *,cBaseMediaFile *)),this,SLOT(DoUpdateItemIcon(QTableWidgetItem *,cBaseMediaFile *)));
}

//====================================================================================================================

QCustomFolderTable::~QCustomFolderTable() {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomFolderTable::~QCustomFolderTable";
    #endif
    // Ensure scan thread is stoped
    if (ScanMediaList.isRunning()) {
        StopScanMediaList=true;
        ScanMediaList.waitForFinished();
        QApplication::processEvents();
        StopScanMediaList=false;
    }
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

    // Ensure scan thread is stoped
    if (ScanMediaList.isRunning()) {
        StopScanMediaList=true;
        ScanMediaList.waitForFinished();
        QApplication::processEvents();
        StopScanMediaList=false;
    }

    // Reset content
    while (rowCount()>0)    removeRow(0);
    while (columnCount()>0) removeColumn(0);

    // Define columns
    QString ColumnDef="";
    int     SizeColumn=1;

    switch (CurrentMode) {
        case DISPLAY_JUKEBOX :
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
            horizontalHeader()->setResizeMode(QHeaderView::Fixed);          //Fixed because ResizeToContents will be done after table filling
            verticalHeader()->hide();
            verticalHeader()->setStretchLastSection(false);
            verticalHeader()->setSortIndicatorShown(false);
            verticalHeader()->setResizeMode(QHeaderView::Fixed);            // Fixed because ResizeToContents will be done after table filling
            setShowGrid(false);                 // Ensure no grid display
            setWordWrap(false);                 // Ensure no word wrap
            setTextElideMode(Qt::ElideNone);    // Ensure no line ellipsis (...)
            switch (CurrentFilter) {
                //case OBJECTTYPE_FOLDER    :
                //case OBJECTTYPE_THUMBNAIL :
                case OBJECTTYPE_UNMANAGED :
                case OBJECTTYPE_MANAGED   :
                    SizeColumn= 2;  // Size column is the second (to force to right alignement)
                    ColumnDef = QApplication::translate("QCustomFolderTable","File","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","File Type","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","File Size","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","File Date","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Duration","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Image Size","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Image Format","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Image Geometry","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Video Codec","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Frame Rate","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Video Bitrate","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Audio Language","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Audio Codec","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Audio Channels","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Audio Bitrate","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Audio Frequency","Column header")+";"+
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
                                QApplication::translate("QCustomFolderTable","File Size","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","File Date","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Duration","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Title","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Author","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Album","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Year","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Composer","Column header");
                    break;
                case OBJECTTYPE_IMAGEFILE :
                    ColumnDef=  QApplication::translate("QCustomFolderTable","File","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","File Size","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","File Date","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Image Size","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Image Format","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Image Geometry","Column header");
                    break;
                case OBJECTTYPE_VIDEOFILE :
                    ColumnDef=  QApplication::translate("QCustomFolderTable","File","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","File Size","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","File Date","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Duration","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Image Size","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Image Format","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Image Geometry","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Video Codec","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Frame Rate","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Video Bitrate","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Audio Language","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Audio Codec","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Audio Channels","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Audio Bitrate","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Audio Frequency","Column header")+";"+
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
                                QApplication::translate("QCustomFolderTable","File Size","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","File Date","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Duration","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Audio Codec","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Audio Channels","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Audio Bitrate","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Audio Frequency","Column header")+";"+
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
        case DISPLAY_WEB :
            this->setSelectionBehavior(QAbstractItemView::SelectRows);
            setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
            horizontalHeader()->hide();
            horizontalHeader()->setStretchLastSection(true);
            horizontalHeader()->setSortIndicatorShown(false);
            horizontalHeader()->setCascadingSectionResizes(false);
            horizontalHeader()->setClickable(false);
            horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
            horizontalHeader()->setMovable(false);
            horizontalHeader()->setResizeMode(QHeaderView::Fixed);          //Fixed because ResizeToContents will be done after table filling
            verticalHeader()->hide();
            verticalHeader()->setStretchLastSection(false);
            verticalHeader()->setSortIndicatorShown(false);
            verticalHeader()->setResizeMode(QHeaderView::Fixed);            // Fixed because ResizeToContents will be done after table filling
            setShowGrid(true);                  // Ensure grid is display
            setWordWrap(false);                 // Ensure no word wrap
            setTextElideMode(Qt::ElideNone);    // Ensure no line ellipsis (...)
            setColumnCount(1);
            setHorizontalHeaderLabels(QString("WEB_VIEW").split(";"));
            break;
    }
}

//====================================================================================================================

QTableWidgetItem *QCustomFolderTable::CreateItem(QString ItemText,int Alignment,QBrush Background) {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomFolderTable::CreateItem";
    #endif
    QTableWidgetItem *Item=new QTableWidgetItem(ItemText);
    Item->setTextAlignment(Alignment);
    Item->setBackground(Background);
    return Item;
}

//====================================================================================================================

void QCustomFolderTable::s_itemDoubleClicked() {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomFolderTable::s_itemDoubleClicked";
    #endif
    emit DoubleClickEvent();
}

//====================================================================================================================
bool MediaListLessThan(cBaseMediaFile *Media1,cBaseMediaFile *Media2) {
    return (QString::compare(Media1->ShortName,Media2->ShortName,Qt::CaseInsensitive)<0);
}

void QCustomFolderTable::FillListFolder(QString Path,cBaseApplicationConfig *ApplicationConfig) {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomFolderTable::FillListFolder";
    #endif

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    // Ensure scan thread is stoped
    if (ScanMediaList.isRunning()) {
        StopScanMediaList=true;
        ScanMediaList.waitForFinished();
        QApplication::processEvents();
        StopScanMediaList=false;
    }

    int i=0,j=0;
    CurrentShowFolderNumber =0;
    CurrentShowFilesNumber  =0;
    CurrentShowFolderNumber =0;
    CurrentTotalFilesNumber =0;
    CurrentShowFolderSize   =0;
    CurrentTotalFolderSize  =0;
    CurrentShowDuration     =QTime(0,0,0,0);

    setUpdatesEnabled(false);

    // Reset content of the table
    while (rowCount()>0) removeRow(0);

    // if new Path, clear actual MediaList
    if (Path!=CurrentPath) {
        // Clear MediaList
        while (!MediaList.isEmpty()>0) delete MediaList.takeLast();
        // Keep current path
        CurrentPath=Path;
    }

    // Adjust given Path
    #if defined(Q_OS_UNIX) && !defined(Q_OS_MACX)
    if (Path.startsWith("~")) Path=QDir::homePath()+Path.mid(1);
    #endif
    if (!Path.endsWith(QDir::separator())) Path=Path+QDir::separator();

    // Scan files and add them to table
    QDir                Folder(Path);
    QFileInfoList       Files=Folder.entryInfoList(QDir::Dirs|QDir::AllDirs|QDir::Files|QDir::Hidden);
    cBaseMediaFile      *MediaObject=NULL;

    // Scan folder entries to remove all file not corresponding to showhiden filter
    i=0; while (i<Files.count()) if (ShowHidden || !((Files[i].isHidden())||(Files[i].fileName().startsWith(".")))) i++; else Files.removeAt(i);

    // Compute total number of files and total folder size
    foreach(QFileInfo File,Files) if (!File.isDir()) {
        CurrentTotalFilesNumber++;
        CurrentTotalFolderSize+=File.size();
    }

    // Scan media list to remove all files wich are no longer present (depending on filter)
    i=0; while (i<MediaList.count()) if (MediaList[i]->IsFilteredFile(CurrentFilter)) i++; else delete MediaList.takeAt(i);

    // Scan media list to remove all files wich are no longer present in folder entries
    i=0;
    while (i<MediaList.count()) {
        j=0;
        while ((j<Files.count())&&(((QFileInfo)Files[j]).absoluteFilePath()!=MediaList[i]->FileName)) j++;
        if ((j<Files.count())&&(((QFileInfo)Files[j]).absoluteFilePath()==MediaList[i]->FileName))
             i++;  else delete MediaList.takeAt(i);
    }

    // Scan folder entries to integrate new files to actual MediaList
    for (i=0;i<Files.count();i++) {
        j=0;
        while ((j<MediaList.count())&&(((QFileInfo)Files[i]).absoluteFilePath()!=MediaList[j]->FileName)) j++;
        if (!((j<MediaList.count())&&(((QFileInfo)Files[i]).absoluteFilePath()==MediaList[j]->FileName))) {
            QFileInfo *File=&Files[i];
            // It's a new file => then add it
            if (File->isDir()) {
                MediaObject=new cFolder(ApplicationConfig);
            } else if (ApplicationConfig->AllowImageExtension.contains(File->suffix().toLower())) {
                MediaObject=new cImageFile(ApplicationConfig);
                // Special case for folder Thumbnails
                if (File->fileName()=="folder.jpg") MediaObject->ObjectType=OBJECTTYPE_THUMBNAIL;
                else if (File->suffix().toLower()=="jpg") {  // Special case for video Thumbnails
                    // Search if a video with same name exist
                    for (int i=0;i<Files.count();i++)
                        if ((ApplicationConfig->AllowVideoExtension.contains(((QFileInfo)Files[i]).suffix().toLower()))&&
                            (((QFileInfo)Files[i]).completeBaseName()==File->completeBaseName())) MediaObject->ObjectType=OBJECTTYPE_THUMBNAIL;
                }
            } else if (ApplicationConfig->AllowVideoExtension.contains(File->suffix().toLower())) {
                MediaObject=new cVideoFile(cVideoFile::VIDEOFILE,ApplicationConfig);
            } else if (ApplicationConfig->AllowMusicExtension.contains(File->suffix().toLower())) {
                MediaObject=new cVideoFile(cVideoFile::MUSICFILE,ApplicationConfig);
            } else if (File->suffix().toLower()=="ffd") {
                MediaObject=new cffDProjectFile(ApplicationConfig);
            } else {
                MediaObject=new cUnmanagedFile(ApplicationConfig);
            }
            // Check if file is valid
            if ((MediaObject)&&(!MediaObject->GetInformationFromFile(File->absoluteFilePath(),NULL,NULL))) {
                delete MediaObject;
                MediaObject=NULL;
            }
            // Check if file correspond to current filer
            if ((MediaObject)&&(!MediaObject->IsFilteredFile(CurrentFilter))) {
                delete MediaObject;
                MediaObject=NULL;
            }
            // If MediaObject still exist then append it
            if (MediaObject) MediaList.append(MediaObject);
        }
    }

    // Sort MediaList
    qSort(MediaList.begin(),MediaList.end(),MediaListLessThan);

    //**********************************************************

    // if ShowFoldersFirst, the 1st step is to display folders
    if (ShowFoldersFirst) foreach(MediaObject,MediaList) if (MediaObject->ObjectType==OBJECTTYPE_FOLDER) AppendMediaToTable(MediaObject);

    // Display all others
    foreach(MediaObject,MediaList) if (((MediaObject->ObjectType==OBJECTTYPE_FOLDER)&&(!ShowFoldersFirst))||(MediaObject->ObjectType!=OBJECTTYPE_FOLDER)) AppendMediaToTable(MediaObject);

    // Adjust row & column size if needed
    if (CurrentMode==DISPLAY_DATA) {
        setVisible(false);                      // To ensure all items of all columns are used to compute size
        resizeColumnsToContents();              // Resize column widht
        resizeRowsToContents();                 // Resize row height
        setVisible(true);                       // To allow display
    }

    setUpdatesEnabled(true);                // To allow and force a general update
    QApplication::restoreOverrideCursor();

    // Send message to ResizeColumns
    emit NeedResizeColumns();

    // Start thread to scan files
    ScanMediaList.setFuture(QtConcurrent::run(this,&QCustomFolderTable::DoScanMediaList));
}

//====================================================================================================================

cBaseMediaFile *QCustomFolderTable::GetCurrentMediaFile() {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomFolderTable::GetCurrentMediaFile";
    #endif
    cBaseMediaFile  *Media=NULL;
    if (currentRow()>=0) {
        QString ShortName="";
        switch (CurrentMode) {
            case DISPLAY_DATA: if (item(currentRow(),0))        ShortName=item(currentRow(),0)->text();                                      break;
            case DISPLAY_WEB : if (cellWidget(currentRow(),0))  ShortName=((QCustomFileInfoLabel *)cellWidget(currentRow(),0))->ShortName;   break;
        }
        int     i=0;
        while ((i<MediaList.count())&&(MediaList[i]->ShortName!=ShortName)) i++;
        if ((i<MediaList.count())&&(MediaList[i]->ShortName==ShortName)) Media=MediaList[i];
    }
    return Media;
}

//====================================================================================================================

void QCustomFolderTable::DoResizeColumns() {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomFolderTable::DoResizeColumns";
    #endif
    // Adjust row & column size if needed
    if (CurrentMode==DISPLAY_DATA) {
        setUpdatesEnabled(false);
        setVisible(false);                      // To ensure all items of all columns are used to compute size
        resizeColumnsToContents();              // Resize column widht
        resizeRowsToContents();                 // Resize row height
        setVisible(true);                       // To allow display
        setUpdatesEnabled(true);
    }
}
//====================================================================================================================

void QCustomFolderTable::AppendMediaToTable(cBaseMediaFile *MediaObject) {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomFolderTable::AppendMediaToTable";
    #endif

    int Row=rowCount();
    insertRow(Row);
    verticalHeader()->setResizeMode(Row,QHeaderView::Fixed);
    setRowHeight(Row,CurrentMode==DISPLAY_DATA?18:50);

    QColor Background=((Row & 0x01)==0x01)?Qt::white:QColor(0xE0,0xE0,0xE0);

    if (MediaObject->ObjectType==OBJECTTYPE_FOLDER) {
        MediaObject->AddIcons(DriveList->GetFolderIcon(MediaObject->FileName));
        CurrentShowFolderNumber++;
    } else {
        CurrentShowFilesNumber++;
        CurrentShowFolderSize+=MediaObject->FileSize;
    }

    if (CurrentMode==DISPLAY_DATA) {
        setItem(Row,0,CreateItem(MediaObject->ShortName,Qt::AlignLeft|Qt::AlignVCenter,Background));
        item(Row,0)->setIcon(QIcon(QPixmap::fromImage(MediaObject->Icon16)));
        for (int Col=1;Col<columnCount();Col++) {
            QString ColName=horizontalHeaderItem(Col)->text();
            if      (ColName==QApplication::translate("QCustomFolderTable","File Type","Column header"))        setItem(Row,Col,CreateItem(MediaObject->GetTypeText(),Qt::AlignLeft|Qt::AlignVCenter,Background));
            else if (ColName==QApplication::translate("QCustomFolderTable","File Size","Column header"))        setItem(Row,Col,CreateItem(MediaObject->FileSizeText,Qt::AlignRight|Qt::AlignVCenter,Background));
            else if (ColName==QApplication::translate("QCustomFolderTable","File Date","Column header"))        setItem(Row,Col,CreateItem(MediaObject->ModifDateTimeText,Qt::AlignLeft|Qt::AlignVCenter,Background));
            else if (ColName==QApplication::translate("QCustomFolderTable","Duration","Column header"))         setItem(Row,Col,CreateItem(MediaObject->GetInformationValue("Duration"),Qt::AlignLeft|Qt::AlignVCenter,Background));
        }
    } else if (CurrentMode==DISPLAY_WEB) {
        QCustomFileInfoLabel *ItemWEB=new QCustomFileInfoLabel(this);
        ItemWEB->Icon32         =MediaObject->Icon32;
        ItemWEB->Icon48         =MediaObject->Icon48;
        ItemWEB->ShortName      =MediaObject->ShortName;
        ItemWEB->TextLeftUpper  =MediaObject->ShortName;
        //ItemWEB->setBackground(Background);
        setCellWidget(Row,0,ItemWEB);
        connect(ItemWEB,SIGNAL(DoubleClickEvent()),this,SLOT(s_itemDoubleClicked()));
    }
    UpdateMediaToTable(Row,MediaObject);
}

//====================================================================================================================

void QCustomFolderTable::UpdateMediaToTable(int Row,cBaseMediaFile *MediaObject) {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomFolderTable::UpdateMediaToTable";
    #endif

    if (!MediaObject->IsInformationValide) return;

    QColor Background=((Row & 0x01)==0x01)?Qt::white:QColor(0xE0,0xE0,0xE0);
    if (CurrentMode==DISPLAY_DATA) {
        for (int Col=1;Col<columnCount();Col++) {
            QString ColName=horizontalHeaderItem(Col)->text();
            if      (ColName==QApplication::translate("QCustomFolderTable","Image Size","Column header"))       setItem(Row,Col,CreateItem(MediaObject->GetImageSizeStr(cBaseMediaFile::SIZEONLY),Qt::AlignCenter|Qt::AlignVCenter,Background));
            else if (ColName==QApplication::translate("QCustomFolderTable","Image Format","Column header"))     setItem(Row,Col,CreateItem(MediaObject->GetImageSizeStr(cBaseMediaFile::FMTONLY),Qt::AlignCenter|Qt::AlignVCenter,Background));
            else if (ColName==QApplication::translate("QCustomFolderTable","Image Geometry","Column header"))   setItem(Row,Col,CreateItem(MediaObject->GetImageSizeStr(cBaseMediaFile::GEOONLY),Qt::AlignCenter|Qt::AlignVCenter,Background));
            else if (ColName==QApplication::translate("QCustomFolderTable","Video Codec","Column header"))      setItem(Row,Col,CreateItem(MediaObject->GetCumulInfoStr("Video","Codec"),Qt::AlignCenter|Qt::AlignVCenter,Background));
            else if (ColName==QApplication::translate("QCustomFolderTable","Frame Rate","Column header"))       setItem(Row,Col,CreateItem(MediaObject->GetCumulInfoStr("Video","Frame rate"),Qt::AlignCenter|Qt::AlignVCenter,Background));
            else if (ColName==QApplication::translate("QCustomFolderTable","Video Bitrate","Column header"))    setItem(Row,Col,CreateItem(MediaObject->GetCumulInfoStr("Video","Bitrate"),Qt::AlignCenter|Qt::AlignVCenter,Background));
            else if (ColName==QApplication::translate("QCustomFolderTable","Audio Language","Column header"))   setItem(Row,Col,CreateItem(MediaObject->GetCumulInfoStr("Audio","language"),Qt::AlignCenter|Qt::AlignVCenter,Background));
            else if (ColName==QApplication::translate("QCustomFolderTable","Audio Codec","Column header"))      setItem(Row,Col,CreateItem(MediaObject->GetCumulInfoStr("Audio","Codec"),Qt::AlignCenter|Qt::AlignVCenter,Background));
            else if (ColName==QApplication::translate("QCustomFolderTable","Audio Channels","Column header"))   setItem(Row,Col,CreateItem(MediaObject->GetCumulInfoStr("Audio","Channels"),Qt::AlignCenter|Qt::AlignVCenter,Background));
            else if (ColName==QApplication::translate("QCustomFolderTable","Audio Bitrate","Column header"))    setItem(Row,Col,CreateItem(MediaObject->GetCumulInfoStr("Audio","Bitrate"),Qt::AlignCenter|Qt::AlignVCenter,Background));
            else if (ColName==QApplication::translate("QCustomFolderTable","Audio Frequency","Column header"))  setItem(Row,Col,CreateItem(MediaObject->GetCumulInfoStr("Audio","Frequency"),Qt::AlignCenter|Qt::AlignVCenter,Background));
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
        // Update Icon (use signal to be sure we are in GUI thread because of pixmap use)
        emit UpdateItemIcon(item(Row,0),MediaObject);

    } else if (CurrentMode==DISPLAY_WEB) {
        QCustomFileInfoLabel *ItemWEB=(QCustomFileInfoLabel *)cellWidget(Row,0);
        ItemWEB->Icon32         =MediaObject->Icon32;
        ItemWEB->Icon48         =MediaObject->Icon48;
        ItemWEB->TextLeftBottom =MediaObject->WEBInfo;
        ItemWEB->TextRightUpper =MediaObject->GetTypeText()+" ("+MediaObject->FileSizeText+")";
        ItemWEB->update();
    }
}

//====================================================================================================================

void QCustomFolderTable::DoUpdateItemIcon(QTableWidgetItem *Item,cBaseMediaFile *Media) {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomFolderTable::DoUpdateItemIcon";
    #endif
    if ((!Item)||(!Media)||(StopScanMediaList)) return;
    Item->setIcon(QIcon(QPixmap::fromImage(Media->Icon16)));
}

//====================================================================================================================

void QCustomFolderTable::DoScanMediaList() {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomFolderTable::DoScanMediaList";
    #endif
    foreach (cBaseMediaFile *MediaObject,MediaList) {
        if (StopScanMediaList) return;
        if (!MediaObject->IsInformationValide) {
            MediaObject->GetFullInformationFromFile();
            // Search correspond row in table
            int Row=0;
            if (CurrentMode==DISPLAY_DATA) {
                while ((Row<rowCount())&&(MediaObject->ShortName!=item(Row,0)->text())) Row++;
                if (!((Row<rowCount())&&(MediaObject->ShortName==item(Row,0)->text()))) Row=-1;
            } else if (CurrentMode==DISPLAY_WEB) {
                while ((Row<rowCount())&&(MediaObject->ShortName!=((QCustomFileInfoLabel *)cellWidget(Row,0))->ShortName)) Row++;
                while (!((Row<rowCount())&&(MediaObject->ShortName==((QCustomFileInfoLabel *)cellWidget(Row,0))->ShortName))) Row=-1;
            } else Row=-1;

            // If Row found, then work ...
            if (Row!=-1) {
                QTime OldFolderDuration=CurrentShowDuration;
                UpdateMediaToTable(Row,MediaObject);
                if ((MediaObject->ObjectType==OBJECTTYPE_MUSICFILE)||(MediaObject->ObjectType==OBJECTTYPE_VIDEOFILE))   CurrentShowDuration=CurrentShowDuration.addMSecs(QTime(0,0,0,0).msecsTo(((cVideoFile *)MediaObject)->Duration));
                if (MediaObject->ObjectType==OBJECTTYPE_FFDFILE)                                                        CurrentShowDuration=CurrentShowDuration.addMSecs(((cffDProjectFile *)MediaObject)->Duration);
                if (OldFolderDuration!=CurrentShowDuration) emit RefreshFolderInfo();
            }
        }
    }
    // Send message to ResizeColumns
    emit NeedResizeColumns();
}
