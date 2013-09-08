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

#include "DlgExportProject.h"
#include "ui_DlgExportProject.h"

#define LATENCY 5

DlgExportProject::DlgExportProject(cDiaporama *ffdProject,cBaseApplicationConfig *ApplicationConfig,cSaveWindowPosition *DlgWSP,QWidget *parent)
    :QCustomDialog(ApplicationConfig,DlgWSP,parent),ui(new Ui::DlgExportProject) {

    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgExportProject::DlgExportProject");

    ui->setupUi(this);
    OkBt    =ui->OkBt;
    CancelBt=ui->CancelBt;
    HelpBt  =ui->HelpBt;
    HelpFile="0107";

    this->ffdProject=ffdProject;
    DestinationPath =BaseApplicationConfig->LastExportPath;
    JobStarted      =false;
    JobCancel       =false;

    HeighRow=QFontMetrics(QApplication::font()).boundingRect("0").height();
    if (HeighRow<16) HeighRow=16; // Not less than Icon
}

//====================================================================================================================

DlgExportProject::~DlgExportProject() {
    delete ui;
}

//====================================================================================================================
// Initialise dialog

void DlgExportProject::DoInitDialog() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgExportProject::DoInitDialog");

    ui->ProgressBar->setEnabled(false);
    ui->ProgressBar->setRange(0,0);
    ui->OkBt->setEnabled(false);
    ui->Table->setColumnCount(3);
    ui->Table->setHorizontalHeaderLabels(QApplication::translate("DlgExportProject","% done;Destination file;Source file").split(";"));
    for (int Col=0;Col<ui->Table->columnCount();Col++) ui->Table->horizontalHeaderItem(Col)->setTextAlignment(Col==0?Qt::AlignHCenter:Qt::AlignLeft);
    #if QT_VERSION >= 0x050000
    ui->Table->horizontalHeader()->setSectionsClickable(false);
    ui->Table->horizontalHeader()->setSectionsMovable(false);
    ui->Table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->Table->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    #else
    ui->Table->horizontalHeader()->setClickable(false);
    ui->Table->horizontalHeader()->setMovable(false);
    ui->Table->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    ui->Table->verticalHeader()->setResizeMode(QHeaderView::Fixed);
    #endif

    ui->DestinationPathED->setText(DestinationPath);
    ui->ProjectSubfolderED->setText(QFileInfo(ffdProject->ProjectFileName).baseName());
    connect(ui->DestinationPathBT,SIGNAL(clicked()),this,SLOT(SelectDestinationPath()));
    connect(ui->DestinationPathED,SIGNAL(editingFinished()),this,SLOT(AdjustDestinationPath()));
    connect(&ThreadCopy,SIGNAL(finished()),this,SLOT(EndAccept()));
    connect(&Timer,SIGNAL(timeout()),this,SLOT(s_TimerEvent()));
    ScanDiaporama();
}

//====================================================================================================================
// Call when user click on Ok button

bool DlgExportProject::DoAccept() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgExportProject::DoAccept");
    BaseApplicationConfig->LastExportPath=DestinationPath;
    JobStarted=true;
    ui->OkBt->setEnabled(false);
    ThreadCopy.setFuture(QtConcurrent::run(this,&DlgExportProject::DoProcessCopy));
    Timer.start(500);
    return false;
}

void DlgExportProject::EndAccept() {
    Timer.stop();
    ui->ProgressBar->setValue(ui->ProgressBar->maximum());
    if (!ErrorMsg.isEmpty()) CustomMessageBox(this,QMessageBox::Information,this->windowTitle(),ErrorMsg,QMessageBox::Close);
    if (!JobCancel)          CustomMessageBox(this,QMessageBox::Information,this->windowTitle(),QApplication::translate("DlgExportProject","Export done","Error message"),QMessageBox::Close);
    SaveWindowState();
    done(0);
}

//====================================================================================================================

