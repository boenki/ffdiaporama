/* ======================================================================
    This file is part of ffDiaporama
    ffDiaporama is a tools to make diaporama as video
    Copyright (C) 2011-2013 Dominique Levray <domledom@laposte.net>

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

#include "cCustomLocationTable.h"
#include "engine/cLocation.h"

//********************************************************************************************************
// QCustomFolderTable
//********************************************************************************************************

class QCustomLocationItemDelegate : public QStyledItemDelegate {
//Q_OBJECT
public:
    cCustomLocationTable  *ParentTable;

    explicit QCustomLocationItemDelegate(QObject *parent);

    virtual void    paint(QPainter *painter,const QStyleOptionViewItem &option,const QModelIndex &index) const;
};

//========================================================================================================================

QCustomLocationItemDelegate::QCustomLocationItemDelegate(QObject *parent):QStyledItemDelegate(parent) {
    ParentTable=(cCustomLocationTable *)parent;
}

//========================================================================================================================

void QCustomLocationItemDelegate::paint(QPainter *Painter,const QStyleOptionViewItem &option,const QModelIndex &index) const {
    if (!ParentTable->CurrentMap) return;
    if (index.row()>ParentTable->CurrentMap->List.count()) return;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    QFont           FontNormal,FontBold;
    QTextOption     OptionText;
    QPen            Pen;
    int             FontFactor=((ParentTable->CurrentMap->ApplicationConfig->TimelineHeight-TIMELINEMINHEIGH)/20+1)*10;

    // Setup default brush
    Painter->setBrush(Qt::white);
    Painter->fillRect(option.rect,Qt::white);

    // Setup default pen
    Pen.setColor(Qt::black);
    Pen.setWidth(1);
    Pen.setStyle(Qt::SolidLine);
    Painter->setPen(Pen);

    // Setup font for first line
    FontBold=QFont("Sans serif",9,QFont::Normal,QFont::StyleNormal);
    Painter->setFont(FontBold);
    #ifdef Q_OS_WIN
    FontBold.setPointSizeF(double(110+FontFactor)/double(Painter->fontMetrics().boundingRect("0").height()));                    // Scale font
    #else
    FontBold.setPointSizeF((double(120+FontFactor)/double(Painter->fontMetrics().boundingRect("0").height()))*ScreenFontAdjust); // Scale font
    #endif
    FontBold.setBold(true);
    FontBold.setUnderline(false);

    // Setup font for second line
    FontNormal=QFont("Sans serif",8,QFont::Normal,QFont::StyleNormal);
    FontNormal.setUnderline(false);
    Painter->setFont(FontNormal);
    #ifdef Q_OS_WIN
    FontNormal.setPointSizeF(double(100+FontFactor)/double(Painter->fontMetrics().boundingRect("0").height()));                  // Scale font
    #else
    FontNormal.setPointSizeF((double(100+FontFactor)/double(Painter->fontMetrics().boundingRect("0").height()))*ScreenFontAdjust);// Scale font
    #endif
    OptionText=QTextOption(Qt::AlignLeft|Qt::AlignVCenter);     // Setup alignement
    OptionText.setWrapMode(QTextOption::NoWrap);                // Setup word wrap text option

    if (index.column()==0) {
        // Display marker

    } else if (index.column()==1) {
        // Display location
        cLocation *Location=(cLocation *)ParentTable->CurrentMap->List.at(index.row());
        QImage    Icon     =Location->Icon.GetImageDiskBrush(QRectF(0,0,32,32),false,0,NULL,1,NULL);
        int       DecalX   =32;
        int       Height   =(option.rect.height()-2)/2;
        Painter->drawImage(option.rect.x()+2,option.rect.y()+2,Icon);
        Painter->setFont(FontBold);     Painter->drawText(QRectF(option.rect.x()+2+DecalX,option.rect.y()+1,option.rect.width()-4-DecalX,Height),Location->Name,OptionText);
        Painter->setFont(FontNormal);   Painter->drawText(QRectF(option.rect.x()+2+DecalX,option.rect.y()+1+Height,option.rect.width()-4-DecalX,Height),Location->Address,OptionText);
    }

    // Selection mode (Note: MouseOver is removed because it works correctly only on KDE !)
    if (option.state & QStyle::State_Selected) {
        Painter->setPen(QPen(Qt::NoPen));
        Painter->setBrush(QBrush(Qt::blue));
        Painter->setOpacity(0.25);
        Painter->drawRect(option.rect.x(),option.rect.y(),option.rect.width(),option.rect.height());
        Painter->setOpacity(1);
    }

    QApplication::restoreOverrideCursor();
}

//********************************************************************************************************
// cCustomLocationTable
//********************************************************************************************************

cCustomLocationTable::cCustomLocationTable(QWidget *parent):QTableWidget(parent) {
    CurrentMap=NULL;
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    horizontalHeader()->show();
    horizontalHeader()->setStretchLastSection(false);
    setShowGrid(true);
    setItemDelegate((QAbstractItemDelegate *)new QCustomLocationItemDelegate(this));
    verticalHeader()->setDefaultSectionSize(36);
}
