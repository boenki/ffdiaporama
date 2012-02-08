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

// Basic inclusions (common to all files)
#include "_QCustomDialog.h"

#include <QtCore>
#include <QtDebug>
#include <QtGui/QApplication>
#include <QFileInfoList>
#include <QFileInfo>
#include <QDir>
#include <QFile>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QModelIndex>
#include <QModelIndexList>

#include "QCustomFolderTable.h"
#include "QCustomFileInfoLabel.h"

//#define DEBUGMODE

#define FILETABLESTATE_FILETOCHEK   1
#define FileToCheckIcon             ":/img/player_time.png"

#define CELLBORDER              8
int DISPLAYFILENAMEHEIGHT=20;           // Will be compute because it's not the same for all operating system

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
    ParentTable=(QCustomFolderTable *)parent;
}

//========================================================================================================================

void QCustomStyledItemDelegate::paint(QPainter *Painter,const QStyleOptionViewItem &option,const QModelIndex &index) const {
    int ItemIndex=index.row()*ParentTable->columnCount()+index.column();
    if (ItemIndex<ParentTable->MediaList.count()) {
        bool ThreadToPause=false;
        if (ParentTable->ScanMediaList.isRunning()) {
            ThreadToPause=true;
            ParentTable->ScanMediaList.pause();
        }

        QImage      *Icon           =NULL;
        QImage      *BigIcon        =NULL;
        int         LinesToDisplay  =0;
        int         IconHeight      =0;
        int         addX            =0;
        int         addY            =0;
        bool        DrawFilename    =false;
        QFont       font;
        QTextOption OptionText;
        QPen        Pen;

        switch (ParentTable->ApplicationConfig->CurrentMode) {
            case DISPLAY_ICON48  :
                if (!Icon && !ParentTable->MediaList[ItemIndex]->IsInformationValide) Icon=ParentTable->ApplicationConfig->DefaultDelayedIcon.GetIcon(cCustomIcon::ICON48);
                if (!Icon) Icon=&ParentTable->MediaList[ItemIndex]->Icon48;
                if ((!Icon)||(Icon->isNull())) Icon=ParentTable->MediaList[ItemIndex]->GetDefaultTypeIcon(cCustomIcon::ICON48);
                IconHeight=option.rect.height()-CELLBORDER-(ParentTable->ApplicationConfig->DisplayFileName?DISPLAYFILENAMEHEIGHT:0);
                LinesToDisplay=0;
                addX=(option.rect.width()-Icon->width())/2;
                if (ParentTable->ApplicationConfig->DisplayFileName) addY=(option.rect.height()-Icon->height()-DISPLAYFILENAMEHEIGHT)/3;
                    else addY=(option.rect.height()-Icon->height())/2;
                DrawFilename=ParentTable->ApplicationConfig->DisplayFileName;
                break;

            case DISPLAY_ICON100 :
                if (!Icon && !ParentTable->MediaList[ItemIndex]->IsInformationValide) Icon=ParentTable->ApplicationConfig->DefaultDelayedIcon.GetIcon(cCustomIcon::ICON100);
                if (!Icon) Icon=&ParentTable->MediaList[ItemIndex]->Icon100;
                if ((!Icon)||(Icon->isNull())) Icon=ParentTable->MediaList[ItemIndex]->GetDefaultTypeIcon(cCustomIcon::ICON100);
                IconHeight=option.rect.height()-CELLBORDER-(ParentTable->ApplicationConfig->DisplayFileName?DISPLAYFILENAMEHEIGHT:0);
                LinesToDisplay=0;
                addX=(option.rect.width()-Icon->width())/2;
                if (ParentTable->ApplicationConfig->DisplayFileName) addY=(option.rect.height()-Icon->height()-DISPLAYFILENAMEHEIGHT)/3;
                    else addY=(option.rect.height()-Icon->height())/2;
                DrawFilename=ParentTable->ApplicationConfig->DisplayFileName;
                break;

            case DISPLAY_ICONBIG :
                if (!Icon && !ParentTable->MediaList[ItemIndex]->IsInformationValide) Icon=ParentTable->ApplicationConfig->DefaultDelayedIcon.GetIcon(cCustomIcon::ICON100);
                if (!Icon) Icon=&ParentTable->MediaList[ItemIndex]->IconBIG;
                if ((!Icon)||(Icon->isNull())) Icon=ParentTable->MediaList[ItemIndex]->GetDefaultTypeIcon(cCustomIcon::ICONBIG);
                IconHeight=option.rect.height()-CELLBORDER-(ParentTable->ApplicationConfig->DisplayFileName?DISPLAYFILENAMEHEIGHT:0);
                LinesToDisplay=0;
                BigIcon=new QImage(Icon->copy());//->scaledToHeight(IconHeight));
                if (BigIcon->width()>option.rect.width()-CELLBORDER) {
                    delete BigIcon;
                    BigIcon=new QImage(Icon->scaledToWidth(option.rect.width()-CELLBORDER));
                }
                addX=(option.rect.width()-BigIcon->width())/2;
                if (ParentTable->ApplicationConfig->DisplayFileName) addY=(option.rect.height()-BigIcon->height()-DISPLAYFILENAMEHEIGHT)/3;
                    else addY=(option.rect.height()-BigIcon->height())/2;
                DrawFilename=ParentTable->ApplicationConfig->DisplayFileName;
                break;

            case DISPLAY_WEBSHORT:
                if (!Icon && !ParentTable->MediaList[ItemIndex]->IsInformationValide) Icon=ParentTable->ApplicationConfig->DefaultDelayedIcon.GetIcon(cCustomIcon::ICON32);
                if (!Icon) Icon=&ParentTable->MediaList[ItemIndex]->Icon32;
                if ((!Icon)||(Icon->isNull())) Icon=ParentTable->MediaList[ItemIndex]->GetDefaultTypeIcon(cCustomIcon::ICON32);
                IconHeight=32;
                LinesToDisplay=2;
                addX=(IconHeight-Icon->width())/2;
                addY=(IconHeight-Icon->height())/2;
                break;

            case DISPLAY_WEBLONG :
                if (!Icon && !ParentTable->MediaList[ItemIndex]->IsInformationValide) Icon=ParentTable->ApplicationConfig->DefaultDelayedIcon.GetIcon(cCustomIcon::ICON48);
                if (!Icon) Icon=&ParentTable->MediaList[ItemIndex]->Icon48;
                if ((!Icon)||(Icon->isNull())) Icon=ParentTable->MediaList[ItemIndex]->GetDefaultTypeIcon(cCustomIcon::ICON48);
                IconHeight=48;
                LinesToDisplay=3;
                addX=(IconHeight-Icon->width())/2;
                addY=(IconHeight-Icon->height())/2;
                break;
        }
        // Draw Icon
        if (BigIcon) Painter->drawImage(QRectF(option.rect.x()+1+addX,option.rect.y()+1+addY,BigIcon->width(),BigIcon->height()),*BigIcon);
            else if (Icon) Painter->drawImage(QRectF(option.rect.x()+1+addX,option.rect.y()+1+addY,Icon->width(),Icon->height()),*Icon);

        // Setup default brush
        Painter->setBrush(Qt::NoBrush);
        // Setup default pen
        Pen.setColor(Qt::black);
        Pen.setWidth(1);
        Pen.setStyle(Qt::SolidLine);
        Painter->setPen(Pen);

        // Setup default font
        font=QFont("Sans serif",8,QFont::Normal,QFont::StyleNormal);
        font.setUnderline(false);
        Painter->setFont(font);

        // Draw file name if icon view
        if (DrawFilename) {
            OptionText=QTextOption(Qt::AlignHCenter|Qt::AlignTop);                      // Setup alignement
            OptionText.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);          // Setup word wrap text option
            Painter->drawText(QRectF(option.rect.x()+1,option.rect.y()+option.rect.height()-1-DISPLAYFILENAMEHEIGHT,option.rect.width()-2,DISPLAYFILENAMEHEIGHT),
                              ParentTable->MediaList[ItemIndex]->ShortName,OptionText);
        }

        if (LinesToDisplay) {
            int         RightWidth=0;

            // Init text to display in summary views
            QString SummaryText[3][2];
            SummaryText[0][0]=ParentTable->MediaList[ItemIndex]->ShortName;
            SummaryText[0][1]=ParentTable->MediaList[ItemIndex]->GetFileSizeStr();
            SummaryText[1][0]=ParentTable->MediaList[ItemIndex]->GetTechInfo();
            SummaryText[1][1]=ParentTable->MediaList[ItemIndex]->GetInformationValue("Duration");
            SummaryText[2][0]=ParentTable->MediaList[ItemIndex]->GetTAGInfo();
            SummaryText[2][1]="";


            for (int i=0;i<LinesToDisplay;i++) for (int j=1;j>=0;j--) { // for each LinesToDisplay, start with right part

                // Setup text options
                if (j==0) {                                                                     // To the left
                    OptionText=QTextOption(Qt::AlignLeft|Qt::AlignVCenter);                     // Setup alignement
                    OptionText.setWrapMode(QTextOption::NoWrap);                                // Setup word wrap text option
                } else if (j==1) {                                                              // To the right
                    OptionText=QTextOption(Qt::AlignRight|Qt::AlignVCenter);                    // Setup alignement
                    OptionText.setWrapMode(QTextOption::NoWrap);                                // Setup word wrap text option
                }

                // Setup font
                if (i==0) {
                    font=QFont("Sans serif",9,QFont::Bold,QFont::StyleNormal);          // First line use bold
                    Painter->setFont(font);
                }

                if (j==1) {
                    // Right part
                    if (SummaryText[i][j]!="") {
                        // calculate with
                        QFontMetrics fm = Painter->fontMetrics();
                        RightWidth=fm.width(SummaryText[i][j])+2;
                        // draw text
                        Painter->drawText(QRectF(option.rect.x()+option.rect.width()-RightWidth,option.rect.y()+1+i*(14+2),RightWidth-2,14),SummaryText[i][j],OptionText);
                    } else RightWidth=0;
                } else {
                    // Left part
                    if (SummaryText[i][j]!="") {
                        if (i==0) {
                            Painter->drawText(QRectF(option.rect.x()+IconHeight+2+18,option.rect.y()+1+i*(14+2),option.rect.width()-IconHeight-3-RightWidth-18,14),SummaryText[i][j],OptionText);
                            QImage *IconF=ParentTable->MediaList[ItemIndex]->GetDefaultTypeIcon(cCustomIcon::ICON16);
                            int addX=16-IconF->width();
                            int addY=16-IconF->height();
                            Painter->drawImage(QRect(option.rect.x()+IconHeight+2+1+addX/2,option.rect.y()+1+addY/2,IconF->width(),IconF->height()),*IconF);
                        } else Painter->drawText(QRectF(option.rect.x()+IconHeight+2,option.rect.y()+1+i*(14+2),option.rect.width()-IconHeight-3-RightWidth,14),SummaryText[i][j],OptionText);
                    }
                }
            }
        }
        if (option.state & QStyle::State_Selected) {
            Painter->setPen(QPen(Qt::NoPen));
            Painter->setBrush(QBrush(Qt::blue));
            Painter->setOpacity(0.25);
            Painter->drawRect(option.rect.x(),option.rect.y(),option.rect.width()-1,option.rect.height()-1);
            Painter->setOpacity(1);

        // MouseOver is removed because it works correctly only on KDE !
        /*} else if (option.state & QStyle::State_MouseOver) {
            Painter->setPen(QPen(Qt::NoPen));
            Painter->setBrush(QBrush(Qt::gray));
            Painter->setOpacity(0.25);
            Painter->drawRect(option.rect.x(),option.rect.y(),option.rect.width()-1,option.rect.height()-1);
            Painter->setOpacity(1);*/
        }

        if (BigIcon) delete BigIcon;
        if (ThreadToPause) ParentTable->ScanMediaList.resume();
    }
}