void DlgExportProject::s_TimerEvent() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgExportProject::s_TimerEvent");
    if (JobStarted) {
        ui->Table->setUpdatesEnabled(false);
        ui->Table->selectionModel()->clear();
        ui->Table->setCurrentCell(CurrentObject,0,QItemSelectionModel::Select|QItemSelectionModel::Current);
        ui->Table->scrollToItem(ui->Table->item(CurrentObject,0));
        ui->Table->setUpdatesEnabled(true);
        ui->ProgressBar->setValue(CurrentObject);
    }
}

//====================================================================================================================
// Call when user click on Ok button

void DlgExportProject::DoRejet() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgExportProject::DoRejet");
    if (JobStarted) {
        Timer.stop();
        JobCancel=true;
        if (ThreadCopy.isRunning()) ThreadCopy.waitForFinished();
        CustomMessageBox(this,QMessageBox::Information,this->windowTitle(),QApplication::translate("DlgExportProject","Export cancel","Error message"),QMessageBox::Close);
    }
}

//====================================================================================================================

void DlgExportProject::SelectDestinationPath() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgExportProject::SelectDestinationPath");
    QString OutputFileName  =QFileDialog::getExistingDirectory(this,QApplication::translate("DlgRenderVideo","Select destination folder"),DestinationPath,
                                                               QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
    if (OutputFileName!="") {
        DestinationPath=OutputFileName;
        ui->DestinationPathED->setText(DestinationPath);
    }
}

//====================================================================================================================

void DlgExportProject::AdjustDestinationPath() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgExportProject::AdjustDestinationPath");
    DestinationPath=ui->DestinationPathED->text();
}

//====================================================================================================================
QTableWidgetItem *CreateTextItem(QString Text,int Alignment) {
    QTableWidgetItem *Item=new QTableWidgetItem(Text,0);
    Item->setTextAlignment(Alignment|Qt::AlignVCenter);
    return Item;
}

void DlgExportProject::SearchAppendObject(QString FileName) {
    if ((FileName.startsWith(ModelFolder))||(FileName.startsWith(ClipArtFolder))) return;
    int Nbr=ReplaceList.List.count();
    ReplaceList.SearchAppendObject(FileName);
    if (Nbr<ReplaceList.List.count()) {
        ui->Table->insertRow(Nbr);
        ui->Table->setItem(Nbr,0,CreateTextItem(QApplication::translate("DlgExportProject","not started"),Qt::AlignHCenter));
        ui->Table->setItem(Nbr,1,CreateTextItem(ReplaceList.List[Nbr].DestFileName,Qt::AlignLeft));
        ui->Table->setItem(Nbr,2,CreateTextItem(AdjustDirForOS(ReplaceList.List[Nbr].SourceFileName),Qt::AlignLeft));
        ui->Table->setRowHeight(Nbr,HeighRow);
    }
}

