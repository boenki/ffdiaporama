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
    StopMaj   =false;
    CurrentMap=NULL;
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
    ui->LocationTable->CurrentBrush=CurrentBrush;
}

//====================================================================================================================

void wgt_QGMapsMap::DoInitDialog() {
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
    connect(ui->LineColorCB,SIGNAL(currentIndexChanged(int)),this,SLOT(LineColorChanged(int)));
    connect(ui->MarkerColorCB,SIGNAL(currentIndexChanged(int)),this,SLOT(MarkerColorChanged(int)));
    connect(ui->LineColorCB,SIGNAL(PopupClosed(int)),this,SLOT(LineColorChanged(int)));
    connect(ui->MarkerColorCB,SIGNAL(PopupClosed(int)),this,SLOT(MarkerColorChanged(int)));
    connect(ui->TextColorCB,SIGNAL(PopupClosed(int)),this,SLOT(TextColorChanged(int)));
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
    emit DoRefreshImageObject();
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

    cLocation                   *CurLocation=(cLocation *)(GetCurLocationIndex()!=-1?CurrentMap->List[GetCurLocationIndex()]:NULL);
    cBrushDefinition::sMarker   *CurMarker  =(CurLocation?&CurrentBrush->Markers[GetCurLocationIndex()]:NULL);

    ui->MarkerSizeLabel->setEnabled(CurLocation);     ui->MarkerSizeCB->setEnabled(CurLocation);
    ui->PositionLabel->setEnabled(CurLocation);       ui->PositionCB->setEnabled(CurLocation);
    ui->DistanceLabel->setEnabled(CurLocation);       ui->DistanceCB->setEnabled(CurLocation);
    ui->LineWidthLabel->setEnabled(CurLocation);      ui->LineWidthSB->setEnabled(CurLocation);
    ui->LineColorLabel->setEnabled(CurLocation);      ui->LineColorCB->setEnabled(CurLocation);
    ui->EndPointLabel->setEnabled(CurLocation);       ui->EndPointCB->setEnabled(CurLocation);
    ui->MarkerColorLabel->setEnabled(CurLocation);    ui->MarkerColorCB->setEnabled(CurLocation);
    ui->TextColorLabel->setEnabled(CurLocation);      ui->TextColorCB->setEnabled(CurLocation);
    ui->VisibilityLabel->setEnabled(CurLocation);     ui->VisibilityCB->setEnabled(CurLocation);

    if (CurLocation) {
        ui->MarkerSizeCB->setCurrentIndex(CurLocation->Size);
        ui->PositionCB->setCurrentIndex(CurLocation->Position);
        ui->DistanceCB->setCurrentIndex(CurLocation->Distance);
        ui->LineWidthSB->setValue(CurLocation->LineWidth);
        ui->LineColorCB->SetCurrentColor(&CurLocation->LineColor);
        ui->EndPointCB->setCurrentIndex(CurLocation->EndPoint);
        ui->TextColorCB->SetCurrentColor(&CurMarker->TextColor);
        ui->MarkerColorCB->SetCurrentColor(&CurMarker->MarkerColor);
        ui->VisibilityCB->setCurrentIndex(CurMarker->Visibility);
    }

    ui->MapSizeCB->setEnabled(!CurrentMap->List.isEmpty());
    ui->EditLocationBT->setEnabled(CurLocation);
    ui->RemoveLocationBT->setEnabled(CurrentMap->List.count()>1);
    StopMaj=false;
}

//====================================================================================================================

void wgt_QGMapsMap::ResetDisplayMap() {
    // Compute marker size
    for (int i=0;i<CurrentMap->List.count();i++) ((cLocation *)CurrentMap->List[i])->ComputeMarkerSize(CurrentMap->GetCurrentImageSize());
    // Setup map
    int H=ui->Map->size().height();
    int W=int(double(16)*(double(H)/double(9)));
    if (W>ui->Map->size().width()) {
        W=ui->Map->size().width();
        H=int(double(9)*(double(W)/double(16)));
    }
    QImage Map=CurrentBrush->GetImageDiskBrush(QRectF(0,0,W,H),true,0,NULL,1,NULL);
    ui->Map->setPixmap(QPixmap::fromImage(Map));
    RefreshControls();
}

//====================================================================================================================

void wgt_QGMapsMap::UpdateDisplayMap() {
    if (StopMaj) return;
    RefreshControls();
    // Clear previous pending
    if (!CurrentMap->RequestList.isEmpty()) CurrentMap->RequestList.clear();
    // if at least one location, do a map generation
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
                     QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes)==QMessageBox::Yes)
        RequestGoogle();
}