//********************************************************************************************************
// QCustomFolderTable
//********************************************************************************************************

QCustomFolderTable::QCustomFolderTable(QWidget *parent):QTableWidget(parent) {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomFolderTable::QCustomFolderTable";
    #endif

    DefaultModel            =model();           // Save default QAbstractItemModel
    DefaultDelegate         =itemDelegate();    // Save default QAbstractItemDelegate
    IconDelegate            =(QAbstractItemDelegate *)new QCustomStyledItemDelegate(this);
    ApplicationConfig       =NULL;

    CurrentPath             ="";
    CurrentShowFolderNumber =0;
    CurrentShowFilesNumber  =0;
    CurrentShowFolderNumber =0;
    CurrentTotalFilesNumber =0;
    CurrentShowFolderSize   =0;
    CurrentTotalFolderSize  =0;
    CurrentDisplayItem      =0;
    CurrentShowDuration     =QTime(0,0,0,0);
    StopScanMediaList       =false;
    ScanMediaListProgress   =false;
    IsThumbCacheReady       =false;
    ThumbCache              =NULL;
    connect(this,SIGNAL(NeedResizeColumns()),this,SLOT(DoResizeColumns()));
}

//====================================================================================================================

QCustomFolderTable::~QCustomFolderTable() {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomFolderTable::~QCustomFolderTable";
    #endif
    // Ensure scan thread is stoped
    EnsureThreadIsStopped();
    // Clear MediaList
    while (!MediaList.isEmpty()>0) {
        cBaseMediaFile *Item=MediaList.takeLast();
        delete Item;
    }
}

