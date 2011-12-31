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

#include "QCustomFileInfoLabel.h"
#include <QPainter>
#include <QPixmap>
#include <QTextOption>
#include <QFont>
#include <QPen>
#include <QFontMetrics>

//#define DEBUGMODE

QCustomFileInfoLabel::QCustomFileInfoLabel(QWidget *parent):QLabel(parent) {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomFileInfoLabel::QCustomFileInfoLabel";
    #endif
}

void QCustomFileInfoLabel::paintEvent(QPaintEvent *) {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomFileInfoLabel::paintEvent";
    #endif
    QPainter    Painter(this);
    if (!IconLeft.isNull()) {
        QPixmap     Image=IconLeft.pixmap(48,48);
        int         addX=48-Image.width();
        Painter.drawPixmap(1+addX/2,1,Image.width(),48,Image);
    }
    if (TextLeftUpper!="") {

        int INFO_RIGHT_WIDTH=0;

        Painter.setBrush(Qt::NoBrush);

        QTextOption OptionText;
        OptionText=QTextOption(Qt::AlignRight|Qt::AlignVCenter);                             // Setup vertical alignement
        OptionText.setWrapMode(QTextOption::NoWrap);                                        // Setup word wrap text option

        QPen Pen;
        Pen.setColor(Qt::black);
        Pen.setWidth(1);
        Pen.setStyle(Qt::SolidLine);
        Painter.setPen(Pen);

        if (TextRightUpper!="") {
            OptionText.setWrapMode(QTextOption::NoWrap);                                        // Setup word wrap text option
            QFont font=QFont("Sans serif",9,QFont::Normal,QFont::StyleNormal);                 // FontSize is always 10 and size if given with setPointSizeF !
            font.setUnderline(false);                                                           // Set underline
            Painter.setFont(font);
            QFontMetrics fm = Painter.fontMetrics();
            INFO_RIGHT_WIDTH=fm.width(TextRightUpper)+2;
            Painter.drawText(QRectF(this->width()-INFO_RIGHT_WIDTH,1,INFO_RIGHT_WIDTH-2,14),TextRightUpper,OptionText);
        }

        OptionText=QTextOption(Qt::AlignLeft|Qt::AlignVCenter);                             // Setup vertical alignement
        QFont font=QFont("Sans serif",9,QFont::Bold,QFont::StyleNormal);                 // FontSize is always 10 and size if given with setPointSizeF !
        font.setUnderline(false);                                                           // Set underline
        Painter.setFont(font);
        Painter.drawText(QRectF(50,1,this->width()-51-INFO_RIGHT_WIDTH,16),TextLeftUpper,OptionText);


        if (TextLeftBottom!="") {
            OptionText.setWrapMode(QTextOption::WordWrap);                                        // Setup word wrap text option
            QFont font=QFont("Sans serif",8,QFont::Normal,QFont::StyleNormal);                 // FontSize is always 10 and size if given with setPointSizeF !
            font.setUnderline(false);                                                           // Set underline
            Painter.setFont(font);
            Painter.drawText(QRectF(50,17,this->width()-51,32),TextLeftBottom,OptionText);
        }
    }
}
