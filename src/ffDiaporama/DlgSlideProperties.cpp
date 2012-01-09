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

#include "DlgSlideProperties.h"
#include "ui_DlgSlideProperties.h"
#include "wgt_QCustomThumbnails.h"
#include "DlgSlideProperties.h"
#include "DlgImageCorrection.h"
#include "DlgVideoEdit.h"
#include "DlgTextEdit.h"
#include "mainwindow.h"

//#define DEBUGMODE

DlgSlideProperties::DlgSlideProperties(cDiaporamaObject *DiaporamaObject,QWidget *parent):QDialog(parent),ui(new Ui::DlgSlideProperties) {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::DlgSlideProperties";
    #endif

    ui->setupUi(this);
    this->DiaporamaObject           =DiaporamaObject;
    GlobalMainWindow->DragItemSource=-1;
    GlobalMainWindow->DragItemDest  =-1;
    GlobalMainWindow->IsDragOn      =0;

#if defined(Q_OS_WIN32)||defined(Q_OS_WIN64)
    setWindowFlags((windowFlags()|Qt::CustomizeWindowHint|Qt::WindowSystemMenuHint|Qt::WindowMaximizeButtonHint)&(~Qt::WindowMinimizeButtonHint));
#else
    setWindowFlags(Qt::Window|Qt::WindowTitleHint|Qt::WindowSystemMenuHint|Qt::WindowMaximizeButtonHint|Qt::WindowMinimizeButtonHint|Qt::WindowCloseButtonHint);
#endif

    ui->SplitterTop->setCollapsible(0,false);
    ui->SplitterTop->setCollapsible(1,false);
    ui->SplitterBottom->setCollapsible(0,false);
    ui->SplitterBottom->setCollapsible(1,true);

    ui->TableInfo->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->TableInfo->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    ui->TableInfo->horizontalHeader()->show();
    ui->TableInfo->horizontalHeader()->setStretchLastSection(false);
    ui->TableInfo->horizontalHeader()->setSortIndicatorShown(false);
    ui->TableInfo->horizontalHeader()->setCascadingSectionResizes(false);
    ui->TableInfo->horizontalHeader()->setClickable(false);
    ui->TableInfo->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->TableInfo->horizontalHeader()->setMovable(false);
    ui->TableInfo->horizontalHeader()->setResizeMode(QHeaderView::Fixed);          //Fixed because ResizeToContents will be done after table filling
    ui->TableInfo->verticalHeader()->hide();
    ui->TableInfo->verticalHeader()->setStretchLastSection(false);
    ui->TableInfo->verticalHeader()->setSortIndicatorShown(false);
    ui->TableInfo->verticalHeader()->setResizeMode(QHeaderView::Fixed);            // Fixed because ResizeToContents will be done after table filling
    ui->TableInfo->setShowGrid(true);                  // Ensure grid display
    ui->TableInfo->setWordWrap(false);                 // Ensure no word wrap
    ui->TableInfo->setTextElideMode(Qt::ElideNone);    // Ensure no line ellipsis (...)
    //ui->tableWidget->setColumnCount(2);
    //ui->tableWidget->setHorizontalHeaderLabels(QString("Propertie;Value").split(";"));

    // Save object before modification for cancel button
    Undo=new QDomDocument(APPLICATION_NAME);
    QDomElement root=Undo->createElement("UNDO-DLG");       // Create xml document and root
    DiaporamaObject->SaveToXML(root,"UNDO-DLG-OBJECT",DiaporamaObject->Parent->ProjectFileName,true);  // Save object
    Undo->appendChild(root);                                // Add object to xml document

    setWindowTitle(windowTitle()+" - "+QApplication::translate("DlgSlideProperties","Slide")+QString(" %1/%2").arg(DiaporamaObject->Parent->CurrentCol+1).arg(DiaporamaObject->Parent->List.count()));
    ui->NewChapterCB->setChecked(DiaporamaObject->StartNewChapter);
    ui->NewChapterCB->setEnabled(DiaporamaObject->Parent->CurrentCol!=0);
    ui->ChapterLabel->setEnabled(DiaporamaObject->Parent->CurrentCol!=0);
    ui->OKPreviousBT->setEnabled(DiaporamaObject->Parent->CurrentCol>0);
    ui->OKNextBT->setEnabled(DiaporamaObject->Parent->CurrentCol<DiaporamaObject->Parent->List.count()-1);

    switch (GlobalMainWindow->Diaporama->ImageGeometry) {
        case GEOMETRY_4_3   : ProjectGeometry=double(1440)/double(1920);  break;
        case GEOMETRY_16_9  : ProjectGeometry=double(1080)/double(1920);  break;
        case GEOMETRY_40_17 : ProjectGeometry=double(816)/double(1920);   break;

    }
    ProjectGeometry=QString("%1").arg(ProjectGeometry,0,'e').toDouble();  // Rounded to same number as style managment

    //******************************

    IsFirstInitDone     = false;                // True when first show window was done
    scene               = NULL;
    NextZValue          = 500;
    BackgroundImage     = NULL;
    CompositionList     = NULL;
    WithPen             = 1;

    // Re-entrence flags
    InBlockTable_SelectionChanged   =false;
    InScene_SelectionChanged        =false;
    InShotTable_SelectionChanged    =false;

    InRefreshStyleControls          =false;
    InRefreshControls               =false;
    InRefreshSceneImage             =false;

    StopMAJSpinbox      = false;
    StopMajFramingStyle = false;

    MagneticRuler.MagneticRuler=DiaporamaObject->Parent->ApplicationConfig->SlideRuler;

    // Init combo box Background form
    for (int i=0;i<12;i++) ui->BackgroundFormCB->addItem("");
    MakeFormIcon(ui->BackgroundFormCB);
    ui->FramingStyleCB->view()->setFixedWidth(300);
    ui->ShadowEffectCB->view()->setFixedWidth(300);

    // Init combo box Background shadow form
    ui->ShadowEffectCB->addItem(QApplication::translate("DlgSlideProperties","None"));
    ui->ShadowEffectCB->addItem(QApplication::translate("DlgSlideProperties","Shadow upper left"));
    ui->ShadowEffectCB->addItem(QApplication::translate("DlgSlideProperties","Shadow upper right"));
    ui->ShadowEffectCB->addItem(QApplication::translate("DlgSlideProperties","Shadow bottom left"));
    ui->ShadowEffectCB->addItem(QApplication::translate("DlgSlideProperties","Shadow bottom right"));
    ui->ShadowEffectED->setRange(1,100);

    // Init combo box external border style
    ui->PenStyleCB->addItem("");    ui->PenStyleCB->setItemData(ui->PenStyleCB->count()-1,(int)Qt::SolidLine);
    ui->PenStyleCB->addItem("");    ui->PenStyleCB->setItemData(ui->PenStyleCB->count()-1,(int)Qt::DashLine);
    ui->PenStyleCB->addItem("");    ui->PenStyleCB->setItemData(ui->PenStyleCB->count()-1,(int)Qt::DotLine);
    ui->PenStyleCB->addItem("");    ui->PenStyleCB->setItemData(ui->PenStyleCB->count()-1,(int)Qt::DashDotLine);
    ui->PenStyleCB->addItem("");    ui->PenStyleCB->setItemData(ui->PenStyleCB->count()-1,(int)Qt::DashDotDotLine);
    MakeBorderStyleIcon(ui->PenStyleCB);
    // Init shape Borders
    ui->PenSizeEd->setMinimum(0);       ui->PenSizeEd->setMaximum(30);

    // Init combo box Background opacity
    ui->OpacityCB->addItem("100%");
    ui->OpacityCB->addItem(" 75%");
    ui->OpacityCB->addItem(" 50%");
    ui->OpacityCB->addItem(" 25%");

    ui->RotateXED->setRange(-180,180);      ui->RotateXSLD->setRange(-180,180);
    ui->RotateYED->setRange(-180,180);      ui->RotateYSLD->setRange(-180,180);
    ui->RotateZED->setRange(-180,180);      ui->RotateZSLD->setRange(-180,180);

    // Init Spinbox

    if (DiaporamaObject->Parent->ApplicationConfig->DisplayUnit==DISPLAYUNIT_PERCENT) {
        ui->PosXEd->setDecimals(2);             ui->PosXEd->setSingleStep(1);       ui->PosXEd->setSuffix("%");
        ui->PosYEd->setDecimals(2);             ui->PosYEd->setSingleStep(1);       ui->PosYEd->setSuffix("%");
        ui->WidthEd->setDecimals(2);            ui->WidthEd->setSingleStep(1);      ui->WidthEd->setSuffix("%");
        ui->HeightEd->setDecimals(2);           ui->HeightEd->setSingleStep(1);     ui->HeightEd->setSuffix("%");
    } else { // DisplayUnit==DISPLAYUNIT_PIXELS
        ui->PosXEd->setDecimals(0);             ui->PosXEd->setSingleStep(1);       ui->PosXEd->setSuffix("");
        ui->PosYEd->setDecimals(0);             ui->PosYEd->setSingleStep(1);       ui->PosYEd->setSuffix("");
        ui->WidthEd->setDecimals(0);            ui->WidthEd->setSingleStep(1);      ui->WidthEd->setSuffix("");
        ui->HeightEd->setDecimals(0);           ui->HeightEd->setSingleStep(1);     ui->HeightEd->setSuffix("");
    }

    // Define handler
    connect(ui->CloseBT,SIGNAL(clicked()),this,SLOT(reject()));
    connect(ui->OKBT,SIGNAL(clicked()),this,SLOT(accept()));
    connect(ui->OKPreviousBT,SIGNAL(clicked()),this,SLOT(OKPrevious()));
    connect(ui->OKNextBT,SIGNAL(clicked()),this,SLOT(OKNext()));
    connect(ui->HelpBT,SIGNAL(clicked()),this,SLOT(Help()));
    connect(ui->TVMarginsBT,SIGNAL(clicked()),this,SLOT(s_TVMarginsBt()));

    connect(ui->CopyBlockBT,SIGNAL(clicked()),this,SLOT(s_CopyBlockBT()));
    connect(ui->CutBlockBT,SIGNAL(clicked()),this,SLOT(s_CutBlockBT()));
    connect(ui->PasteBlockBT,SIGNAL(clicked()),this,SLOT(s_PasteBlockBT()));

    connect(ui->VisibleBT,SIGNAL(clicked()),this,SLOT(ChangeVisibleState()));
    connect(ui->SoundBT,SIGNAL(clicked()),this,SLOT(GetSound()));

    connect(ui->BlockUpBT,SIGNAL(clicked()),this,SLOT(BlockUp()));
    connect(ui->BlockDownBT,SIGNAL(clicked()),this,SLOT(BlockDown()));

    connect(ui->SlideNameED,SIGNAL(textEdited(QString)),this,SLOT(s_SlideNameChange(QString)));
    connect(ui->NewChapterCB,SIGNAL(stateChanged(int)),this,SLOT(s_NewChapter(int)));
    connect(ui->ShotDurationED,SIGNAL(timeChanged(QTime)),this,SLOT(s_ShotDurationChange(QTime)));

    connect(ui->TextEditBT,SIGNAL(clicked()),this,SLOT(TextEditor()));
    connect(ui->ImageEditCorrectBT,SIGNAL(clicked()),this,SLOT(ImageEditCorrect()));
    connect(ui->VideoEditBT,SIGNAL(clicked()),this,SLOT(VideoEdit()));

    connect(ui->PosXEd,SIGNAL(valueChanged(double)),this,SLOT(s_ChgPosXValue(double)));
    connect(ui->PosYEd,SIGNAL(valueChanged(double)),this,SLOT(s_ChgPosYValue(double)));
    connect(ui->WidthEd,SIGNAL(valueChanged(double)),this,SLOT(s_ChgWidthValue(double)));
    connect(ui->HeightEd,SIGNAL(valueChanged(double)),this,SLOT(s_ChgHeightValue(double)));
    connect(ui->BackgroundFormCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChangeBackgroundForm(int)));
    connect(ui->OpacityCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChangeOpacity(int)));
    connect(ui->PenStyleCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChangePenStyle(int)));
    connect(ui->ShadowEffectCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChgShadowFormValue(int)));
    connect(ui->ShadowEffectED,SIGNAL(valueChanged(int)),this,SLOT(s_ChgShadowDistanceValue(int)));

    connect(ui->RotateXED,SIGNAL(valueChanged(int)),this,SLOT(s_ChgRotateXValue(int))); connect(ui->RotateXSLD,SIGNAL(valueChanged(int)),this,SLOT(s_ChgRotateXValue(int)));
    connect(ui->RotateYED,SIGNAL(valueChanged(int)),this,SLOT(s_ChgRotateYValue(int))); connect(ui->RotateYSLD,SIGNAL(valueChanged(int)),this,SLOT(s_ChgRotateYValue(int)));
    connect(ui->RotateZED,SIGNAL(valueChanged(int)),this,SLOT(s_ChgRotateZValue(int))); connect(ui->RotateZSLD,SIGNAL(valueChanged(int)),this,SLOT(s_ChgRotateZValue(int)));
    connect(ui->PenColorCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChPenColorCB(int)));
    connect(ui->PenSizeEd,SIGNAL(valueChanged(int)),this,SLOT(s_ChgPenSize(int)));
    connect(ui->ShadowColorCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChgShadowColorCB(int)));

    // Shot table part
    connect(ui->ShotTable,SIGNAL(itemSelectionChanged()),this,SLOT(s_ShotTable_SelectionChanged()));
    connect(ui->ShotTable,SIGNAL(DragMoveItem()),this,SLOT(s_ShotTableDragMoveItem()));
    connect(ui->AddShot,SIGNAL(pressed()),this,SLOT(s_ShotTable_AddShot()));
    connect(ui->RemoveShot,SIGNAL(pressed()),this,SLOT(s_ShotTable_RemoveShot()));
    connect(ui->ShotTableMoveLeftBt,SIGNAL(clicked()),this,SLOT(s_ShotTableMoveLeft()));
    connect(ui->ShotTableMoveRightBt,SIGNAL(clicked()),this,SLOT(s_ShotTableMoveRight()));

    // Block table/scene part
    connect(ui->BlockTable,SIGNAL(itemSelectionChanged()),this,SLOT(s_BlockTable_SelectionChanged()));
    connect(ui->BlockTable,SIGNAL(itemDoubleClicked(QTableWidgetItem *)),this,SLOT(s_BlockTable_ItemDoubleClicked(QTableWidgetItem *)));
    connect(ui->AddTextBlock,SIGNAL(pressed()),this,SLOT(s_BlockTable_AddNewTextBlock()));
    connect(ui->AddFileBlock,SIGNAL(pressed()),this,SLOT(s_BlockTable_AddNewFileBlock()));
    connect(ui->RemoveBlock,SIGNAL(pressed()),this,SLOT(s_BlockTable_RemoveBlock()));

    // Style buttons
    connect(ui->FramingStyleCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChangeFramingStyle(int)));
    connect(ui->CoordinateStyleBT,SIGNAL(pressed()),this,SLOT(s_CoordinateStyleBT()));
    connect(ui->BlockShapeStyleBT,SIGNAL(pressed()),this,SLOT(s_BlockShapeStyleBT()));
}

//====================================================================================================================

