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

#include "wgt_QBackgroundDefWidget.h"
#include "wgt_QCustomThumbnails.h"
#include "wgt_QCustomScene.h"
#include "DlgImageProperties.h"
#include "ui_DlgImageProperties.h"

//======================================
// Specific defines for this dialog box
//======================================
#define PAGE_GLOBALFILTER           0
#define PAGE_FRAMING                1

#define SUBPAGE_SHOTFRAMING         0
#define SUBPAGE_SHOTCOMPOSITION     1

//---------------------------------------------------------------------------------------------------------------------------------------------------------------
// Dialog box
//---------------------------------------------------------------------------------------------------------------------------------------------------------------

DlgImageProperties::DlgImageProperties(cDiaporamaObject *DiaporamaObject,QWidget *parent) : QDialog(parent), ui(new Ui::DlgImageProperties) {
    ui->setupUi(this);
    this->DiaporamaObject   = DiaporamaObject;

    // Save object before modification for cancel button
    Undo=new QDomDocument(APPLICATION_NAME);
    QDomElement root=Undo->createElement("UNDO-DLG");       // Create xml document and root
    DiaporamaObject->SaveToXML(root,"UNDO-DLG-OBJECT","");  // Save object
    Undo->appendChild(root);                                // Add object to xml document

    // Init embeded widgets
    ui->CustomScene->SetDiaporamaShot(&DiaporamaObject->List[0].FilterCorrection);
    ui->ImageFilterTransform->SetFilter(&DiaporamaObject->FilterTransform,DiaporamaObject);

    IsTabCanvasInit         =false;                 // Flag for Tab Canvas
    IsTabImageInit          =false;                 // Flag for Tab Image
    IsTabAnimationInit      =false;                 // Flag for Tab Animation
    IsTabCompositionInit    =false;                 // Flag for Tab Composition

    // Define handler
    connect(ui->CloseBT,SIGNAL(clicked()),this,SLOT(reject()));
    connect(ui->OKBT,SIGNAL(clicked()),this,SLOT(accept()));
    connect(ui->HelpBT,SIGNAL(clicked()),this,SLOT(Help()));

    // Select first tab to display
    if (DiaporamaObject->TypeObject==DIAPORAMAOBJECTTYPE_EMPTY) {
        ui->tabWidget->setTabEnabled(PAGE_GLOBALFILTER,false);          // disable global filter tab
        ui->tabWidget->setCurrentIndex(PAGE_FRAMING);
    } else {
        ui->tabWidget->setCurrentIndex(PAGE_FRAMING);
        ui->ShotTabWidget->setCurrentIndex(0);
    }
    SetTabAnimation();
    connect(ui->tabWidget,SIGNAL(currentChanged(int)),this,SLOT(s_TabCurrentChanged(int)));
}

//====================================================================================================================

DlgImageProperties::~DlgImageProperties() {
    delete ui;
    delete Undo;
}

//====================================================================================================================

void DlgImageProperties::Help() {
    QDesktopServices::openUrl(QUrl(QString("http://ffdiaporama.tuxfamily.org/")+CurrentLanguage+QString("/HelpImageProperties.php")));
}

//====================================================================================================================

void DlgImageProperties::SetSavedWindowGeometry() {
    DiaporamaObject->Parent->ApplicationConfig->DlgImagePropertiesWSP->ApplyToWindow(this);
}

//====================================================================================================================

void DlgImageProperties::showEvent(QShowEvent *ev) {
    QDialog::showEvent(ev);
    QTimer::singleShot(0,this,SLOT(SetSavedWindowGeometry()));
}

//====================================================================================================================

void DlgImageProperties::reject() {
    // Save Window size and position
    DiaporamaObject->Parent->ApplicationConfig->DlgImagePropertiesWSP->SaveWindowState(this);
    QDomElement root=Undo->documentElement();
    if (root.tagName()=="UNDO-DLG") DiaporamaObject->LoadFromXML(root,"UNDO-DLG-OBJECT","");
    done(1);
}

//====================================================================================================================