void DlgExportProject::ScanDiaporama() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgExportProject::ScanDiaporama");
    // ProjectThumbnail
    for (int j=0;j<ffdProject->ProjectThumbnail->ObjectComposition.List.count();j++) if (ffdProject->ProjectThumbnail->ObjectComposition.List[j]->BackgroundBrush) {
        if (ffdProject->ProjectThumbnail->ObjectComposition.List[j]->BackgroundBrush->Image)            SearchAppendObject(ffdProject->ProjectThumbnail->ObjectComposition.List[j]->BackgroundBrush->Image->FileName);
            else if (ffdProject->ProjectThumbnail->ObjectComposition.List[j]->BackgroundBrush->Video)   SearchAppendObject(ffdProject->ProjectThumbnail->ObjectComposition.List[j]->BackgroundBrush->Video->FileName);
    }
    // Objects
    for (int i=0;i<ffdProject->List.count();i++) {
        // MusicList
        if ((ffdProject->List[i]->MusicType)) {
            for (int j=0;j<ffdProject->List[i]->MusicList.count();j++)  SearchAppendObject(ffdProject->List[i]->MusicList[j].FileName);
        }
        // BackgroundBrush of object
        if ((ffdProject->List[i]->BackgroundType)&&(ffdProject->List[i]->BackgroundBrush)) {
            if (ffdProject->List[i]->BackgroundBrush->Image)            SearchAppendObject(ffdProject->List[i]->BackgroundBrush->Image->FileName);
                else if (ffdProject->List[i]->BackgroundBrush->Video)   SearchAppendObject(ffdProject->List[i]->BackgroundBrush->Video->FileName);
        }
        // BackgroundBrush of shots
        for (int j=0;j<ffdProject->List[i]->ObjectComposition.List.count();j++) if (ffdProject->List[i]->ObjectComposition.List[j]->BackgroundBrush) {
            if (ffdProject->List[i]->ObjectComposition.List[j]->BackgroundBrush->Image)            SearchAppendObject(ffdProject->List[i]->ObjectComposition.List[j]->BackgroundBrush->Image->FileName);
                else if (ffdProject->List[i]->ObjectComposition.List[j]->BackgroundBrush->Video)   SearchAppendObject(ffdProject->List[i]->ObjectComposition.List[j]->BackgroundBrush->Video->FileName);
        }
    }
    int Nbr=ReplaceList.List.count();
    ReplaceList.List.append(cReplaceObject("-",QApplication::translate("DlgExportProject","Project file")));
    ui->Table->insertRow(Nbr);
    ui->Table->setRowHeight(Nbr,HeighRow);
    ui->Table->setItem(Nbr,0,CreateTextItem(QApplication::translate("DlgExportProject","not started"),Qt::AlignHCenter));
    ui->Table->setItem(Nbr,1,CreateTextItem(ReplaceList.List[Nbr].DestFileName,Qt::AlignLeft));
    ui->Table->setItem(Nbr,2,CreateTextItem(AdjustDirForOS(ReplaceList.List[Nbr].SourceFileName),Qt::AlignLeft));
    ui->ProgressBar->setRange(0,ReplaceList.List.count()-1);
    ui->OkBt->setEnabled(true);
}

//====================================================================================================================

void DlgExportProject::DoProcessCopy() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgExportProject::DoProcessCopy");
    QString DestPath=DestinationPath;
    if (!DestPath.endsWith(QDir::separator())) DestPath=DestPath+QDir::separator();
    DestPath=DestPath+ui->ProjectSubfolderED->text();
    if (!QDir().mkpath(DestPath)) {
        ErrorMsg=QApplication::translate("DlgExportProject","Error during the creation of the %1 folder","Error message");
        JobCancel=true;
    } else {
        if (!DestPath.endsWith(QDir::separator())) DestPath=DestPath+QDir::separator();
        CurrentObject=0;
        while ((!JobCancel)&&(CurrentObject<ReplaceList.List.count())) {
            ui->Table->setItem(CurrentObject,0,CreateTextItem(QApplication::translate("DlgExportProject","started"),Qt::AlignHCenter));
            if (ReplaceList.List[CurrentObject].SourceFileName=="-") {
                QString FName=DestPath+ui->ProjectSubfolderED->text()+".ffd";
                if (ffdProject->SaveFile(NULL,&ReplaceList,&FName)) ui->Table->setItem(CurrentObject,0,CreateTextItem(QApplication::translate("DlgExportProject","done"),Qt::AlignHCenter)); else {
                    ui->Table->setItem(CurrentObject,0,CreateTextItem(QApplication::translate("DlgExportProject","error"),Qt::AlignHCenter));
                    JobCancel=true;
                    ErrorMsg=QApplication::translate("DlgExportProject","Error during the export of the project file","Error message");
                }
                CurrentObject++;
            } else {
                if (!QFile(ReplaceList.List[CurrentObject].SourceFileName).copy(DestPath+ReplaceList.List[CurrentObject].DestFileName)) {
                    JobCancel=true;
                    ErrorMsg=QApplication::translate("DlgExportProject","Error during the copy of %1 to %2","Error message").arg(ReplaceList.List[CurrentObject].SourceFileName).arg(DestPath+ReplaceList.List[CurrentObject].DestFileName);
                    ui->Table->setItem(CurrentObject,0,CreateTextItem(QApplication::translate("DlgExportProject","error"),Qt::AlignHCenter));
                } else {
                    ui->Table->setItem(CurrentObject,0,CreateTextItem(QApplication::translate("DlgExportProject","done"),Qt::AlignHCenter));
                    CurrentObject++;
                }
            }
        }
    }
}