void DlgSlideProperties::MakeFormIcon(QComboBox *UICB) {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::MakeFormIcon";
    #endif

    for (int i=0;i<UICB->count();i++) {
        cCompositionObject Object(COMPOSITIONTYPE_BACKGROUND,0,GlobalMainWindow->ApplicationConfig);
        Object.Text                     ="";
        Object.x                        =0;
        Object.y                        =0;
        Object.w                        =1;
        Object.h                        =1;
        Object.BackgroundForm           =i+1;
        Object.Opacity                  =4;
        Object.PenSize                  =1;
        Object.PenStyle                 =Qt::SolidLine;
        Object.PenColor                 ="#000000";
        Object.BackgroundBrush->BrushType=BRUSHTYPE_NOBRUSH;
        QPixmap  Image(UICB->iconSize());
        QPainter Painter;
        Painter.begin(&Image);
        Painter.fillRect(QRect(0,0,UICB->iconSize().width(),UICB->iconSize().height()),"#ffffff");
        Object.DrawCompositionObject(&Painter,1,0,0,UICB->iconSize().width(),UICB->iconSize().height(),true,0,0,NULL,1,NULL,false);
        Painter.end();
        UICB->setItemIcon(i,QIcon(Image));
    }
}

//====================================================================================================================

void DlgSlideProperties::MakeBorderStyleIcon(QComboBox *UICB) {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::MakeBorderStyleIcon";
    #endif

    for (int i=0;i<UICB->count();i++) {
        QPixmap  Image(32,32);
        QPainter Painter;
        Painter.begin(&Image);
        Painter.fillRect(QRect(0,0,32,32),"#ffffff");
        QPen Pen;
        Pen.setColor(Qt::black);
        Pen.setStyle((Qt::PenStyle)UICB->itemData(i).toInt());
        Pen.setWidth(2);
        Painter.setPen(Pen);
        Painter.setBrush(QBrush(QColor("#ffffff")));
        Painter.drawLine(0,16,32,16);
        Painter.end();
        UICB->setItemIcon(i,QIcon(Image));
    }
}

//====================================================================================================================

DlgSlideProperties::~DlgSlideProperties() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::~DlgSlideProperties";
    #endif

    Clean();
    StopMAJSpinbox=true;
    if (BackgroundImage!=NULL) {
        delete BackgroundImage;
        BackgroundImage=NULL;
    }
    delete ui;
    delete Undo;
}

//====================================================================================================================

void DlgSlideProperties::Clean() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::Clean";
    #endif

    StopMAJSpinbox=true;
    InScene_SelectionChanged=true;
    // Delete scene and all of it's content, if exist
    if (scene!=NULL) {
        // delete all items
        while (scene->items().count()>0) {
            QGraphicsItem *Item=scene->items().at(0);
            QString       data =Item->data(0).toString();

            scene->removeItem(Item);    // Remove item from the scene

            if (data=="CustomGraphicsRectItem")         delete (cCustomGraphicsRectItem *)Item;
            else if (data=="ResizeGraphicsRectItem")    delete ((cResizeGraphicsRectItem *)Item)->RectItem;
            else if (data=="image")                     delete (QGraphicsPixmapItem *)Item;
            else ExitApplicationWithFatalError("Unkwnon item type in DlgSlideProperties::Clean");
        }

        delete scene;
        scene=NULL;
    }
    InScene_SelectionChanged=false;
    StopMAJSpinbox=false;
}

//====================================================================================================================

void DlgSlideProperties::Help() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::Help";
    #endif

    GlobalMainWindow->OpenHelp(HELPFILE_DlgSlideProperties);
}

//====================================================================================================================

void DlgSlideProperties::resizeEvent(QResizeEvent *) {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::resizeEvent";
    #endif

    if (IsFirstInitDone) RefreshShotTable(ui->ShotTable->currentColumn());      // Fill the ShotTable and select 1st shot
}

//====================================================================================================================

void DlgSlideProperties::SetSavedWindowGeometry() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::SetSavedWindowGeometry";
    #endif

    DiaporamaObject->Parent->ApplicationConfig->DlgSlidePropertiesWSP->ApplyToWindow(this,ui->SplitterTop,ui->SplitterBottom);
}

//====================================================================================================================

void DlgSlideProperties::showEvent(QShowEvent *ev) {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::showEvent";
    #endif

    QDialog::showEvent(ev);
    if (IsFirstInitDone) return;
    QTimer::singleShot(100,this,SLOT(SetSavedWindowGeometry()));
    IsFirstInitDone=true;                                   // Set this flag to true to indicate that now we can prepeare display
    RefreshShotTable(0);                                    // Fill the ShotTable and select 1st shot
}

//====================================================================================================================

void DlgSlideProperties::GetForDisplayUnit(double &DisplayW,double &DisplayH) {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::GetForDisplayUnit";
    #endif

    if (DiaporamaObject->Parent->ImageGeometry==GEOMETRY_4_3)        { DisplayW=1440; DisplayH=1080; }
    else if (DiaporamaObject->Parent->ImageGeometry==GEOMETRY_16_9)  { DisplayW=1920; DisplayH=1080; }
    else if (DiaporamaObject->Parent->ImageGeometry==GEOMETRY_40_17) { DisplayW=1920; DisplayH=816;  }
    else { DisplayW=0; DisplayH=0; }
}

//====================================================================================================================

void DlgSlideProperties::s_NewChapter(int) {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::s_SlideNameChange";
    #endif

    DiaporamaObject->StartNewChapter=ui->NewChapterCB->isChecked();
}

//====================================================================================================================

void DlgSlideProperties::s_SlideNameChange(QString NewText) {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::s_SlideNameChange";
    #endif

    DiaporamaObject->SlideName=NewText;
}

//====================================================================================================================

void DlgSlideProperties::s_ShotDurationChange(QTime NewValue) {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::s_ShotDurationChange";
    #endif

    int Current=ui->ShotTable->currentColumn();
    if ((Current<0)||(Current>=DiaporamaObject->List.count())) return;
    DiaporamaObject->List[Current]->StaticDuration=QTime(0,0,0,0).msecsTo(NewValue);
    if (!StopMAJSpinbox) RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::reject() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::reject";
    #endif

    // Save Window size and position
    DiaporamaObject->Parent->ApplicationConfig->DlgSlidePropertiesWSP->SaveWindowState(this,ui->SplitterTop,ui->SplitterBottom);
    QDomElement root=Undo->documentElement();
    if (root.tagName()=="UNDO-DLG") DiaporamaObject->LoadFromXML(root,"UNDO-DLG-OBJECT","",NULL);
    done(1);
}

//====================================================================================================================

void DlgSlideProperties::accept() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::accept";
    #endif

    // Save Window size and position
    DiaporamaObject->Parent->ApplicationConfig->DlgSlidePropertiesWSP->SaveWindowState(this,ui->SplitterTop,ui->SplitterBottom);
    // Close the box
    done(0);
}

void DlgSlideProperties::OKPrevious() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::OKPrevious";
    #endif

    // Save Window size and position
    DiaporamaObject->Parent->ApplicationConfig->DlgSlidePropertiesWSP->SaveWindowState(this,ui->SplitterTop,ui->SplitterBottom);
    // Close the box
    done(2);
}

void DlgSlideProperties::OKNext() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::OKNext";
    #endif

    // Save Window size and position
    DiaporamaObject->Parent->ApplicationConfig->DlgSlidePropertiesWSP->SaveWindowState(this,ui->SplitterTop,ui->SplitterBottom);
    // Close the box
    done(3);
}

//====================================================================================================================

void DlgSlideProperties::GetSound() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::GetSound";
    #endif

    int CurrentShot=ui->ShotTable->currentColumn();
    if ((CurrentShot>=0)&&(CurrentShot<DiaporamaObject->List.count())) CompositionList=&DiaporamaObject->List[CurrentShot]->ShotComposition;
    int CurrentBlock=ui->BlockTable->currentRow();
    cCompositionObject  *CurrentTextItem=NULL;
    if ((CompositionList)&&(CurrentBlock>=0)&&(CurrentBlock<CompositionList->List.count())) CurrentTextItem=CompositionList->List[CurrentBlock];

    bool IsVideo  =(CurrentTextItem)&&(CurrentTextItem->BackgroundBrush->BrushType==BRUSHTYPE_IMAGEDISK)&&(CurrentTextItem->BackgroundBrush->Video!=NULL);
    bool HaveSound=IsVideo && (CurrentTextItem->BackgroundBrush->SoundVolume!=0);

    // Only if this block is a video and don't have sound yet
    if (IsVideo && !HaveSound) {
        for (int i=0;i<CompositionList->List.count();i++) {
            if ((CurrentTextItem!=CompositionList->List[i])&&(CompositionList->List[i]->BackgroundBrush->BrushType==BRUSHTYPE_IMAGEDISK)&&(CompositionList->List[i]->BackgroundBrush->Video))
                CompositionList->List[i]->BackgroundBrush->SoundVolume=0;
        }
        CurrentTextItem->BackgroundBrush->SoundVolume=1;
        RefreshBlockTable(ui->BlockTable->currentRow());
    }
}

//====================================================================================================================

void DlgSlideProperties::RefreshStyleControls() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::RefreshStyleControls";
    #endif

    if (InRefreshStyleControls) return;
    InRefreshStyleControls=true;

    int CurrentBlock=ui->BlockTable->currentRow();
    cCompositionObject  *CurrentTextItem=NULL;
    if ((CompositionList)&&(CurrentBlock>=0)&&(CurrentBlock<CompositionList->List.count())) CurrentTextItem=CompositionList->List[CurrentBlock];

    bool IsVisible=(CurrentTextItem)&&(CurrentTextItem->IsVisible);
    if (ui->CoordinateStyleBT->isEnabled()!=IsVisible)  ui->CoordinateStyleBT->setEnabled(IsVisible);
    if (ui->CoordinateStyleED->isEnabled()!=IsVisible)  ui->CoordinateStyleED->setEnabled(IsVisible);
    if (ui->BlockShapeStyleBT->isEnabled()!=IsVisible)  ui->BlockShapeStyleBT->setEnabled(IsVisible);
    if (ui->BlockShapeStyleED->isEnabled()!=IsVisible)  ui->BlockShapeStyleED->setEnabled(IsVisible);
    if (ui->FramingStyleCB->isEnabled()!=IsVisible)     ui->FramingStyleCB->setEnabled(IsVisible);
    if (ui->FramingStyleLabel->isEnabled()!=IsVisible)  ui->FramingStyleLabel->setEnabled(IsVisible);

    if (CurrentTextItem) ui->BlockShapeStyleED->setText(GlobalMainWindow->ApplicationConfig->StyleBlockShapeCollection.GetStyleName(CurrentTextItem->GetBlockShapeStyle())); else ui->BlockShapeStyleED->setText("");

    if ((CurrentTextItem==NULL)||((CurrentTextItem->BackgroundBrush->Image==NULL)&&(CurrentTextItem->BackgroundBrush->Video==NULL))) {

        if ((CurrentTextItem!=NULL)&&(CurrentTextItem->BackgroundBrush->Image==NULL)&&(CurrentTextItem->BackgroundBrush->Video==NULL)) {

            // It's a text block
            StopMajFramingStyle=true;
            if (!ui->FramingStyleLabel->isEnabled()) ui->FramingStyleLabel->setEnabled(true);
            if (FramingStyleLabelPixmap!="Geometry.png") {
                FramingStyleLabelPixmap="Geometry.png";
                ui->FramingStyleLabel->setPixmap(QPixmap(QString("img")+QString(QDir::separator())+FramingStyleLabelPixmap));
            }
            if (ui->FramingStyleCB->itemText(0)!=QApplication::translate("DlgImageCorrection","Unlock")) {
                ui->FramingStyleCB->setUpdatesEnabled(false);
                ui->FramingStyleCB->clear();
                ui->FramingStyleCB->addItem(QIcon(ICON_GEOMETRY_UNLOCKED),QApplication::translate("DlgImageCorrection","Unlock"));
                ui->FramingStyleCB->addItem(QIcon(ICON_GEOMETRY_LOCKED),  QApplication::translate("DlgImageCorrection","Lock to this geometry"));
                ui->FramingStyleCB->addItem(QIcon(ICON_GEOMETRY_PROJECT), QApplication::translate("DlgImageCorrection","Lock to project geometry"));
            }
            int Index=1;
            if (!CurrentTextItem->BackgroundBrush->BrushFileCorrect.LockGeometry)                         Index=0;
                else if (CurrentTextItem->BackgroundBrush->BrushFileCorrect.AspectRatio==ProjectGeometry) Index=2;
            if (ui->FramingStyleCB->currentIndex()!=Index) ui->FramingStyleCB->setCurrentIndex(Index);
            if (!ui->FramingStyleCB->updatesEnabled()) ui->FramingStyleCB->setUpdatesEnabled(true);
            StopMajFramingStyle=false;

        } else {

            // No block
            if (ui->FramingStyleLabel->isEnabled()) ui->FramingStyleLabel->setEnabled(false);
            if (FramingStyleLabelPixmap!="EditImage.png") {
                FramingStyleLabelPixmap="EditImage.png";
                ui->FramingStyleLabel->setPixmap(QPixmap(QString("img")+QString(QDir::separator())+FramingStyleLabelPixmap));
            }
            if (ui->FramingStyleCB->isEnabled()) ui->FramingStyleCB->setEnabled(false);
            if ((ui->FramingStyleCB->count()!=1)||(ui->FramingStyleCB->currentIndex()!=0)||(ui->FramingStyleCB->itemText(0)!=QApplication::translate("DlgManageStyle","Custom"))) {
                ui->FramingStyleCB->clear();
                ui->FramingStyleCB->addItem(QIcon(ICON_FRAMING_CUSTOM),QApplication::translate("DlgManageStyle","Custom"));
                ui->FramingStyleCB->setCurrentIndex(0);
            }
        }

    } else {

        if (FramingStyleLabelPixmap!="EditImage.png") {
            FramingStyleLabelPixmap="EditImage.png";
            ui->FramingStyleLabel->setPixmap(QPixmap(QString("img")+QString(QDir::separator())+FramingStyleLabelPixmap));
        }
        CurrentTextItem->BackgroundBrush->InitDefaultFramingStyle(CurrentTextItem->BackgroundBrush->BrushFileCorrect.LockGeometry,CurrentTextItem->BackgroundBrush->BrushFileCorrect.AspectRatio);
        if (CurrentTextItem->BackgroundBrush->Image!=NULL) GlobalMainWindow->ApplicationConfig->StyleImageFramingCollection.SetImageGeometryFilter(GlobalMainWindow->Diaporama->ImageGeometry,CurrentTextItem->BackgroundBrush->Image->ObjectGeometry);
            else if (CurrentTextItem->BackgroundBrush->Video!=NULL) GlobalMainWindow->ApplicationConfig->StyleImageFramingCollection.SetImageGeometryFilter(GlobalMainWindow->Diaporama->ImageGeometry,CurrentTextItem->BackgroundBrush->Video->ObjectGeometry);
        QString StyleDef=CurrentTextItem->GetFramingStyle();
        QString StyleName=GlobalMainWindow->ApplicationConfig->StyleImageFramingCollection.GetStyleName(StyleDef);
        if (StyleName=="") {
            if (StyleDef==CurrentTextItem->BackgroundBrush->DefaultFramingW)      StyleName=QApplication::translate("DlgManageStyle","Adjust to image width");
            else if (StyleDef==CurrentTextItem->BackgroundBrush->DefaultFramingH) StyleName=QApplication::translate("DlgManageStyle","Adjust to image height");
            else if (StyleDef==CurrentTextItem->BackgroundBrush->DefaultFramingF) StyleName=QApplication::translate("DlgManageStyle","Adjust to full image");
            else StyleName=QApplication::translate("DlgManageStyle","Custom");
        }
        StopMajFramingStyle=true;
        GlobalMainWindow->ApplicationConfig->StyleImageFramingCollection.FillCollectionCB(ui->FramingStyleCB,StyleName,true);
        StopMajFramingStyle=false;

    }
    if (CurrentTextItem) ui->CoordinateStyleED->setText(GlobalMainWindow->ApplicationConfig->StyleCoordinateCollection.GetStyleName(CurrentTextItem->GetCoordinateStyle())); else ui->CoordinateStyleED->setText("");
    InRefreshStyleControls=false;
}

//====================================================================================================================