void DlgImageProperties::accept() {
    // Save Window size and position
    DiaporamaObject->Parent->ApplicationConfig->DlgImagePropertiesWSP->SaveWindowState(this);
    // Close the box
    done(0);
}

//====================================================================================================================

void DlgImageProperties::s_TabCurrentChanged(int Page) {
    switch (Page) {
        case PAGE_FRAMING     : SetTabAnimation(); break;
    }
}

//****************************************************************************************************************************
//
//
//
// Framing/Animation Tab
//
//
//
//****************************************************************************************************************************

void DlgImageProperties::SetTabAnimation() {
    if (!IsTabAnimationInit) {
        ui->StaticCustomEd->setMinimum(0);
        ui->StaticCustomEd->setMaximum(99);
        ui->MobilCustomEd->setMinimum(0);
        ui->MobilCustomEd->setMaximum(99);

        // Fill the TableSeq
        for (int i=0;i<DiaporamaObject->List.count();i++) {
            ui->TableSeq->insertRow(i);
            wgt_QCustomThumbnails *Object=new wgt_QCustomThumbnails(ui->TableSeq,THUMBNAILTYPE_IMAGESEQUENCE);
            ui->TableSeq->setCellWidget(i,0,Object);
            ui->TableSeq->setRowHeight(i,DiaporamaObject->Parent->GetHeightForWidth(ui->TableSeq->columnWidth(0)));
        }

        // Init embeded widgets
//          ui->ImageFilters->SetDiaporamaObject(DiaporamaObject);
        ui->CompositionShotWidget->SetCompositionObject(&DiaporamaObject->List[0].ShotComposition,NULL);

        // Connect signals
        connect(ui->TableSeq,SIGNAL(itemSelectionChanged()),this,SLOT(s_ItemSelectionChanged()));

        connect(ui->StaticSetCustomBt,SIGNAL(clicked()),this,SLOT(s_StaticSetCustom()));
        connect(ui->StaticCustomEd,SIGNAL(valueChanged(int)),this,SLOT(s_DefineCustom(int)));
        connect(ui->MobilSetCustomBt,SIGNAL(clicked()),this,SLOT(s_MobilSetCustom()));
        connect(ui->MobilCustomEd,SIGNAL(valueChanged(int)),this,SLOT(s_MobilCustom(int)));
        connect(ui->addSequenceBT,SIGNAL(clicked()),this,SLOT(s_addSequence()));
        connect(ui->removeSequenceBT,SIGNAL(clicked()),this,SLOT(s_removeSequence()));
        connect(ui->UpSequenceBT,SIGNAL(clicked()),this,SLOT(s_UpSequence()));
        connect(ui->DownSequenceBT,SIGNAL(clicked()),this,SLOT(s_DownSequence()));
        connect(ui->CustomScene,SIGNAL(ModifyDataSignal()),this,SLOT(s_CustomSceneModifyFlag()));
        connect(ui->CompositionShotWidget,SIGNAL(NeedRefreshBackgroundImage()),this,SLOT(s_CompositionInShotNeedRefreshBackgroundImage()));
        connect(ui->CompositionShotWidget,SIGNAL(BackgroundImageUpdated()),this,SLOT(s_BackgroundImageUpdated()));
        connect(ui->ShotTabWidget,SIGNAL(currentChanged(int)),this,SLOT(s_ShotTabWidgetCurrentChanged(int)));

        // Set current cell to finish dialog initialisation
        ui->TableSeq->setCurrentCell(0,0);

        IsTabAnimationInit=true;
    } else {
        ui->CustomScene->RefreshWidget();
        ui->CompositionShotWidget->SetCompositionObject(&DiaporamaObject->List[0].ShotComposition,NULL);
    }
}

//====================================================================================================================

