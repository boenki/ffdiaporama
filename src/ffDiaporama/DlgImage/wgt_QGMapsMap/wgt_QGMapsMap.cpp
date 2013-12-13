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

#include "wgt_QGMapsMap.h"
#include "ui_wgt_QGMapsMap.h"
#include "DlgImage/DlgImageCorrection.h"
#include "DlgGMapsLocation/DlgGMapsLocation.h"
#include "DlgGMapsLocation/DlgGMapsGeneration.h"

wgt_QGMapsMap::wgt_QGMapsMap(QWidget *parent):QWidget(parent),ui(new Ui::wgt_QGMapsMap) {
    ui->setupUi(this);
    StopMaj         =false;
    CurrentMap      =NULL;
    DisplayMap      =NULL;
}

//====================================================================================================================

wgt_QGMapsMap::~wgt_QGMapsMap() {
    delete ui;
}

//====================================================================================================================

void wgt_QGMapsMap::DoInitWidget(QCustomDialog *ParentDialog,cBrushDefinition *CurrentBrush) {
    this->ParentDialog=ParentDialog;
    this->CurrentBrush=CurrentBrush;
    CurrentMap=(cGMapsMap *)CurrentBrush->MediaObject;
    ui->LocationTable->CurrentMap=CurrentMap;
}

//====================================================================================================================

void wgt_QGMapsMap::DoInitDialog() {
    // Set Map to Map label
    DisplayMap=CurrentMap->ImageAt(false);

    // fill combo box
    ui->MapTypeCB->addItems(CurrentMap->GetMapTypeNames());
    ui->ImageSizeCB->addItems(CurrentMap->GetImageSizeNames());

    // define handler
    connect(ui->AddGMapsLocationBT,SIGNAL(pressed()),SLOT(AddGMapsLocation()));
    connect(ui->EditLocationBT,SIGNAL(pressed()),SLOT(EditLocation()));
    connect(ui->RemoveLocationBT,SIGNAL(pressed()),SLOT(RemoveLocation()));
    connect(ui->MapTypeCB,SIGNAL(currentIndexChanged(int)),SLOT(MapTypeChanged(int)));
    connect(ui->ImageSizeCB,SIGNAL(currentIndexChanged(int)),SLOT(ImageSizeChanged(int)));
    connect(ui->MapSizeCB,SIGNAL(currentIndexChanged(int)),SLOT(MapSizeChanged(int)));
    connect(ui->LocationTable,SIGNAL(doubleClicked(QModelIndex)),SLOT(DoubleClickedLocation(QModelIndex)));
    connect(ui->LocationTable,SIGNAL(itemSelectionChanged()),SLOT(LocationSelectionChanged()));

    if (!CurrentMap->RequestList.isEmpty()) QTimer::singleShot(LATENCY,this,SLOT(RestartRequest()));
}

//====================================================================================================================

void wgt_QGMapsMap::resizeEvent(QResizeEvent *ev) {
    QWidget::resizeEvent(ev);
    ResetDisplayMap();
}

//====================================================================================================================

void wgt_QGMapsMap::WinFocus() {
    ResetDisplayMap();
}

void wgt_QGMapsMap::LostFocus() {
}

bool wgt_QGMapsMap::DoAccept() {
    return true;
}

//====================================================================================================================

void wgt_QGMapsMap::LocationSelectionChanged() {
    RefreshControls();
}

//====================================================================================================================

void wgt_QGMapsMap::RefreshControls() {
    if (StopMaj) return;
    StopMaj=true;
    ui->MapTypeCB->setCurrentIndex(CurrentMap->MapType);
    ui->ImageSizeCB->setCurrentIndex(CurrentMap->ImageSize);
    ui->LocationTable->setUpdatesEnabled(false);
    ui->LocationTable->setRowCount(CurrentMap->List.count());
    ui->LocationTable->setUpdatesEnabled(true);

    // Fill Map Size CB
    UpdateMapSizes();

    ui->MapSizeCB->setEnabled(!CurrentMap->List.isEmpty());
    ui->EditLocationBT->setEnabled(GetCurLocationIndex()!=-1);
    ui->RemoveLocationBT->setEnabled(CurrentMap->List.count()>1);
    StopMaj=false;
}

//====================================================================================================================

void wgt_QGMapsMap::ResetDisplayMap() {
    if (DisplayMap) {
        QImage Adjusted;
        QImage Disp(ui->Map->size(),QImage::Format_ARGB32_Premultiplied);
        if (DisplayMap->width() >ui->Map->width()) Adjusted=DisplayMap->scaledToWidth(ui->Map->width());
        if ((!Adjusted.isNull())&&(Adjusted.height()>ui->Map->height())) Adjusted=DisplayMap->scaledToHeight(ui->Map->height());
                else Adjusted=DisplayMap->scaledToHeight(ui->Map->height());

        QPainter P;
        P.begin(&Disp);
        P.fillRect(QRectF(0,0,Disp.width(),Disp.height()),Transparent);
        P.drawImage(QRectF((Disp.width()-Adjusted.width())/2,(Disp.height()-Adjusted.height())/2,Adjusted.width(),Adjusted.height()),Adjusted);
        P.end();

        ui->Map->setPixmap(QPixmap::fromImage(Disp));
    }
}

//====================================================================================================================

void wgt_QGMapsMap::UpdateDisplayMap() {
    if (StopMaj) return;
    RefreshControls();
    // start request process
    if (!CurrentMap->List.isEmpty()) RequestGoogle();
}

//====================================================================================================================