void DlgSlideProperties::RefreshControls() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::RefreshControls";
    #endif

    if (InRefreshControls) return;

    // Ensure box is init and Current contain index of currented selected sequence
    if ((!IsFirstInitDone)||(!CompositionList)) return;

    int CurrentShot=ui->ShotTable->currentColumn();
    if ((CurrentShot<0)||(CurrentShot>=DiaporamaObject->List.count())) return;

    InRefreshControls=true;
    StopMAJSpinbox   =true;    // Avoid controls to send refreshcontrol

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    //--------------------------------------------------------------------
    // Update controls
    //--------------------------------------------------------------------
    CompositionList=&DiaporamaObject->List[CurrentShot]->ShotComposition;
    ui->ShotTableMoveLeftBt->setEnabled(CurrentShot>0);
    ui->ShotTableMoveRightBt->setEnabled(CurrentShot<ui->ShotTable->columnCount()-1);

    int CurrentBlock=ui->BlockTable->currentRow();
    cCompositionObject  *CurrentTextItem=NULL;
    if ((CompositionList)&&(CurrentBlock>=0)&&(CurrentBlock<CompositionList->List.count())) CurrentTextItem=CompositionList->List[CurrentBlock];
    ui->BlockUpBT->setEnabled(CurrentBlock>0);
    ui->BlockDownBT->setEnabled(CurrentBlock<ui->BlockTable->rowCount()-1);
    ui->RemoveShot->setEnabled(DiaporamaObject->List.count()>1);

    ui->CopyBlockBT->setEnabled(CurrentTextItem!=NULL);
    ui->CutBlockBT->setEnabled(CurrentTextItem!=NULL);
    ui->PasteBlockBT->setEnabled(GlobalMainWindow->Clipboard_Block!=NULL);

    // Slide name & duration and shot duration
    ui->SlideNameED->setText(DiaporamaObject->SlideName);
    ui->SlideDurationLabel->setText(QTime(0,0,0,0).addMSecs(DiaporamaObject->GetDuration()).toString("hh:mm:ss.zzz"));
    ui->ShotDurationED->setTime(QTime(0,0,0,0).addMSecs(DiaporamaObject->List[CurrentShot]->StaticDuration));

    // Minimum shot duration display
    int  AddingDuration=0;
    if (CurrentShot==(DiaporamaObject->List.count()-1)) {   // If it's the last shot
        int TotalDuration=DiaporamaObject->GetDuration();
        int Duration=0;
        for (int i=0;i<DiaporamaObject->List.count();i++) Duration=Duration+DiaporamaObject->List[i]->StaticDuration;
        if (Duration<TotalDuration) AddingDuration=TotalDuration-Duration;
    }
    if (AddingDuration==0) ui->MinShotDurationLabel->setText("");
        else ui->MinShotDurationLabel->setText(QString(QApplication::translate("DlgSlideProperties","Lengthened to %1 sec")).arg(double(DiaporamaObject->List[CurrentShot]->StaticDuration+AddingDuration)/1000));

    //====================================================================================================================

    cBrushDefinition    *CurrentBrush   =(CurrentTextItem!=NULL)?CurrentTextItem->BackgroundBrush:NULL;
    bool                IsVisible       =(CurrentTextItem)&&(CurrentTextItem->IsVisible);
    bool                IsVideo         =(CurrentTextItem)&&(CurrentTextItem->BackgroundBrush->BrushType==BRUSHTYPE_IMAGEDISK)&&(CurrentTextItem->BackgroundBrush->Video!=NULL);
    bool                HaveSound       =IsVideo && (CurrentTextItem->BackgroundBrush->SoundVolume!=0);
    bool                Allow_File      =(IsVisible)&&(CurrentBrush!=NULL)&&(CurrentBrush->BrushType==BRUSHTYPE_IMAGEDISK);

    ui->VisibleBT->setIcon(QIcon(QString(!IsVisible?ICON_VISIBLE_OK:ICON_VISIBLE_KO)));
    ui->SoundBT->setIcon(QIcon(QString(!HaveSound?ICON_SOUND_OK:ICON_SOUND_KO)));
    ui->TVMarginsBT->setIcon(QIcon(QString(MagneticRuler.MagneticRuler?ICON_RULER_ON:ICON_RULER_OFF)));
    ui->SoundBT->setEnabled(IsVideo && !HaveSound);

    ui->ImageEditCorrectBT->setEnabled(IsVisible && Allow_File);
    ui->VideoEditBT->setEnabled(IsVisible && Allow_File && (CurrentBrush->Video!=NULL));
    ui->TextEditBT->setEnabled(IsVisible);
    ui->VisibleBT->setEnabled(CurrentTextItem!=NULL);

    ui->RemoveBlock->setEnabled(CurrentTextItem!=NULL);

    //***********************
    // Size & Position
    //***********************
    ui->PosXEd->setEnabled(IsVisible);
    ui->PosYEd->setEnabled(IsVisible);
    ui->WidthEd->setEnabled(IsVisible);
    ui->HeightEd->setEnabled(IsVisible);
    ui->RotateXED->setEnabled(IsVisible);
    ui->RotateXSLD->setEnabled(IsVisible);
    ui->RotateYED->setEnabled(IsVisible);
    ui->RotateYSLD->setEnabled(IsVisible);
    ui->RotateZED->setEnabled(IsVisible);
    ui->RotateZSLD->setEnabled(IsVisible);
    ui->PosSize_X->setEnabled(IsVisible);
    ui->PosSize_Y->setEnabled(IsVisible);
    ui->PosSize_Width->setEnabled(IsVisible);
    ui->PosSize_Height->setEnabled(IsVisible);
    ui->Rotate_X->setEnabled(IsVisible);
    ui->Rotate_Y->setEnabled(IsVisible);
    ui->Rotate_Z->setEnabled(IsVisible);

    //***********************
    // Shape part
    //***********************
    ui->BackgroundFormCB->setEnabled(IsVisible);
    ui->PenSizeEd->setEnabled(IsVisible);
    ui->PenColorCB->setEnabled(IsVisible&&(CurrentTextItem)&&(CurrentTextItem->PenSize!=0));
    ui->PenStyleCB->setEnabled(IsVisible&&(CurrentTextItem)&&(CurrentTextItem->PenSize!=0));
    ui->OpacityCB->setEnabled(IsVisible);
    ui->ShadowEffectCB->setEnabled(IsVisible);
    ui->ShadowEffectED->setEnabled((IsVisible)&&(CurrentTextItem)&&(CurrentTextItem->FormShadow!=0));
    ui->ShadowColorCB->setEnabled((IsVisible)&&(CurrentTextItem)&&(CurrentTextItem->FormShadow!=0));

    // Refresh Scene Image
    RefreshSceneImage();

    // Refresh information zone
    UpdateDockInfo();

    QApplication::restoreOverrideCursor();

    StopMAJSpinbox   =false;    // Allow controls to send refreshcontrol
    InRefreshControls=false;
}

//====================================================================================================================
// Refresh background image of the scene

void DlgSlideProperties::s_RefreshSceneImage() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::s_RefreshSceneImage";
    #endif
    RefreshSceneImage();
}

void DlgSlideProperties::RefreshSceneImage() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::RefreshSceneImage";
    #endif

    if (InRefreshSceneImage) return;
    InRefreshSceneImage=true;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    // Get current shot object composition list
    int                 CurrentShot     =ui->ShotTable->currentColumn();
    cCompositionList    *CurrentList    =NULL;
    if ((CurrentShot>=0)&&(CurrentShot<DiaporamaObject->List.count())) CurrentList=&DiaporamaObject->List[CurrentShot]->ShotComposition;

    // Get current block object in the current shot object composition list
    int                 CurrentBlock    =ui->BlockTable->currentRow();
    cCompositionObject  *CurrentTextItem=NULL;
    if ((CurrentList)&&(CurrentBlock>=0)&&(CurrentBlock<CurrentList->List.count())) CurrentTextItem=CurrentList->List[CurrentBlock];

    // Draw image of the scene under the background
    QPixmap NewImage=QPixmap::fromImage(*BackgroundImage);
    QPainter P;
    P.begin(&NewImage);

    double SizeRatio=double(BackgroundImage->width())/double(ui->GraphicsView->width());
    WithPen  =int(SizeRatio); if (double(WithPen)<SizeRatio) WithPen++;

    for (int i=0;i<CurrentList->List.count();i++) {
        // Draw composition block
        if (CurrentList->List[i]->IsVisible) {
            if (CurrentList->List[i]->BackgroundBrush->Video) {
                // If it's a video block, calc video position
                int StartVideoPos=QTime(0,0,0,0).msecsTo(CurrentList->List[i]->BackgroundBrush->Video->StartPos);
                for (int k=0;k<CurrentShot;k++) {
                    for (int l=0;l<DiaporamaObject->List[k]->ShotComposition.List.count();l++) {
                        if (DiaporamaObject->List[k]->ShotComposition.List[l]->IndexKey==CurrentList->List[i]->IndexKey) {
                            if (DiaporamaObject->List[k]->ShotComposition.List[l]->IsVisible) StartVideoPos+=DiaporamaObject->List[k]->StaticDuration;
                            l=DiaporamaObject->List[k]->ShotComposition.List.count();    // stop loop
                        }
                    }
                }
                CurrentList->List[i]->DrawCompositionObject(&P,double(ymax)/double(1080),0,0,xmax,ymax,true,0,StartVideoPos,NULL,1,NULL,true);
            } else CurrentList->List[i]->DrawCompositionObject(&P,double(ymax)/double(1080),0,0,xmax,ymax,true,0,0,NULL,1,NULL,true);
        }

        // Draw frame border
        if (CurrentTextItem==CurrentList->List[i]) {
            // draw rect out of the rectangle
            if (CurrentList->List[i]->IsVisible) {
                P.setCompositionMode(QPainter::RasterOp_SourceXorDestination);
                QPen pen=QPen(Qt::red);
                pen.setWidth(WithPen);
                pen.setStyle(Qt::DotLine);
                P.setPen(pen);
                P.setBrush(Qt::NoBrush);
                P.drawRect(QRectF(CurrentTextItem->x*xmax,CurrentTextItem->y*ymax,CurrentTextItem->w*xmax,CurrentTextItem->h*ymax));
                P.setCompositionMode(QPainter::CompositionMode_SourceOver);
            }

            StopMAJSpinbox=true;    // Avoid controls to send refreshcontrol

            // Update controls with position & size value
            if (DiaporamaObject->Parent->ApplicationConfig->DisplayUnit==DISPLAYUNIT_PERCENT) {
                ui->PosXEd->setRange(-200,200);      ui->PosXEd->setValue(CurrentTextItem->x*100);
                ui->PosYEd->setRange(-200,200);      ui->PosYEd->setValue(CurrentTextItem->y*100);
                ui->WidthEd->setRange(3,200);        ui->WidthEd->setValue(CurrentTextItem->w*100);
                ui->HeightEd->setRange(3,200);       ui->HeightEd->setValue(CurrentTextItem->h*100);
            } else { // DisplayUnit==DISPLAYUNIT_PIXELS
                double DisplayW=0,DisplayH=0;
                GetForDisplayUnit(DisplayW,DisplayH);
                ui->PosXEd->setRange(-2*DisplayW,2*DisplayW);   ui->PosXEd->setValue(  CurrentTextItem->x*DisplayW);
                ui->PosYEd->setRange(-2*DisplayH,2*DisplayH);   ui->PosYEd->setValue(  CurrentTextItem->y*DisplayH);
                ui->WidthEd->setRange(3,2*DisplayW);            ui->WidthEd->setValue( CurrentTextItem->w*DisplayW);
                ui->HeightEd->setRange(3,2*DisplayH);           ui->HeightEd->setValue(CurrentTextItem->h*DisplayH);
            }
            ui->RotateXED->setValue(CurrentTextItem->RotateXAxis);      ui->RotateXSLD->setValue(CurrentTextItem->RotateXAxis);
            ui->RotateYED->setValue(CurrentTextItem->RotateYAxis);      ui->RotateYSLD->setValue(CurrentTextItem->RotateYAxis);
            ui->RotateZED->setValue(CurrentTextItem->RotateZAxis);      ui->RotateZSLD->setValue(CurrentTextItem->RotateZAxis);

            if ((CurrentTextItem->BackgroundBrush->BrushType==BRUSHTYPE_IMAGEDISK)&&(!CurrentTextItem->BackgroundBrush->BrushFileCorrect.LockGeometry))
                CurrentTextItem->BackgroundBrush->BrushFileCorrect.AspectRatio=(CurrentTextItem->h*ymax)/(CurrentTextItem->w*xmax);

            //***********************
            // Shape TAB
            //***********************
            if (CurrentTextItem->BackgroundForm-1!=ui->BackgroundFormCB->currentIndex())    ui->BackgroundFormCB->setCurrentIndex(CurrentTextItem->BackgroundForm-1);
            if (CurrentTextItem->PenSize!=ui->PenSizeEd->value())                           ui->PenSizeEd->setValue(int(CurrentTextItem->PenSize));
            if (CurrentTextItem->Opacity!=ui->OpacityCB->currentIndex())                    ui->OpacityCB->setCurrentIndex(CurrentTextItem->Opacity);
            if (CurrentTextItem->FormShadow!=ui->ShadowEffectCB->currentIndex())            ui->ShadowEffectCB->setCurrentIndex(CurrentTextItem->FormShadow);
            if (CurrentTextItem->FormShadowDistance!=ui->ShadowEffectED->value())           ui->ShadowEffectED->setValue(int(CurrentTextItem->FormShadowDistance));
            ui->PenColorCB->SetCurrentColor(&CurrentTextItem->PenColor);
            ui->ShadowColorCB->SetCurrentColor(&CurrentTextItem->FormShadowColor);

            for (int i=0;i<ui->PenStyleCB->count();i++) if (ui->PenStyleCB->itemData(i).toInt()==CurrentTextItem->PenStyle) {
                if (i!=ui->PenStyleCB->currentIndex()) ui->PenStyleCB->setCurrentIndex(i);
                break;
            }

            StopMAJSpinbox=false;    // Allow controls to send refreshcontrol

        } else {
            if (CurrentList->List[i]->IsVisible) {
                // draw rect out of the rectangle
                P.setCompositionMode(QPainter::RasterOp_SourceXorDestination);
                QPen pen=QPen(Qt::lightGray);
                pen.setWidth(WithPen);
                pen.setStyle(Qt::DotLine);
                P.setPen(pen);
                P.setBrush(Qt::NoBrush);
                P.drawRect(QRectF(CurrentList->List[i]->x*xmax,CurrentList->List[i]->y*ymax,CurrentList->List[i]->w*xmax,CurrentList->List[i]->h*ymax));
                P.setCompositionMode(QPainter::CompositionMode_SourceOver);
            }
        }
    }

    // Draw rullers if they was enabled
    if (MagneticRuler.MagneticRuler==true) {
        QColor col=QColor(0,255,0);
        QPen   pen=QPen(col);
        pen.setWidth(WithPen);
        pen.setJoinStyle(Qt::RoundJoin);
        pen.setStyle(Qt::DotLine);
        P.setPen(pen);
        P.setCompositionMode(QPainter::RasterOp_SourceXorDestination);
        P.setBrush(Qt::NoBrush);
        P.drawLine(MagneticRuler.MagnetX1,0,MagneticRuler.MagnetX1,ymax);
        P.drawLine(0,MagneticRuler.MagnetY1,xmax,MagneticRuler.MagnetY1);
        P.drawLine(MagneticRuler.MagnetX2,0,MagneticRuler.MagnetX2,ymax);
        P.drawLine(0,MagneticRuler.MagnetY2,xmax,MagneticRuler.MagnetY2);
        P.drawLine(MagneticRuler.MagnetX3,0,MagneticRuler.MagnetX3,ymax);
        P.drawLine(0,MagneticRuler.MagnetY3,xmax,MagneticRuler.MagnetY3);
        P.setCompositionMode(QPainter::CompositionMode_SourceOver);
    }

    P.end();

    // Remove old image if exist
    int i=0;
    while ((i<scene->items().count())&&(scene->items().at(i)->data(0).toString()!="image")) i++;
    if ((i<scene->items().count())&&(scene->items().at(i)->data(0).toString()=="image")) {
        QGraphicsPixmapItem *im=(QGraphicsPixmapItem *)scene->items().at(i);
        scene->removeItem(im);
        delete im;
    }

    // Add image to the background of the scene
    QGraphicsPixmapItem *im=scene->addPixmap(NewImage);    // Image will be delete by scene
    im->setData(0,QVariant(QString("image")));
    im->setZValue(200);
    im->setPos(0,0);
    //delete NewImage;

    // Refresh thumbnail
    ui->ShotTable->setUpdatesEnabled(false);
    ui->ShotTable->setUpdatesEnabled(true);

    RefreshStyleControls();

    QApplication::restoreOverrideCursor();
    InRefreshSceneImage=false;
}