void wgt_QGMapsMap::RequestGoogle() {
    DlgGMapsGeneration Dlg(CurrentMap,ParentDialog->ApplicationConfig,this);
    Dlg.InitDialog();
    Dlg.exec();
    ParentDialog->ApplicationConfig->ImagesCache.RemoveImageObject(CurrentMap->RessourceKey,-1);
    ResetDisplayMap();
    emit DoRefreshImageObject();
}

//====================================================================================================================

void wgt_QGMapsMap::MapTypeChanged(int newtype) {
    if (StopMaj) return;
    ParentDialog->AppendPartialUndo(DlgImageCorrection::UNDOACTION_GMAPSMAPPART,ui->MapTypeCB,true,this,true);
    CurrentMap->MapType=(cGMapsMap::GMapsMapType)newtype;
    UpdateDisplayMap();
}

//====================================================================================================================

void wgt_QGMapsMap::ImageSizeChanged(int newsize) {
    if (StopMaj) return;
    ParentDialog->AppendPartialUndo(DlgImageCorrection::UNDOACTION_GMAPSMAPPART,ui->ImageSizeCB,true,this,true);
    CurrentMap->ImageSize=(cGMapsMap::GMapsImageSize)newsize;
    UpdateDisplayMap();
}

//====================================================================================================================

void wgt_QGMapsMap::MapSizeChanged(int newsize) {
    if (StopMaj) return;
    ParentDialog->AppendPartialUndo(DlgImageCorrection::UNDOACTION_GMAPSMAPPART,ui->MapSizeCB,true,this,true);
    CurrentMap->ZoomLevel=ui->MapSizeCB->itemData(newsize).toInt();
    UpdateDisplayMap();
}

//====================================================================================================================

void wgt_QGMapsMap::AddGMapsLocation() {
    if (StopMaj) return;
    ParentDialog->AppendPartialUndo(DlgImageCorrection::UNDOACTION_GMAPSMAPPART,ui->LocationTable,true,this,true);
    cLocation           *Location=new cLocation(ParentDialog->ApplicationConfig);
    DlgGMapsLocation    Dlg(Location,ParentDialog->ApplicationConfig,this);
    Dlg.InitDialog();
    if (Dlg.exec()==0) {
        ui->LocationTable->setUpdatesEnabled(false);
        CurrentMap->List.append(Location);
        cBrushDefinition::sMarker Marker;
        Marker.MarkerColor="#ffffff";
        Marker.TextColor="#000000";
        Marker.Visibility =cBrushDefinition::sMarker::MARKERSHOW;

        // Append this marker to all shots
        QObject *Object =CurrentBrush->CompositionObject;   // cCompositionObject;
        int     IndexKey=(Object?((cCompositionObject *)Object)->IndexKey:-1);
        bool    Found   =false;
        if (IndexKey!=-1) {
            while ((Object)&&(Object->objectName()!="cDiaporamaObject")) Object=Object->parent();
            cDiaporamaObject *DiaporamaObject=(cDiaporamaObject *)Object;
            if (DiaporamaObject) for (int i=0;i<DiaporamaObject->List.count();i++) for (int j=0;j<DiaporamaObject->List[i]->ShotComposition.List.count();j++) {
                if (DiaporamaObject->List[i]->ShotComposition.List[j]->IndexKey==IndexKey) {
                    Found=true;
                    DiaporamaObject->List[i]->ShotComposition.List[j]->BackgroundBrush->Markers.append(Marker);
                }
            }
        }
        if (!Found) CurrentBrush->Markers.append(Marker);   // if not shots found then add it directly to CurrentBrush

        ui->LocationTable->setRowCount(CurrentMap->List.count());
        ui->LocationTable->setUpdatesEnabled(true);
        UpdateDisplayMap();
    } else {
        delete Location;
        ParentDialog->RemoveLastPartialUndo();
    }
}

//====================================================================================================================

void wgt_QGMapsMap::RemoveLocation() {
    if (StopMaj) return;
    int CurIndex=GetCurLocationIndex();
    if (CurIndex!=-1) {
        ParentDialog->AppendPartialUndo(DlgImageCorrection::UNDOACTION_GMAPSMAPPART,ui->LocationTable,true,this,false);
        ui->LocationTable->setUpdatesEnabled(false);
        ui->LocationTable->setRowCount(CurrentMap->List.count()-1);
        delete (cLocation *)CurrentMap->List.takeAt(CurIndex);

        // Remove this marker from all shots
        QObject *Object =CurrentBrush->CompositionObject;   // cCompositionObject;
        int     IndexKey=(Object?((cCompositionObject *)Object)->IndexKey:-1);
        bool    Found   =false;
        if (IndexKey!=-1) {
            while ((Object)&&(Object->objectName()!="cDiaporamaObject")) Object=Object->parent();
            cDiaporamaObject *DiaporamaObject=(cDiaporamaObject *)Object;
            if (DiaporamaObject) for (int i=0;i<DiaporamaObject->List.count();i++) for (int j=0;j<DiaporamaObject->List[i]->ShotComposition.List.count();j++) {
                if (DiaporamaObject->List[i]->ShotComposition.List[j]->IndexKey==IndexKey) {
                    Found=true;
                    DiaporamaObject->List[i]->ShotComposition.List[j]->BackgroundBrush->Markers.removeAt(CurIndex);
                }
            }
        }
        if (!Found) CurrentBrush->Markers.removeAt(CurIndex);   // if not shots found then remove it directly from CurrentBrush

        ui->LocationTable->setUpdatesEnabled(true);
        ResetDisplayMap();
    }
}

