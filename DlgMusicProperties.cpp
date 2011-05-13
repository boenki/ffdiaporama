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

#include "DlgMusicProperties.h"
#include "ui_DlgMusicProperties.h"
#include "mainwindow.h"

DlgMusicProperties::DlgMusicProperties(cDiaporamaObject *TheDiaporamaObject,QWidget *parent) : QDialog(parent), ui(new Ui::DlgMusicProperties) {
    ui->setupUi(this);

    DiaporamaObject = TheDiaporamaObject;

    // Save object before modification for cancel button
    Undo=new QDomDocument(APPLICATION_NAME);
    QDomElement root=Undo->createElement("UNDO-DLG");       // Create xml document and root
    DiaporamaObject->SaveToXML(root,"UNDO-DLG-OBJECT","");  // Save object
    Undo->appendChild(root);                                // Add object to xml document

    // Init embeded widgets
    ui->PlayListTable->horizontalHeader()->setResizeMode(0,QHeaderView::ResizeToContents);
    ui->PlayListTable->horizontalHeader()->setResizeMode(1,QHeaderView::ResizeToContents);
    ui->PlayListTable->horizontalHeader()->setResizeMode(2,QHeaderView::ResizeToContents);
    ui->PlayListTable->horizontalHeader()->setResizeMode(3,QHeaderView::ResizeToContents);

    // Add music to PlayListTable
    for (int CurIndex=0;CurIndex<DiaporamaObject->MusicList.count();CurIndex++) {
        int j=ui->PlayListTable->rowCount();     // Item will be add at end of the list
        ui->PlayListTable->insertRow(j);
        QTableWidgetItem *Item;
        Item=new QTableWidgetItem(QFileInfo(DiaporamaObject->MusicList[CurIndex].FilePath).fileName()); Item->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);     ui->PlayListTable->setItem(j,0,Item);
        Item=new QTableWidgetItem(DiaporamaObject->MusicList[CurIndex].Duration.toString("hh:mm:ss"));  Item->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);  ui->PlayListTable->setItem(j,1,Item);
        QComboBox *InternalCB=new QComboBox(ui->PlayListTable);
        for (int Factor=150;Factor>=10;Factor-=10) InternalCB->addItem(QString("%1%").arg(Factor));
        InternalCB->setCurrentIndex(InternalCB->findText(QString("%1%").arg(int(DiaporamaObject->MusicList[CurIndex].Volume*100))));
        ui->PlayListTable->setCellWidget(j,2,InternalCB);
        Item=new QTableWidgetItem(QFileInfo(DiaporamaObject->MusicList[CurIndex].FilePath).path());     Item->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);     ui->PlayListTable->setItem(j,3,Item);
        ui->PlayListTable->verticalHeader()->setResizeMode(j,QHeaderView::ResizeToContents);
    }

    // Setup enable/disable state
    SetupUi();

    // Define handler
    connect(ui->CloseBT,SIGNAL(clicked()),this,SLOT(reject()));
    connect(ui->OKBT,SIGNAL(clicked()),this,SLOT(accept()));
    connect(ui->HelpBT,SIGNAL(clicked()),this,SLOT(Help()));

    connect(ui->SameMusicRD,SIGNAL(clicked()),this,SLOT(s_SameMusic()));
    connect(ui->SameMusicNormalRD,SIGNAL(clicked()),this,SLOT(s_SameMusicNormal()));
    connect(ui->SameMusicReduceVolumeRD,SIGNAL(clicked()),this,SLOT(s_SameMusicReduceVolume()));
    connect(ui->SameMusicPauseRD,SIGNAL(clicked()),this,SLOT(s_SameMusicPause()));

    connect(ui->NewMusicRD,SIGNAL(clicked()),this,SLOT(s_NewMusic()));
    connect(ui->AddMusicBt,SIGNAL(pressed()),this,SLOT(s_AddMusic()));
    connect(ui->RemoveMusicBt,SIGNAL(pressed()),this,SLOT(s_RemoveMusic()));
    connect(ui->PlayListTable,SIGNAL(itemDoubleClicked(QTableWidgetItem *)),this,SLOT(s_PlayListTable_DoubleClick(QTableWidgetItem *)));
    connect(ui->PlayListTable,SIGNAL(itemSelectionChanged()),this,SLOT(s_PlayListTable_SelectionChanged()));
}

//====================================================================================================================

DlgMusicProperties::~DlgMusicProperties() {
    delete ui;
}

//====================================================================================================================

void DlgMusicProperties::Help() {
    QDesktopServices::openUrl(QUrl(QString("http://ffdiaporama.tuxfamily.org/")+CurrentLanguage+QString("/HelpMusicProperties.php")));
}

//====================================================================================================================

void DlgMusicProperties::SetSavedWindowGeometry() {
    DiaporamaObject->Parent->ApplicationConfig->DlgMusicPropertiesWSP->ApplyToWindow(this);
}

//====================================================================================================================