//====================================================================================================================
// Refresh the ShotTable

void DlgSlideProperties::RefreshShotTable(int SetCurrentIndex) {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::RefreshShotTable";
    #endif

    int CellHeight=ui->ShotTable->rowHeight(0);
    int CellWidth =GlobalMainWindow->Diaporama->GetWidthForHeight(CellHeight);

    StopMAJSpinbox=true;
    ui->ShotTable->setUpdatesEnabled(false);
    while (ui->ShotTable->columnCount()>0) ui->ShotTable->removeColumn(0);    // Clear all
    for (int i=0;i<DiaporamaObject->List.count();i++) {
        ui->ShotTable->insertColumn(i);
        wgt_QCustomThumbnails *Object=new wgt_QCustomThumbnails(ui->ShotTable,THUMBNAILTYPE_SHOT);
        Object->DiaporamaObject=DiaporamaObject;
        ui->ShotTable->setCellWidget(0,i,Object);
        ui->ShotTable->setColumnWidth(i,CellWidth);
    }
    ui->ShotTable->setUpdatesEnabled(true);
    StopMAJSpinbox=false;

    ui->ShotTable->setCurrentCell(0,SetCurrentIndex);
}

//====================================================================================================================
// Refresh the BlockTable

void DlgSlideProperties::RefreshBlockTable(int SetCurrentIndex) {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::RefreshBlockTable";
    #endif

    // Calculate scene size
    GetForDisplayUnit(xmax,ymax);
    StopMAJSpinbox          =true;

    // Delete scene and all of it's content, if exist
    if (scene) while (scene->items().count()>0) {
        QGraphicsItem *Item=scene->items().at(0);
        QString       data =Item->data(0).toString();
        scene->removeItem(Item);
        if (data=="CustomGraphicsRectItem")         delete (cCustomGraphicsRectItem *)Item;
        else if (data=="image")                     delete (QGraphicsPixmapItem *)Item;
        else if (data!="ResizeGraphicsRectItem")    ExitApplicationWithFatalError("Unkwnon item type in DlgSlideProperties::Clean");
    }

    // Ensure scene is created. If not : create it
    if (!scene) {
        // Create the scene and setup scene to control
        scene = new QGraphicsScene();
        ui->GraphicsView->setScene(scene);
        ui->GraphicsView->setInteractive(true);
        ui->GraphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
        connect(scene,SIGNAL(selectionChanged()),this,SLOT(s_Scene_SelectionChanged()));
    }
    ui->GraphicsView->fitInView(QRectF(0,0,xmax,ymax),Qt::KeepAspectRatio);

    // Check if BackgroundImage have correct size
    if ((BackgroundImage)&&((xmax!=BackgroundImage->width())||(ymax!=BackgroundImage->height()))) {
        delete BackgroundImage;
        BackgroundImage=NULL;
    }

    // Prepare BackgroundImage if not exist
    if (!BackgroundImage) {
        QPainter Painter;
        BackgroundImage=new QImage(xmax,ymax,QImage::Format_ARGB32_Premultiplied);
        Painter.begin(BackgroundImage);
        DiaporamaObject->Parent->PrepareBackground(DiaporamaObject->Parent->GetObjectIndex(DiaporamaObject),xmax,ymax,&Painter,0,0);
        Painter.end();

        // Ensure scene size if correct
        scene->setSceneRect(QRectF(0,0,xmax,ymax));
    }

    // define 5% TV Margins
    MagneticRuler.MagnetX1=xmax*0.05;
    MagneticRuler.MagnetY1=ymax*0.05;
    MagneticRuler.MagnetX2=xmax-xmax*0.05;
    MagneticRuler.MagnetY2=ymax-ymax*0.05;

    // define centering ruller
    MagneticRuler.MagnetX3=xmax*0.5;
    MagneticRuler.MagnetY3=ymax*0.5;

    // Fill the scene with block item by creating cCustomGraphicsRectItem associate to existing cCompositionObject
    NextZValue=500;
    for (int i=0;i<CompositionList->List.count();i++) if (!CompositionList->List[i]->IsVisible) { // First loop for invisible items
        // Create and add to scene a cCustomGraphicsRectItem
        new cCustomGraphicsRectItem(scene,NextZValue,&CompositionList->List[i]->x,&CompositionList->List[i]->y,
                    NULL,&CompositionList->List[i]->w,&CompositionList->List[i]->h,xmax,ymax,
                    ((!CompositionList->List[i]->BackgroundBrush->BrushFileCorrect.LockGeometry)&&
                    (CompositionList->List[i]->BackgroundBrush->BrushType!=BRUSHTYPE_IMAGEDISK))?false:true,
                    CompositionList->List[i]->BackgroundBrush->BrushFileCorrect.AspectRatio,
                    &MagneticRuler,this,TYPE_DlgSlideProperties,CompositionList->List[i]->IndexKey,false);

        NextZValue+=10;  // 10 by 10 step for ZValue
    }
    for (int i=0;i<CompositionList->List.count();i++) if (CompositionList->List[i]->IsVisible) { // Second loop for visible items
        // Create and add to scene a cCustomGraphicsRectItem
        new cCustomGraphicsRectItem(scene,NextZValue,&CompositionList->List[i]->x,&CompositionList->List[i]->y,
                    NULL,&CompositionList->List[i]->w,&CompositionList->List[i]->h,xmax,ymax,
                    ((!CompositionList->List[i]->BackgroundBrush->BrushFileCorrect.LockGeometry)&&
                    (CompositionList->List[i]->BackgroundBrush->BrushType!=BRUSHTYPE_IMAGEDISK))?false:true,
                    CompositionList->List[i]->BackgroundBrush->BrushFileCorrect.AspectRatio,
                    &MagneticRuler,this,TYPE_DlgSlideProperties,CompositionList->List[i]->IndexKey,true);

        NextZValue+=10;  // 10 by 10 step for ZValue
    }

    ui->BlockTable->setUpdatesEnabled(false);
    while (ui->BlockTable->rowCount()>0) ui->BlockTable->removeRow(0);    // Clear all
    for (int i=0;i<CompositionList->List.count();i++) {
        ui->BlockTable->insertRow(i);
        cCompositionObject  *CompoObject=CompositionList->List[i];
        QTableWidgetItem    *ItemC0,*ItemC1,*ItemC2;
        // Image block
        if ((CompoObject->BackgroundBrush->BrushType==BRUSHTYPE_IMAGEDISK)&&(CompoObject->BackgroundBrush->Image)) {
            ItemC0=new QTableWidgetItem(QIcon(CompoObject->IsVisible?ICON_OBJECT_IMAGE:ICON_OBJECT_IMAGEHIDE),"");
            ItemC1=new QTableWidgetItem("");
            ItemC2=new QTableWidgetItem(QFileInfo(CompoObject->BackgroundBrush->Image?CompoObject->BackgroundBrush->Image->FileName:CompoObject->BackgroundBrush->Video?CompoObject->BackgroundBrush->Video->FileName:"").fileName());
        // Video block
        } else if ((CompoObject->BackgroundBrush->BrushType==BRUSHTYPE_IMAGEDISK)&&(CompoObject->BackgroundBrush->Video)) {
            ItemC0=new QTableWidgetItem(QIcon(CompoObject->IsVisible?ICON_OBJECT_MOVIE:ICON_OBJECT_MOVIEHIDE),"");
            ItemC1=CompoObject->BackgroundBrush->SoundVolume!=0?new QTableWidgetItem(QIcon(ICON_SOUND_OK),""):new QTableWidgetItem("");
            ItemC2=new QTableWidgetItem(QFileInfo(CompoObject->BackgroundBrush->Image?CompoObject->BackgroundBrush->Image->FileName:CompoObject->BackgroundBrush->Video?CompoObject->BackgroundBrush->Video->FileName:"").fileName());
        // Text block
        } else {
            ItemC0=new QTableWidgetItem(QIcon(CompoObject->IsVisible?ICON_OBJECT_TEXT:ICON_OBJECT_TEXTHIDE),"");
            ItemC1=new QTableWidgetItem("");
            ItemC2=new QTableWidgetItem(CompoObject->Text);
        }
        if (!CompoObject->IsVisible) {
            QFont  font=QFont("Sans serif",9,QFont::Normal,QFont::StyleItalic);
            ItemC2->setFont(font);
        }
        ui->BlockTable->setItem(i,0,ItemC0);
        ui->BlockTable->setItem(i,1,ItemC1);
        ui->BlockTable->setItem(i,2,ItemC2);
    }
    ui->BlockTable->setColumnWidth(0,24);
    ui->BlockTable->setColumnWidth(1,24);
    ui->BlockTable->resizeRowsToContents();
    ui->BlockTable->setUpdatesEnabled(true);
    ui->BlockTable->setCurrentCell(SetCurrentIndex,0);      // Changing current index make a call to RefreshControls();

    StopMAJSpinbox          =false;

    if (ui->BlockTable->rowCount()==0) RefreshControls();   // except if no block in the list
}

//====================================================================================================================

void DlgSlideProperties::RefreshSceneImageAndCache(cCompositionObject *CurrentTextItem,cBrushDefinition *CurrentBrush) {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::RefreshSceneImageAndCache";
    #endif

    if (CurrentBrush->Image) {
        cLuLoImageCacheObject *ImageObject=CurrentBrush->Image->ApplicationConfig->ImagesCache.FindObject(CurrentBrush->Image->FileName,&CurrentBrush->Image->BrushFileTransform,GlobalMainWindow->ApplicationConfig->Smoothing,true);
        ImageObject->ClearAll();
    } else if (CurrentBrush->Video) {
        if (CurrentBrush->Video->CacheFirstImage) {
            delete CurrentBrush->Video->CacheFirstImage;
            CurrentBrush->Video->CacheFirstImage=NULL;
        }
        if (CurrentBrush->Video->CacheLastImage) {
            delete CurrentBrush->Video->CacheLastImage;
            CurrentBrush->Video->CacheLastImage=NULL;
        }
        for (int i=0;i<DiaporamaObject->List.count();i++) for (int j=0;j<DiaporamaObject->List[i]->ShotComposition.List.count();j++)
            if ((DiaporamaObject->List[i]->ShotComposition.List[j]->IndexKey==CurrentTextItem->IndexKey)&&(DiaporamaObject->List[i]->ShotComposition.List[j]->CachedBrushBrush)) {
            delete DiaporamaObject->List[i]->ShotComposition.List[j]->CachedBrushBrush;
            DiaporamaObject->List[i]->ShotComposition.List[j]->CachedBrushBrush=NULL;
        }
    }
    RefreshSceneImage();
}

//====================================================================================================================
// Display or hide TV Margins
//====================================================================================================================

void DlgSlideProperties::s_TVMarginsBt() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::s_TVMarginsBt";
    #endif

    if (MagneticRuler.MagneticRuler==true) MagneticRuler.MagneticRuler=false; else MagneticRuler.MagneticRuler=true;
    DiaporamaObject->Parent->ApplicationConfig->SlideRuler=MagneticRuler.MagneticRuler;
    RefreshControls();
}

//====================================================================================================================
// Update dock informations
//====================================================================================================================

void DlgSlideProperties::UpdateDockInfo() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::UpdateDockInfo";
    #endif

    ui->TableInfo->setUpdatesEnabled(false);               // To allow and force a general update
    while (ui->TableInfo->rowCount()>0) ui->TableInfo->removeRow(0);
    int CurrentBlock=ui->BlockTable->currentRow();
    cCompositionObject  *CurrentTextItem=NULL;
    if ((CompositionList)&&(CurrentBlock>=0)&&(CurrentBlock<CompositionList->List.count())) CurrentTextItem=CompositionList->List[CurrentBlock];
    if (CurrentTextItem) {
        ui->TableInfo->insertRow(ui->TableInfo->rowCount());
        ui->TableInfo->setItem(ui->TableInfo->rowCount()-1,0,new QTableWidgetItem(QApplication::translate("DlgSlideProperties","Object type")));
        ui->TableInfo->setItem(ui->TableInfo->rowCount()-1,1,new QTableWidgetItem(CurrentTextItem->BackgroundBrush->BrushType!=BRUSHTYPE_IMAGEDISK?QApplication::translate("DlgSlideProperties","Title"):
                                                                                  CurrentTextItem->BackgroundBrush->Image!=NULL?QApplication::translate("DlgSlideProperties","Image"):
                                                                                  QApplication::translate("DlgSlideProperties","Video")));

        if (CurrentTextItem->BackgroundBrush->BrushType==BRUSHTYPE_IMAGEDISK) {
            ui->TableInfo->insertRow(ui->TableInfo->rowCount());
            ui->TableInfo->setItem(ui->TableInfo->rowCount()-1,0,new QTableWidgetItem(QApplication::translate("DlgSlideProperties","Filename")));
            ui->TableInfo->setItem(ui->TableInfo->rowCount()-1,1,new QTableWidgetItem(CurrentTextItem->BackgroundBrush->Image!=NULL?QFileInfo(CurrentTextItem->BackgroundBrush->Image->FileName).fileName():
                                                                                      CurrentTextItem->BackgroundBrush->Video!=NULL?QFileInfo(CurrentTextItem->BackgroundBrush->Video->FileName).fileName():""));
            QStringList *Info=NULL;
            if (CurrentTextItem->BackgroundBrush->Image!=NULL) {
                // If exiv value not loaded then call exiv2 now !
                if (CurrentTextItem->BackgroundBrush->Image->InformationList.count()==0) {
                    int ImageOrientation=1;
                    CurrentTextItem->BackgroundBrush->Image->CallEXIF(ImageOrientation);
                }
                Info=&CurrentTextItem->BackgroundBrush->Image->InformationList;
            } else if (CurrentTextItem->BackgroundBrush->Video!=NULL) {
                ui->TableInfo->insertRow(ui->TableInfo->rowCount());
                ui->TableInfo->setItem(ui->TableInfo->rowCount()-1,0,new QTableWidgetItem(QApplication::translate("DlgSlideProperties","Image size")));
                ui->TableInfo->setItem(ui->TableInfo->rowCount()-1,1,new QTableWidgetItem(QString("%1x%2").arg(CurrentTextItem->BackgroundBrush->Video->ImageWidth).arg(CurrentTextItem->BackgroundBrush->Video->ImageHeight)));
                ////////////////// On peut en rajouter ICI!
                Info=&CurrentTextItem->BackgroundBrush->Video->InformationList;
            }
            // Fill table with Information List
            if (Info) for (int i=0;i<Info->count();i++) {
                ui->TableInfo->insertRow(ui->TableInfo->rowCount());
                QStringList Value=((QString)Info->at(i)).split("##");
                if (((QString)Value[0]).indexOf(".")!=-1) {
                    ui->TableInfo->setItem(ui->TableInfo->rowCount()-1,0,new QTableWidgetItem(((QString)Value[0]).mid(((QString)Value[0]).lastIndexOf(".")+1)));
                    ui->TableInfo->item(ui->TableInfo->rowCount()-1,0)->setToolTip(Value[0]);
                } else ui->TableInfo->setItem(ui->TableInfo->rowCount()-1,0,new QTableWidgetItem(Value[0]));
                ui->TableInfo->setItem(ui->TableInfo->rowCount()-1,1,new QTableWidgetItem(Value[1]));
                ui->TableInfo->item(ui->TableInfo->rowCount()-1,1)->setToolTip(Value[0]);
            }
        }
        ui->TableInfo->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
        ui->TableInfo->setVisible(false);                      // To ensure all items of all columns are used to compute size
        ui->TableInfo->resizeColumnsToContents();              // Resize column widht
        ui->TableInfo->resizeRowsToContents();                 // Resize row height
        ui->TableInfo->setVisible(true);                       // To allow display
        ui->TableInfo->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
    }
    ui->TableInfo->setUpdatesEnabled(true);                // To allow and force a general update
}