void wgt_QGMapsMap::UpdateMapSizes() {
    ui->MapSizeCB->clear();
    if (CurrentMap->List.count()>0) {
        QStringList AllSize     =CurrentMap->GetMapSizesPerZoomLevel();
        int         MinZoomLevel=-1;
        int         MaxZoomLevel=-1;
        for (int i=0;i<AllSize.count();i++) if (!AllSize[i].isNull()) {
            if (MinZoomLevel==-1) MinZoomLevel=i;
            MaxZoomLevel=i;
            ui->MapSizeCB->addItem(AllSize[i],QVariant(i));
        }
        if (CurrentMap->ZoomLevel>MaxZoomLevel) CurrentMap->ZoomLevel=MaxZoomLevel;
        if (CurrentMap->ZoomLevel<MinZoomLevel) CurrentMap->ZoomLevel=MinZoomLevel;
        for (int i=0;i<ui->MapSizeCB->count();i++) if (CurrentMap->ZoomLevel==ui->MapSizeCB->itemData(i)) ui->MapSizeCB->setCurrentIndex(i);
        ui->MapSizeCB->setEnabled(true);
    }
}

//====================================================================================================================

void wgt_QGMapsMap::RestartRequest() {
    if (CustomMessageBox(this,QMessageBox::Question,APPLICATION_NAME,
                     QApplication::translate("DlgGMapsLocation","The map has not been fully generated.\nDo you want to resume the generation now?"),
                     QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes)==QMessageBox::Yes) RequestGoogle();
}

void wgt_QGMapsMap::RequestGoogle() {
    DlgGMapsGeneration Dlg(DisplayMap,CurrentMap,ParentDialog->ApplicationConfig,this);
    Dlg.DoInitDialog();
    ResetDisplayMap();  // update display
    Dlg.exec();
    ResetDisplayMap();
    emit DoRefreshImageObject();
}

//====================================================================================================================

void wgt_QGMapsMap::MapTypeChanged(int newtype) {
    if (StopMaj) return;
    ParentDialog->AppendPartialUndo(DlgImageCorrection::UNDOACTION_GMAPSMAPPART,ui->MapTypeCB,true,this);
    CurrentMap->MapType=(cGMapsMap::GMapsMapType)newtype;
    UpdateDisplayMap();
}

//====================================================================================================================

void wgt_QGMapsMap::ImageSizeChanged(int newsize) {
    if (StopMaj) return;
    ParentDialog->AppendPartialUndo(DlgImageCorrection::UNDOACTION_GMAPSMAPPART,ui->ImageSizeCB,true,this);
    CurrentMap->ImageSize=(cGMapsMap::GMapsImageSize)newsize;
    UpdateDisplayMap();
}

//====================================================================================================================

void wgt_QGMapsMap::MapSizeChanged(int newsize) {
    if (StopMaj) return;
    ParentDialog->AppendPartialUndo(DlgImageCorrection::UNDOACTION_GMAPSMAPPART,ui->MapSizeCB,true,this);
    CurrentMap->ZoomLevel=ui->MapSizeCB->itemData(newsize).toInt();
    UpdateDisplayMap();
}

//====================================================================================================================

void wgt_QGMapsMap::AddGMapsLocation() {
    if (StopMaj) return;
    ParentDialog->AppendPartialUndo(DlgImageCorrection::UNDOACTION_GMAPSMAPPART,ui->LocationTable,true,this);
    cLocation           *Location=new cLocation(ParentDialog->ApplicationConfig);
    DlgGMapsLocation    Dlg(Location,ParentDialog->ApplicationConfig,this);
    Dlg.InitDialog();
    if (Dlg.exec()==0) {
        ui->LocationTable->setUpdatesEnabled(false);
        CurrentMap->List.append(Location);
        ui->LocationTable->setRowCount(CurrentMap->List.count());
        ui->LocationTable->setUpdatesEnabled(true);
        UpdateDisplayMap();
    } else {
        delete Location;
        ParentDialog->RemoveLastPartialUndo();
    }
}

//====================================================================================================================

void wgt_QGMapsMap::EditLocation() {
    DoubleClickedLocation(QModelIndex());
}

//====================================================================================================================

void wgt_QGMapsMap::RemoveLocation() {
    if (StopMaj) return;
    int CurIndex=GetCurLocationIndex();
    if (CurIndex!=-1) {
        ParentDialog->AppendPartialUndo(DlgImageCorrection::UNDOACTION_GMAPSMAPPART,ui->LocationTable,true,this);
        delete (cLocation *)CurrentMap->List.takeAt(CurIndex);
        RefreshControls();
    }
}

//====================================================================================================================

int wgt_QGMapsMap::GetCurLocationIndex() {
    QModelIndexList SelList=ui->LocationTable->selectionModel()->selectedIndexes();
    int             CurIndex=-1;
    if (!SelList.isEmpty()) CurIndex=SelList.at(0).row();
    if ((CurIndex<0)||(CurIndex>=CurrentMap->List.count())) return -1;
    return CurIndex;
}

//====================================================================================================================

void wgt_QGMapsMap::DoubleClickedLocation(QModelIndex) {
    if (StopMaj) return;
    int CurIndex=GetCurLocationIndex();
    if (CurIndex!=-1) {
        ParentDialog->AppendPartialUndo(DlgImageCorrection::UNDOACTION_GMAPSMAPPART,ui->LocationTable,true,this);
        DlgGMapsLocation Dlg((cLocation *)CurrentMap->List.at(CurIndex),ParentDialog->ApplicationConfig,this);
        Dlg.InitDialog();
        if (Dlg.exec()==0) {
            ui->LocationTable->setUpdatesEnabled(false);
            ui->LocationTable->setUpdatesEnabled(true);
            UpdateDisplayMap();
        } else ParentDialog->RemoveLastPartialUndo();
    }
}
