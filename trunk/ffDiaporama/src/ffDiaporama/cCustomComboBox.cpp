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

#include "cCustomComboBox.h"

#include <QHeaderView>

//======================================
// Specific defines for this dialog box
//======================================
#define ICONWIDTH               64
#define ICONHEIGHT              16

//******************************************************************************************************************
// Custom Brush ComboBox ITEM
//******************************************************************************************************************

cBackgroundComboBoxItem::cBackgroundComboBoxItem(QObject *parent):QStyledItemDelegate(parent) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBackgroundComboBoxItem::cBackgroundComboBoxItem");
}

//========================================================================================================================
void cBackgroundComboBoxItem::paint(QPainter *painter,const QStyleOptionViewItem &option,const QModelIndex &index) const {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBackgroundComboBoxItem::paint");
    int BackgroundNum=index.row();
    if (BackgroundNum<BackgroundList.List.count()) {
        painter->drawPixmap(option.rect.left(),option.rect.top(),BackgroundList.List[BackgroundNum].Icon);
    } else {
        painter->fillRect(option.rect,Qt::white);
    }
    if (option.state & QStyle::State_Selected) {
        painter->setPen(QPen(Qt::blue));
        painter->setBrush(QBrush(Qt::NoBrush));
        painter->drawRect(option.rect.x(),option.rect.y(),option.rect.width()-1,option.rect.height()-1);
        painter->drawRect(option.rect.x()+1,option.rect.y()+1,option.rect.width()-1-2,option.rect.height()-1-2);
        painter->setPen(QPen(Qt::black));
        painter->drawRect(option.rect.x()+2,option.rect.y()+2,option.rect.width()-1-4,option.rect.height()-1-4);
    }
}

//========================================================================================================================

QSize cBackgroundComboBoxItem::sizeHint(const QStyleOptionViewItem &/*option*/,const QModelIndex &/*index*/) const {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBackgroundComboBoxItem::sizeHint");
    if (BackgroundList.List.count()>0) return QSize(BackgroundList.List[0].Icon.width(),64);
        else return QSize(114,64);
}

//******************************************************************************************************************
// Custom Brush ComboBox
//******************************************************************************************************************

cBackgroundComboBox::cBackgroundComboBox(QWidget *parent):QComboBox(parent) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBackgroundComboBox::cBackgroundComboBox");
    STOPMAJ=false;
    QTableWidget    *Table=new QTableWidget();
    Table->horizontalHeader()->hide();
    Table->verticalHeader()->hide();
    int ThumbWith=(BackgroundList.List.count()>0)?BackgroundList.List[0].Icon.width():114;
    setIconSize(QSize(ThumbWith,64));
    Table->insertColumn(Table->columnCount());  Table->setColumnWidth(Table->columnCount()-1,ThumbWith);
    setModel(Table->model());
    setView(Table);
    int i=0;
    while (i<BackgroundList.List.count()) {
        addItem("");    //automaticaly do a Table->insertRow(Table->rowCount());
        Table->setRowHeight(Table->rowCount()-1,64);
        i++;
    }
    ItemDelegate.ComboBox=this;
    setItemDelegate(&ItemDelegate);
    MakeIcons();
    this->view()->setFixedWidth(114+18);
    this->view()->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    connect(Table,SIGNAL(itemSelectionChanged()),this,SLOT(s_ItemSelectionChanged()));
}

//========================================================================================================================

void cBackgroundComboBox::SetCurrentBackground(QString BrushImage) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBackgroundComboBox::SetCurrentBackground");
    if (STOPMAJ) return;
    int i=0;
    while ((i<BackgroundList.List.count())&&(BrushImage!=BackgroundList.List[i].Name)) i++;
    if (i>=BackgroundList.List.count()) {
        i=0;
        ((QTableWidget *)view())->setCurrentCell(0,i);
        s_ItemSelectionChanged();
    }
    ((QTableWidget *)view())->setCurrentCell(0,i);
    setCurrentIndex(i);
    MakeIcons();
}

//========================================================================================================================

QString cBackgroundComboBox::GetCurrentBackground() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBackgroundComboBox::GetCurrentBackground");
    int i=currentIndex();
    MakeIcons();
    return BackgroundList.List[i].Name;
}

//========================================================================================================================

void cBackgroundComboBox::MakeIcons() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBackgroundComboBox::MakeIcons");
    int CurrentRow=currentIndex();
    if (CurrentRow<0) return;
    int CurrentCol=((QTableWidget *)view())->currentColumn();
    if (CurrentCol<0) CurrentCol=0;
    int BackgroundNum=CurrentRow;
    int ThumbWith=(BackgroundList.List.count()>0)?BackgroundList.List[0].Icon.width():114;
    QPixmap  Image(ThumbWith,64);
    QPainter Painter;
    Painter.begin(&Image);
    if (BackgroundNum<BackgroundList.List.count()) {
        Painter.drawPixmap(0,0,BackgroundList.List[BackgroundNum].Icon);
    } else {
        Painter.setBrush(QBrush(Qt::white));
        Painter.drawRect(QRectF(0,0,ThumbWith,64));
    }
    Painter.end();
    setItemIcon(CurrentRow,QIcon(Image));
}

//========================================================================================================================

void cBackgroundComboBox::s_ItemSelectionChanged() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBackgroundComboBox::s_ItemSelectionChanged");
    STOPMAJ=true;
    setCurrentIndex(((QTableWidget *)view())->currentRow());
    MakeIcons();
    emit currentIndexChanged(((QTableWidget *)view())->currentRow());
    STOPMAJ=false;
}