//====================================================================================================================

void QCustomFolderTable::EnsureThreadIsStopped() {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomFolderTable::~QCustomFolderTable";
    #endif
    // Ensure scan thread is stoped
    if (ScanMediaList.isRunning()) {
        StopScanMediaList=true;
        ScanMediaList.waitForFinished();
        // flush event queue"
        //while (QApplication::hasPendingEvents()) QApplication::processEvents();
        StopScanMediaList=false;
    }
    // Flush ThumbCacheFile
    if (ThumbCache) {
        delete ThumbCache;
        ThumbCache=NULL;
    }
}

//====================================================================================================================

int QCustomFolderTable::GetWidthForIcon() {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomFolderTable::GetWidthForIcon";
    #endif

    int SizeColumn;
    if (ApplicationConfig->CurrentMode==DISPLAY_ICON48)            SizeColumn=(ApplicationConfig->DisplayFileName?80:48)+CELLBORDER;
        else if (ApplicationConfig->CurrentMode==DISPLAY_ICON100)  SizeColumn=100+CELLBORDER;
        else {
        if (ApplicationConfig->CurrentFilter==OBJECTTYPE_VIDEOFILE)          SizeColumn=ApplicationConfig->Video_ThumbWidth+CELLBORDER;
            else if (ApplicationConfig->CurrentFilter==OBJECTTYPE_IMAGEFILE) SizeColumn=ApplicationConfig->Image_ThumbWidth+CELLBORDER;
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
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomFolderTable::GetHeightForIcon";
    #endif

    int SizeColumn;
    if (ApplicationConfig->CurrentMode==DISPLAY_ICON48)         SizeColumn=48+CELLBORDER+(ApplicationConfig->DisplayFileName?DISPLAYFILENAMEHEIGHT:0);
    else if (ApplicationConfig->CurrentMode==DISPLAY_ICON100)   SizeColumn=100+CELLBORDER+(ApplicationConfig->DisplayFileName?DISPLAYFILENAMEHEIGHT:0);
    else {
        if (ApplicationConfig->CurrentFilter==OBJECTTYPE_VIDEOFILE)      SizeColumn=ApplicationConfig->Video_ThumbHeight+CELLBORDER+(ApplicationConfig->DisplayFileName?DISPLAYFILENAMEHEIGHT:0);
        else if (ApplicationConfig->CurrentFilter==OBJECTTYPE_IMAGEFILE) SizeColumn=ApplicationConfig->Image_ThumbHeight+CELLBORDER+(ApplicationConfig->DisplayFileName?DISPLAYFILENAMEHEIGHT:0);
        else {
            SizeColumn=ApplicationConfig->Image_ThumbHeight;
            if (SizeColumn<ApplicationConfig->Music_ThumbHeight) SizeColumn=ApplicationConfig->Music_ThumbHeight;
            if (SizeColumn<ApplicationConfig->Video_ThumbHeight) SizeColumn=ApplicationConfig->Video_ThumbHeight;
            SizeColumn+=CELLBORDER+(ApplicationConfig->DisplayFileName?DISPLAYFILENAMEHEIGHT:0);
        }
    }
    return SizeColumn;
}

//====================================================================================================================

void QCustomFolderTable::resizeEvent(QResizeEvent *ev) {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomFolderTable::resizeEvent";
    #endif

    if ((ApplicationConfig->CurrentMode==DISPLAY_ICON48)||(ApplicationConfig->CurrentMode==DISPLAY_ICON100)||(ApplicationConfig->CurrentMode==DISPLAY_ICONBIG)) {
        int SizeColumn=GetWidthForIcon();
        if (columnCount()!=(viewport()->width()/SizeColumn)) {
            if (viewport()->width()/SizeColumn==0) setColumnCount(1); else setColumnCount(viewport()->width()/SizeColumn);
            for (int i=0;i<columnCount();i++) setColumnWidth(i,SizeColumn);
            setRowCount((CurrentDisplayItem/columnCount())+1);
            int RHeight=GetHeightForIcon();
            for (int i=0;i<(CurrentDisplayItem/columnCount())+1;i++) setRowHeight(RHeight,SizeColumn);
            // Fill empty cell with unselecable item if needed
            if ((ApplicationConfig->CurrentMode==DISPLAY_ICON48)||(ApplicationConfig->CurrentMode==DISPLAY_ICON100)||(ApplicationConfig->CurrentMode==DISPLAY_ICONBIG)) {
                int toFill=CurrentDisplayItem-((rowCount()-1)*columnCount());
                while (toFill<columnCount()) {
                    setItem(rowCount()-1,toFill,new QNullTableWidgetItem(""));
                    toFill++;
                }
            }
        }
    } else if ((ApplicationConfig->CurrentMode==DISPLAY_WEBSHORT)||(ApplicationConfig->CurrentMode==DISPLAY_WEBLONG)) {
        setColumnWidth(0,viewport()->width());
    }
    QTableWidget::resizeEvent(ev);
}

//====================================================================================================================

void QCustomFolderTable::SetMode(int Mode,int Filter) {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomFolderTable::SetMode";
    #endif

    // Ensure scan thread is stoped
    EnsureThreadIsStopped();

    ApplicationConfig->CurrentMode  =Mode;
    ApplicationConfig->CurrentFilter=Filter;

    if ((ApplicationConfig->CurrentMode==DISPLAY_ICON48)||(ApplicationConfig->CurrentMode==DISPLAY_ICON100)||(ApplicationConfig->CurrentMode==DISPLAY_ICONBIG)) {
        // Compute DISPLAYFILENAMEHEIGHT
        QImage      Img(100,100,QImage::Format_ARGB32);
        QPainter    Painter;
        Painter.begin(&Img);
        QFont font("Sans serif",8,QFont::Normal,QFont::StyleNormal);
        Painter.setFont(font);
        QFontMetrics fm = Painter.fontMetrics();
        if (ApplicationConfig->CurrentMode==DISPLAY_ICON48) DISPLAYFILENAMEHEIGHT=fm.height()*3;    // 3 lines for ICON48 mode
            else                                            DISPLAYFILENAMEHEIGHT=fm.height()*2;    // 2 lines for bigest mode
        Painter.end();
    }

    // Reset content
    setRowCount(0);
    setColumnCount(0);

    // Define columns
    QString ColumnDef="";
    int     SizeColumn=1;

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

    switch (ApplicationConfig->CurrentMode) {
        case DISPLAY_ICON48 :
        case DISPLAY_ICON100 :
        case DISPLAY_ICONBIG :
            setItemDelegate(IconDelegate);
            setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            setSelectionBehavior(QAbstractItemView::SelectItems);
            horizontalHeader()->hide();
            horizontalHeader()->setStretchLastSection(false);
            setShowGrid(false);
            break;

        case DISPLAY_DATA :
            setItemDelegate(DefaultDelegate);
            setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
            setSelectionBehavior(QAbstractItemView::SelectRows);
            horizontalHeader()->show();
            horizontalHeader()->setStretchLastSection(false);
            setShowGrid(false);
            switch (ApplicationConfig->CurrentFilter) {
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
            horizontalHeaderItem(SizeColumn)->setTextAlignment(Qt::AlignRight);  // Size to the right
            break;

        case DISPLAY_WEBSHORT :
        case DISPLAY_WEBLONG :
            setItemDelegate(IconDelegate);
            setSelectionBehavior(QAbstractItemView::SelectRows);
            setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            horizontalHeader()->hide();
            horizontalHeader()->setStretchLastSection(true);
            setHorizontalHeaderLabels(QString("WEB_VIEW").split(";"));
            setShowGrid(true);
            setColumnCount(1);
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

void QCustomFolderTable::mouseDoubleClickEvent(QMouseEvent *) {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomFolderTable::mouseDoubleClickEvent";
    #endif
    emit DoubleClickEvent();
}

//====================================================================================================================

void QCustomFolderTable::mouseReleaseEvent(QMouseEvent *ev) {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomFolderTable::mouseDoubleClickEvent";
    #endif
    if (ev->button()==Qt::RightButton) emit RightClickEvent(ev);
        else QTableWidget::mouseReleaseEvent(ev);
}

//====================================================================================================================

void QCustomFolderTable::RefreshListFolder() {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomFolderTable::FillListFolder";
    #endif
    FillListFolder(CurrentPath);
}

//====================================================================================================================

void QCustomFolderTable::FillListFolder(QString Path) {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomFolderTable::FillListFolder";
    #endif
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
    CurrentShowDuration     =QTime(0,0,0,0);

    // Reset content of the table (but keep column)
    setRowCount(0);

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

    // Compute ThumbCache FileName
    ThumbCache=new cThumbCache(Path);

    // Scan files and add them to table
    QDir                Folder(Path);
    QFileInfoList       Files=Folder.entryInfoList(QDir::Dirs|QDir::AllDirs|QDir::Files|QDir::Hidden);
    cBaseMediaFile      *MediaObject=NULL;

    // Scan folder entries to remove all file not corresponding to showhiden filter
    i=0; while (i<Files.count()) if (ApplicationConfig->ShowHiddenFilesAndDir || !((Files[i].isHidden())||(Files[i].fileName().startsWith(".")))) i++; else Files.removeAt(i);

    // Compute total number of files and total folder size
    foreach(QFileInfo File,Files) if (!File.isDir()) {
        CurrentTotalFilesNumber++;
        CurrentTotalFolderSize+=File.size();
    }

    //**********************************************************

    // Scan media list to remove all files wich are no longer present (depending on filter)
    i=0; while (i<MediaList.count()) if (MediaList[i]->IsFilteredFile(ApplicationConfig->CurrentFilter)) i++; else delete MediaList.takeAt(i);

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
            if ((MediaObject)&&(!MediaObject->IsFilteredFile(ApplicationConfig->CurrentFilter))) {
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
    if ((ApplicationConfig->CurrentMode==DISPLAY_ICON48)||(ApplicationConfig->CurrentMode==DISPLAY_ICON100)||(ApplicationConfig->CurrentMode==DISPLAY_ICONBIG)) {
        int SizeColumn=GetWidthForIcon();
        if (viewport()->width()/SizeColumn==0) setColumnCount(1); else setColumnCount(viewport()->width()/SizeColumn);
        for (i=0;i<columnCount();i++) setColumnWidth(i,SizeColumn);
    }

    // Append Media to table
    foreach(MediaObject,MediaList) AppendMediaToTable(MediaObject);

    // Fill empty cell with unselecable item if needed
    if ((ApplicationConfig->CurrentMode==DISPLAY_ICON48)||(ApplicationConfig->CurrentMode==DISPLAY_ICON100)||(ApplicationConfig->CurrentMode==DISPLAY_ICONBIG)) {
        int toFill=CurrentDisplayItem-((rowCount()-1)*columnCount());
        while (toFill<columnCount()) {
            setItem(rowCount()-1,toFill,new QNullTableWidgetItem(""));
            toFill++;
        }
    }

    // Update display
    DoResizeColumns();

    // Start thread to scan files
    ScanMediaList.setFuture(QtConcurrent::run(this,&QCustomFolderTable::DoScanMediaList));

    QApplication::restoreOverrideCursor();

}

//====================================================================================================================

QList<cBaseMediaFile*> QCustomFolderTable::GetCurrentSelectedMediaFile() {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomFolderTable::GetCurrentMediaFile";
    #endif

    QList<cBaseMediaFile*>  SelMediaList;
    QModelIndexList         SelList=selectionModel()->selectedIndexes();
    for (int i=0;i<SelList.count();i++) {
        int Col=SelList[i].column();
        int Row=SelList[i].row();
        if (ApplicationConfig->CurrentMode==DISPLAY_DATA) SelMediaList.append(MediaList[Row]);    // One item per row
            else SelMediaList.append(MediaList[Row*columnCount()+Col]);                           // Multiple items per row
    }
    return SelMediaList;
}

//====================================================================================================================

cBaseMediaFile *QCustomFolderTable::GetCurrentMediaFile() {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomFolderTable::GetCurrentMediaFile";
    #endif
    cBaseMediaFile  *Media=NULL;
    if (currentRow()>=0) {
        if (ApplicationConfig->CurrentMode==DISPLAY_DATA) {
            QString ShortName="";
            int     i=0;
            if (item(currentRow(),0)) ShortName=item(currentRow(),0)->text();
            while ((i<MediaList.count())&&(MediaList[i]->ShortName!=ShortName)) i++;
            if ((i<MediaList.count())&&(MediaList[i]->ShortName==ShortName)) Media=MediaList[i];
        } else if (currentRow()*columnCount()+currentColumn()<MediaList.count()) Media=MediaList[currentRow()*columnCount()+currentColumn()];
    }
    return Media;
}

//====================================================================================================================

void QCustomFolderTable::DoResizeColumns() {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomFolderTable::DoResizeColumns";
    #endif
    // Adjust row & column size if needed
    if (ApplicationConfig->CurrentMode==DISPLAY_DATA) {
        setUpdatesEnabled(false);
        setVisible(false);                      // To ensure all items of all columns are used to compute size
        resizeColumnsToContents();              // Resize column widht
        resizeRowsToContents();                 // Resize row height
        setVisible(true);                       // To allow display
        setUpdatesEnabled(true);
    }
    this->viewport()->update();
    emit RefreshFolderInfo();
}

//====================================================================================================================

void QCustomFolderTable::AppendMediaToTable(cBaseMediaFile *MediaObject) {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomFolderTable::AppendMediaToTable";
    #endif

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

    if (ApplicationConfig->CurrentMode==DISPLAY_DATA) {

        QColor Background=((Row & 0x01)==0x01)?Qt::white:QColor(0xE0,0xE0,0xE0);
        insertRow(Row);
        verticalHeader()->setResizeMode(Row,QHeaderView::Fixed);
        setRowHeight(Row,16+2);
        setItem(Row,0,CreateItem(MediaObject->ShortName,Qt::AlignLeft|Qt::AlignVCenter,Background));
        if (MediaObject->ObjectType==OBJECTTYPE_FOLDER) item(Row,0)->setIcon(QIcon(QPixmap::fromImage(MediaObject->Icon16)));
            else item(Row,0)->setIcon(QIcon(QPixmap().fromImage(*MediaObject->GetDefaultTypeIcon(cCustomIcon::ICON16))));

        for (int iCol=1;iCol<columnCount();iCol++) {
            QString ColName=horizontalHeaderItem(iCol)->text();
            if      (ColName==QApplication::translate("QCustomFolderTable","File Type","Column header")) setItem(Row,iCol,CreateItem(MediaObject->GetFileTypeStr(),Qt::AlignLeft|Qt::AlignVCenter,Background));
            else if (ColName==QApplication::translate("QCustomFolderTable","File Size","Column header")) setItem(Row,iCol,CreateItem(MediaObject->FileSizeText,Qt::AlignRight|Qt::AlignVCenter,Background));
            else if (ColName==QApplication::translate("QCustomFolderTable","File Date","Column header")) setItem(Row,iCol,CreateItem(MediaObject->GetFileDateTimeStr(),Qt::AlignLeft|Qt::AlignVCenter,Background));
        }

    } else if ((ApplicationConfig->CurrentMode==DISPLAY_WEBSHORT)||(ApplicationConfig->CurrentMode==DISPLAY_WEBLONG)) {

        insertRow(Row);
        verticalHeader()->setResizeMode(Row,QHeaderView::Fixed);
        if (ApplicationConfig->CurrentMode==DISPLAY_WEBSHORT) setRowHeight(Row,32+2); else setRowHeight(Row,48+2);  // DISPLAY_WEBLONG

    } else if ((ApplicationConfig->CurrentMode==DISPLAY_ICON48)||(ApplicationConfig->CurrentMode==DISPLAY_ICON100)||(ApplicationConfig->CurrentMode==DISPLAY_ICONBIG)) {

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
    UpdateMediaToTable(Row,Col,MediaObject);
    CurrentDisplayItem++;
}

//====================================================================================================================

void QCustomFolderTable::UpdateMediaToTable(int Row,int Col,cBaseMediaFile *MediaObject) {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomFolderTable::UpdateMediaToTable";
    #endif

    if (!MediaObject->IsValide) return;

    if ((ApplicationConfig->CurrentMode==DISPLAY_DATA)) {

        QColor Background=((Row & 0x01)==0x01)?Qt::white:QColor(0xE0,0xE0,0xE0);
        for (int Col=1;Col<columnCount();Col++) {
            int NbrChapter=0;
            if (MediaObject->ObjectType==OBJECTTYPE_VIDEOFILE) NbrChapter=((cVideoFile *)MediaObject)->NbrChapters;
            if (MediaObject->ObjectType==OBJECTTYPE_FFDFILE)   NbrChapter=((cffDProjectFile *)MediaObject)->NbrChapters;

            QString ColName=horizontalHeaderItem(Col)->text();
            if      (ColName==QApplication::translate("QCustomFolderTable","Chapters","Column header"))         setItem(Row,Col,CreateItem(NbrChapter>0?QString("%1").arg(NbrChapter):"",Qt::AlignCenter|Qt::AlignVCenter,Background));
            else if (ColName==QApplication::translate("QCustomFolderTable","Duration","Column header"))         setItem(Row,Col,CreateItem(MediaObject->GetInformationValue("Duration"),Qt::AlignLeft|Qt::AlignVCenter,Background));
            else if (ColName==QApplication::translate("QCustomFolderTable","Image Size","Column header"))       setItem(Row,Col,CreateItem(MediaObject->GetImageSizeStr(cBaseMediaFile::SIZEONLY),Qt::AlignCenter|Qt::AlignVCenter,Background));
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

    }
    update(model()->index(Row,Col));
}

//====================================================================================================================

void QCustomFolderTable::DoScanMediaList() {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomFolderTable::DoScanMediaList";
    #endif

    // Parse all items to update them
    for (int ItemIndex=0;ItemIndex<MediaList.count();ItemIndex++) if (!MediaList[ItemIndex]->IsInformationValide)  {
        // Get full information
        if  (MediaList[ItemIndex]->ObjectType==OBJECTTYPE_IMAGEFILE) ((cImageFile *)MediaList[ItemIndex])->GetFullInformationFromFile(ThumbCache);
            else MediaList[ItemIndex]->GetFullInformationFromFile();
        // Update display
        if (ApplicationConfig->CurrentMode==DISPLAY_DATA) UpdateMediaToTable(ItemIndex,0,MediaList[ItemIndex]); else {
            int Row=ItemIndex/columnCount();
            int Col=ItemIndex-Row*columnCount();
            UpdateMediaToTable(Row,Col,MediaList[ItemIndex]);
        }
        if (StopScanMediaList) break;
    }

    // compute CurrentShowDuration
    CurrentShowDuration=QTime(0,0,0,0);
    for (int ItemIndex=0;ItemIndex<MediaList.count();ItemIndex++) {
        if ((MediaList[ItemIndex]->ObjectType==OBJECTTYPE_MUSICFILE)||(MediaList[ItemIndex]->ObjectType==OBJECTTYPE_VIDEOFILE)) CurrentShowDuration=CurrentShowDuration.addMSecs(QTime(0,0,0,0).msecsTo(((cVideoFile *)MediaList[ItemIndex])->Duration));
            else if (MediaList[ItemIndex]->ObjectType==OBJECTTYPE_FFDFILE)                                                      CurrentShowDuration=CurrentShowDuration.addMSecs(((cffDProjectFile *)MediaList[ItemIndex])->Duration);
    }

    // Clear ScanMediaListProgress flag to inform that scan is done
    ScanMediaListProgress=false;

    // Send message to ResizeColumns
    emit NeedResizeColumns();
}