void DlgMusicProperties::showEvent(QShowEvent *ev) {
    QDialog::showEvent(ev);
    QTimer::singleShot(0,this,SLOT(SetSavedWindowGeometry()));
}

//====================================================================================================================

void DlgMusicProperties::reject() {
    // Save Window size and position
    DiaporamaObject->Parent->ApplicationConfig->DlgMusicPropertiesWSP->SaveWindowState(this);
    QDomElement root=Undo->documentElement();
    if (root.tagName()=="UNDO-DLG") DiaporamaObject->LoadFromXML(root,"UNDO-DLG-OBJECT","");
    done(1);
}

//====================================================================================================================

void DlgMusicProperties::accept() {
    DiaporamaObject->MusicReduceFactor=double(ui->VolumeReductionFactorCB->currentText().toInt())/100;
    // Get Music volume for each file of the playlist
    if (DiaporamaObject->MusicType) {
        for (int CurIndex=0;CurIndex<DiaporamaObject->MusicList.count();CurIndex++) {
            QComboBox *CB   =(QComboBox *)ui->PlayListTable->cellWidget(CurIndex,2);
            QString   Volume=CB->currentText(); Volume=Volume.left(Volume.length()-1);
            DiaporamaObject->MusicList[CurIndex].Volume=double(Volume.toInt())/100;
        }
    } else DiaporamaObject->MusicList.clear();
    // Save Window size and position
    DiaporamaObject->Parent->ApplicationConfig->DlgMusicPropertiesWSP->SaveWindowState(this);
    done(0);
}

//====================================================================================================================

void DlgMusicProperties::s_SameMusic() {
    if (DiaporamaObject==NULL) return;
    DiaporamaObject->MusicType=false;
    DiaporamaObject->MusicPause=false;
    DiaporamaObject->MusicReduceVolume=false;
    SetupUi();
}

//====================================================================================================================

void DlgMusicProperties::s_SameMusicNormal() {
    if (DiaporamaObject==NULL) return;
    DiaporamaObject->MusicType=false;
    DiaporamaObject->MusicPause=false;
    DiaporamaObject->MusicReduceVolume=false;
    SetupUi();
}

void DlgMusicProperties::s_SameMusicReduceVolume() {
    DiaporamaObject->MusicType=false;
    DiaporamaObject->MusicPause=false;
    DiaporamaObject->MusicReduceVolume=true;
    SetupUi();
}

void DlgMusicProperties::s_SameMusicPause() {
    if (DiaporamaObject==NULL) return;
    DiaporamaObject->MusicType=false;
    DiaporamaObject->MusicPause=true;
    DiaporamaObject->MusicReduceVolume=false;
    SetupUi();
}

//====================================================================================================================

void DlgMusicProperties::s_NewMusic() {
    if (DiaporamaObject==NULL) return;
    DiaporamaObject->MusicType=true;
    DiaporamaObject->MusicPause=false;
    DiaporamaObject->MusicReduceVolume=false;
    SetupUi();
}

//====================================================================================================================

void DlgMusicProperties::SetupUi() {
    if (DiaporamaObject!=NULL) {
        ui->SameMusicRD->setChecked(!DiaporamaObject->MusicType);
        ui->SameMusicNormalRD->setEnabled(!DiaporamaObject->MusicType);
        ui->SameMusicNormalRD->setChecked((!DiaporamaObject->MusicType)&&(!DiaporamaObject->MusicPause)&&(!DiaporamaObject->MusicReduceVolume));
        ui->SameMusicReduceVolumeRD->setEnabled(!DiaporamaObject->MusicType);
        ui->SameMusicReduceVolumeRD->setChecked((!DiaporamaObject->MusicType)&&(!DiaporamaObject->MusicPause)&&(DiaporamaObject->MusicReduceVolume));
        ui->VolumeReductionFactorCB->setEnabled((!DiaporamaObject->MusicType)&&(!DiaporamaObject->MusicPause)&&(DiaporamaObject->MusicReduceVolume));
        ui->SameMusicPauseRD->setEnabled(!DiaporamaObject->MusicType);
        ui->SameMusicPauseRD->setChecked((!DiaporamaObject->MusicType)&&(DiaporamaObject->MusicPause)&&(!DiaporamaObject->MusicReduceVolume));
        ui->NewMusicRD->setChecked(DiaporamaObject->MusicType);
        ui->VolumeReductionFactorCB->setCurrentIndex(ui->VolumeReductionFactorCB->findText(QString("%1").arg(int(DiaporamaObject->MusicReduceFactor*100))));

        ui->PlayListTable->setEnabled(DiaporamaObject->MusicType);
        ui->AddMusicBt->setEnabled(DiaporamaObject->MusicType);
        ui->RemoveMusicBt->setEnabled((DiaporamaObject->MusicType)&&(DiaporamaObject->MusicList.count()>0)&&(ui->PlayListTable->currentRow()!=-1));
        ui->UpMusicBt->setEnabled((DiaporamaObject->MusicType)&&(DiaporamaObject->MusicList.count()>0)&&(ui->PlayListTable->currentRow()>0));
        ui->DownMusicBt->setEnabled((DiaporamaObject->MusicType)&&(DiaporamaObject->MusicList.count()>0)&&(ui->PlayListTable->currentRow()<ui->PlayListTable->rowCount()-1));
        if (DiaporamaObject->MusicType) {
            int TotalDuration=0;
            for (int CurIndex=0;CurIndex<DiaporamaObject->MusicList.count();CurIndex++) TotalDuration+=QTime(0,0,0,0).msecsTo(DiaporamaObject->MusicList[CurIndex].Duration);
            ui->LabelDuration->setText(QTime(0,0,0,0).addMSecs(TotalDuration).toString("hh:mm:ss"));
        } else {
            ui->Label1->setText("");
            ui->LabelDuration->setText("");
        }
    } else {
        ui->PlayListTable->setEnabled(false);
        ui->AddMusicBt->setEnabled(false);
        ui->RemoveMusicBt->setEnabled(false);
        ui->UpMusicBt->setEnabled(false);
        ui->DownMusicBt->setEnabled(false);
        ui->Label1->setText("");
        ui->LabelDuration->setText("");
    }
}