//====================================================================================================================

void wgt_QGMapsMap::EditLocation() {
    DoubleClickedLocation(QModelIndex());
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
        double GPSxOld=((cLocation *)CurrentMap->List.at(CurIndex))->GPS_cx;
        double GPSyOld=((cLocation *)CurrentMap->List.at(CurIndex))->GPS_cy;
        ParentDialog->AppendPartialUndo(DlgImageCorrection::UNDOACTION_GMAPSMAPPART,ui->LocationTable,true,this,true);
        DlgGMapsLocation Dlg((cLocation *)CurrentMap->List.at(CurIndex),ParentDialog->ApplicationConfig,this);
        Dlg.InitDialog();
        if (Dlg.exec()==0) {
            ui->LocationTable->setUpdatesEnabled(false);
            ui->LocationTable->setUpdatesEnabled(true);
            if ((GPSxOld!=((cLocation *)CurrentMap->List.at(CurIndex))->GPS_cx)||(GPSyOld!=((cLocation *)CurrentMap->List.at(CurIndex))->GPS_cy)) UpdateDisplayMap();
                else ResetDisplayMap(); // if same GPS position, only redraw markers
        } else ParentDialog->RemoveLastPartialUndo();
    }
}

//====================================================================================================================

void wgt_QGMapsMap::LineColorChanged(int) {
    if ((StopMaj)||(ui->LineColorCB->IsPopupOpen)) return;
    int Index=GetCurLocationIndex();
    if ((Index<0)||(Index>=CurrentMap->List.count())) return;
    ParentDialog->AppendPartialUndo(DlgImageCorrection::UNDOACTION_GMAPSMAPPART,ui->LineColorCB,false,this,false);
    ((cLocation *)CurrentMap->List[Index])->LineColor=ui->LineColorCB->GetCurrentColor();
    if (!ui->MarkerColorCB->IsPopupOpen) {
        StopMaj=true;
        ui->LineColorCB->SetCurrentColor(&((cLocation *)CurrentMap->List[Index])->LineColor);
        StopMaj=false;
        ResetDisplayMap();
        ui->LocationTable->setUpdatesEnabled(false);
        ui->LocationTable->setUpdatesEnabled(true);
    }
}

//====================================================================================================================

void wgt_QGMapsMap::MarkerColorChanged(int) {
    if (StopMaj) return;
    int Index=GetCurLocationIndex();
    if ((Index<0)||(Index>=CurrentMap->List.count())) return;
    ParentDialog->AppendPartialUndo(DlgImageCorrection::UNDOACTION_GMAPSMAPPART,ui->MarkerColorCB,false,this,false);
    CurrentBrush->Markers[Index].MarkerColor=ui->MarkerColorCB->GetCurrentColor();
    if (!ui->MarkerColorCB->IsPopupOpen) {
        StopMaj=true;
        ui->MarkerColorCB->SetCurrentColor(&CurrentBrush->Markers[Index].MarkerColor);
        StopMaj=false;
        ResetDisplayMap();
        ui->LocationTable->setUpdatesEnabled(false);
        ui->LocationTable->setUpdatesEnabled(true);
    }
}

//====================================================================================================================

void wgt_QGMapsMap::TextColorChanged(int) {
    if (StopMaj) return;
    int Index=GetCurLocationIndex();
    if ((Index<0)||(Index>=CurrentMap->List.count())) return;
    ParentDialog->AppendPartialUndo(DlgImageCorrection::UNDOACTION_GMAPSMAPPART,ui->TextColorCB,false,this,false);
    CurrentBrush->Markers[Index].TextColor=ui->TextColorCB->GetCurrentColor();
    if (!ui->MarkerColorCB->IsPopupOpen) {
        StopMaj=true;
        ui->TextColorCB->SetCurrentColor(&CurrentBrush->Markers[Index].TextColor);
        StopMaj=false;
        ResetDisplayMap();
        ui->LocationTable->setUpdatesEnabled(false);
        ui->LocationTable->setUpdatesEnabled(true);
    }
}