void DlgImageProperties::RefreshControls() {
    // Ensure Current contain index of currented selected sequence
    int Current=ui->TableSeq->currentRow();
    if ((Current<0)||(Current>=DiaporamaObject->List.count())) return;

    //--------------------------------------------------------------------
    // Update controls
    //--------------------------------------------------------------------
    ui->StaticSetCustomBt->setChecked(DiaporamaObject->List[Current].DefaultStaticDuration);
    ui->StaticCustomEd->setValue(DiaporamaObject->List[Current].StaticDuration/1000);
    if (DiaporamaObject->List[Current].DefaultStaticDuration) {
        ui->StaticCustomEd->setEnabled(false);
        ui->StaticCustomEd->setVisible(false);
        ui->StaticSpacer->setVisible(true);
    } else {
        ui->StaticCustomEd->setEnabled(true);
        ui->StaticCustomEd->setVisible(true);
        ui->StaticSpacer->setVisible(false);
    }

    int  AddingDuration=0;
    if (Current==(DiaporamaObject->List.count()-1)) {   // If it's the last shot
        int TotalDuration  =DiaporamaObject->GetDuration();
        int Duration=0;
        for (int i=0;i<DiaporamaObject->List.count();i++) Duration=Duration+(i>0?DiaporamaObject->List[i].GetMobilDuration():0)+DiaporamaObject->List[i].GetStaticDuration();
        if (Duration<TotalDuration) AddingDuration=TotalDuration-Duration;
    }
    if (AddingDuration==0) {
        if (DiaporamaObject->List[Current].DefaultStaticDuration) {
            int Default=((DiaporamaObject->List.count()>1?DiaporamaObject->Parent->FixedDuration:DiaporamaObject->Parent->NoShotDuration)/1000);
            ui->StaticDefault->setText(QString(QCoreApplication::translate("DlgImageProperties","Default project value=%1 sec")).arg(Default));
        } else ui->StaticDefault->setText(QCoreApplication::translate("DlgImageProperties","sec"));
    } else {
        ui->StaticDefault->setText(QString(QCoreApplication::translate("DlgImageProperties","Lengthened to %1 sec to allow transitions")).arg((DiaporamaObject->List[Current].GetStaticDuration()+AddingDuration)/1000));
    }

    ui->MobilSetCustomBt->setChecked(DiaporamaObject->List[Current].DefaultMobilDuration);
    ui->MobilCustomEd->setValue(DiaporamaObject->List[Current].MobilDuration/1000);

    if (Current>0) {
        ui->MobilSetCustomBt->setVisible(true);
        if (DiaporamaObject->List[Current].DefaultMobilDuration) {
            ui->MobilCustomEd->setEnabled(false);
            ui->MobilCustomEd->setVisible(false);
            ui->MobilSpacer->setVisible(true);
            ui->MobilDefault->setText(QString(QCoreApplication::translate("DlgImageProperties","Default project value=%1 sec")).arg(DiaporamaObject->Parent->MobilDuration/1000));
        } else {
            ui->MobilCustomEd->setEnabled(true);
            ui->MobilCustomEd->setVisible(true);
            ui->MobilSpacer->setVisible(false);
            ui->MobilDefault->setText(QCoreApplication::translate("DlgImageProperties","sec"));
        }
    } else {
        ui->MobilDefault->setText("");
        ui->MobilSetCustomBt->setVisible(false);
        ui->MobilCustomEd->setVisible(false);
    }

    ui->removeSequenceBT->setEnabled(ui->TableSeq->rowCount()>1);
    ui->UpSequenceBT->setEnabled((Current>0)&&(ui->TableSeq->rowCount()>1));
    ui->DownSequenceBT->setEnabled(Current<ui->TableSeq->rowCount()-1);

    ui->CustomScene->RefreshControls();

    // Refresh thumbnail
    ui->TableSeq->cellWidget(Current,0)->repaint();
}

//====================================================================================================================

void DlgImageProperties::s_CustomSceneModifyFlag() {
    int Current=ui->TableSeq->currentRow();
    if ((Current<0)||(Current>=DiaporamaObject->List.count())) return;
    if (ui->TableSeq->cellWidget(Current,0)!=NULL) ui->TableSeq->cellWidget(Current,0)->repaint();
}

//====================================================================================================================