//====================================================================================================================

cCompositionObject *DlgSlideProperties::GetSelectedGlobalCompositionObject() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::GetSelectedGlobalCompositionObject";
    #endif

    int CurrentBlock=ui->BlockTable->currentRow();
    if ((CurrentBlock<0)||(CurrentBlock>=CompositionList->List.count())) return NULL;
    int i=0;
    while ((i<DiaporamaObject->ObjectComposition.List.count())&&(DiaporamaObject->ObjectComposition.List[i]->IndexKey!=CompositionList->List[CurrentBlock]->IndexKey)) i++;
    if (i<DiaporamaObject->ObjectComposition.List.count()) return DiaporamaObject->ObjectComposition.List[i]; else return NULL;
}

//====================================================================================================================

cCompositionObject *DlgSlideProperties::GetSelectedCompositionObject() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::GetSelectedCompositionObject";
    #endif

    int CurrentBlock=ui->BlockTable->currentRow();
    if ((CurrentBlock<0)||(CurrentBlock>=CompositionList->List.count())) return NULL; else return CompositionList->List[CurrentBlock];
}

//====================================================================================================================
// Handler for position, size & rotation controls
//====================================================================================================================

//========= Ensure current context is ok for modifing properties
bool DlgSlideProperties::PrepContexte() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::PrepContexte";
    #endif
    if (StopMAJSpinbox) return false;

    // Get current block object
    int CurrentBlock=ui->BlockTable->currentRow();
    if ((CurrentBlock<0)||(CurrentBlock>=CompositionList->List.count())) CompositionObject=NULL; else CompositionObject=CompositionList->List[CurrentBlock];
    if ((!CompositionObject)||(!CompositionObject->IsVisible)) return false;

    // Get current Item selected in the scene
    if (scene==NULL) return NULL;
    RectItem=NULL;
    for (int i=0;i<scene->selectedItems().count();i++) {
        QGraphicsItem   *Item=scene->selectedItems()[i];
        QString         data =Item->data(0).toString();
        if (data=="CustomGraphicsRectItem")      RectItem=(cCustomGraphicsRectItem *)Item;
        else if (data=="ResizeGraphicsRectItem") RectItem=((cResizeGraphicsRectItem *)Item)->RectItem;
    }
    if (RectItem==NULL) return false;
    // Ensure we have correcte value for DisplayW and DisplayH
    GetForDisplayUnit(DisplayW,DisplayH);
    return true;
}

//========= Apply modification
void DlgSlideProperties::ApplyToContexte(bool ReposRectItem,bool ApplyGlobal) {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::ApplyToContexte";
    #endif
    if (ApplyGlobal) ApplyGlobalPropertiesToAllShots(CompositionObject);
    if (ReposRectItem) {
        RectItem->setPos(CompositionObject->x*xmax,CompositionObject->y*ymax);
        QRectF Rect=RectItem->mapRectFromScene(QRectF(CompositionObject->x*xmax,CompositionObject->y*ymax,xmax*CompositionObject->w,ymax*CompositionObject->h));
        RectItem->setRect(Rect);
        RectItem->RecalcEmbededResizeRectItem();
    }
    RefreshSceneImage();
}

//========= X position
void DlgSlideProperties::s_ChgPosXValue(double Value) {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::s_ChgPosXValue";
    #endif
    if (!PrepContexte()) return;
    if (DiaporamaObject->Parent->ApplicationConfig->DisplayUnit==DISPLAYUNIT_PERCENT) CompositionObject->x=Value/100;           // DisplayUnit==DISPLAYUNIT_PERCENT
        else                                                                          CompositionObject->x=(Value/DisplayW);    // DisplayUnit==DISPLAYUNIT_PIXELS
    ApplyToContexte(true,false);
}

//========= Y position
void DlgSlideProperties::s_ChgPosYValue(double Value) {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::s_ChgPosYValue";
    #endif
    if (!PrepContexte()) return;
    if (DiaporamaObject->Parent->ApplicationConfig->DisplayUnit==DISPLAYUNIT_PERCENT) CompositionObject->y=Value/100;           // DisplayUnit==DISPLAYUNIT_PERCENT
        else                                                                          CompositionObject->y=(Value/DisplayH);    // DisplayUnit==DISPLAYUNIT_PIXELS
    ApplyToContexte(true,false);
}

//========= Width
void DlgSlideProperties::s_ChgWidthValue(double Value) {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::s_ChgWidthValue";
    #endif
    if (!PrepContexte()) return;
    if (DiaporamaObject->Parent->ApplicationConfig->DisplayUnit==DISPLAYUNIT_PERCENT) CompositionObject->w=Value/100;           // DisplayUnit==DISPLAYUNIT_PERCENT
        else CompositionObject->w=(Value/DisplayW);                                                                             // DisplayUnit==DISPLAYUNIT_PIXELS
    if (RectItem->KeepAspectRatio) CompositionObject->h=((CompositionObject->w*DisplayW)*RectItem->AspectRatio)/DisplayH;
    ApplyToContexte(true,false);
}

//========= Height
void DlgSlideProperties::s_ChgHeightValue(double Value) {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::s_ChgHeightValue";
    #endif
    if (!PrepContexte()) return;
    if (DiaporamaObject->Parent->ApplicationConfig->DisplayUnit==DISPLAYUNIT_PERCENT) CompositionObject->h=Value/100;           // DisplayUnit==DISPLAYUNIT_PERCENT
        else CompositionObject->h=(Value/DisplayH);                                                                             // DisplayUnit==DISPLAYUNIT_PIXELS
    if (RectItem->KeepAspectRatio) CompositionObject->w=((CompositionObject->h*DisplayH)/RectItem->AspectRatio)/DisplayW;
    ApplyToContexte(true,false);
}

//========= X Rotation value
void DlgSlideProperties::s_ChgRotateXValue(int Value) {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::s_ChgRotateXValue";
    #endif
    if (!PrepContexte()) return;
    CompositionObject->RotateXAxis=Value;
    ApplyToContexte(false,false);
}

//========= Y Rotation value
void DlgSlideProperties::s_ChgRotateYValue(int Value) {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::s_ChgRotateYValue";
    #endif
    if (!PrepContexte()) return;
    CompositionObject->RotateYAxis=Value;
    ApplyToContexte(false,false);
}

//========= Z Rotation value
void DlgSlideProperties::s_ChgRotateZValue(int Value) {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::s_ChgRotateZValue";
    #endif
    if (!PrepContexte()) return;
    CompositionObject->RotateZAxis=Value;
    ApplyToContexte(false,false);
}

//========= Background forme
void DlgSlideProperties::s_ChangeBackgroundForm(int Style) {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::s_ChangeBackgroundForm";
    #endif
    if (!PrepContexte()) return;
    CompositionObject->BackgroundForm=Style+1;
    ApplyToContexte(false,true);
}

//========= Opacity
void DlgSlideProperties::s_ChangeOpacity(int Style) {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::s_ChangeOpacity";
    #endif
    if (!PrepContexte()) return;
    CompositionObject->Opacity=Style;
    ApplyToContexte(false,true);
}

//========= Border pen size
void DlgSlideProperties::s_ChgPenSize(int) {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::s_ChgPenSize";
    #endif
    if (!PrepContexte()) return;
    CompositionObject->PenSize=ui->PenSizeEd->value();
    ui->PenColorCB->setEnabled(CompositionObject->PenSize!=0);
    ui->PenStyleCB->setEnabled(CompositionObject->PenSize!=0);
    ApplyToContexte(false,true);
}

//========= Border pen style
void DlgSlideProperties::s_ChangePenStyle(int index) {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::s_ChangePenStyle";
    #endif
    if (!PrepContexte()) return;
    CompositionObject->PenStyle=ui->PenStyleCB->itemData(index).toInt();
    ApplyToContexte(false,true);
}

//========= Border pen color
void DlgSlideProperties::s_ChPenColorCB(int) {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::s_ChPenColorCB";
    #endif
    if (!PrepContexte()) return;
    CompositionObject->PenColor=ui->PenColorCB->GetCurrentColor();
    ApplyToContexte(false,true);
}

//========= Shape shadow style
void DlgSlideProperties::s_ChgShadowFormValue(int value) {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::s_ChgShadowFormValue";
    #endif
    if (!PrepContexte()) return;
    CompositionObject->FormShadow=value;
    ui->ShadowEffectED->setEnabled(CompositionObject->FormShadow!=0);
    ui->ShadowColorCB->setEnabled(CompositionObject->FormShadow!=0);
    ApplyToContexte(false,true);
}

//========= Shape shadow distance
void DlgSlideProperties::s_ChgShadowDistanceValue(int value) {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::s_ChgShadowDistanceValue";
    #endif
    if (!PrepContexte()) return;
    CompositionObject->FormShadowDistance =value;
    ApplyToContexte(false,true);
}

//========= shadow color
void DlgSlideProperties::s_ChgShadowColorCB(int) {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::s_ChgShadowColorCB";
    #endif
    if (!PrepContexte()) return;
    CompositionObject->FormShadowColor=ui->ShadowColorCB->GetCurrentColor();
    ApplyToContexte(false,true);
}

//====================================================================================================================
// Handler for style sheet management
//====================================================================================================================

void DlgSlideProperties::s_CoordinateStyleBT() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::s_CoordinateStyleBT";
    #endif
    if (!PrepContexte()) return;
    // Define StyleCoordinateCollection filter
    if (CompositionObject->BackgroundBrush->Image!=NULL)         GlobalMainWindow->ApplicationConfig->StyleCoordinateCollection.SetImageGeometryFilter(GlobalMainWindow->Diaporama->ImageGeometry,CompositionObject->BackgroundBrush->Image->ObjectGeometry);
    else if (CompositionObject->BackgroundBrush->Video!=NULL)    GlobalMainWindow->ApplicationConfig->StyleCoordinateCollection.SetImageGeometryFilter(GlobalMainWindow->Diaporama->ImageGeometry,CompositionObject->BackgroundBrush->Video->ObjectGeometry);
    else                                                        GlobalMainWindow->ApplicationConfig->StyleCoordinateCollection.SetProjectGeometryFilter(GlobalMainWindow->Diaporama->ImageGeometry);

    // Get Actual style definition
    QString ActualStyle=CompositionObject->GetCoordinateStyle();

    // Open popup menu
    QString Item=GlobalMainWindow->ApplicationConfig->StyleCoordinateCollection.PopupCollectionMenu(this,ActualStyle);
    ui->CoordinateStyleBT->setDown(false);
    if (Item!="") {
        CompositionObject->ApplyCoordinateStyle(GlobalMainWindow->ApplicationConfig->StyleCoordinateCollection.GetStyleDef(Item));

        // Report new shot framing to RectItem
        RectItem->AspectRatio=CompositionObject->BackgroundBrush->BrushFileCorrect.AspectRatio;
        if (CompositionObject->h>(CompositionObject->w*xmax*RectItem->AspectRatio)/ymax) CompositionObject->h=(CompositionObject->w*xmax*RectItem->AspectRatio)/ymax;
            else CompositionObject->w=(CompositionObject->h*ymax/RectItem->AspectRatio)/xmax;

        // Refresh block table
        int CurrentRow=ui->BlockTable->currentRow();
        RefreshBlockTable(CurrentRow>0?CurrentRow:0);
    }
    RefreshStyleControls();
}

//====================================================================================================================

void DlgSlideProperties::s_BlockShapeStyleBT() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::s_BlockShapeStyleBT";
    #endif
    if (!PrepContexte()) return;
    QString ActualStyle =CompositionObject->GetBlockShapeStyle();
    QString Item        =GlobalMainWindow->ApplicationConfig->StyleBlockShapeCollection.PopupCollectionMenu(this,ActualStyle);
    ui->BlockShapeStyleBT->setDown(false);
    if (Item!="") {
        CompositionObject->ApplyBlockShapeStyle(GlobalMainWindow->ApplicationConfig->StyleBlockShapeCollection.GetStyleDef(Item));
        int CurrentRow=ui->BlockTable->currentRow();
        RefreshBlockTable(CurrentRow>0?CurrentRow:0);
    }
    RefreshStyleControls();
}

//====================================================================================================================

void DlgSlideProperties::s_ChangeFramingStyle(int Value) {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::s_ChangeFramingStyle";
    #endif

    if (StopMajFramingStyle) return;
    if (!PrepContexte()) return;

    cBrushDefinition *CurrentBrush=CompositionObject->BackgroundBrush;

    if ((CurrentBrush->Image==NULL)&&(CurrentBrush->Video==NULL)) {
        // It's a text block
        switch (Value) {
            case 0: // Unlock
                CurrentBrush->BrushFileCorrect.LockGeometry=false;
                break;
            case 1: // Lock to this geometry
                CurrentBrush->BrushFileCorrect.LockGeometry=true;
                CurrentBrush->BrushFileCorrect.AspectRatio =RectItem->AspectRatio;
                break;
            case 2: // Lock to project geometry
                CurrentBrush->BrushFileCorrect.LockGeometry=true;
                CurrentBrush->BrushFileCorrect.AspectRatio=ProjectGeometry;
                break;
            default:
                return;
        }
        RectItem->KeepAspectRatio=CurrentBrush->BrushFileCorrect.LockGeometry;
        RectItem->AspectRatio    =CurrentBrush->BrushFileCorrect.AspectRatio;
        if (CompositionObject->h>(CompositionObject->w*xmax*RectItem->AspectRatio)/ymax) CompositionObject->h=(CompositionObject->w*xmax*RectItem->AspectRatio)/ymax;
            else CompositionObject->w=(CompositionObject->h*ymax/RectItem->AspectRatio)/xmax;
    } else {
        // It's an image or video block
        int         i;
        QStringList List;

        switch (Value) {
            case 0: // Adjust to width
                GlobalMainWindow->ApplicationConfig->StyleImageFramingCollection.StringDefToStringList(CurrentBrush->DefaultFramingW,List);
                break;
            case 1: // Adjust to heigth
                GlobalMainWindow->ApplicationConfig->StyleImageFramingCollection.StringDefToStringList(CurrentBrush->DefaultFramingH,List);
                break;
            case 2: // Adjust to full
                GlobalMainWindow->ApplicationConfig->StyleImageFramingCollection.StringDefToStringList(CurrentBrush->DefaultFramingF,List);
                break;
            case 3: // None
                break;
            default:
                GlobalMainWindow->ApplicationConfig->StyleImageFramingCollection.StringToStringList(ui->FramingStyleCB->itemText(Value),List);
                break;
        }
        if (List.count()>0) {
            for (i=0;i<List.count();i++) {
                if      (List[i].startsWith("X:"))              CurrentBrush->BrushFileCorrect.X             =List[i].mid(QString("X:").length()).toDouble();
                else if (List[i].startsWith("Y:"))              CurrentBrush->BrushFileCorrect.Y             =List[i].mid(QString("Y:").length()).toDouble();
                else if (List[i].startsWith("ZoomFactor:"))     CurrentBrush->BrushFileCorrect.ZoomFactor    =List[i].mid(QString("ZoomFactor:").length()).toDouble();
                else if (List[i].startsWith("LockGeometry:"))   CurrentBrush->BrushFileCorrect.LockGeometry  =List[i].mid(QString("LockGeometry:").length()).toInt()==1;
                else if (List[i].startsWith("AspectRatio:"))    CurrentBrush->BrushFileCorrect.AspectRatio   =List[i].mid(QString("AspectRatio:").length()).toDouble();
            }
            RectItem->AspectRatio=CurrentBrush->BrushFileCorrect.AspectRatio;
            if (CompositionObject->h>(CompositionObject->w*xmax*RectItem->AspectRatio)/ymax) CompositionObject->h=(CompositionObject->w*xmax*RectItem->AspectRatio)/ymax;
                else CompositionObject->w=(CompositionObject->h*ymax/RectItem->AspectRatio)/xmax;
        }

    }
    int CurrentRow=ui->BlockTable->currentRow();
    RefreshBlockTable(CurrentRow>0?CurrentRow:0);
}

