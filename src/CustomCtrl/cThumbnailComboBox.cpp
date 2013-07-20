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

#include "cThumbnailComboBox.h"

#define IMAGEWIDTH   THUMBWITH/10
#define IMAGEHEIGHT  THUMBHEIGHT/10
#define NBRCOLUMN    1

cThumbnailComboBoxItem::cThumbnailComboBoxItem(QObject *parent):QStyledItemDelegate(parent) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cThumbnailComboBoxItem::cThumbnailComboBoxItem");
}

//========================================================================================================================
void cThumbnailComboBoxItem::paint(QPainter *painter,const QStyleOptionViewItem &option,const QModelIndex &index) const {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cThumbnailComboBoxItem::paint");
    int CurIndex=index.row()*NBRCOLUMN+index.column();

    if ((CurIndex>=0)&&(CurIndex<ComboBox->ModelTable->List.count())) {
        painter->drawImage(option.rect.left(),option.rect.top(),ComboBox->ModelTable->List[CurIndex].Thumbnail.scaledToHeight(IMAGEHEIGHT+4,Qt::SmoothTransformation));
    } else {
        painter->fillRect(option.rect,Qt::white);
    }
    if (ComboBox->CurrentSel==index.row()*((QTableWidget *)ComboBox->view())->columnCount()+index.column()) {
        painter->setPen(QPen(Qt::red));
        painter->setBrush(QBrush(Qt::NoBrush));
        painter->drawRect(option.rect.x()+3,option.rect.y()+3,option.rect.width()-6-1,option.rect.height()-6-1);
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

QSize cThumbnailComboBoxItem::sizeHint(const QStyleOptionViewItem &/*option*/,const QModelIndex &/*index*/) const {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cThumbnailComboBoxItem::sizeHint");
    return QSize(IMAGEWIDTH+4,IMAGEHEIGHT+4);
}

//******************************************************************************************************************
// Custom Background ComboBox
//******************************************************************************************************************

cThumbnailComboBox::cThumbnailComboBox(QWidget *parent):QComboBox(parent) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cThumbnailComboBox::cThumbnailComboBox");
    ModelTable=NULL;
    CurrentSel=-1;
    STOPMAJ   =false;

    QTableWidget *Table=new QTableWidget();
    Table->horizontalHeader()->hide();
    Table->verticalHeader()->hide();
    for (int i=0;i<NBRCOLUMN;i++) {
        Table->insertColumn(0);
        Table->setColumnWidth(0,IMAGEWIDTH);
    }
    setModel(Table->model());
    setView(Table);
    ItemDelegate.ComboBox=this;
    setItemDelegate(&ItemDelegate);
    this->view()->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setIconSize(QSize(IMAGEWIDTH,IMAGEHEIGHT));
    connect(Table,SIGNAL(itemSelectionChanged()),this,SLOT(s_ItemSelectionChanged()));
}

//========================================================================================================================

void cThumbnailComboBox::PrepareTable(cModelList *Table) {
    ModelTable=Table;

    this->view()->setFixedWidth(IMAGEWIDTH*NBRCOLUMN+18+8);
    setIconSize(QSize(IMAGEWIDTH,IMAGEHEIGHT));
    setFixedSize(QSize(IMAGEWIDTH+18+8,IMAGEHEIGHT+4));
    for (int i=0;i<NBRCOLUMN;i++) ((QTableWidget *)view())->setColumnWidth(i,IMAGEWIDTH+4);

    int CurIndex  =((QTableWidget *)view())->currentRow()*NBRCOLUMN+((QTableWidget *)view())->currentColumn();
    while (count()>0) removeItem(count()-1);
    int NbrItem=ModelTable->List.count();
    int NbrRow=NbrItem/NBRCOLUMN;
    if (NbrRow*NBRCOLUMN<NbrItem) NbrRow++;
    for (int i=0;i<NbrRow;i++) {
        addItem(QIcon(QPixmap().fromImage(ModelTable->List[i*NBRCOLUMN].Thumbnail)),"");    //automaticaly do a Table->insertRow(Table->rowCount());
        ((QTableWidget *)view())->setRowHeight(((QTableWidget *)view())->rowCount()-1,IMAGEHEIGHT+4);
    }
    ((QTableWidget *)view())->setCurrentCell(CurIndex/NBRCOLUMN,CurIndex-(CurIndex/NBRCOLUMN)*NBRCOLUMN);
    setCurrentIndex(CurIndex/NBRCOLUMN);
}

//========================================================================================================================

void cThumbnailComboBox::hidePopup() {
    QComboBox::hidePopup();
    emit itemSelectionHaveChanged();
    MakeIcons();
    CurrentSel=((QTableWidget *)view())->currentRow()*NBRCOLUMN+((QTableWidget *)view())->currentColumn();
    if (CurrentSel>=ModelTable->List.count()) CurrentSel=ModelTable->List.count()-1;
}

//========================================================================================================================

void cThumbnailComboBox::keyReleaseEvent(QKeyEvent *event) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cThumbnailComboBox::keyReleaseEvent");
    int CurIndex=((QTableWidget *)view())->currentRow()*NBRCOLUMN+((QTableWidget *)view())->currentColumn();
    int aCurIndex=CurIndex;
    if ((event->key()==Qt::Key_Right)||(event->key()==Qt::Key_Down)) {
        if (CurIndex<ModelTable->List.count()-1) CurIndex++;
    } else if ((event->key()==Qt::Key_Left)||(event->key()==Qt::Key_Up)) {
        if (CurIndex>0) CurIndex--;
    } else QComboBox::keyReleaseEvent(event);
    if (aCurIndex!=CurIndex) {
        STOPMAJ=true;
        ((QTableWidget *)view())->setCurrentCell(CurIndex/NBRCOLUMN,CurIndex-(CurIndex/NBRCOLUMN)*NBRCOLUMN);
        setCurrentIndex(CurIndex/NBRCOLUMN);
        MakeIcons();
        STOPMAJ=false;
        emit itemSelectionHaveChanged();
    }
}

