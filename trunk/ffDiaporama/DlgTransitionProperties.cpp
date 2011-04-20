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

#include "DlgTransitionProperties.h"
#include "ui_DlgTransitionProperties.h"

//======================================
// Specific defines for this dialog box
//======================================
#define ROWHEIGHT   80
#define DECAL       10

int     CurrentSelectRow,CurrentSelectCol;

class BackgroundDelegate : public QStyledItemDelegate {
public:
    explicit BackgroundDelegate(QObject *parent = 0):QStyledItemDelegate(parent) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option,const QModelIndex &index) const {
        if ((index.row()==CurrentSelectRow)&&(index.column()==CurrentSelectCol)) painter->fillRect(option.rect,Qt::blue); else {
            QVariant background = index.data(Qt::BackgroundRole);
            if (background.canConvert<QBrush>()) painter->fillRect(option.rect, background.value<QBrush>());
            else painter->fillRect(option.rect,Qt::white);
        }
    }
};

//====================================================================================================================

DlgTransitionProperties::DlgTransitionProperties(cDiaporamaObject *DiaporamaObject,bool IsBackgroundTransition,QWidget *parent):QDialog(parent),ui(new Ui::DlgTransitionProperties) {
    ui->setupUi(this);
    this->DiaporamaObject       =DiaporamaObject;
    this->IsBackgroundTransition=IsBackgroundTransition;

    // Save object before modification for cancel button
    Undo=new QDomDocument(APPLICATION_NAME);
    QDomElement root=Undo->createElement("UNDO-DLG");       // Create xml document and root
    DiaporamaObject->SaveToXML(root,"UNDO-DLG-OBJECT","");  // Save object
    Undo->appendChild(root);                                // Add object to xml document

    // Init internal values
    PreviousFrame=NULL;
    AnimationTime=0;
    ui->TransitionTable->setItemDelegate(new BackgroundDelegate(this));
    MaxItem=0;
    H=ROWHEIGHT;
    W=DiaporamaObject->Parent->GetWidthForHeight(ROWHEIGHT);
    for (int i=0;i<ui->TransitionTable->columnCount();i++) ui->TransitionTable->setColumnWidth(i,W+DECAL*2);

    // Save Object settings before force a transition
    TransitionFamilly =DiaporamaObject->TransitionFamilly;
    TransitionSubType =DiaporamaObject->TransitionSubType;
    TransitionDuration=DiaporamaObject->TransitionDuration;

    //=================> Init PreviousFrame object

    // Change Object settings to force a transition
    DiaporamaObject->TransitionFamilly =1;
    DiaporamaObject->TransitionSubType =0;
    DiaporamaObject->TransitionDuration=TransitionDuration;

    // Retrieve time information
    TimePosition=DiaporamaObject->Parent->GetObjectStartPosition(DiaporamaObject->Parent->GetObjectIndex(DiaporamaObject));

    // Retrieve object informations and create PreviousFrame
    PreviousFrame=new cDiaporamaObjectInfo(NULL,TimePosition,DiaporamaObject->Parent,double(1000)/DiaporamaObject->Parent->ApplicationConfig->PreviewFPS);
    DiaporamaObject->Parent->LoadSources(PreviousFrame,W,H,true);                       // Load background and image
    DiaporamaObject->Parent->PrepareImage(PreviousFrame,W,H,0,true);                        // Current Object
    DiaporamaObject->Parent->PrepareImage(PreviousFrame,W,H,0,false);                       // Transition Object

    // Set old values
    DiaporamaObject->TransitionFamilly =TransitionFamilly;
    DiaporamaObject->TransitionSubType =TransitionSubType;
    DiaporamaObject->TransitionDuration=TransitionDuration;

    //=================> Init controls

    QString Duration=QString("%1").arg(double(TransitionDuration)/double(1000),0,'f');
    while (Duration.endsWith('0')) Duration=Duration.left(Duration.length()-1);
    while (Duration.endsWith('.')) Duration=Duration.left(Duration.length()-1);
    ui->TransitionDurationCB->setCurrentIndex(ui->TransitionDurationCB->findText(Duration));

    ui->TransitionTypeCB->setCurrentIndex(TransitionFamilly);
    s_ChTransitionTypeCB(TransitionFamilly);

    //=================> Define handlers

    connect(ui->TransitionTypeCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChTransitionTypeCB(int)));
    connect(ui->TransitionTable,SIGNAL(currentCellChanged(int,int,int,int)),this,SLOT(s_TableCellChanged(int,int,int,int)));
    connect(ui->TransitionDurationCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChTransitionDurationCB(int)));
    connect(&Timer,SIGNAL(timeout()),this,SLOT(s_TimerEvent()));
    connect(ui->CloseBT,SIGNAL(clicked()),this,SLOT(reject()));
    connect(ui->OKBT,SIGNAL(clicked()),this,SLOT(accept()));
}