//====================================================================================================================
// Buttons associated to a Dialog box
//====================================================================================================================

//========= Open text editor
void DlgSlideProperties::TextEditor() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::TextEditor";
    #endif
    if (!PrepContexte()) return;
    DlgTextEdit Dlg(CompositionObject,HELPFILE_DlgTextEdit,GlobalMainWindow->ApplicationConfig,GlobalMainWindow->ApplicationConfig->DlgTextEditWSP,this);
    Dlg.InitDialog();
    connect(&Dlg,SIGNAL(RefreshDisplay()),this,SLOT(s_RefreshSceneImage()));
    if (Dlg.exec()==0) {
        ui->BlockTable->item(ui->BlockTable->currentRow(),2)->setText(CompositionObject->Text);
        ApplyToContexte(false,true);
    }
}

//========= Open video editor
void DlgSlideProperties::VideoEdit() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::VideoEdit";
    #endif
    if ((!PrepContexte())||(!CompositionObject->BackgroundBrush->Video)) return;
    DlgVideoEdit(CompositionObject->BackgroundBrush,this).exec();

    // free CachedBrushBrush (for this object only)
    for (int i=0;i<DiaporamaObject->List.count();i++) for (int j=0;j<DiaporamaObject->List[i]->ShotComposition.List.count();j++) {
        if ((DiaporamaObject->List[i]->ShotComposition.List[j]->IndexKey==CompositionObject->IndexKey)&&(DiaporamaObject->List[i]->ShotComposition.List[j]->CachedBrushBrush)) {
            delete DiaporamaObject->List[i]->ShotComposition.List[j]->CachedBrushBrush;
            DiaporamaObject->List[i]->ShotComposition.List[j]->CachedBrushBrush=NULL;
        }
    }

    ApplyToContexte(false,true);
}

//========= Open image correction editor
void DlgSlideProperties::ImageEditCorrect() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::ImageEditCorrect";
    #endif
    if (!PrepContexte()) return;

    cBrushDefinition *CurrentBrush=CompositionObject->BackgroundBrush;

    int Position=0;
    // Compute position of video
    if (CurrentBrush->Video) for (int i=0;i<ui->ShotTable->currentColumn();i++) for (int j=0;j<DiaporamaObject->List[i]->ShotComposition.List.count();j++)
        if (DiaporamaObject->List[i]->ShotComposition.List[j]->IndexKey==CompositionObject->IndexKey) {
            if (DiaporamaObject->List[i]->ShotComposition.List[j]->IsVisible) Position+=DiaporamaObject->List[i]->StaticDuration;
    }

    QString FileName    =QFileInfo(CurrentBrush->Image?CurrentBrush->Image->FileName:CurrentBrush->Video->FileName).fileName();
    bool UpdateSlideName=(DiaporamaObject->SlideName==FileName);
    if (DlgImageCorrection(CompositionObject,CompositionObject->BackgroundForm,CompositionObject->BackgroundBrush,Position,this).exec()==0) {

        RectItem->AspectRatio=CurrentBrush->BrushFileCorrect.AspectRatio;
        CompositionObject->h =(CompositionObject->w*xmax*RectItem->AspectRatio)/ymax;

        // Adjust height and width to image stay in screen
        if (((CompositionObject->y+CompositionObject->h)*ymax)>ymax) {
            CompositionObject->h=1-CompositionObject->y;
            CompositionObject->w=((CompositionObject->h*ymax)/RectItem->AspectRatio)/xmax;
        }

        // free CachedBrushBrush (for this object only)
        for (int i=0;i<DiaporamaObject->List.count();i++) for (int j=0;j<DiaporamaObject->List[i]->ShotComposition.List.count();j++) {
            if ((DiaporamaObject->List[i]->ShotComposition.List[j]->IndexKey==CompositionObject->IndexKey)&&(DiaporamaObject->List[i]->ShotComposition.List[j]->CachedBrushBrush)) {
                delete DiaporamaObject->List[i]->ShotComposition.List[j]->CachedBrushBrush;
                DiaporamaObject->List[i]->ShotComposition.List[j]->CachedBrushBrush=NULL;
            }
        }

        if (CurrentBrush->Image) {
            // free Image CacheObject in lulo
            cLuLoImageCacheObject *ImageObject=CurrentBrush->Image->ApplicationConfig->ImagesCache.FindObject(CurrentBrush->Image->FileName,&CurrentBrush->Image->BrushFileTransform,GlobalMainWindow->ApplicationConfig->Smoothing,true);
            ImageObject->ClearAll();
        }

        // if Slide name is name of this file
        if (UpdateSlideName) {
            DiaporamaObject->SlideName=QFileInfo(CurrentBrush->Image?CurrentBrush->Image->FileName:CurrentBrush->Video->FileName).fileName();
            ui->SlideNameED->setText(DiaporamaObject->SlideName);
        }

        ApplyToContexte(true,true);
        RefreshBlockTable(ui->BlockTable->currentRow());
        //UpdateDockInfo();
    }
}

//====================================================================================================================
// Shot table part
//====================================================================================================================

void DlgSlideProperties::s_ShotTableDragMoveItem() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::s_ShotTableDragMoveItem";
    #endif

    if (GlobalMainWindow->DragItemSource<GlobalMainWindow->DragItemDest) GlobalMainWindow->DragItemDest--;
    DiaporamaObject->List.move(GlobalMainWindow->DragItemSource,GlobalMainWindow->DragItemDest);
    ui->ShotTable->setUpdatesEnabled(false);
    ui->ShotTable->SetCurrentCell(GlobalMainWindow->DragItemDest);
    ui->ShotTable->setUpdatesEnabled(true);  // Reset timeline painting
}

//====================================================================================================================

void DlgSlideProperties::s_ShotTableMoveLeft() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::s_ShotTableMoveLeft";
    #endif

    int CurrentShot=ui->ShotTable->currentColumn();
    DiaporamaObject->List.swap(CurrentShot,CurrentShot-1);
    RefreshShotTable(CurrentShot-1);
}

//====================================================================================================================

void DlgSlideProperties::s_ShotTableMoveRight() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::s_ShotTableMoveRight";
    #endif

    int CurrentShot=ui->ShotTable->currentColumn();
    DiaporamaObject->List.swap(CurrentShot+1,CurrentShot);
    RefreshShotTable(CurrentShot+1);
}

//====================================================================================================================
// User select a shot in the ShotTable widget

void DlgSlideProperties::s_ShotTable_SelectionChanged() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::s_ShotTable_SelectionChanged";
    #endif

    if (InShotTable_SelectionChanged) return;
    int Current=ui->ShotTable->currentColumn();
    if ((Current<0)||(Current>=DiaporamaObject->List.count())) return;

    InShotTable_SelectionChanged=true;
    CompositionList=&DiaporamaObject->List[Current]->ShotComposition;

    int i       =ui->BlockTable->currentRow();
    int IndexKey=-1;
    if ((i>=0)&&(i<CompositionList->List.count())) IndexKey=CompositionList->List[i]->IndexKey;

    int CurrentBlockTableIndex=0;
    if (IndexKey!=-1) {
        while ((CurrentBlockTableIndex<CompositionList->List.count())&&(CompositionList->List[CurrentBlockTableIndex]->IndexKey!=IndexKey)) CurrentBlockTableIndex++;
        if (CurrentBlockTableIndex>=CompositionList->List.count()) CurrentBlockTableIndex=0;
    }
    RefreshBlockTable(CurrentBlockTableIndex);

    InShotTable_SelectionChanged=false;
}

//====================================================================================================================

void DlgSlideProperties::s_ShotTable_AddShot() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::s_ShotTable_AddShot";
    #endif

    int Current=ui->ShotTable->currentColumn();
    if ((Current<0)||(Current>=DiaporamaObject->List.count())) return;

    DiaporamaObject->List.insert(Current+1,new cDiaporamaShot(DiaporamaObject));
    cDiaporamaShot *imagesequence=DiaporamaObject->List[Current+1];
    for (int i=0;i<CompositionList->List.count();i++) {
        imagesequence->ShotComposition.List.append(new cCompositionObject(COMPOSITIONTYPE_SHOT,CompositionList->List[i]->IndexKey,GlobalMainWindow->ApplicationConfig));
        imagesequence->ShotComposition.List[i]->CopyFromCompositionObject(CompositionList->List[i]);
    }
    RefreshShotTable(Current+1);
}

//====================================================================================================================

void DlgSlideProperties::s_ShotTable_RemoveShot() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::s_ShotTable_RemoveShot";
    #endif

    int Current=ui->ShotTable->currentColumn();
    if ((Current<0)||(Current>=DiaporamaObject->List.count())) return;
    if (DiaporamaObject->List.count()<2) return;
    if ((GlobalMainWindow->ApplicationConfig->AskUserToRemove)&&(QMessageBox::question(this,QApplication::translate("DlgSlideProperties","Remove shot"),QApplication::translate("DlgSlideProperties","Are you sure to want to delete this shot?"),
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)==QMessageBox::No)) return;
    delete DiaporamaObject->List.takeAt(Current);
    ui->ShotTable->setUpdatesEnabled(false);
    ui->ShotTable->removeColumn(Current);
    ui->ShotTable->setUpdatesEnabled(true);
    RefreshShotTable(ui->ShotTable->currentColumn()>0?ui->ShotTable->currentColumn():0);
}

//====================================================================================================================
// Block table/scene part
//====================================================================================================================

//====================================================================================================================
// User select a block in the BlockTable widget

void DlgSlideProperties::s_BlockTable_SelectionChanged() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::s_BlockTable_SelectionChanged";
    #endif

    if (InBlockTable_SelectionChanged) return;
    InBlockTable_SelectionChanged=true;

    int CurrentBlock=ui->BlockTable->currentRow();

    // If nothing selected then ensure no visible item is selected in the scene
    if ((CurrentBlock<0)||(CurrentBlock>=CompositionList->List.count())) {

        for (int i=0;i<scene->items().count();i++) {
            QString data =((QGraphicsItem *)scene->items()[i])->data(0).toString();
            // Unselect item if it's a cCustomGraphicsRectItem or a cResizeGraphicsRectItem
            if ((data=="CustomGraphicsRectItem")&&(((cCustomGraphicsRectItem *)scene->items()[i])->IsVisible)) ((cCustomGraphicsRectItem *)scene->items()[i])->setSelected(false);
            if ((data=="ResizeGraphicsRectItem")&&(((cResizeGraphicsRectItem *)scene->items()[i])->RectItem->IsVisible)) ((cResizeGraphicsRectItem *)scene->items()[i])->setSelected(false);
        }

    } else {

        // Parse item to select corresponding item
        for (int i=0;i<scene->items().count();i++) {
            QString data =((QGraphicsItem *)scene->items()[i])->data(0).toString();
            if (data=="CustomGraphicsRectItem") {
                if (((cCustomGraphicsRectItem *)scene->items()[i])->IndexKey==CompositionList->List[CurrentBlock]->IndexKey) {
                    // If item is not selected then select it
                    if (!((cCustomGraphicsRectItem *)scene->items()[i])->isSelected()) ((cCustomGraphicsRectItem *)scene->items()[i])->setSelected(true);
                } else {
                    // If item is selected then deselect it
                    if (((cCustomGraphicsRectItem *)scene->items()[i])->isSelected()) ((cCustomGraphicsRectItem *)scene->items()[i])->setSelected(false);
                }
            } else if (data=="ResizeGraphicsRectItem") {
                // If item is selected then deselect it
                if (((cResizeGraphicsRectItem *)scene->items()[i])->isSelected()) ((cResizeGraphicsRectItem *)scene->items()[i])->setSelected(false);
            }
        }
    }

    RefreshControls();
    InBlockTable_SelectionChanged=false;
}

//====================================================================================================================
// User select a block in the scene widget

void DlgSlideProperties::s_Scene_SelectionChanged() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::s_Scene_SelectionChanged";
    #endif

    if ((scene==NULL)||(InScene_SelectionChanged)) return;

    // Nothing is selected
    if (scene->selectedItems().count()==0) {
        if (ui->BlockTable->currentRow()!=-1) ui->BlockTable->setCurrentCell(-1,0);  // Remove selection from block table
        return;
    }
    cCustomGraphicsRectItem *CurrentSceneItem=(cCustomGraphicsRectItem *)scene->selectedItems()[0];    // Only the first item
    QString                 data =CurrentSceneItem->data(0).toString();

    // Check if item is of correct type
    if (data!="CustomGraphicsRectItem") {
        // if it's a resize box then give it's parent
        if (data=="ResizeGraphicsRectItem") CurrentSceneItem=((cResizeGraphicsRectItem *)CurrentSceneItem)->RectItem; else {
            // Else, there is an error !
            CurrentSceneItem->setSelected(false);   // Unselect it
            if (ui->BlockTable->currentRow()!=-1) ui->BlockTable->setCurrentCell(-1,0);  // Remove selection from block table
            return;
        }
    }

    // Search corresponding item index in the block table
    int BlockTableItemIndex=0;
    while ((BlockTableItemIndex<CompositionList->List.count())&&(CompositionList->List[BlockTableItemIndex]->IndexKey!=CurrentSceneItem->IndexKey)) BlockTableItemIndex++;
    if ((BlockTableItemIndex<CompositionList->List.count())&&(CompositionList->List[BlockTableItemIndex]->IndexKey==CurrentSceneItem->IndexKey)) {
        // if corresponding item is found in the block table
        if (ui->BlockTable->currentRow()!=BlockTableItemIndex) ui->BlockTable->setCurrentCell(BlockTableItemIndex,0);
    } else {
        // Else, there is an error : then remove all selection
        CurrentSceneItem->setSelected(false);   // Unselect it
        if (ui->BlockTable->currentRow()!=-1) ui->BlockTable->setCurrentCell(-1,0);  // Remove selection from block table
    }

    //RefreshControls();
}

//====================================================================================================================
// User double click on a block in the BlockTable widget

void DlgSlideProperties::s_BlockTable_ItemDoubleClicked(QTableWidgetItem *) {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::s_BlockTable_ItemDoubleClicked";
    #endif

    s_Scene_DoubleClick();
}

//====================================================================================================================
// User double click on a block in the scene widget

void DlgSlideProperties::s_Scene_DoubleClick() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::s_Scene_DoubleClick";
    #endif

    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if (CurrentTextItem==NULL) return;
    if (CurrentTextItem->BackgroundBrush->BrushType!=BRUSHTYPE_IMAGEDISK)    TextEditor();
    else if (CurrentTextItem->BackgroundBrush->Image!=NULL)                  ImageEditCorrect();
    else if (CurrentTextItem->BackgroundBrush->Video!=NULL)                  VideoEdit();
}

//====================================================================================================================