//========================================================================================================================

void cThumbnailComboBox::MakeIcons() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cThumbnailComboBox::MakeIcons");
    if (!ModelTable) return;
    int CurrentRow=currentIndex();
    if (CurrentRow<0) return;
    int CurrentCol=((QTableWidget *)view())->currentColumn();
    if (CurrentCol<0) CurrentCol=0;
    int CurIndex=CurrentRow*NBRCOLUMN+CurrentCol;
    if (CurIndex<ModelTable->List.count()) setItemIcon(CurrentRow,QIcon(QPixmap().fromImage(ModelTable->List[CurIndex].Thumbnail)));
}

//========================================================================================================================

void cThumbnailComboBox::s_ItemSelectionChanged() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cThumbnailComboBox::s_ItemSelectionChanged");
    STOPMAJ=true;
    setCurrentIndex(((QTableWidget *)view())->currentRow());
    MakeIcons();
    emit currentIndexChanged(((QTableWidget *)view())->currentRow()*NBRCOLUMN+((QTableWidget *)view())->currentColumn());
    STOPMAJ=false;
}

//========================================================================================================================

void cThumbnailComboBox::SetCurrentModel(QString ModelName) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCBrushComboBox::SetCurrentModel");
    if (STOPMAJ) return;
    CurrentSel=ModelTable->SearchModel(ModelName);
    ((QTableWidget *)view())->setCurrentCell(CurrentSel/NBRCOLUMN,CurrentSel-(CurrentSel/NBRCOLUMN)*NBRCOLUMN);
    setCurrentIndex(CurrentSel/NBRCOLUMN);
    MakeIcons();
}

//========================================================================================================================

QString cThumbnailComboBox::GetCurrentModel() {
    CurrentSel=((QTableWidget *)view())->currentRow()*NBRCOLUMN+((QTableWidget *)view())->currentColumn();
    if (CurrentSel>=ModelTable->List.count()) CurrentSel=ModelTable->List.count()-1;
    return ModelTable->List[CurrentSel].Name;
}