//====================================================================================================================

DlgTransitionProperties::~DlgTransitionProperties() {
    delete ui;
    delete Undo;
    if (PreviousFrame) {
        delete PreviousFrame;
        PreviousFrame=NULL;
    }
}

//====================================================================================================================

void DlgTransitionProperties::SetSavedWindowGeometry() {
    DiaporamaObject->Parent->ApplicationConfig->DlgTransitionPropertiesWSP->ApplyToWindow(this);
}

//====================================================================================================================

void DlgTransitionProperties::showEvent(QShowEvent *ev) {
    QDialog::showEvent(ev);
    QTimer::singleShot(0,this,SLOT(SetSavedWindowGeometry()));
}

//====================================================================================================================

void DlgTransitionProperties::reject() {
    // Save Window size and position
    DiaporamaObject->Parent->ApplicationConfig->DlgTransitionPropertiesWSP->SaveWindowState(this);
    QDomElement root=Undo->documentElement();
    if (root.tagName()=="UNDO-DLG") DiaporamaObject->LoadFromXML(root,"UNDO-DLG-OBJECT","");
    done(1);
}

//====================================================================================================================

void DlgTransitionProperties::accept() {
    // Save Window size and position
    DiaporamaObject->Parent->ApplicationConfig->DlgTransitionPropertiesWSP->SaveWindowState(this);

    DiaporamaObject->TransitionFamilly =ui->TransitionTypeCB->currentIndex();
    DiaporamaObject->TransitionSubType =ui->TransitionTable->currentRow()*ui->TransitionTable->columnCount()+ui->TransitionTable->currentColumn();
    DiaporamaObject->TransitionDuration=int(ui->TransitionDurationCB->currentText().toDouble()*double(1000));

    // Close the box
    done(0);
}

//====================================================================================================================

