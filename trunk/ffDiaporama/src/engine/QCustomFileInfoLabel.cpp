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

#include "QCustomFileInfoLabel.h"
#include <QPainter>
#include <QPixmap>
#include <QTextOption>
#include <QFont>
#include <QPen>
#include <QFontMetrics>
#include <QTableWidget>

QCustomFileInfoLabel::QCustomFileInfoLabel(QWidget *parent):QLabel(parent) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomFileInfoLabel::QCustomFileInfoLabel");

    DisplayMode =DISPLAY_WEBLONG;
    QList<cBaseMediaFile*> EmptyList;
    SetupFileInfoLabel(EmptyList);
}

//====================================================================================================================

void QCustomFileInfoLabel::mouseDoubleClickEvent(QMouseEvent *) {
    emit DoubleClickEvent();
}

//====================================================================================================================

void QCustomFileInfoLabel::paintEvent(QPaintEvent *) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomFileInfoLabel::paintEvent");

    if (!((QTableWidget *)parent())->updatesEnabled()) return;

    QPainter    Painter(this);
    QFont       font;
    QTextOption OptionText;
    QPen        Pen;
    int         RightWidth=0;

    int         LinesToDisplay=0;
    int         IconHeight=0;
    QImage      *Icon=NULL;

    switch (DisplayMode) {
        case DISPLAY_DATA    :  LinesToDisplay=0;       IconHeight=16;       Icon=Icon16;   break;
        case DISPLAY_WEBLONG :  LinesToDisplay=3;       IconHeight=48;       Icon=Icon48;   break;
    }

    if ((Icon)&&(!Icon->isNull())) {
        int addX=IconHeight-Icon->width();
        int addY=IconHeight-Icon->height();
        Painter.drawImage(QRect(1+addX/2,1+addY/2,Icon->width(),Icon->height()),*Icon);
    }

    if (LinesToDisplay) {
        // Setup default brush
        Painter.setBrush(Qt::NoBrush);

        // Setup default pen
        Pen.setColor(Qt::black);
        Pen.setWidth(1);
        Pen.setStyle(Qt::SolidLine);
        Painter.setPen(Pen);

        for (int i=0;i<LinesToDisplay;i++) for (int j=1;j>=0;j--) { // for each LinesToDisplay, start with right part

            // Setup text options
            if (j==0) {                 // To the left
                OptionText=QTextOption(Qt::AlignLeft|Qt::AlignVCenter);                     // Setup alignement
                OptionText.setWrapMode(QTextOption::NoWrap);                                // Setup word wrap text option
            } else if (j==1) {          // To the right
                OptionText=QTextOption(Qt::AlignRight|Qt::AlignVCenter);                    // Setup alignement
                OptionText.setWrapMode(QTextOption::NoWrap);                                // Setup word wrap text option
            }

            // Setup font
            if (i==0)   font=QFont("Sans serif",9,QFont::Bold,QFont::StyleNormal);          // First line use bold
                else    font=QFont("Sans serif",8,QFont::Normal,QFont::StyleNormal);        // other lines use small font
            font.setUnderline(false);
            Painter.setFont(font);

            if (j==1) {
                // Right part
                if (Text[i][j]!="") {
                    // calculate with
                    QFontMetrics fm = Painter.fontMetrics();
                    RightWidth=fm.width(Text[i][j])+2;
                    // draw text
                    Painter.drawText(QRectF(this->width()-RightWidth,1+i*(14+2),RightWidth-2,14),Text[i][j],OptionText);
                } else RightWidth=0;
            } else {
                // Left part
                if (Text[i][j]!="") {
                    if (i==0) {
                        Painter.drawText(QRectF(IconHeight+2+18,1+i*(14+2),this->width()-IconHeight-3-RightWidth-18,14),Text[i][j],OptionText);
                        if (IconType) {
                            int addX=16-IconType->width();
                            int addY=16-IconType->height();
                            Painter.drawImage(QRect(IconHeight+2+1+addX/2,1+addY/2,IconType->width(),IconType->height()),*IconType);
                        }
                    } else Painter.drawText(QRectF(IconHeight+2,1+i*(14+2),this->width()-IconHeight-3-RightWidth,14),Text[i][j],OptionText);
                }
            }
        }
    }
}

//====================================================================================================================

void QCustomFileInfoLabel::SetupFileInfoLabel(QList<cBaseMediaFile*> MediaList) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomFileInfoLabel::AppendMediaToTable");

    if (MediaList.count()==1) {

        // One file selection
        cBaseMediaFile *MediaObject=MediaList[0];

        IconType    =MediaObject->GetDefaultTypeIcon(cCustomIcon::ICON16);
        Icon16      =&MediaObject->Icon16;
        Icon48      =&MediaObject->Icon48;
        Icon100     =&MediaObject->Icon100;
        ShortName   =MediaObject->ShortName;

        Text[0][0]  =MediaObject->ShortName;
        Text[0][1]  =MediaObject->GetFileSizeStr();
        Text[1][0]  =MediaObject->GetTechInfo();
        Text[1][1]  =MediaObject->GetInformationValue("Duration");
        Text[2][0]  =MediaObject->GetTAGInfo();
        Text[2][1]  ="";

    } else if (MediaList.count()>1) {

        // Multi select

        IconType    =NULL;
        Icon16      =NULL;
        Icon32      =NULL;
        Icon48      =NULL;
        Icon100     =NULL;
        for (int i=0;i<3;i++) for (int j=0;j<2;j++) Text[i][j]="";

        Text[0][0]  ="Multiple files";

    } else {

        // No selection

        IconType    =NULL;
        Icon16      =NULL;
        Icon32      =NULL;
        Icon48      =NULL;
        Icon100     =NULL;
        for (int i=0;i<3;i++) for (int j=0;j<2;j++) Text[i][j]="";
    }
}
