/* ======================================================================
    This file is part of ffDiaporama
    ffDiaporama is a tools to make diaporama as video
    Copyright (C) 2011-2013 Dominique Levray <levray.dominique@bbox.fr>

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

// Basic inclusions (common to all files)
#include "_QCustomDialog.h"

#include <QFileInfoList>
#include <QFileInfo>
#include <QDir>
#include <QFile>

#include <QPainter>
#include <QModelIndex>
#include <QModelIndexList>
#include <qimageblitz/qimageblitz.h>

#include "QCustomFolderTable.h"
#include "QCustomFolderTree.h"

#define FILETABLESTATE_FILETOCHEK   1
#define FileToCheckIcon             ":/img/player_time.png"
#define LOWQUALITYITEMNBR           100                         // Number of item in list to use low quality big icon
#define LOWQUALITYITEMHEIGHT        50                          // Maximum size for *2
#define CELLBORDER                  8

int DISPLAYFILENAMEHEIGHT           =20;                        // Will be compute because it's not the same for all operating system

//********************************************************************************************************
// Utility functions use to sort table
//********************************************************************************************************

bool MediaListLessThan(cBaseMediaFile *Media1,cBaseMediaFile *Media2) {
    return (QString::compare(Media1->ShortName,Media2->ShortName,Qt::CaseInsensitive)<0);
}

bool MediaListLessThanWithFolder(cBaseMediaFile *Media1,cBaseMediaFile *Media2) {
    if ((Media1->ObjectType==OBJECTTYPE_FOLDER)&&(Media2->ObjectType==OBJECTTYPE_FOLDER))               // the 2 are folders
        return (QString::compare(Media1->ShortName,Media2->ShortName,Qt::CaseInsensitive)<0);
    else if ((Media1->ObjectType!=OBJECTTYPE_FOLDER)&&(Media2->ObjectType!=OBJECTTYPE_FOLDER))          // the 2 are not folders
        return (QString::compare(Media1->ShortName,Media2->ShortName,Qt::CaseInsensitive)<0);
    else if ((Media1->ObjectType==OBJECTTYPE_FOLDER)&&(Media2->ObjectType!=OBJECTTYPE_FOLDER))          // first is folders and second not
        return true;
    else return false;                                                                                      // second is folders and first not
}

//********************************************************************************************************
// QCustomFolderTable
//********************************************************************************************************

class QCustomStyledItemDelegate : public QStyledItemDelegate {
//Q_OBJECT
public:
    QCustomFolderTable  *ParentTable;

    explicit QCustomStyledItemDelegate(QObject *parent);

    virtual void    paint(QPainter *painter,const QStyleOptionViewItem &option,const QModelIndex &index) const;
};

//========================================================================================================================

QCustomStyledItemDelegate::QCustomStyledItemDelegate(QObject *parent):QStyledItemDelegate(parent) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomStyledItemDelegate::QCustomStyledItemDelegate");

    ParentTable=(QCustomFolderTable *)parent;
}

//========================================================================================================================

void QCustomStyledItemDelegate::paint(QPainter *Painter,const QStyleOptionViewItem &option,const QModelIndex &index) const {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomStyledItemDelegate::paint");

    if (((ParentTable->CurrentMode==DISPLAY_DATA)&&(index.row()>=ParentTable->MediaList.count()))||
        ((ParentTable->CurrentMode!=DISPLAY_DATA)&&(index.row()*ParentTable->columnCount()+index.column()>=ParentTable->MediaList.count()))) {

        // index is out of range
        Painter->fillRect(option.rect,Qt::white);

    } else {
        int ItemIndex=(ParentTable->CurrentMode==DISPLAY_DATA?index.row():index.row()*ParentTable->columnCount()+index.column());
        if (ItemIndex>=ParentTable->MediaList.count()) return;

        bool ThreadToPause=false;
        if (ParentTable->ScanMediaList.isRunning()) {
            ThreadToPause=true;
            ParentTable->ScanMediaList.pause();
        }

        if (ParentTable->CurrentMode==DISPLAY_DATA) {

            QString         TextToDisplay  =ParentTable->GetTextForColumn(index.column(),ParentTable->MediaList[ItemIndex]);
            QImage          *ImageToDisplay=ParentTable->GetImageForColumn(index.column(),ParentTable->MediaList[ItemIndex]);
            Qt::Alignment   Alignment      =((Qt::Alignment)(ParentTable->horizontalHeaderItem(index.column())?ParentTable->horizontalHeaderItem(index.column())->textAlignment():Qt::AlignHCenter))|Qt::AlignVCenter;
            int             DecalX         =(ImageToDisplay!=NULL?18:0);
            int             addY           =(option.rect.height()-16)/2;
            QColor          Background     =((index.row() & 0x01)==0x01)?Qt::white:QColor(0xE0,0xE0,0xE0);
            QFont           font;
            QTextOption     OptionText;
            QPen            Pen;

            // Setup default brush
            Painter->setBrush(Background);
            // Setup default pen
            Pen.setColor(Qt::black);
            Pen.setWidth(1);
            Pen.setStyle(Qt::SolidLine);
            Painter->setPen(Pen);

            // Setup font and text options
            font=QFont("Sans serif",9,QFont::Normal,QFont::StyleNormal);
            font.setBold(ParentTable->MediaList[ItemIndex]->ObjectType==OBJECTTYPE_FOLDER);
            font.setUnderline(false);
            Painter->setFont(font);
            OptionText=QTextOption(Alignment);                    // Setup alignement
            OptionText.setWrapMode(QTextOption::NoWrap);          // Setup word wrap text option

            // Drawing
            Painter->fillRect(option.rect,Background);
            if (ImageToDisplay) Painter->drawImage(QRectF(option.rect.x()+1,option.rect.y()+addY,16,16),*ImageToDisplay);
            Painter->drawText(QRectF(option.rect.x()+2+DecalX,option.rect.y()+1,option.rect.width()-4-DecalX,option.rect.height()-2),TextToDisplay,OptionText);

        } else {

            QImage      *Icon     =NULL;
            int         addX      =0;
            int         addY      =0;
            QFont       font;
            QTextOption OptionText;
            QPen        Pen;

            if (!ParentTable->MediaList[ItemIndex]->IsInformationValide) Icon=ParentTable->ApplicationConfig->DefaultDelayedIcon.GetIcon(cCustomIcon::ICON100);
                else Icon=&ParentTable->MediaList[ItemIndex]->Icon100;

            if (Icon->isNull()) Icon=ParentTable->MediaList[ItemIndex]->GetDefaultTypeIcon(cCustomIcon::ICON100);

            // Draw Icon
            if (Icon->height()<=LOWQUALITYITEMHEIGHT) {
                QImage NewIcon=Icon->scaledToHeight(Icon->height()*2,Qt::SmoothTransformation);
                NewIcon=Blitz::sharpen(NewIcon);
                addX=(option.rect.width()-NewIcon.width())/2;
                if (ParentTable->DisplayFileName) addY=(option.rect.height()-NewIcon.height()-DISPLAYFILENAMEHEIGHT)/3;
                    else addY=(option.rect.height()-NewIcon.height())/2;
                Painter->drawImage(QRectF(option.rect.x()+1+addX,option.rect.y()+1+addY,NewIcon.width(),NewIcon.height()),NewIcon);
            } else {
                addX=(option.rect.width()-Icon->width())/2;
                if (ParentTable->DisplayFileName) addY=(option.rect.height()-Icon->height()-DISPLAYFILENAMEHEIGHT)/3;
                    else addY=(option.rect.height()-Icon->height())/2;
                Painter->drawImage(QRectF(option.rect.x()+1+addX,option.rect.y()+1+addY,Icon->width(),Icon->height()),*Icon);
            }

            // Setup default brush
            Painter->setBrush(Qt::NoBrush);
            // Setup default pen
            Pen.setColor(Qt::black);
            Pen.setWidth(1);
            Pen.setStyle(Qt::SolidLine);
            Painter->setPen(Pen);

            // Draw file name if needed
            if (ParentTable->DisplayFileName) {
                // Setup default font
                font=QFont("Sans serif",8,QFont::Normal,QFont::StyleNormal);
                font.setUnderline(false);
                Painter->setFont(font);
                #ifdef Q_OS_WIN
                font.setPointSizeF(double(120)/double(Painter->fontMetrics().boundingRect("0").height()));                  // Scale font
                #else
                font.setPointSizeF(double(100)/double(Painter->fontMetrics().boundingRect("0").height()));                  // Scale font
                #endif
                Painter->setFont(font);

                OptionText=QTextOption(Qt::AlignHCenter|Qt::AlignTop);                      // Setup alignement
                OptionText.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);          // Setup word wrap text option
                Painter->drawText(QRectF(option.rect.x()+1,option.rect.y()+option.rect.height()-1-DISPLAYFILENAMEHEIGHT,option.rect.width()-2,DISPLAYFILENAMEHEIGHT),
                                  ParentTable->MediaList[ItemIndex]->ShortName,OptionText);
            }

        }

        // Selection mode (Note: MouseOver is removed because it works correctly only on KDE !)
        if (option.state & QStyle::State_Selected) {
            Painter->setPen(QPen(Qt::NoPen));
            Painter->setBrush(QBrush(Qt::blue));
            Painter->setOpacity(0.25);
            Painter->drawRect(option.rect.x(),option.rect.y(),option.rect.width(),option.rect.height());
            Painter->setOpacity(1);
        }

        if (ThreadToPause) ParentTable->ScanMediaList.resume();
    }
}

//********************************************************************************************************
// QCustomFolderTable
//********************************************************************************************************

QCustomFolderTable::QCustomFolderTable(QWidget *parent):QTableWidget(parent) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomFolderTable::QCustomFolderTable");

    DefaultModel            =model();               // Save default QAbstractItemModel
    DefaultDelegate         =itemDelegate();        // Save default QAbstractItemDelegate
    IconDelegate            =(QAbstractItemDelegate *)new QCustomStyledItemDelegate(this);
    ApplicationConfig       =NULL;
    StopAllEvent            =false;
    CurrentPath             ="";
    AllowedFilter           =FILTERALLOW_OBJECTTYPE_ALL;
    CurrentMode             =0;
    CurrentFilter           =0;
    CurrentShowFolderNumber =0;
    CurrentShowFilesNumber  =0;
    CurrentShowFolderNumber =0;
    CurrentTotalFilesNumber =0;
    CurrentShowFolderSize   =0;
    CurrentTotalFolderSize  =0;
    CurrentDisplayItem      =0;
    CurrentShowDuration     =0;
    ShowHiddenFilesAndDir   =false;
    DisplayFileName         =true;
    StopScanMediaList       =false;
    ScanMediaListProgress   =false;
    InScanMediaFunction     =false;
    connect(this,SIGNAL(NeedResizeColumns()),this,SLOT(DoResizeColumns()));
}

//====================================================================================================================

QCustomFolderTable::~QCustomFolderTable() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomFolderTable::~QCustomFolderTable");

    // Ensure scan thread is stoped
    StopAllEvent=true;
    EnsureThreadIsStopped();
    // Clear MediaList
    while (!MediaList.isEmpty()>0) {
        cBaseMediaFile *Item=MediaList.takeLast();
        delete Item;
    }
}

//====================================================================================================================

QMimeData *QCustomFolderTable::mimeData(const QList <QTableWidgetItem *>) const {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomFolderTable::mimeData");
    QMimeData   *mimeData=new QMimeData;
    QList<QUrl> UrlList;
    QList<cBaseMediaFile*> SelMediaList=GetCurrentSelectedMediaFile();
    for (int i=0;i<SelMediaList.count();i++) UrlList.append(QUrl().fromLocalFile(AdjustDirForOS(SelMediaList[i]->FileName)));
    mimeData->setUrls(UrlList);
    return mimeData;
}


//====================================================================================================================

void QCustomFolderTable::keyReleaseEvent(QKeyEvent *event) {
    if (selectionModel()->selectedIndexes().count()>0) {
        if (event->matches(QKeySequence::Delete))   emit RemoveFiles();
            else if (event->key()==Qt::Key_Insert)  emit InsertFiles();
            else if (event->key()==Qt::Key_Enter)   emit OpenFiles();
            else if (event->key()==Qt::Key_Return)  emit OpenFiles();
            else if (event->key()==Qt::Key_F5)      emit Refresh();
            else if (event->key()==Qt::Key_F2)      emit RenameFiles();
            else QTableWidget::keyReleaseEvent(event);
    } else QTableWidget::keyReleaseEvent(event);
}

//====================================================================================================================

QImage *QCustomFolderTable::GetImageForColumn(int Col,cBaseMediaFile *MediaObject) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomFolderTable::GetImageForColumn");

    if (StopAllEvent) return NULL;

    QImage *ImageToDisplay=NULL;
    QString ColName       =(horizontalHeaderItem(Col)!=NULL)?horizontalHeaderItem(Col)->text():"";

    if (ColName==QApplication::translate("QCustomFolderTable","File","Column header")) {
        if (MediaObject->IsInformationValide) ImageToDisplay=&MediaObject->Icon16;
            else ImageToDisplay=ApplicationConfig->DefaultDelayedIcon.GetIcon(cCustomIcon::ICON16);
    } else if (ColName==QApplication::translate("QCustomFolderTable","File Type","Column header")) {
        ImageToDisplay=MediaObject->GetDefaultTypeIcon(cCustomIcon::ICON16);
    }

    return ImageToDisplay;
}

//====================================================================================================================

QString QCustomFolderTable::GetTextForColumn(int Col,cBaseMediaFile *MediaObject) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomFolderTable::GetTextForColumn");

    if ((StopAllEvent)||(Col>=columnCount())) return "";

    QString TextToDisplay="";
    QString ColName      =(horizontalHeaderItem(Col)!=NULL)?horizontalHeaderItem(Col)->text():"";

    if      (ColName==QApplication::translate("QCustomFolderTable","File","Column header"))             TextToDisplay=MediaObject->ShortName;
    else if (ColName==QApplication::translate("QCustomFolderTable","File Type","Column header"))        TextToDisplay=MediaObject->GetFileTypeStr();
    else if (ColName==QApplication::translate("QCustomFolderTable","File Size","Column header"))        TextToDisplay=MediaObject->FileSizeText;
    else if (ColName==QApplication::translate("QCustomFolderTable","File Date","Column header"))        TextToDisplay=MediaObject->GetFileDateTimeStr();
    else if (ColName==QApplication::translate("QCustomFolderTable","Duration","Column header"))         TextToDisplay=MediaObject->GetInformationValue("Duration");
    else if (ColName==QApplication::translate("QCustomFolderTable","Image Size","Column header"))       TextToDisplay=MediaObject->GetImageSizeStr(cBaseMediaFile::SIZEONLY);
    else if (ColName==QApplication::translate("QCustomFolderTable","Image Format","Column header"))     TextToDisplay=MediaObject->GetImageSizeStr(cBaseMediaFile::FMTONLY);
    else if (ColName==QApplication::translate("QCustomFolderTable","Image Geometry","Column header"))   TextToDisplay=MediaObject->GetImageSizeStr(cBaseMediaFile::GEOONLY);
    else if (ColName==QApplication::translate("QCustomFolderTable","Video Codec","Column header"))      TextToDisplay=MediaObject->GetCumulInfoStr("Video","Codec");
    else if (ColName==QApplication::translate("QCustomFolderTable","Frame Rate","Column header"))       TextToDisplay=MediaObject->GetCumulInfoStr("Video","Frame rate");
    else if (ColName==QApplication::translate("QCustomFolderTable","Video Bitrate","Column header"))    TextToDisplay=MediaObject->GetCumulInfoStr("Video","Bitrate");
    else if (ColName==QApplication::translate("QCustomFolderTable","Audio Language","Column header"))   TextToDisplay=MediaObject->GetCumulInfoStr("Audio","language");
    else if (ColName==QApplication::translate("QCustomFolderTable","Audio Codec","Column header"))      TextToDisplay=MediaObject->GetCumulInfoStr("Audio","Codec");
    else if (ColName==QApplication::translate("QCustomFolderTable","Audio Channels","Column header"))   TextToDisplay=MediaObject->GetCumulInfoStr("Audio","Channels");
    else if (ColName==QApplication::translate("QCustomFolderTable","Audio Bitrate","Column header"))    TextToDisplay=MediaObject->GetCumulInfoStr("Audio","Bitrate");
    else if (ColName==QApplication::translate("QCustomFolderTable","Audio Frequency","Column header"))  TextToDisplay=MediaObject->GetCumulInfoStr("Audio","Frequency");
    else if (ColName==QApplication::translate("QCustomFolderTable","Title","Column header"))            TextToDisplay=MediaObject->GetInformationValue("title");
    else if (ColName==QApplication::translate("QCustomFolderTable","Artist","Column header"))           TextToDisplay=MediaObject->GetInformationValue("artist");
    else if (ColName==QApplication::translate("QCustomFolderTable","Album","Column header"))            TextToDisplay=MediaObject->GetInformationValue("album");
    else if (ColName==QApplication::translate("QCustomFolderTable","Year","Column header"))             TextToDisplay=MediaObject->GetInformationValue("date");
    else if (ColName==QApplication::translate("QCustomFolderTable","Track","Column header"))            TextToDisplay=MediaObject->GetInformationValue("track");
    else if (ColName==QApplication::translate("QCustomFolderTable","Genre","Column header"))            TextToDisplay=MediaObject->GetInformationValue("genre");
    else if (ColName==QApplication::translate("QCustomFolderTable","Comment","Column header"))          TextToDisplay=MediaObject->GetInformationValue("comment");
    else if (ColName==QApplication::translate("QCustomFolderTable","Composer","Column header"))         TextToDisplay=MediaObject->GetInformationValue("composer");
    else if (ColName==QApplication::translate("QCustomFolderTable","Encoder","Column header"))          TextToDisplay=MediaObject->GetInformationValue("encoder");
    else if (ColName==QApplication::translate("QCustomFolderTable","Chapters","Column header")) {
        int NbrChapter=0;
        if (MediaObject->ObjectType==OBJECTTYPE_VIDEOFILE) NbrChapter=((cVideoFile *)MediaObject)->NbrChapters;
        if (MediaObject->ObjectType==OBJECTTYPE_FFDFILE)   NbrChapter=((cffDProjectFile *)MediaObject)->NbrChapters;
        TextToDisplay=(NbrChapter>0?QString("%1").arg(NbrChapter):"");
    }
    return TextToDisplay;
}

//====================================================================================================================

int QCustomFolderTable::GetAlignmentForColumn(int Col) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomFolderTable::GetTextForColumn");

    if ((StopAllEvent)||(Col>=columnCount())) return Qt::AlignLeft;

    int     Alignment=Qt::AlignLeft;
    QString ColName  =(horizontalHeaderItem(Col)!=NULL)?horizontalHeaderItem(Col)->text():"";

    if      (ColName==QApplication::translate("QCustomFolderTable","File","Column header"))             Alignment=Qt::AlignLeft;
    else if (ColName==QApplication::translate("QCustomFolderTable","File Type","Column header"))        Alignment=Qt::AlignLeft;
    else if (ColName==QApplication::translate("QCustomFolderTable","File Size","Column header"))        Alignment=Qt::AlignRight;
    else if (ColName==QApplication::translate("QCustomFolderTable","File Date","Column header"))        Alignment=Qt::AlignLeft;
    else if (ColName==QApplication::translate("QCustomFolderTable","Duration","Column header"))         Alignment=Qt::AlignRight;
    else if (ColName==QApplication::translate("QCustomFolderTable","Image Size","Column header"))       Alignment=Qt::AlignHCenter;
    else if (ColName==QApplication::translate("QCustomFolderTable","Image Format","Column header"))     Alignment=Qt::AlignHCenter;
    else if (ColName==QApplication::translate("QCustomFolderTable","Chapters","Column header"))         Alignment=Qt::AlignHCenter;
    else if (ColName==QApplication::translate("QCustomFolderTable","Image Geometry","Column header"))   Alignment=Qt::AlignHCenter;
    else if (ColName==QApplication::translate("QCustomFolderTable","Video Codec","Column header"))      Alignment=Qt::AlignHCenter;
    else if (ColName==QApplication::translate("QCustomFolderTable","Frame Rate","Column header"))       Alignment=Qt::AlignHCenter;
    else if (ColName==QApplication::translate("QCustomFolderTable","Video Bitrate","Column header"))    Alignment=Qt::AlignHCenter;
    else if (ColName==QApplication::translate("QCustomFolderTable","Audio Language","Column header"))   Alignment=Qt::AlignHCenter;
    else if (ColName==QApplication::translate("QCustomFolderTable","Audio Codec","Column header"))      Alignment=Qt::AlignHCenter;
    else if (ColName==QApplication::translate("QCustomFolderTable","Audio Channels","Column header"))   Alignment=Qt::AlignHCenter;
    else if (ColName==QApplication::translate("QCustomFolderTable","Audio Bitrate","Column header"))    Alignment=Qt::AlignHCenter;
    else if (ColName==QApplication::translate("QCustomFolderTable","Audio Frequency","Column header"))  Alignment=Qt::AlignHCenter;
    else if (ColName==QApplication::translate("QCustomFolderTable","Title","Column header"))            Alignment=Qt::AlignLeft;
    else if (ColName==QApplication::translate("QCustomFolderTable","Artist","Column header"))           Alignment=Qt::AlignLeft;
    else if (ColName==QApplication::translate("QCustomFolderTable","Album","Column header"))            Alignment=Qt::AlignLeft;
    else if (ColName==QApplication::translate("QCustomFolderTable","Year","Column header"))             Alignment=Qt::AlignHCenter;
    else if (ColName==QApplication::translate("QCustomFolderTable","Track","Column header"))            Alignment=Qt::AlignHCenter;
    else if (ColName==QApplication::translate("QCustomFolderTable","Genre","Column header"))            Alignment=Qt::AlignLeft;
    else if (ColName==QApplication::translate("QCustomFolderTable","Comment","Column header"))          Alignment=Qt::AlignLeft;
    else if (ColName==QApplication::translate("QCustomFolderTable","Composer","Column header"))         Alignment=Qt::AlignLeft;
    else if (ColName==QApplication::translate("QCustomFolderTable","Encoder","Column header"))          Alignment=Qt::AlignLeft;
    return Alignment;
}

//====================================================================================================================

void QCustomFolderTable::EnsureThreadIsStopped() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomFolderTable::EnsureThreadIsStopped");

    // Ensure scan thread is stoped
    if (ScanMediaList.isRunning()) {
        StopScanMediaList=true;
        ScanMediaList.waitForFinished();
        // flush event queue"
        //while (QApplication::hasPendingEvents()) QApplication::processEvents();
        StopScanMediaList=false;
    }
}

//====================================================================================================================

int QCustomFolderTable::GetWidthForIcon() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomFolderTable::GetWidthForIcon");

    int SizeColumn;
    if (CurrentMode==DISPLAY_ICON100) {
        SizeColumn=100+CELLBORDER;
    } else {
        if (CurrentFilter==OBJECTTYPE_VIDEOFILE)          SizeColumn=ApplicationConfig->Video_ThumbWidth+CELLBORDER;
            else if (CurrentFilter==OBJECTTYPE_IMAGEFILE) SizeColumn=ApplicationConfig->Image_ThumbWidth+CELLBORDER;
            else {
            SizeColumn=ApplicationConfig->Image_ThumbWidth;
            if (SizeColumn<ApplicationConfig->Music_ThumbWidth) SizeColumn=ApplicationConfig->Music_ThumbWidth;
            if (SizeColumn<ApplicationConfig->Video_ThumbWidth) SizeColumn=ApplicationConfig->Video_ThumbWidth;
            SizeColumn+=CELLBORDER;
        }
    }
    return SizeColumn;
}

//====================================================================================================================

int QCustomFolderTable::GetHeightForIcon() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomFolderTable::GetHeightForIcon");

    int SizeColumn;
    if (CurrentMode==DISPLAY_ICON100)   {

        SizeColumn=100+CELLBORDER+(DisplayFileName?DISPLAYFILENAMEHEIGHT:0);

    } else {

        SizeColumn=QFontMetrics(QApplication::font()).boundingRect("0").height();
        if (SizeColumn<16) SizeColumn=16; // Not less than Icon

    }
    return SizeColumn;
}

//====================================================================================================================

void QCustomFolderTable::resizeEvent(QResizeEvent *ev) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomFolderTable::resizeEvent");

    // Update view
    if (CurrentMode==DISPLAY_ICON100) {
        int ColumnWidth   =GetWidthForIcon();
        int RowHeight     =GetHeightForIcon();
        int NewColumnCount=(viewport()->width()/ColumnWidth);	if (NewColumnCount<=0) NewColumnCount=1;
        int NewRowCount   =CurrentDisplayItem/NewColumnCount;   if (NewRowCount*NewColumnCount<CurrentDisplayItem) NewRowCount++;

        if ((NewColumnCount!=columnCount())||(NewRowCount!=rowCount())) {
            setColumnCount(NewColumnCount); for (int i=0;i<NewColumnCount;i++)  setColumnWidth(i,ColumnWidth);
            setRowCount(NewRowCount);       for (int i=0;i<NewRowCount;i++)     setRowHeight(i,RowHeight);
        }
    }
    QTableWidget::resizeEvent(ev);
}

//====================================================================================================================

void QCustomFolderTable::SetMode(int Mode,int Filter) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomFolderTable::SetMode");

    // Ensure scan thread is stoped
    EnsureThreadIsStopped();
    if (Mode>DISPLAY_ICON100) Mode=DISPLAY_ICON100;
    CurrentMode  =Mode;
    CurrentFilter=Filter;

    if (CurrentMode==DISPLAY_ICON100) {
        // Compute DISPLAYFILENAMEHEIGHT
        QImage      Img(100,100,QImage::Format_ARGB32);
        QPainter    Painter;
        Painter.begin(&Img);
        QFont font("Sans serif",8,QFont::Normal,QFont::StyleNormal);
        Painter.setFont(font);
        #ifdef Q_OS_WIN
        font.setPointSizeF(double(120)/double(Painter.fontMetrics().boundingRect("0").height()));                  // Scale font
        #else
        font.setPointSizeF(double(100)/double(Painter.fontMetrics().boundingRect("0").height()));                  // Scale font
        #endif
        Painter.setFont(font);
        DISPLAYFILENAMEHEIGHT=Painter.fontMetrics().boundingRect("0").height()*2;                                   // 2 lines for bigest mode
        Painter.end();
    }

    // Reset content
    setRowCount(0);
    setColumnCount(0);

    // Define columns
    QString ColumnDef="";

    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setWordWrap(false);                 // Ensure no word wrap
    setTextElideMode(Qt::ElideNone);    // Ensure no line ellipsis (...)

    horizontalHeader()->setSortIndicatorShown(false);
    horizontalHeader()->setCascadingSectionResizes(false);
    horizontalHeader()->setClickable(false);
    horizontalHeader()->setMovable(false);
    horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    horizontalHeader()->setResizeMode(QHeaderView::Fixed);          //Fixed because ResizeToContents will be done after table filling

    verticalHeader()->setStretchLastSection(false);
    verticalHeader()->setSortIndicatorShown(false);
    verticalHeader()->hide();
    verticalHeader()->setResizeMode(QHeaderView::Fixed);            // Fixed because ResizeToContents will be done after table filling

    setItemDelegate(IconDelegate);

    switch (CurrentMode) {
        case DISPLAY_ICON100 :
            setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            setSelectionBehavior(QAbstractItemView::SelectItems);
            horizontalHeader()->hide();
            horizontalHeader()->setStretchLastSection(false);
            setShowGrid(false);
            break;

        case DISPLAY_DATA :
            setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
            setSelectionBehavior(QAbstractItemView::SelectRows);
            horizontalHeader()->show();
            horizontalHeader()->setStretchLastSection(false);
            setShowGrid(false);
            switch (CurrentFilter) {
                //case OBJECTTYPE_FOLDER    :
                //case OBJECTTYPE_THUMBNAIL :
                case OBJECTTYPE_UNMANAGED :
                case OBJECTTYPE_MANAGED   :
                    ColumnDef = QApplication::translate("QCustomFolderTable","File","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","File Type","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","File Size","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","File Date","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Duration","Column header")+";"+
                                QApplication::translate("QCustomFolderTable","Chapters","Column header")+";"+
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
                                QApplication::translate("QCustomFolderTable","Chapters","Column header")+";"+
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
            for (int Col=0;Col<columnCount();Col++) horizontalHeaderItem(Col)->setTextAlignment(GetAlignmentForColumn(Col));  // Size to the right
            break;
    }
}

//====================================================================================================================

QTableWidgetItem *QCustomFolderTable::CreateItem(QString ItemText,int Alignment,QBrush Background) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomFolderTable::CreateItem");

    QTableWidgetItem *Item=new QTableWidgetItem(ItemText);
    Item->setTextAlignment(Alignment);
    Item->setBackground(Background);
    return Item;
}

//====================================================================================================================

void QCustomFolderTable::mouseDoubleClickEvent(QMouseEvent *ev) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomFolderTable::mouseDoubleClickEvent");

    emit DoubleClickEvent(ev);
}

//====================================================================================================================

void QCustomFolderTable::mouseReleaseEvent(QMouseEvent *ev) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomFolderTable::mouseDoubleClickEvent");

    if (ev->button()==Qt::RightButton) emit RightClickEvent(ev);
        else QTableWidget::mouseReleaseEvent(ev);
}

//====================================================================================================================

void QCustomFolderTable::RefreshListFolder() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomFolderTable::RefreshListFolder");

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    // Ensure scan thread is stoped
    EnsureThreadIsStopped();

    // Clear selection
    selectionModel()->clear();

    // Scan files and add them to table
    QDir                Folder(CurrentPath);
    QFileInfoList       Files=Folder.entryInfoList(QDir::Dirs|QDir::AllDirs|QDir::Files|QDir::Hidden);
    cBaseMediaFile      *MediaObject=NULL;
    int                 i,j;

    // Scan folder entries to remove all file not corresponding to showhiden filter
    i=0;
    while (i<Files.count()) if ((Files[i].fileName()==".")||(Files[i].fileName()=="..")||
        ((!ShowHiddenFilesAndDir)&&((Files[i].isHidden())||(Files[i].fileName().startsWith(".")))))
        Files.removeAt(i); else i++;

    // Compute total number of files and total folder size
    foreach(QFileInfo File,Files) if (!File.isDir()) {
        CurrentTotalFilesNumber++;
        CurrentTotalFolderSize+=File.size();
    }

    //**********************************************************

    // Scan media list to remove all files wich are no longer present (depending on filter)
    i=0; while (i<MediaList.count()) if (MediaList[i]->IsFilteredFile(CurrentFilter,AllowedFilter)) i++; else delete MediaList.takeAt(i);

    // Scan media list to remove all files wich are no longer present in folder entries or files for which date/time has changed
    i=0;
    while (i<MediaList.count()) {
        j=0;
        while ((j<Files.count())&&(((QFileInfo)Files[j]).absoluteFilePath()!=MediaList[i]->FileName)&&(((QFileInfo)Files[j]).lastModified()!=MediaList[i]->ModifDateTime)) j++;
        if ((j<Files.count())&&(((QFileInfo)Files[j]).absoluteFilePath()==MediaList[i]->FileName)&&(((QFileInfo)Files[j]).lastModified()==MediaList[i]->ModifDateTime))
             i++;  else delete MediaList.takeAt(i);
    }

    // Scan folder entries to integrate new files to actual MediaList
    for (i=0;i<Files.count();i++) {
        j=0;
        while ((j<MediaList.count())&&(((QFileInfo)Files[i]).absoluteFilePath()!=MediaList[j]->FileName)) j++;
        if (!((j<MediaList.count())&&(((QFileInfo)Files[i]).absoluteFilePath()==MediaList[j]->FileName))) {
            QFileInfo *File=&Files[i];
            // It's a new file => then add it
            if (ApplicationConfig->AllowImageExtension.contains(File->suffix().toLower())) {
                MediaObject=new cImageFile(ApplicationConfig);

                // Special case for folder Thumbnails
                if (File->fileName()=="folder.jpg") MediaObject->ObjectType=OBJECTTYPE_THUMBNAIL;
                else if (File->suffix().toLower()=="jpg") {  // Special case for video Thumbnails
                    // Search if a video with same name exist
                    for (int i=0;i<Files.count();i++)
                        if ((ApplicationConfig->AllowVideoExtension.contains(((QFileInfo)Files[i]).suffix().toLower()))&&
                            (((QFileInfo)Files[i]).completeBaseName()==File->completeBaseName())) MediaObject->ObjectType=OBJECTTYPE_THUMBNAIL;
                }
            }   else if (ApplicationConfig->AllowVideoExtension.contains(File->suffix().toLower())) MediaObject=new cVideoFile(OBJECTTYPE_VIDEOFILE,ApplicationConfig);
                else if (ApplicationConfig->AllowMusicExtension.contains(File->suffix().toLower())) MediaObject=new cVideoFile(OBJECTTYPE_MUSICFILE,ApplicationConfig);
                else if (File->suffix().toLower()=="ffd")                                           MediaObject=new cffDProjectFile(ApplicationConfig);
                else if (File->isDir())                                                             MediaObject=new cFolder(ApplicationConfig);
                else                                                                                MediaObject=new cUnmanagedFile(ApplicationConfig);

            // Check if file is valid
            if ((MediaObject)&&(!MediaObject->GetInformationFromFile(File->absoluteFilePath(),NULL,NULL))) {
                delete MediaObject;
                MediaObject=NULL;
            }
            // Check if file correspond to current filer
            if ((MediaObject)&&(!MediaObject->IsFilteredFile(CurrentFilter,AllowedFilter))) {
                delete MediaObject;
                MediaObject=NULL;
            }
            // If MediaObject still exist then append it
            if (MediaObject) MediaList.append(MediaObject);
        }
    }

    CurrentDisplayItem=MediaList.count();

    // Sort MediaList
    if (ApplicationConfig->ShowFoldersFirst) qSort(MediaList.begin(),MediaList.end(),MediaListLessThanWithFolder);
        else qSort(MediaList.begin(),MediaList.end(),MediaListLessThan);

    // Set Rows and Columns
    if (CurrentMode==DISPLAY_ICON100) {
        if (MediaList.count()>0) setRowCount((MediaList.count()/columnCount())+1); else setRowCount(0);
        int SizeColumn=GetWidthForIcon();
        int RHeight=GetHeightForIcon();
        for (int i=0;i<columnCount();i++) setColumnWidth(i,SizeColumn);
        for (int i=0;i<(CurrentDisplayItem/columnCount())+1;i++) setRowHeight(RHeight,SizeColumn);
    } else {
        setRowCount(MediaList.count());
        for (int Row=0;Row<rowCount();Row++) setRowHeight(Row,GetHeightForIcon()+2);
    }

    // Update display
    DoResizeColumns();

    // Start thread to scan files
    ScanMediaList.setFuture(QtConcurrent::run(this,&QCustomFolderTable::DoScanMediaList));

    QApplication::restoreOverrideCursor();
}

//====================================================================================================================

bool QCustomFolderTable::CanBrowseToPreviousPath() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomFolderTable::CanBrowseToPreviousPath");
    return (BrowsePathList.count()>1);
}

//====================================================================================================================

QString QCustomFolderTable::BrowseToPreviousPath() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomFolderTable::BrowseToPreviousPath");
    if (BrowsePathList.count()>1) {
        QString Path=BrowsePathList.takeLast();     // Actual folder
        Path=BrowsePathList.takeLast();             // Previous folder
        return Path;
    } else return "";
}

//====================================================================================================================

bool QCustomFolderTable::CanBrowseToUpperPath() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomFolderTable::CanBrowseToUpperPath");
    if (BrowsePathList.count()>0) {
        QString Path=AdjustDirForOS(BrowsePathList[BrowsePathList.count()-1]);     // Actual folder
        if (Path.endsWith(QDir::separator())) Path=Path.left(Path.length()-1);
        #ifdef Q_OS_WIN
        if ((Path.length()==2)&&(Path.at(1)==':')) return false;    // if it's a drive !
        #endif
        QStringList PathList=Path.split(QDir::separator());
        return PathList.count()>0;
    } else return false;
}

//====================================================================================================================

QString QCustomFolderTable::BrowseToUpperPath() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomFolderTable::BrowseToUpperPath");
    QString Path="";
    if (BrowsePathList.count()>0) {
        Path=AdjustDirForOS(BrowsePathList[BrowsePathList.count()-1]);     // Actual folder
        if (Path.endsWith(QDir::separator())) Path=Path.left(Path.length()-1);
        #ifdef Q_OS_WIN
        if ((Path.length()==2)&&(Path.at(1)==':')) return "";    // if it's a drive !
        #endif
        QStringList PathList=Path.split(QDir::separator());
        Path="";
        for (int i=0;i<PathList.count()-1;i++) {
            if (Path!="") Path=Path+QDir::separator();
            Path=Path+PathList[i];
        }
    }
    return Path;
}

//====================================================================================================================

void QCustomFolderTable::FillListFolder(QString Path) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomFolderTable::FillListFolder");
    ToLog(LOGMSG_INFORMATION,QApplication::translate("QCustomFolderTable","Reading directory content (%1)").arg(Path));

    if ((BrowsePathList.count()==0)||(BrowsePathList[BrowsePathList.count()-1]!=Path)) BrowsePathList.append(Path);
    while (BrowsePathList.count()>20) BrowsePathList.removeFirst(); // Not more than 20 path in the history !

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    // Ensure scan thread is stoped
    EnsureThreadIsStopped();

    // Set ScanMediaListProgress flag to inform that scan is not done
    ScanMediaListProgress=true;

    int i=0,j=0;
    CurrentShowFolderNumber =0;
    CurrentShowFilesNumber  =0;
    CurrentShowFolderNumber =0;
    CurrentTotalFilesNumber =0;
    CurrentShowFolderSize   =0;
    CurrentTotalFolderSize  =0;
    CurrentDisplayItem      =0;
    CurrentShowDuration     =0;

    // Reset content of the table (but keep column)
    setRowCount(0);

    // Adjust given Path
    #ifdef Q_OS_LINUX
    if (Path.startsWith("~")) Path=QDir::homePath()+Path.mid(1);
    #endif
    #ifdef Q_OS_WIN
    if (Path.startsWith(PersonalFolder)) Path=QDir::homePath()+Path.mid(PersonalFolder.length());
    Path=AdjustDirForOS(Path);
    #endif
    if (!Path.endsWith(QDir::separator())) Path=Path+QDir::separator();

    // if new Path, clear actual MediaList
    if (Path!=CurrentPath) {
        // Clear MediaList
        while (!MediaList.isEmpty()>0) delete MediaList.takeLast();
        // Keep current path
        CurrentPath=Path;
    }

    // Scan files and add them to table
    QDir                Folder(Path);
    QFileInfoList       Files=Folder.entryInfoList(QDir::Dirs|QDir::AllDirs|QDir::Files|QDir::Hidden);
    cBaseMediaFile      *MediaObject=NULL;

    // Scan folder entries to remove all file not corresponding to showhiden filter
    i=0;
    while (i<Files.count()) if ((Files[i].fileName()==".")||(Files[i].fileName()=="..")||
        ((!ShowHiddenFilesAndDir)&&((Files[i].isHidden())||(Files[i].fileName().startsWith(".")))))
        Files.removeAt(i); else i++;

    // Compute total number of files and total folder size
    foreach(QFileInfo File,Files) if (!File.isDir()) {
        CurrentTotalFilesNumber++;
        CurrentTotalFolderSize+=File.size();
    }

    //**********************************************************

    // Scan media list to remove all files wich are no longer present (depending on filter)
    i=0; while (i<MediaList.count()) if (MediaList[i]->IsFilteredFile(CurrentFilter,AllowedFilter)) i++; else delete MediaList.takeAt(i);

    // Scan media list to remove all files wich are no longer present in folder entries
    i=0;
    while (i<MediaList.count()) {
        j=0;
        while ((j<Files.count())&&(((QFileInfo)Files[j]).absoluteFilePath()!=MediaList[i]->FileName)) j++;
        if ((j<Files.count())&&(((QFileInfo)Files[j]).absoluteFilePath()==MediaList[i]->FileName))
             i++;  else delete MediaList.takeAt(i);
    }

    // Scan folder entries to integrate new files to actual MediaList
    for (i=0;i<Files.count();i++) if (QFileInfo(((QFileInfo)Files[i]).absoluteFilePath()).exists()){
        j=0;
        while ((j<MediaList.count())&&(((QFileInfo)Files[i]).absoluteFilePath()!=MediaList[j]->FileName)) j++;
        if (!((j<MediaList.count())&&(((QFileInfo)Files[i]).absoluteFilePath()==MediaList[j]->FileName))) {
            QFileInfo *File=&Files[i];
            // It's a new file => then add it
            if (ApplicationConfig->AllowImageExtension.contains(File->suffix().toLower())) {
                MediaObject=new cImageFile(ApplicationConfig);
                // Special case for folder Thumbnails
                if (File->fileName()=="folder.jpg") MediaObject->ObjectType=OBJECTTYPE_THUMBNAIL;
                else if (File->suffix().toLower()=="jpg") {  // Special case for video Thumbnails
                    // Search if a video with same name exist
                    for (int i=0;i<Files.count();i++)
                        if ((ApplicationConfig->AllowVideoExtension.contains(((QFileInfo)Files[i]).suffix().toLower()))&&
                            (((QFileInfo)Files[i]).completeBaseName()==File->completeBaseName())) MediaObject->ObjectType=OBJECTTYPE_THUMBNAIL;
                }
            }   else if (ApplicationConfig->AllowVideoExtension.contains(File->suffix().toLower())) MediaObject=new cVideoFile(OBJECTTYPE_VIDEOFILE,ApplicationConfig);
                else if (ApplicationConfig->AllowMusicExtension.contains(File->suffix().toLower())) MediaObject=new cVideoFile(OBJECTTYPE_MUSICFILE,ApplicationConfig);
                else if (File->suffix().toLower()=="ffd")                                           MediaObject=new cffDProjectFile(ApplicationConfig);
                else if (File->isDir())                                                             MediaObject=new cFolder(ApplicationConfig);
                else                                                                                MediaObject=new cUnmanagedFile(ApplicationConfig);

            // Check if file is valid
            if ((MediaObject)&&(!MediaObject->GetInformationFromFile(File->absoluteFilePath(),NULL,NULL))) {
                delete MediaObject;
                MediaObject=NULL;
            }
            // Check if file correspond to current filer
            if ((MediaObject)&&(!MediaObject->IsFilteredFile(CurrentFilter,AllowedFilter))) {
                delete MediaObject;
                MediaObject=NULL;
            }
            // If MediaObject still exist then append it
            if (MediaObject) MediaList.append(MediaObject);
        }
    }

    // Sort MediaList
    if (ApplicationConfig->ShowFoldersFirst) qSort(MediaList.begin(),MediaList.end(),MediaListLessThanWithFolder);
        else qSort(MediaList.begin(),MediaList.end(),MediaListLessThan);

    //**********************************************************

    // Create column (if needed)
    if (CurrentMode==DISPLAY_ICON100) {
        int SizeColumn=GetWidthForIcon();
        if (viewport()->width()/SizeColumn==0) setColumnCount(1); else setColumnCount(viewport()->width()/SizeColumn);
        for (i=0;i<columnCount();i++) setColumnWidth(i,SizeColumn);
    }

    // Append Media to table
    foreach(MediaObject,MediaList) AppendMediaToTable(MediaObject);

    // Update display
    DoResizeColumns();
    if (updatesEnabled()) setUpdatesEnabled(false);
    setUpdatesEnabled(true);

    // Start thread to scan files
    ScanMediaList.setFuture(QtConcurrent::run(this,&QCustomFolderTable::DoScanMediaList));

    QApplication::restoreOverrideCursor();

}

//====================================================================================================================

QStringList QCustomFolderTable::GetCurrentSelectedFiles() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomFolderTable::GetCurrentSelectedFiles");
    QStringList             Files;
    QList<cBaseMediaFile*>  SelMediaList=GetCurrentSelectedMediaFile();
    for (int i=0;i<SelMediaList.count();i++) Files.append(SelMediaList.at(i)->FileName);
    return Files;
}

//====================================================================================================================

QList<cBaseMediaFile*> QCustomFolderTable::GetCurrentSelectedMediaFile() const {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomFolderTable::GetCurrentSelectedMediaFile");

    QList<cBaseMediaFile*>  SelMediaList;
    QModelIndexList         SelList=selectionModel()->selectedIndexes();
    if (CurrentMode==DISPLAY_DATA) {
        QList<int> List;
        for (int i=0;i<MediaList.count();i++) List.append(0);
        for (int i=0;i<SelList.count();i++) List[SelList[i].row()]=1;
        for (int i=0;i<List.count();i++) if (List[i]==1) SelMediaList.append(MediaList[i]);
    } else {
        for (int i=0;i<SelList.count();i++) {
            int Col   =SelList[i].column();
            int Row   =SelList[i].row();
            if (Row*columnCount()+Col<MediaList.count()) SelMediaList.append(MediaList[Row*columnCount()+Col]); // Multiple items per row
        }
    }
    return SelMediaList;
}

//====================================================================================================================

cBaseMediaFile *QCustomFolderTable::GetCurrentMediaFile() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomFolderTable::GetCurrentMediaFile");

    cBaseMediaFile  *Media=NULL;
    if (currentRow()>=0) {
        int Index;
        if (CurrentMode==DISPLAY_DATA) Index=currentRow();
            else Index=currentRow()*columnCount()+currentColumn();
        if (Index<MediaList.count()) Media=MediaList[Index];
    }
    return Media;
}

//====================================================================================================================

void QCustomFolderTable::DoResizeColumns() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomFolderTable::DoResizeColumns");

    if (!StopAllEvent) {
        if (CurrentMode==DISPLAY_DATA) {
            int      ColSize[100]; for (int i=0;i<100;i++) ColSize[i]=horizontalHeader()->sectionSizeHint(i);
            QImage   Image(100,100,QImage::Format_ARGB32_Premultiplied);
            QPainter Painter;
            Painter.begin(&Image);

            for (int ItemIndex=0;ItemIndex<MediaList.count();ItemIndex++) for (int Col=0;Col<columnCount();Col++) {
                QString         TextToDisplay  =GetTextForColumn(Col,MediaList[ItemIndex]);
                QImage          *ImageToDisplay=GetImageForColumn(Col,MediaList[ItemIndex]);
                int             DecalX         =(ImageToDisplay!=NULL?18:0);
                QFont           font("Sans serif",9,QFont::Normal,QFont::StyleNormal);
                font.setBold(MediaList[ItemIndex]->ObjectType==OBJECTTYPE_FOLDER);
                font.setUnderline(false);
                Painter.setFont(font);

                QFontMetrics fm  =Painter.fontMetrics();
                int          Size=fm.width(TextToDisplay)+4+DecalX;
                if (ColSize[Col]<Size) ColSize[Col]=Size;
            }
            Painter.end();
            for (int Col=0;Col<columnCount();Col++) {
                if (ColSize[Col]>500) ColSize[Col]=500;
                if (columnWidth(Col)!=ColSize[Col]) setColumnWidth(Col,ColSize[Col]);
            }
        }
        this->viewport()->update();
        emit RefreshFolderInfo();
    }
}

//====================================================================================================================

void QCustomFolderTable::AppendMediaToTable(cBaseMediaFile *MediaObject) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomFolderTable::AppendMediaToTable");

    int Row=rowCount();
    int Col=0;

    if (MediaObject->ObjectType==OBJECTTYPE_FOLDER) {
        // Specific for folder : don't wait thread but call GetFullInformationFromFile now
        MediaObject->GetFullInformationFromFile();
        CurrentShowFolderNumber++;
    } else {
        CurrentShowFilesNumber++;
        CurrentShowFolderSize+=MediaObject->FileSize;
    }

    if (CurrentMode==DISPLAY_DATA) {

        insertRow(Row);
        verticalHeader()->setResizeMode(Row,QHeaderView::Fixed);
        setRowHeight(Row,GetHeightForIcon()+2);

    } else {

        int NbrCol=columnCount();

        // Check if we need to create a new line
        if (CurrentDisplayItem/NbrCol==rowCount()) {
            insertRow(Row);
            verticalHeader()->setResizeMode(Row,QHeaderView::Fixed);
            setRowHeight(Row,GetHeightForIcon());
        } else {
            Row--;
        }
    }
    update(model()->index(Row,Col));
    CurrentDisplayItem++;
}

//====================================================================================================================
QCustomFolderTable *CurrentTable=NULL;

void ScanMedia(const int &ItemIndex) {
    if ((!CurrentTable)||CurrentTable->StopScanMediaList || CurrentTable->StopAllEvent ||
        (ItemIndex>CurrentTable->MediaList.count())) return;

    if (!CurrentTable->MediaList[ItemIndex]->IsInformationValide) {

        CurrentTable->MediaList[ItemIndex]->IsIconNeeded=true;
        CurrentTable->MediaList[ItemIndex]->GetFullInformationFromFile(); // Get full information

        #if defined(Q_OS_WIN32) || defined(Q_OS_LINUX32)
        // Reduce quality of thumbnails to reduce memory used when huge number of files in folder
        if ((CurrentTable->MediaList.count()>=LOWQUALITYITEMNBR)&&
            (!CurrentTable->MediaList[ItemIndex]->Icon100.isNull())&&
            (CurrentTable->MediaList[ItemIndex]->Icon100.height()>LOWQUALITYITEMHEIGHT))
            CurrentTable->MediaList[ItemIndex]->Icon100=CurrentTable->MediaList[ItemIndex]->Icon100.scaledToHeight(CurrentTable->MediaList[ItemIndex]->Icon100.height()/2,Qt::SmoothTransformation);
        #endif

        // Update display
        if (CurrentTable->CurrentMode==DISPLAY_DATA) {
            for (int Col=0;Col<CurrentTable->columnCount();Col++) CurrentTable->update(CurrentTable->model()->index(ItemIndex,Col));
        } else {
            int Row=ItemIndex/CurrentTable->columnCount();
            int Col=ItemIndex-Row*CurrentTable->columnCount();
            CurrentTable->update(CurrentTable->model()->index(Row,Col));
        }
    }
    if ((CurrentTable->MediaList[ItemIndex]->ObjectType==OBJECTTYPE_MUSICFILE)||
        (CurrentTable->MediaList[ItemIndex]->ObjectType==OBJECTTYPE_VIDEOFILE))
            CurrentTable->CurrentShowDuration=CurrentTable->CurrentShowDuration+
                    QTime(0,0,0,0).msecsTo(((cVideoFile *)CurrentTable->MediaList[ItemIndex])->Duration);

        else if (CurrentTable->MediaList[ItemIndex]->ObjectType==OBJECTTYPE_FFDFILE)
            CurrentTable->CurrentShowDuration=CurrentTable->CurrentShowDuration+
                    ((cffDProjectFile *)CurrentTable->MediaList[ItemIndex])->Duration;
}

void QCustomFolderTable::DoScanMediaList() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomFolderTable::DoScanMediaList");
    if (InScanMediaFunction) return;
    InScanMediaFunction=true;
    CurrentShowDuration=0;

    // Create a list with item index
    QList<int> IndexList;
    for (int ItemIndex=0;ItemIndex<MediaList.count();ItemIndex++) IndexList.append(ItemIndex);
    CurrentTable=this;

    // Parse all items to update them
    QFuture<void> DoScanMedia = QtConcurrent::map(IndexList,ScanMedia);
    DoScanMedia.waitForFinished();

    // compute CurrentShowDuration
    for (int ItemIndex=0;(ItemIndex<MediaList.count())&&(!StopAllEvent)&&(!StopScanMediaList);ItemIndex++) {
        if ((MediaList[ItemIndex]->ObjectType==OBJECTTYPE_MUSICFILE)||(MediaList[ItemIndex]->ObjectType==OBJECTTYPE_VIDEOFILE)) CurrentShowDuration=CurrentShowDuration+QTime(0,0,0,0).msecsTo(((cVideoFile *)MediaList[ItemIndex])->Duration);
            else if (MediaList[ItemIndex]->ObjectType==OBJECTTYPE_FFDFILE)                                                      CurrentShowDuration=CurrentShowDuration+((cffDProjectFile *)MediaList[ItemIndex])->Duration;
    }

    // Clear ScanMediaListProgress flag to inform that scan is done
    ScanMediaListProgress=false;

    // Send message to ResizeColumns
    if (!StopAllEvent) emit NeedResizeColumns();
    InScanMediaFunction=false;
}