void DlgTransitionProperties::s_ChTransitionTypeCB(int NewValue) {
    Timer.stop();
    if (PreviousFrame==NULL) return;    // Nothing to do if previsous frame is not initialize

    ui->TransitionTable->setUpdatesEnabled(false);

    // clear table
    while (ui->TransitionTable->rowCount()>0) ui->TransitionTable->removeRow(0);

    MaxItem=0;
    switch (NewValue) {
    case TRANSITIONFAMILLY_BASE        : MaxItem=TRANSITIONMAXSUBTYPE_BASE;         break;
    case TRANSITIONFAMILLY_ZOOMINOUT   : MaxItem=TRANSITIONMAXSUBTYPE_ZOOMINOUT;    break;
    case TRANSITIONFAMILLY_PUSH        : MaxItem=TRANSITIONMAXSUBTYPE_PUSH;         break;
    case TRANSITIONFAMILLY_SLIDE       : MaxItem=TRANSITIONMAXSUBTYPE_SLIDE;        break;
    case TRANSITIONFAMILLY_LUMA_BAR    : MaxItem=LumaList_Bar.List.count();         break;
    case TRANSITIONFAMILLY_LUMA_BOX    : MaxItem=LumaList_Box.List.count();         break;
    case TRANSITIONFAMILLY_LUMA_CENTER : MaxItem=LumaList_Center.List.count();      break;
    case TRANSITIONFAMILLY_LUMA_CHECKER: MaxItem=LumaList_Checker.List.count();     break;
    case TRANSITIONFAMILLY_LUMA_CLOCK  : MaxItem=LumaList_Clock.List.count();       break;
    case TRANSITIONFAMILLY_LUMA_SNAKE  : MaxItem=LumaList_Snake.List.count();       break;
    default                            : MaxItem=0;                                 break;
    }

    AnimationTime=0;

    // Adjust TransitionFamilly
    PreviousFrame->TransitionFamilly=NewValue;
    // Create a frame object base on PreviousFrame
    cDiaporamaObjectInfo *Frame=new cDiaporamaObjectInfo(PreviousFrame);
    // Ajdust Transition PCT done
    Frame->TransitionPCTDone =double(AnimationTime)/double(Frame->TransitionDuration);

    // Now add items
    int CurCol=0;
    ui->TransitionTable->insertRow(ui->TransitionTable->rowCount());    // Create first row
    ui->TransitionTable->setRowHeight(ui->TransitionTable->rowCount()-1,ROWHEIGHT+DECAL*2);

    for (int i=0;i<MaxItem;i++) {
        // Adjust transition subtype
        Frame->TransitionSubType=i;
        // Render images
        if (Frame->RenderedImage!=NULL) {
            delete Frame->RenderedImage;
            Frame->RenderedImage=NULL;
        }
        DiaporamaObject->Parent->DoAssembly(Frame,W,H);

        // Create a label object to handle the bitmap
        QLabel *Widget=new QLabel();
        Widget->setAlignment(Qt::AlignCenter);
        Widget->setPixmap(QPixmap().fromImage(*Frame->RenderedImage));
        if (CurCol>=ui->TransitionTable->columnCount()) {
            ui->TransitionTable->insertRow(ui->TransitionTable->rowCount());
            ui->TransitionTable->setRowHeight(ui->TransitionTable->rowCount()-1,ROWHEIGHT+DECAL*2);
            CurCol=0;
        }
        ui->TransitionTable->setCellWidget(ui->TransitionTable->rowCount()-1,CurCol,Widget);
        CurCol++;
    }
    // Free buffers
    delete Frame;

    ui->TransitionTable->setCurrentCell(0,0,QItemSelectionModel::ClearAndSelect);
    ui->TransitionTable->setUpdatesEnabled(true);

    // Select a cell
    if (NewValue==TransitionFamilly) {
        CurrentSelectRow=TransitionSubType/ui->TransitionTable->columnCount();
        CurrentSelectCol=TransitionSubType % ui->TransitionTable->columnCount();
        ui->TransitionTable->setCurrentCell(CurrentSelectRow,CurrentSelectCol,QItemSelectionModel::ClearAndSelect);
    }

    Timer.start(double(1000)/DiaporamaObject->Parent->ApplicationConfig->PreviewFPS);   // Start timer for animation
}

//====================================================================================================================
// Change of transition duration : Reload frame with new value
//====================================================================================================================