//====================================================================================================================

void DlgMusicProperties::s_AddMusic() {
    QStringList FileList=QFileDialog::getOpenFileNames(this,QApplication::translate("DlgMusicProperties","Add music files"),
                                                       DiaporamaObject->Parent->ApplicationConfig->RememberLastDirectories?DiaporamaObject->Parent->ApplicationConfig->LastMusicPath:"",
                                                       GlobalMainWindow->ApplicationConfig->GetFilterForMediaFile(cApplicationConfig::MUSICFILE));
    QCoreApplication::processEvents();
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    int CurIndex=DiaporamaObject->MusicList.count();

    // Add files
    for (int i=0;i<FileList.count();i++) {
        QString NewFile=FileList[i];
        if (DiaporamaObject->Parent->ApplicationConfig->RememberLastDirectories)
            DiaporamaObject->Parent->ApplicationConfig->LastMusicPath=QFileInfo(NewFile).absolutePath();     // Keep folder for next use

        DiaporamaObject->MusicList.insert(CurIndex,cMusicObject());
        if (DiaporamaObject->MusicList[CurIndex].LoadMedia(NewFile)) {

            // Add music to PlayListTable
            int j=ui->PlayListTable->rowCount();     // Item will be add at end of the list
            ui->PlayListTable->insertRow(j);
            QTableWidgetItem *Item;
            Item=new QTableWidgetItem(QFileInfo(DiaporamaObject->MusicList[CurIndex].FilePath).fileName()); Item->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);     ui->PlayListTable->setItem(j,0,Item);
            Item=new QTableWidgetItem(DiaporamaObject->MusicList[CurIndex].Duration.toString("hh:mm:ss"));  Item->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);  ui->PlayListTable->setItem(j,1,Item);
            QComboBox *InternalCB=new QComboBox(ui->PlayListTable);
            for (int Factor=150;Factor>=10;Factor-=10) InternalCB->addItem(QString("%1%").arg(Factor));
            InternalCB->setCurrentIndex(InternalCB->findText(QString("%1%").arg(int(DiaporamaObject->MusicList[CurIndex].Volume*100))));
            ui->PlayListTable->setCellWidget(j,2,InternalCB);
            Item=new QTableWidgetItem(QFileInfo(DiaporamaObject->MusicList[CurIndex].FilePath).path());     Item->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);     ui->PlayListTable->setItem(j,3,Item);

            ui->PlayListTable->verticalHeader()->setResizeMode(j,QHeaderView::ResizeToContents);
            CurIndex++;
        } else {
            QMessageBox::critical(NULL,QCoreApplication::translate("DlgMusicProperties","Error","Error message"),
                                  NewFile+"\n\n"+QCoreApplication::translate("DlgMusicProperties","Format not supported","Error message"),QMessageBox::Close);
            DiaporamaObject->MusicList.removeAt(CurIndex);
        }
    }
    QApplication::restoreOverrideCursor();
    SetupUi();
}

//====================================================================================================================

void DlgMusicProperties::s_RemoveMusic() {
    int CurIndex=ui->PlayListTable->currentRow();
    if (CurIndex==-1) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    DiaporamaObject->MusicList.removeAt(CurIndex);
    ui->PlayListTable->removeRow(CurIndex);
    if (CurIndex<ui->PlayListTable->rowCount()) ui->PlayListTable->setCurrentCell(CurIndex,0); else ui->PlayListTable->setCurrentCell(0,0);
    QApplication::restoreOverrideCursor();
    SetupUi();
}

//====================================================================================================================

void DlgMusicProperties::s_PlayListTable_DoubleClick(QTableWidgetItem *) {
    SetupUi();
}

//====================================================================================================================

void DlgMusicProperties::s_PlayListTable_SelectionChanged() {
    SetupUi();
}