void DlgSlideProperties::s_BlockTable_AddNewTextBlock() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::s_BlockTable_AddNewTextBlock";
    #endif

    int CurrentShot=ui->ShotTable->currentColumn();

    // Create and append a composition block to the object list
    DiaporamaObject->ObjectComposition.List.append(new cCompositionObject(COMPOSITIONTYPE_OBJECT,DiaporamaObject->NextIndexKey,GlobalMainWindow->ApplicationConfig));
    cCompositionObject *CompositionObject=DiaporamaObject->ObjectComposition.List[DiaporamaObject->ObjectComposition.List.count()-1];
    CompositionObject->BackgroundBrush->BrushFileCorrect.LockGeometry=false;
    CompositionObject->BackgroundBrush->BrushFileCorrect.AspectRatio=1;

    // Apply Styles
    CompositionObject->ApplyTextStyle(DiaporamaObject->Parent->ApplicationConfig->StyleTextCollection.GetStyleDef(DiaporamaObject->Parent->ApplicationConfig->StyleTextCollection.DecodeString(DiaporamaObject->Parent->ApplicationConfig->DefaultBlock_Text_TextST)));
    CompositionObject->ApplyBackgroundStyle(DiaporamaObject->Parent->ApplicationConfig->StyleTextBackgroundCollection.GetStyleDef(DiaporamaObject->Parent->ApplicationConfig->StyleTextBackgroundCollection.DecodeString(DiaporamaObject->Parent->ApplicationConfig->DefaultBlock_Text_BackGST)));
    CompositionObject->ApplyBlockShapeStyle(DiaporamaObject->Parent->ApplicationConfig->StyleBlockShapeCollection.GetStyleDef(DiaporamaObject->Parent->ApplicationConfig->StyleBlockShapeCollection.DecodeString(DiaporamaObject->Parent->ApplicationConfig->DefaultBlock_Text_ShapeST)));
    // Force filtering for CoordinateStyle
    DiaporamaObject->Parent->ApplicationConfig->StyleCoordinateCollection.SetProjectGeometryFilter(DiaporamaObject->Parent->ImageGeometry);
    CompositionObject->ApplyCoordinateStyle(DiaporamaObject->Parent->ApplicationConfig->StyleCoordinateCollection.GetStyleDef(DiaporamaObject->Parent->ApplicationConfig->StyleCoordinateCollection.DecodeString(DiaporamaObject->Parent->ApplicationConfig->DefaultBlock_Text_CoordST[DiaporamaObject->Parent->ImageGeometry])));

    CompositionObject->Text=QApplication::translate("DlgSlideProperties","Text","Default text value");

    // Now create and append a shot composition block to all shot
    for (int i=0;i<DiaporamaObject->List.count();i++) {
        DiaporamaObject->List[i]->ShotComposition.List.append(new cCompositionObject(COMPOSITIONTYPE_SHOT,CompositionObject->IndexKey,GlobalMainWindow->ApplicationConfig));
        DiaporamaObject->List[i]->ShotComposition.List[DiaporamaObject->List[i]->ShotComposition.List.count()-1]->CopyFromCompositionObject(CompositionObject);
        // Ensure new object is not visible in previous shot
        if (i<CurrentShot) DiaporamaObject->List[i]->ShotComposition.List[DiaporamaObject->List[i]->ShotComposition.List.count()-1]->IsVisible=false;
    }

    // Inc NextIndexKey
    DiaporamaObject->NextIndexKey++;

    // 10 by 10 step for ZValue
    NextZValue+=10;

    RefreshBlockTable(CompositionList->List.count()-1);
    TextEditor();   // Open text editor
}

//====================================================================================================================

void DlgSlideProperties::s_BlockTable_AddNewFileBlock() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::s_BlockTable_AddNewFileBlock";
    #endif

    QStringList FileList=QFileDialog::getOpenFileNames(this,QApplication::translate("DlgSlideProperties","Add files"),
                                                       DiaporamaObject->Parent->ApplicationConfig->RememberLastDirectories?DiaporamaObject->Parent->ApplicationConfig->LastMediaPath:"",
                                                       DiaporamaObject->Parent->ApplicationConfig->GetFilterForMediaFile(ALLFILE));

    // Sort files in the fileList
    if (DiaporamaObject->Parent->ApplicationConfig->SortFile) {
        // Sort by last number
        for (int i=0;i<FileList.count();i++) for (int j=0;j<FileList.count()-1;j++) {
            QString NameA=QFileInfo(FileList[j]).completeBaseName();
            int NumA=NameA.length()-1;
            while ((NumA>0)&&(NameA[NumA]>='0')&&(NameA[NumA]<='9')) NumA--;
            if (NumA>=0) NumA=NameA.mid(NumA+1).toInt();

            QString NameB=QFileInfo(FileList[j+1]).completeBaseName();
            int NumB=NameB.length()-1;
            while ((NumB>0)&&(NameB[NumB]>='0')&&(NameB[NumB]<='9')) NumB--;
            if (NumB>=0) NumB=NameB.mid(NumB+1).toInt();

            if (NumA>NumB) FileList.swap(j,j+1);
        }
    } else {
        // Sort by alphabetical order
        for (int i=0;i<FileList.count();i++) for (int j=0;j<FileList.count()-1;j++) {
            if (QFileInfo(FileList[j]).completeBaseName()>QFileInfo(FileList[j+1]).completeBaseName()) FileList.swap(j,j+1);
        }
    }

    QApplication::processEvents();

    // Add files
    QStringList AliasList;
    for (int i=0;i<FileList.count();i++) {
        QString NewFile=FileList[i];
        QString ErrorMessage=QApplication::translate("MainWindow","Format not supported","Error message");

        if (GlobalMainWindow->ApplicationConfig->RememberLastDirectories) GlobalMainWindow->ApplicationConfig->LastMediaPath=QFileInfo(NewFile).absolutePath();     // Keep folder for next use

        // Create and append a composition block to the object list
        DiaporamaObject->ObjectComposition.List.append(new cCompositionObject(COMPOSITIONTYPE_OBJECT,DiaporamaObject->NextIndexKey,GlobalMainWindow->ApplicationConfig));
        cCompositionObject  *CompositionObject  =DiaporamaObject->ObjectComposition.List[DiaporamaObject->ObjectComposition.List.count()-1];
        cBrushDefinition    *CurrentBrush       =CompositionObject->BackgroundBrush;

        CompositionObject->Text     ="";
        CompositionObject->PenSize  =0;
        CurrentBrush->BrushType     =BRUSHTYPE_IMAGEDISK;

        QString BrushFileName =QFileInfo(NewFile).absoluteFilePath();

        bool    IsValide =false;
        QString Extension=QFileInfo(BrushFileName).suffix().toLower();

        // Search if file is an image
        for (int i=0;i<GlobalMainWindow->ApplicationConfig->AllowImageExtension.count();i++) if (GlobalMainWindow->ApplicationConfig->AllowImageExtension[i]==Extension) {
            // Create an image wrapper
            CurrentBrush->Image=new cImageFile(GlobalMainWindow->ApplicationConfig);
            bool ModifyFlag=false;
            IsValide=CurrentBrush->Image->GetInformationFromFile(BrushFileName,&AliasList,&ModifyFlag);
            if (!IsValide) {
                delete CurrentBrush->Image;
                CurrentBrush->Image=NULL;
            } else if (ModifyFlag) GlobalMainWindow->SetModifyFlag(true);
            break;
        }
        // If it's not an image : search if file is a video
        if (CurrentBrush->Image==NULL) for (int i=0;i<GlobalMainWindow->ApplicationConfig->AllowVideoExtension.count();i++) if (GlobalMainWindow->ApplicationConfig->AllowVideoExtension[i]==Extension) {
            // Create a video wrapper
            CurrentBrush->Video=new cVideoFile(cVideoFile::VIDEOFILE,GlobalMainWindow->ApplicationConfig);
            bool ModifyFlag=false;
            IsValide=(CurrentBrush->Video->GetInformationFromFile(BrushFileName,&AliasList,&ModifyFlag))&&(CurrentBrush->Video->OpenCodecAndFile());
            if (IsValide) {
                // Check if file have at least one sound track compatible
                if ((CurrentBrush->Video->AudioStreamNumber!=-1)&&(CurrentBrush->Video->ffmpegAudioFile->streams[CurrentBrush->Video->AudioStreamNumber]->codec->sample_fmt!=AV_SAMPLE_FMT_S16)) {
                    ErrorMessage=ErrorMessage+"\n"+QApplication::translate("MainWindow","This application support only audio track with signed 16 bits sample format","Error message");
                    IsValide=false;
                }
                if ((CurrentBrush->Video->AudioStreamNumber!=-1)&&(CurrentBrush->Video->ffmpegAudioFile->streams[CurrentBrush->Video->AudioStreamNumber]->codec->channels>2)) {
                    ErrorMessage=ErrorMessage+"\n"+QApplication::translate("MainWindow","This application support only mono or stereo audio track","Error message");
                    IsValide=false;
                }
            }
            if (!IsValide) {
                delete CurrentBrush->Video;
                CurrentBrush->Video=NULL;
            } else {
                if (ModifyFlag) GlobalMainWindow->SetModifyFlag(true);
                CurrentBrush->Video->EndPos=CurrentBrush->Video->Duration;
                //DiaporamaObject->List[0].StaticDuration=CurrentBrush->Video->StartPos.msecsTo(CurrentBrush->Video->EndPos);
            }
            break;
        }
        if (IsValide) {

            QImage *Image=(CurrentBrush->Image?CurrentBrush->Image->ImageAt(true,true,&CurrentBrush->Image->BrushFileTransform):
                           CurrentBrush->Video?CurrentBrush->Video->ImageAt(true,0,QTime(0,0,0,0).msecsTo(CurrentBrush->Video->StartPos),true,NULL,1,false,&CurrentBrush->Video->BrushFileTransform,false):
                           NULL);
            if (!Image) {
                IsValide=false;
                if (CurrentBrush->Image) {
                    delete CurrentBrush->Image;
                    CurrentBrush->Image=NULL;
                } else if (CurrentBrush->Video) {
                    delete CurrentBrush->Video;
                    CurrentBrush->Video=NULL;
                }
            } else {

                // Apply Styles
                CompositionObject->ApplyTextStyle(DiaporamaObject->Parent->ApplicationConfig->StyleTextCollection.GetStyleDef(DiaporamaObject->Parent->ApplicationConfig->StyleTextCollection.DecodeString(DiaporamaObject->Parent->ApplicationConfig->DefaultBlockBA_IMG_TextST)));
                CompositionObject->ApplyBlockShapeStyle(DiaporamaObject->Parent->ApplicationConfig->StyleBlockShapeCollection.GetStyleDef(DiaporamaObject->Parent->ApplicationConfig->StyleBlockShapeCollection.DecodeString(DiaporamaObject->Parent->ApplicationConfig->DefaultBlockBA_IMG_ShapeST)));
                // Force filtering for CoordinateStyle
                DiaporamaObject->Parent->ApplicationConfig->StyleCoordinateCollection.SetImageGeometryFilter(DiaporamaObject->Parent->ImageGeometry,CurrentBrush->Image?CurrentBrush->Image->ObjectGeometry:CurrentBrush->Video->ObjectGeometry);
                CompositionObject->ApplyCoordinateStyle(DiaporamaObject->Parent->ApplicationConfig->StyleCoordinateCollection.GetStyleDef(DiaporamaObject->Parent->ApplicationConfig->StyleCoordinateCollection.DecodeString(
                    DiaporamaObject->Parent->ApplicationConfig->DefaultBlockBA_IMG_CoordST[CurrentBrush->Image?CurrentBrush->Image->ObjectGeometry:CurrentBrush->Video->ObjectGeometry][DiaporamaObject->Parent->ImageGeometry])));

                // Special case for nonstandard image => force to image geometry constraint and adapt frame coordinates
                if ((CurrentBrush->Image?CurrentBrush->Image->ObjectGeometry:CurrentBrush->Video->ObjectGeometry)==IMAGE_GEOMETRY_UNKNOWN) {
                    double ImageGeometry=1;
                    double ProjectGeometry=1;
                    double NewW,NewH;

                    switch (GlobalMainWindow->Diaporama->ImageGeometry) {
                        case GEOMETRY_4_3   : ProjectGeometry=double(1440)/double(1920);  break;
                        case GEOMETRY_16_9  : ProjectGeometry=double(1080)/double(1920);  break;
                        case GEOMETRY_40_17 : ProjectGeometry=double(816)/double(1920);   break;

                    }
                    ProjectGeometry=QString("%1").arg(ProjectGeometry,0,'e').toDouble();  // Rounded to same number as style managment
                    switch (DiaporamaObject->Parent->ApplicationConfig->DefaultBlockBA_CLIPARTLOCK[DiaporamaObject->Parent->ImageGeometry]) {
                        case 0 :    // Adjust to Full in lock to image geometry mode
                            if (CurrentBrush->Image)            ImageGeometry=double(CurrentBrush->Image->ImageHeight)/double(CurrentBrush->Image->ImageWidth);
                                else if (CurrentBrush->Video)   ImageGeometry=double(CurrentBrush->Video->ImageHeight)/double(CurrentBrush->Video->ImageWidth);
                            CurrentBrush->InitDefaultFramingStyle(true,ImageGeometry);
                            CurrentBrush->ApplyStyle(true,CurrentBrush->DefaultFramingF);
                            NewW=CompositionObject->w*GlobalMainWindow->Diaporama->InternalWidth;
                            NewH=NewW*CurrentBrush->BrushFileCorrect.AspectRatio;
                            NewW=NewW/GlobalMainWindow->Diaporama->InternalWidth;
                            NewH=NewH/GlobalMainWindow->Diaporama->InternalHeight;
                            if (NewH>1) {
                                NewH=CompositionObject->h*GlobalMainWindow->Diaporama->InternalHeight;
                                NewW=NewH/CurrentBrush->BrushFileCorrect.AspectRatio;
                                NewW=NewW/GlobalMainWindow->Diaporama->InternalWidth;
                                NewH=NewH/GlobalMainWindow->Diaporama->InternalHeight;
                            }
                            CompositionObject->w=NewW;
                            CompositionObject->h=NewH;
                            break;
                        case 1 :    // Lock to project geometry - To full
                            ImageGeometry=ProjectGeometry;
                            CurrentBrush->InitDefaultFramingStyle(true,ImageGeometry);
                            CurrentBrush->ApplyStyle(true,CurrentBrush->DefaultFramingF);
                            break;
                        case 2 :    // Lock to project geometry - To width
                            ImageGeometry=ProjectGeometry;
                            CurrentBrush->InitDefaultFramingStyle(true,ImageGeometry);
                            CurrentBrush->ApplyStyle(true,CurrentBrush->DefaultFramingW);
                            break;
                        case 3 :    // Lock to project geometry - To height
                            ImageGeometry=ProjectGeometry;
                            CurrentBrush->InitDefaultFramingStyle(true,ImageGeometry);
                            CurrentBrush->ApplyStyle(true,CurrentBrush->DefaultFramingH);
                            break;
                    }
                }
                delete Image;
            }
        }
        if (IsValide) {
            int CurrentShot=ui->ShotTable->currentColumn();

            // Now create and append a shot composition block to all shot
            for (int i=0;i<DiaporamaObject->List.count();i++) {
                DiaporamaObject->List[i]->ShotComposition.List.append(new cCompositionObject(COMPOSITIONTYPE_SHOT,CompositionObject->IndexKey,GlobalMainWindow->ApplicationConfig));
                DiaporamaObject->List[i]->ShotComposition.List[DiaporamaObject->List[i]->ShotComposition.List.count()-1]->CopyFromCompositionObject(CompositionObject);
                // Ensure new object is not visible in previous shot
                if (i<CurrentShot) DiaporamaObject->List[i]->ShotComposition.List[DiaporamaObject->List[i]->ShotComposition.List.count()-1]->IsVisible=false;
            }

            // If this object is a video will gain sound from this shots !
            if (CurrentBrush->Video!=NULL) for (int k=0;k<DiaporamaObject->List.count();k++) for (int l=0;l<DiaporamaObject->List[k]->ShotComposition.List.count();l++)
                if ((DiaporamaObject->List[k]->ShotComposition.List[l]->BackgroundBrush->BrushType==BRUSHTYPE_IMAGEDISK)&&
                    (DiaporamaObject->List[k]->ShotComposition.List[l]->BackgroundBrush->Video)) {
                    if (k<CurrentShot) {
                        // Set this new block to SoundVolume=0
                        if (CurrentBrush->Video==DiaporamaObject->List[k]->ShotComposition.List[l]->BackgroundBrush->Video)
                            DiaporamaObject->List[k]->ShotComposition.List[l]->BackgroundBrush->SoundVolume=0;
                    } else {
                        // Set all other block to SoundVolume=0 and this block to SoundVolume=1
                        if (CurrentBrush->Video!=DiaporamaObject->List[k]->ShotComposition.List[l]->BackgroundBrush->Video)
                            DiaporamaObject->List[k]->ShotComposition.List[l]->BackgroundBrush->SoundVolume=0;
                        else DiaporamaObject->List[k]->ShotComposition.List[l]->BackgroundBrush->SoundVolume=1;
                    }
            }

            // Inc NextIndexKey
            DiaporamaObject->NextIndexKey++;

            // 10 by 10 step for ZValue
            NextZValue+=10;

        } else {
            QMessageBox::critical(NULL,QApplication::translate("MainWindow","Error","Error message"),NewFile+"\n\n"+ErrorMessage,QMessageBox::Close);
        }
    }
    RefreshBlockTable(CompositionList->List.count()-1);
}