void DlgTransitionProperties::s_ChTransitionDurationCB(int) {
    // Stop timer
    Timer.stop();

    // Clear PreviousFrame before create a new one
    if (PreviousFrame!=NULL) {
        delete PreviousFrame;
        PreviousFrame=NULL;
    }

    // Change Object settings to force a transition
    DiaporamaObject->TransitionFamilly =1;
    DiaporamaObject->TransitionSubType =0;
    DiaporamaObject->TransitionDuration=int(ui->TransitionDurationCB->currentText().toDouble()*double(1000));

    // Retrieve time information
    TimePosition=DiaporamaObject->Parent->GetObjectStartPosition(DiaporamaObject->Parent->GetObjectIndex(DiaporamaObject));

    // Retrieve object informations and create PreviousFrame
    PreviousFrame=new cDiaporamaObjectInfo(NULL,TimePosition,DiaporamaObject->Parent,double(1000)/DiaporamaObject->Parent->ApplicationConfig->PreviewFPS);
    DiaporamaObject->Parent->LoadSources(PreviousFrame,W,H,true);                       // Load background and image
    DiaporamaObject->Parent->PrepareImage(PreviousFrame,W,H,0,true);                        // Current Object
    DiaporamaObject->Parent->PrepareImage(PreviousFrame,W,H,0,false);                       // Transition Object

    // Set old values
    DiaporamaObject->TransitionFamilly =TransitionFamilly;
    DiaporamaObject->TransitionSubType =TransitionSubType;
    DiaporamaObject->TransitionDuration=TransitionDuration;

    // Reset AnimationTime
    AnimationTime=0;

    // Adjust Transition
    PreviousFrame->TransitionFamilly=ui->TransitionTypeCB->currentIndex();
    PreviousFrame->TransitionSubType=ui->TransitionTable->currentRow()*ui->TransitionTable->columnCount()+ui->TransitionTable->currentColumn();
    PreviousFrame->TransitionDuration=int(ui->TransitionDurationCB->currentText().toDouble()*double(1000));

    // Restart timer
    Timer.start(double(1000)/DiaporamaObject->Parent->ApplicationConfig->PreviewFPS);   // Re-start timer for animation
}

//====================================================================================================================
// Timer event : update pixmap in the table
//====================================================================================================================
void DlgTransitionProperties::s_TimerEvent() {
    // Update all pixmap in the table
    int CurCol=0;
    int CurRow=0;
    AnimationTime+=double(1000)/DiaporamaObject->Parent->ApplicationConfig->PreviewFPS;
    if (AnimationTime>int(ui->TransitionDurationCB->currentText().toDouble()*double(1000))) AnimationTime=0;

    // Create a frame object base on PreviousFrame
    cDiaporamaObjectInfo *Frame=new cDiaporamaObjectInfo(PreviousFrame);
    // Ajdust Transition PCT done
    Frame->TransitionPCTDone=double(AnimationTime)/double(Frame->TransitionDuration);
    Frame->TransitionPCTEnd =double(AnimationTime+double(1000)/DiaporamaObject->Parent->ApplicationConfig->PreviewFPS)/double(Frame->TransitionDuration);

    for (int i=0;i<MaxItem;i++) {
        // Adjust transition subtype
        Frame->TransitionSubType=i;
        // Render images
        if (Frame->RenderedImage!=NULL) {
            delete Frame->RenderedImage;
            Frame->RenderedImage=NULL;
        }
        DiaporamaObject->Parent->DoAssembly(Frame,W,H);

        // Add icon in the bottom left corner
        QPainter P;
        QImage   *Img=IconList.GetIcon(Frame->TransitionFamilly,Frame->TransitionSubType);
        P.begin(Frame->RenderedImage);
        P.drawImage(QRect(0,Frame->RenderedImage->height()-32,32,32),*Img);
        P.end();
        delete Img;

        // Create a label object to handle the bitmap
        QLabel *Widget=(QLabel *)ui->TransitionTable->cellWidget(CurRow,CurCol);
        if (Widget) Widget->setPixmap(QPixmap().fromImage(*Frame->RenderedImage));

        // Go to next image
        CurCol++;
        if (CurCol>=ui->TransitionTable->columnCount()) {
            CurCol=0;
            CurRow++;
        }
    }
    // Free buffers
    delete Frame;
}

//====================================================================================================================
// Change of selected cell in the table
//====================================================================================================================
void DlgTransitionProperties::s_TableCellChanged(int currentRow,int currentColumn,int previousRow,int previousColumn) {
    int NewCell=currentRow*ui->TransitionTable->columnCount()+currentColumn;
    if (NewCell>=MaxItem) {
        CurrentSelectRow=previousRow;
        CurrentSelectCol=previousColumn;
        ui->TransitionTable->setCurrentCell(previousRow,previousColumn,QItemSelectionModel::ClearAndSelect);
    } else {
        CurrentSelectRow=currentRow;
        CurrentSelectCol=currentColumn;
    }
}