void DlgImageProperties::s_ItemSelectionChanged() {
    int Current=ui->TableSeq->currentRow();
    if ((Current<0)||(Current>=DiaporamaObject->List.count())) return;
    // Refresh embeded widget
    ui->CustomScene->SetDiaporamaShot(&DiaporamaObject->List[Current].FilterCorrection);
    ui->CustomScene->RefreshWidget();
    ui->CompositionShotWidget->SetCompositionObject(&DiaporamaObject->List[Current].ShotComposition,NULL);
    if (ui->ShotTabWidget->currentIndex()==SUBPAGE_SHOTCOMPOSITION) s_CompositionInShotNeedRefreshBackgroundImage();
    RefreshControls();
}

//====================================================================================================================

void DlgImageProperties::s_StaticSetCustom() {
    int Current=ui->TableSeq->currentRow();
    if ((Current<0)||(Current>=DiaporamaObject->List.count())) return;
    DiaporamaObject->List[Current].DefaultStaticDuration=ui->StaticSetCustomBt->isChecked();
    ui->StaticCustomEd->setEnabled(!DiaporamaObject->List[Current].DefaultStaticDuration);
    RefreshControls();
}

//====================================================================================================================

void DlgImageProperties::s_DefineCustom(int Value) {
    int Current=ui->TableSeq->currentRow();
    if ((Current<0)||(Current>=DiaporamaObject->List.count())) return;
    DiaporamaObject->List[Current].StaticDuration=Value*1000;
    RefreshControls();
}

//====================================================================================================================

void DlgImageProperties::s_MobilSetCustom() {
    int Current=ui->TableSeq->currentRow();
    if ((Current<0)||(Current>=DiaporamaObject->List.count())) return;
    DiaporamaObject->List[Current].DefaultMobilDuration=ui->MobilSetCustomBt->isChecked();
    ui->MobilCustomEd->setEnabled(DiaporamaObject->List[Current].DefaultMobilDuration);
    RefreshControls();
}

//====================================================================================================================

void DlgImageProperties::s_MobilCustom(int Value) {
    int Current=ui->TableSeq->currentRow();
    if ((Current<0)||(Current>=DiaporamaObject->List.count())) return;
    DiaporamaObject->List[Current].MobilDuration=Value*1000;
    RefreshControls();
}

//====================================================================================================================

void DlgImageProperties::s_addSequence() {
    int Current=ui->TableSeq->currentRow();
    if ((Current<0)||(Current>=DiaporamaObject->List.count())) return;

    cDiaporamaShot *imagesequence   =new cDiaporamaShot(DiaporamaObject);
    imagesequence->FilterCorrection =DiaporamaObject->List[Current].FilterCorrection;

    DiaporamaObject->List.append(*imagesequence);
    ui->TableSeq->insertRow(ui->TableSeq->rowCount());
    wgt_QCustomThumbnails *Object=new wgt_QCustomThumbnails(ui->TableSeq,THUMBNAILTYPE_IMAGESEQUENCE);
    ui->TableSeq->setCellWidget(ui->TableSeq->rowCount()-1,0,Object);
    ui->TableSeq->setRowHeight(ui->TableSeq->rowCount()-1,DiaporamaObject->Parent->GetHeightForWidth(ui->TableSeq->columnWidth(0)));
    ui->TableSeq->setCurrentCell(ui->TableSeq->rowCount()-1,0);
    ui->CustomScene->RefreshWidget();
    ui->CompositionShotWidget->SetCompositionObject(&DiaporamaObject->List[ui->TableSeq->currentRow()].ShotComposition,NULL);
    RefreshControls();
}

//====================================================================================================================

void DlgImageProperties::s_removeSequence() {
    int Current=ui->TableSeq->currentRow();
    if ((Current<0)||(Current>=DiaporamaObject->List.count())) return;
    if (DiaporamaObject->List.count()<2) return;
    DiaporamaObject->List.removeAt(Current);
    ui->TableSeq->setUpdatesEnabled(false);
    ui->TableSeq->removeRow(Current);
    ui->TableSeq->setUpdatesEnabled(true);
    ui->CustomScene->RefreshControls();
    ui->CompositionShotWidget->SetCompositionObject(&DiaporamaObject->List[ui->TableSeq->currentRow()].ShotComposition,NULL);
    RefreshControls();
}