//====================================================================================================================

void DlgSlideProperties::s_BlockTable_RemoveBlock() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::s_BlockTable_RemoveBlock";
    #endif

    int                 CurrentRow=ui->BlockTable->currentRow();
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if (CurrentTextItem==NULL) return;
    if ((GlobalMainWindow->ApplicationConfig->AskUserToRemove)&&(QMessageBox::question(this,QApplication::translate("DlgSlideProperties","Remove bloc"),QApplication::translate("DlgSlideProperties","Are you sure to want to delete this bloc?"),
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)==QMessageBox::No)) return;
    int i=0;
    while ((i<CompositionList->List.count())&&(CompositionList->List[i]->IndexKey!=CurrentTextItem->IndexKey)) i++;

    if ((i<CompositionList->List.count())) {
        int KeyToDelete=CompositionList->List[i]->IndexKey;
        StopMAJSpinbox=true;
        for (int j=0;j<DiaporamaObject->List.count();j++) {
            int k=0;
            while (k<DiaporamaObject->List[j]->ShotComposition.List.count()) {
                if (DiaporamaObject->List[j]->ShotComposition.List[k]->IndexKey==KeyToDelete) delete DiaporamaObject->List[j]->ShotComposition.List.takeAt(k);
                    else k++;
            }
        }
        int k=0;
        while (k<DiaporamaObject->ObjectComposition.List.count()) {
            if (DiaporamaObject->ObjectComposition.List[k]->IndexKey==KeyToDelete) delete DiaporamaObject->ObjectComposition.List.takeAt(k);
                else k++;
        }

        StopMAJSpinbox=false;
    }
    RefreshBlockTable(CurrentRow>0?CurrentRow-1:0);
}

//====================================================================================================================

void DlgSlideProperties::CopyBlockProperties(cCompositionObject *SourceBlock,cCompositionObject *DestBlock) {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::CopyBlockProperties";
    #endif

    if (SourceBlock==DestBlock) return;

    // Attribut of the text part
    DestBlock->Text                 =SourceBlock->Text;                     // Text of the object
    DestBlock->FontName             =SourceBlock->FontName;                 // font name
    DestBlock->FontSize             =SourceBlock->FontSize;                 // font size
    DestBlock->FontColor            =SourceBlock->FontColor;                // font color
    DestBlock->FontShadowColor      =SourceBlock->FontShadowColor;          // font shadow color
    DestBlock->IsBold               =SourceBlock->IsBold;                   // true if bold mode
    DestBlock->IsItalic             =SourceBlock->IsItalic;                 // true if Italic mode
    DestBlock->IsUnderline          =SourceBlock->IsUnderline;              // true if Underline mode
    DestBlock->HAlign               =SourceBlock->HAlign;                   // Horizontal alignement : 0=left, 1=center, 2=right, 3=justif
    DestBlock->VAlign               =SourceBlock->VAlign;                   // Vertical alignement : 0=up, 1=center, 2=bottom
    DestBlock->StyleText            =SourceBlock->StyleText;                // Style : 0=normal, 1=outerline, 2=shadow up-left, 3=shadow up-right, 4=shadow bt-left, 5=shadow bt-right

    // Attribut of the shap part
    DestBlock->BackgroundForm       =SourceBlock->BackgroundForm;           // Type of the form : 0=None, 1=Rectangle, 2=RoundRect, 3=Buble, 4=Ellipse, 5=Triangle UP (Polygon)
    DestBlock->PenSize              =SourceBlock->PenSize;                  // Width of the border of the form
    DestBlock->PenStyle             =SourceBlock->PenStyle;                 // Style of the pen border of the form
    DestBlock->PenColor             =SourceBlock->PenColor;                 // Color of the border of the form
    DestBlock->FormShadow           =SourceBlock->FormShadow;               // 0=none, 1=shadow up-left, 2=shadow up-right, 3=shadow bt-left, 4=shadow bt-right
    DestBlock->FormShadowDistance   =SourceBlock->FormShadowDistance;       // Distance from form to shadow
    DestBlock->FormShadowColor      =SourceBlock->FormShadowColor;          // 0=none, 1=shadow up-left, 2=shadow up-right, 3=shadow bt-left, 4=shadow bt-right
    DestBlock->Opacity              =SourceBlock->Opacity;                  // Opacity of the form

    // Attribut of the BackgroundBrush of the shap part
    DestBlock->BackgroundBrush->BrushType           =SourceBlock->BackgroundBrush->BrushType;
    DestBlock->BackgroundBrush->PatternType         =SourceBlock->BackgroundBrush->PatternType;
    DestBlock->BackgroundBrush->GradientOrientation =SourceBlock->BackgroundBrush->GradientOrientation;
    DestBlock->BackgroundBrush->ColorD              =SourceBlock->BackgroundBrush->ColorD;
    DestBlock->BackgroundBrush->ColorF              =SourceBlock->BackgroundBrush->ColorF;
    DestBlock->BackgroundBrush->ColorIntermed       =SourceBlock->BackgroundBrush->ColorIntermed;
    DestBlock->BackgroundBrush->Intermediate        =SourceBlock->BackgroundBrush->Intermediate;
    DestBlock->BackgroundBrush->BrushImage          =SourceBlock->BackgroundBrush->BrushImage;
}

void DlgSlideProperties::ApplyGlobalPropertiesToAllShots(cCompositionObject *GlobalBlock) {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::ApplyGlobalPropertiesToAllShots";
    #endif

    // Apply to GlobalComposition objects
    for (int j=0;j<DiaporamaObject->ObjectComposition.List.count();j++) if (GlobalBlock->IndexKey==DiaporamaObject->ObjectComposition.List[j]->IndexKey)
        CopyBlockProperties(GlobalBlock,DiaporamaObject->ObjectComposition.List[j]);

    // Apply to Shots Composition objects
    for (int i=0;i<DiaporamaObject->List.count();i++) for (int j=0;j<DiaporamaObject->List[i]->ShotComposition.List.count();j++) if (GlobalBlock->IndexKey==DiaporamaObject->List[i]->ShotComposition.List[j]->IndexKey)
        CopyBlockProperties(GlobalBlock,DiaporamaObject->List[i]->ShotComposition.List[j]);
}

//====================================================================================================================

void DlgSlideProperties::ChangeVisibleState() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::ChangeVisibleState";
    #endif

    cCompositionObject      *CurrentTextItem=GetSelectedCompositionObject();
    if (!CurrentTextItem)   return;
    CurrentTextItem->IsVisible=!CurrentTextItem->IsVisible;
    if (CurrentTextItem->BackgroundBrush->Video!=NULL) {
        if (!CurrentTextItem->IsVisible) {
            CurrentTextItem->BackgroundBrush->SoundVolume=0;
        } else {
            int CurrentShot=ui->ShotTable->currentColumn();
            bool SomeOneHaveSound=false;
            // Parse table to know if a block have sound for this shot
            for (int i=0;i<DiaporamaObject->List[CurrentShot]->ShotComposition.List.count();i++)
                if ((DiaporamaObject->List[CurrentShot]->ShotComposition.List[i]->BackgroundBrush->Video!=NULL)&&
                    (DiaporamaObject->List[CurrentShot]->ShotComposition.List[i]->BackgroundBrush->SoundVolume!=0)) SomeOneHaveSound=true;
            // If no block have sound => get sound to this video
            if (!SomeOneHaveSound) CurrentTextItem->BackgroundBrush->SoundVolume=1;
        }
    }
    RefreshBlockTable(ui->BlockTable->currentRow());
}

//====================================================================================================================

void DlgSlideProperties::BlockUp() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::BlockUp";
    #endif

    int CurrentBlock=ui->BlockTable->currentRow();
    CompositionList->List.swap(CurrentBlock,CurrentBlock-1);
    RefreshBlockTable(CurrentBlock-1);
}

//====================================================================================================================

void DlgSlideProperties::BlockDown() {
#ifdef DEBUGMODE
qDebug() << "IN:DlgSlideProperties::BlockDown";
#endif

    int CurrentBlock=ui->BlockTable->currentRow();
    CompositionList->List.swap(CurrentBlock+1,CurrentBlock);
    RefreshBlockTable(CurrentBlock+1);
}

//====================================================================================================================

void DlgSlideProperties::s_CopyBlockBT() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::s_CopyBlockBT";
    #endif

    cCompositionObject      *GlobalBlock=GetSelectedGlobalCompositionObject();
    cCompositionObject      *ShotBlock  =GetSelectedCompositionObject();
    if ((!GlobalBlock)||(!ShotBlock))   return;

    if (GlobalMainWindow->Clipboard_Block) delete GlobalMainWindow->Clipboard_Block;
    GlobalMainWindow->Clipboard_Block=new QDomDocument(APPLICATION_NAME);
    QDomElement root=GlobalMainWindow->Clipboard_Block->createElement("CLIPBOARD");
    GlobalMainWindow->Clipboard_Block->appendChild(root);
    GlobalBlock->SaveToXML(root,"CLIPBOARD-BLOCK-GLOBAL",DiaporamaObject->Parent->ProjectFileName,true);  // Save global object
    ShotBlock->SaveToXML(root,"CLIPBOARD-BLOCK-SHOT",DiaporamaObject->Parent->ProjectFileName,true);      // Save shot object
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_CutBlockBT() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::s_CutBlockBT";
    #endif

    cCompositionObject      *GlobalBlock=GetSelectedGlobalCompositionObject();
    cCompositionObject      *ShotBlock  =GetSelectedCompositionObject();
    if ((!GlobalBlock)||(!ShotBlock))   return;

    if (GlobalMainWindow->Clipboard_Block) delete GlobalMainWindow->Clipboard_Block;
    GlobalMainWindow->Clipboard_Block=new QDomDocument(APPLICATION_NAME);
    QDomElement root=GlobalMainWindow->Clipboard_Block->createElement("CLIPBOARD");
    GlobalMainWindow->Clipboard_Block->appendChild(root);
    GlobalBlock->SaveToXML(root,"CLIPBOARD-BLOCK-GLOBAL",DiaporamaObject->Parent->ProjectFileName,true);  // Save global object
    ShotBlock->SaveToXML(root,"CLIPBOARD-BLOCK-SHOT",DiaporamaObject->Parent->ProjectFileName,true);      // Save shot object
    s_BlockTable_RemoveBlock();
}

//====================================================================================================================

void DlgSlideProperties::s_PasteBlockBT() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgSlideProperties::s_PasteBlockBT";
    #endif

    if (!GlobalMainWindow->Clipboard_Block) return;

    QDomElement root=GlobalMainWindow->Clipboard_Block->documentElement();
    if (root.tagName()=="CLIPBOARD") {
        int CurrentShot=ui->ShotTable->currentColumn();

        // Create and append a composition block to the object list
        DiaporamaObject->ObjectComposition.List.append(new cCompositionObject(COMPOSITIONTYPE_OBJECT,DiaporamaObject->NextIndexKey,GlobalMainWindow->ApplicationConfig));
        cCompositionObject *GlobalBlock=DiaporamaObject->ObjectComposition.List[DiaporamaObject->ObjectComposition.List.count()-1];
        GlobalBlock->LoadFromXML(root,"CLIPBOARD-BLOCK-GLOBAL","",NULL,NULL);
        GlobalBlock->IndexKey=DiaporamaObject->NextIndexKey;

        cCompositionObject ShotBlock(COMPOSITIONTYPE_SHOT,DiaporamaObject->NextIndexKey,GlobalMainWindow->ApplicationConfig);
        ShotBlock.LoadFromXML(root,"CLIPBOARD-BLOCK-SHOT","",NULL,NULL);
        ShotBlock.IndexKey=DiaporamaObject->NextIndexKey;
        ShotBlock.BackgroundBrush->Image=GlobalBlock->BackgroundBrush->Image;
        ShotBlock.BackgroundBrush->Video=GlobalBlock->BackgroundBrush->Video;
        ShotBlock.Text=GlobalBlock->Text;
        if (ShotBlock.Text!="") {
            ShotBlock.FontName        =GlobalBlock->FontName;
            ShotBlock.FontSize        =GlobalBlock->FontSize;
            ShotBlock.FontColor       =GlobalBlock->FontColor;
            ShotBlock.FontShadowColor =GlobalBlock->FontShadowColor;
            ShotBlock.IsBold          =GlobalBlock->IsBold;
            ShotBlock.IsItalic        =GlobalBlock->IsItalic;
            ShotBlock.IsUnderline     =GlobalBlock->IsUnderline;
            ShotBlock.HAlign          =GlobalBlock->HAlign;
            ShotBlock.VAlign          =GlobalBlock->VAlign;
            ShotBlock.StyleText       =GlobalBlock->StyleText;
        }

        // Now create and append a shot composition block to all shot
        for (int i=0;i<DiaporamaObject->List.count();i++) {
            DiaporamaObject->List[i]->ShotComposition.List.append(new cCompositionObject(COMPOSITIONTYPE_SHOT,DiaporamaObject->NextIndexKey,GlobalMainWindow->ApplicationConfig));
            DiaporamaObject->List[i]->ShotComposition.List[DiaporamaObject->List[i]->ShotComposition.List.count()-1]->CopyFromCompositionObject(&ShotBlock);
            // Ensure new object is not visible in previous shot
            if (i<CurrentShot) {
                DiaporamaObject->List[i]->ShotComposition.List[DiaporamaObject->List[i]->ShotComposition.List.count()-1]->IsVisible=false;
                // Ensure unvisible video have no sound !
                if (DiaporamaObject->List[i]->ShotComposition.List[DiaporamaObject->List[i]->ShotComposition.List.count()-1]->BackgroundBrush->Video!=NULL)
                    DiaporamaObject->List[i]->ShotComposition.List[DiaporamaObject->List[i]->ShotComposition.List.count()-1]->BackgroundBrush->SoundVolume=0;
            }
        }

        // Inc NextIndexKey
        DiaporamaObject->NextIndexKey++;

        // 10 by 10 step for ZValue
        NextZValue+=10;

        RefreshBlockTable(CompositionList->List.count()-1);
    }
}
