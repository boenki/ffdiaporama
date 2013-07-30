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

#define NBRCOLUMN    1
#define DECAL       10

cThumbnailComboBoxItem::cThumbnailComboBoxItem(QObject *parent):QStyledItemDelegate(parent) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cThumbnailComboBoxItem::cThumbnailComboBoxItem");
}

//========================================================================================================================
void cThumbnailComboBoxItem::paint(QPainter *painter,const QStyleOptionViewItem &option,const QModelIndex &index) const {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cThumbnailComboBoxItem::paint");
    int CurIndex=index.row()*NBRCOLUMN+index.column();

    if (option.state & QStyle::State_Selected)  painter->fillRect(option.rect,Qt::blue);
        else                                    painter->fillRect(option.rect,Qt::white);

    if ((CurIndex>=0)&&(CurIndex<ComboBox->ModelTable->List.count()))
        painter->drawImage(option.rect.left()+DECAL,option.rect.top()+DECAL,
                           ComboBox->ModelTable->List[CurIndex].PrepareImage(0,ComboBox->ModelTable->List[CurIndex].Name=="*"?ComboBox->ProjectThumbnail:NULL));

    if (CurIndex==ComboBox->CurrentSel) {
        painter->setPen(QPen(Qt::red));
        painter->setBrush(QBrush(Qt::NoBrush));
        painter->drawRect(option.rect.x()+3,option.rect.y()+3,option.rect.width()-6-1,option.rect.height()-6-1);
    }
}

//========================================================================================================================

QSize cThumbnailComboBoxItem::sizeHint(const QStyleOptionViewItem &/*option*/,const QModelIndex &/*index*/) const {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cThumbnailComboBoxItem::sizeHint");
    return QSize(THUMB_THUMBWITH+DECAL*2,THUMB_THUMBHEIGHT+DECAL*2);
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
    for (int i=0;i<NBRCOLUMN;i++) Table->insertColumn(0);
    setModel(Table->model());
    setView(Table);
    ItemDelegate.ComboBox=this;
    setItemDelegate(&ItemDelegate);
    this->view()->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    connect(Table,SIGNAL(itemSelectionChanged()),this,SLOT(s_ItemSelectionChanged()));
}

//========================================================================================================================

void cThumbnailComboBox::PrepareTable(bool AllowCustomized,cModelList *Table) {
    ModelTable=Table;
    this->view()->setFixedWidth((THUMB_THUMBWITH+DECAL*2)*NBRCOLUMN+18);
    setIconSize(QSize(THUMB_THUMBWITH,THUMB_THUMBHEIGHT));
    setFixedSize(QSize((THUMB_THUMBWITH+DECAL*2)*NBRCOLUMN+18+4,THUMB_THUMBHEIGHT+DECAL*2+4));

    int CurIndex  =((QTableWidget *)view())->currentRow()*NBRCOLUMN+((QTableWidget *)view())->currentColumn();
    while (count()>0) removeItem(count()-1);
    int NbrItem=ModelTable->List.count();
    if ((!AllowCustomized)&&(ModelTable->List[ModelTable->List.count()-1].Name=="*")) NbrItem--;
    int NbrRow=NbrItem/NBRCOLUMN;
    if (NbrRow*NBRCOLUMN<NbrItem) NbrRow++;

    for (int i=0;i<NBRCOLUMN;i++) ((QTableWidget *)view())->setColumnWidth(i,THUMB_THUMBWITH+DECAL*2);

    for (int i=0;i<NbrRow;i++) {
        addItem(QIcon(QPixmap().fromImage(ModelTable->List[i*NBRCOLUMN].PrepareImage(0,ModelTable->List[i*NBRCOLUMN].Name=="*"?ProjectThumbnail:NULL))),"");
        ((QTableWidget *)view())->setRowHeight(i,THUMB_THUMBHEIGHT+DECAL*2);
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
    if (CurIndex<ModelTable->List.count()) setItemIcon(CurrentRow,QIcon(QPixmap().fromImage(ModelTable->List[CurIndex].PrepareImage(0,ModelTable->List[CurIndex].Name=="*"?ProjectThumbnail:NULL))));
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
    if ((CurrentSel>=0)&&(CurrentSel<ModelTable->List.count())) return ModelTable->List[CurrentSel].Name;
        else return "";
}