//====================================================================================================================

void DlgImageProperties::s_UpSequence() {
    int Current=ui->TableSeq->currentRow();
    if ((Current<1)||(Current>=DiaporamaObject->List.count())) return;
    DiaporamaObject->List.swap(Current,Current-1);
    ui->TableSeq->setCurrentCell(Current-1,0);
    ui->CustomScene->RefreshControls();
}

//====================================================================================================================

void DlgImageProperties::s_DownSequence() {
    int Current=ui->TableSeq->currentRow();
    if ((Current<0)||(Current>=DiaporamaObject->List.count()-1)) return;
    DiaporamaObject->List.swap(Current,Current+1);
    ui->TableSeq->setCurrentCell(Current+1,0);
    ui->CustomScene->RefreshControls();
}

//====================================================================================================================

void DlgImageProperties::s_CompositionInShotNeedRefreshBackgroundImage() {
    int Current=ui->TableSeq->currentRow();
    if ((Current<0)||(Current>=DiaporamaObject->List.count())) return;
    QSize SceneboxSize=ui->CompositionShotWidget->GetSceneBoxSize();
    if (DiaporamaObject->Parent->GetHeightForWidth(SceneboxSize.width())<SceneboxSize.height()) SceneboxSize=QSize(DiaporamaObject->Parent->GetWidthForHeight(SceneboxSize.height()),SceneboxSize.height());
        else SceneboxSize=QSize(SceneboxSize.width(),DiaporamaObject->Parent->GetHeightForWidth(SceneboxSize.width()));
    // Set image
    QImage *Image;
    double   Ratio=1;
    if (DiaporamaObject->TypeObject==DIAPORAMAOBJECTTYPE_EMPTY) {
        QPainter Painter;
        double w,h;
        w=SceneboxSize.width();
        h=DiaporamaObject->Parent->GetHeightForWidth(w);
        if (h<SceneboxSize.height()) {
            h=SceneboxSize.height();
            w=DiaporamaObject->Parent->GetWidthForHeight(h);
        }
        Image=new QImage(SceneboxSize.width(),SceneboxSize.height(),QImage::Format_ARGB32_Premultiplied);
        Painter.begin(Image);
        DiaporamaObject->Parent->PrepareBackground(DiaporamaObject->Parent->GetObjectIndex(DiaporamaObject),SceneboxSize.width(),SceneboxSize.height(),&Painter,0,0,false);
        Painter.end();
        Ratio=w/h;
    } else {
        int Position = 0;
        for (int i=0;i<Current;i++) Position=Position+(i>0?DiaporamaObject->List[i].GetMobilDuration():0)+DiaporamaObject->List[i].GetStaticDuration();
        Position=Position+(Current>0?DiaporamaObject->List[Current].GetMobilDuration():0);
        Image=DiaporamaObject->CanvasImageAt(SceneboxSize.width(),SceneboxSize.height(),Position,NULL,0,0,NULL,NULL,false,true,true,NULL);
    }
    ui->CompositionShotWidget->SetCompositionObject(&DiaporamaObject->List[Current].ShotComposition,new QPixmap(QPixmap().fromImage(*Image)));
    delete Image;
}

//====================================================================================================================

void DlgImageProperties::s_BackgroundImageUpdated() {
    int Current=ui->TableSeq->currentRow();
    if ((Current<0)||(Current>=DiaporamaObject->List.count())) return;
    // Refresh thumbnail
    ui->TableSeq->cellWidget(Current,0)->repaint();
}

//====================================================================================================================

void DlgImageProperties::s_ShotTabWidgetCurrentChanged(int Page) {
    if (Page==SUBPAGE_SHOTCOMPOSITION) s_CompositionInShotNeedRefreshBackgroundImage(); // Refresh background image
}

