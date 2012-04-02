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

#include "../sharedfiles/DlgInfoFile.h"
#include "cCustomBlockTable.h"
#include "DlgSlideProperties.h"
#include "ui_DlgSlideProperties.h"
#include "wgt_QCustomThumbnails.h"
#include "DlgSlideProperties.h"
#include "DlgImageCorrection.h"
#include "DlgVideoEdit.h"
#include "DlgTextEdit.h"
#include "DlgRulerDef.h"
#include "_ApplicationDefinitions.h"
#include "mainwindow.h"

#include <QClipboard>
#include <QMimeData>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QTextBlockFormat>
#include <QMenu>

#define ICON_FRAMING_CUSTOM                 ":/img/action_cancel.png"
#define ICON_FRAMING_FULL                   ":/img/AdjustWH.png"
#define ICON_FRAMING_WIDTH                  ":/img/AdjustW.png"
#define ICON_FRAMING_HEIGHT                 ":/img/AdjustH.png"
#define ICON_GLOBALCONF                     ":/img/db.png"
#define ICON_USERCONF                       ":/img/db_update.png"

#define ICON_GEOMETRY_LOCKED                ":/img/Geometry_Lock.png"
#define ICON_GEOMETRY_UNLOCKED              ":/img/Geometry_Unlock.png"
#define ICON_GEOMETRY_PROJECT               ":/img/Geometry_ProjectLock.png"
#define ICON_VISIBLE_OK                     ":/img/Visible_OK.png"
#define ICON_VISIBLE_KO                     ":/img/Visible_KO.png"
#define ICON_SOUND_OK                       ":/img/object_sound.png"
#define ICON_SOUND_KO                       ":/img/sound_KO.png"
#define ICON_OBJECT_TEXT                    ":/img/object_text.png"
#define ICON_OBJECT_SOUND                   ":/img/object_sound.png"
#define ICON_OBJECT_MOVIE                   ":/img/object_movie.png"
#define ICON_OBJECT_IMAGE                   ":/img/object_image.png"
#define ICON_OBJECT_TEXTHIDE                ":/img/object_textHide.png"
#define ICON_OBJECT_MOVIEHIDE               ":/img/object_movieHide.png"
#define ICON_OBJECT_IMAGEHIDE               ":/img/object_imageHide.png"
#define ICON_RULER_ON                       ":/img/ruler_ok.png"
#define ICON_RULER_OFF                      ":/img/ruler_ko.png"

//********************************************************************************************************************************
// DlgSlideProperties : Slide Dialog
//********************************************************************************************************************************

DlgSlideProperties::DlgSlideProperties(cDiaporamaObject *DiaporamaObject,QString HelpURL,cBaseApplicationConfig *ApplicationConfig,cSaveWindowPosition *DlgWSP,QWidget *parent):QCustomDialog(HelpURL,ApplicationConfig,DlgWSP,parent),ui(new Ui::DlgSlideProperties) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::DlgSlideProperties");

    ui->setupUi(this);
    OkBt        =ui->OKBT;
    CancelBt    =ui->CancelBt;
    HelpBt      =ui->HelpBT;
    CurrentSlide=DiaporamaObject;
    CurrentShot =NULL;

    GlobalMainWindow->DragItemSource=-1;
    GlobalMainWindow->DragItemDest  =-1;
    GlobalMainWindow->IsDragOn      =0;

    ui->InteractiveZone->BlockTable     =ui->BlockTable;
    ui->InteractiveZone->DiaporamaObject=CurrentSlide;
    ui->InteractiveZone->MagneticRuler  =CurrentSlide->Parent->ApplicationConfig->SlideRuler;
}

//====================================================================================================================

DlgSlideProperties::~DlgSlideProperties() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::~DlgSlideProperties");
    InRefreshControls=true; // To ensure no controls will change during delete
    delete ui;
}

//====================================================================================================================
// Initialise dialog

void DlgSlideProperties::DoInitDialog() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::DoInitDialog");

    ui->SplitterTop->setCollapsible(0,false);
    ui->SplitterTop->setCollapsible(1,false);

    setWindowTitle(windowTitle()+" - "+QApplication::translate("DlgSlideProperties","Slide")+QString(" %1/%2").arg(CurrentSlide->Parent->CurrentCol+1).arg(CurrentSlide->Parent->List.count()));
    ui->NewChapterCB->setChecked(CurrentSlide->StartNewChapter);
    ui->NewChapterCB->setEnabled(CurrentSlide->Parent->CurrentCol!=0);
    ui->ChapterLabel->setEnabled(CurrentSlide->Parent->CurrentCol!=0);
    ui->OKPreviousBT->setEnabled(CurrentSlide->Parent->CurrentCol>0);
    ui->OKNextBT->setEnabled(CurrentSlide->Parent->CurrentCol<CurrentSlide->Parent->List.count()-1);
    ui->TVMarginsBT->setIcon(QIcon(QString(ui->InteractiveZone->MagneticRuler!=0?ICON_RULER_ON:ICON_RULER_OFF)));

    switch (CurrentSlide->Parent->ImageGeometry) {
        case GEOMETRY_4_3   : DisplayH=1440;    DisplayW=1920;     break;
        case GEOMETRY_16_9  : DisplayH=1080;    DisplayW=1920;     break;
        case GEOMETRY_40_17 : DisplayH=816;     DisplayW=1920;     break;

    }
    ProjectGeometry=DisplayH/DisplayW;
    ProjectGeometry=QString("%1").arg(ProjectGeometry,0,'e').toDouble();  // Rounded to same number as style managment

    //******************************

    CompositionList         = NULL;

    // Re-entrence flags
    InRefreshStyleControls  =false;
    InRefreshControls       =false;
    StopMajFramingStyle     =false;
    InSelectionChange       =false;

    ui->SlideNameED->setText(CurrentSlide->SlideName);

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
    if (CurrentSlide->Parent->ApplicationConfig->DisplayUnit==DISPLAYUNIT_PERCENT) {
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

    // Init block animation type
    ui->BlockAnimCB->addItem(QApplication::translate("DlgSlideProperties","None"));
    ui->BlockAnimCB->addItem(QApplication::translate("DlgSlideProperties","Multiple block turn"));
    ui->BlockAnimCB->addItem(QApplication::translate("DlgSlideProperties","Dissolve"));

    // Init Dissolve animation value
    ui->DissolveCB->addItem(QApplication::translate("DlgSlideProperties","Appear"));
    ui->DissolveCB->addItem(QApplication::translate("DlgSlideProperties","Disappear"));
    ui->DissolveCB->addItem(QApplication::translate("DlgSlideProperties","Blink at slow speed"));
    ui->DissolveCB->addItem(QApplication::translate("DlgSlideProperties","Blink at medium speed"));
    ui->DissolveCB->addItem(QApplication::translate("DlgSlideProperties","Blink at fast speed"));
    ui->DissolveCB->addItem(QApplication::translate("DlgSlideProperties","Blink at very fast speed"));

    // Force icon in contextual menu
    ui->actionAddTextBlock->setIconVisibleInMenu(true);
    ui->actionAddFile->setIconVisibleInMenu(true);
    ui->actionEditText->setIconVisibleInMenu(true);
    ui->actionEditImage->setIconVisibleInMenu(true);
    ui->actionEditVideo->setIconVisibleInMenu(true);
    ui->actionUpBlock->setIconVisibleInMenu(true);
    ui->actionDownBlock->setIconVisibleInMenu(true);
    ui->actionSetVisible->setIconVisibleInMenu(true);
    ui->actionTakeSound->setIconVisibleInMenu(true);
    ui->actionCopy->setIconVisibleInMenu(true);
    ui->actionCut->setIconVisibleInMenu(true);
    ui->actionPaste->setIconVisibleInMenu(true);
    ui->actionRemoveBlock->setIconVisibleInMenu(true);
    ui->actionInfo->setIconVisibleInMenu(true);
    ui->actionTop->setIconVisibleInMenu(true);
    ui->actionMiddle->setIconVisibleInMenu(true);
    ui->actionBottom->setIconVisibleInMenu(true);
    ui->actionLeft->setIconVisibleInMenu(true);
    ui->actionCenter->setIconVisibleInMenu(true);
    ui->actionRight->setIconVisibleInMenu(true);

    // Define handler
    connect(ui->OKPreviousBT,SIGNAL(clicked()),this,SLOT(OKPrevious()));
    connect(ui->OKNextBT,SIGNAL(clicked()),this,SLOT(OKNext()));
    connect(ui->TVMarginsBT,SIGNAL(clicked()),this,SLOT(s_TVMarginsBt()));

    connect(ui->CopyBlockBT,SIGNAL(clicked()),this,SLOT(s_BlockTable_Copy()));                                          connect(ui->actionCopy,SIGNAL(triggered()),this,SLOT(s_BlockTable_Copy()));
    connect(ui->CutBlockBT,SIGNAL(clicked()),this,SLOT(s_BlockTable_Cut()));                                            connect(ui->actionCut,SIGNAL(triggered()),this,SLOT(s_BlockTable_Cut()));
    connect(ui->PasteBlockBT,SIGNAL(clicked()),this,SLOT(s_BlockTable_Paste()));                                        connect(ui->actionPaste,SIGNAL(triggered()),this,SLOT(s_BlockTable_Paste()));

    connect(ui->VisibleBT,SIGNAL(clicked()),this,SLOT(s_BlockSettings_ToggleVisibleState()));                           connect(ui->actionSetVisible,SIGNAL(triggered()),this,SLOT(s_BlockSettings_ToggleVisibleState()));
    connect(ui->SoundBT,SIGNAL(clicked()),this,SLOT(s_BlockSettings_GetSound()));                                       connect(ui->actionTakeSound,SIGNAL(triggered()),this,SLOT(s_BlockSettings_GetSound()));

    connect(ui->BlockUpBT,SIGNAL(clicked()),this,SLOT(s_BlockTable_MoveBlockUp()));                                     connect(ui->actionUpBlock,SIGNAL(triggered()),this,SLOT(s_BlockTable_MoveBlockUp()));
    connect(ui->BlockDownBT,SIGNAL(clicked()),this,SLOT(s_BlockTable_MoveBlockDown()));                                 connect(ui->actionDownBlock,SIGNAL(triggered()),this,SLOT(s_BlockTable_MoveBlockDown()));

    connect(ui->actionTop,SIGNAL(triggered()),this,SLOT(s_BlockTable_AlignTop()));
    connect(ui->actionMiddle,SIGNAL(triggered()),this,SLOT(s_BlockTable_AlignMiddle()));
    connect(ui->actionBottom,SIGNAL(triggered()),this,SLOT(s_BlockTable_AlignBottom()));
    connect(ui->actionLeft,SIGNAL(triggered()),this,SLOT(s_BlockTable_AlignLeft()));
    connect(ui->actionCenter,SIGNAL(triggered()),this,SLOT(s_BlockTable_AlignCenter()));
    connect(ui->actionRight,SIGNAL(triggered()),this,SLOT(s_BlockTable_AlignRight()));

    connect(ui->SlideNameED,SIGNAL(textEdited(QString)),this,SLOT(s_SlideSet_SlideNameChange(QString)));
    connect(ui->NewChapterCB,SIGNAL(stateChanged(int)),this,SLOT(s_SlideSet_NewChapter(int)));
    connect(ui->ShotDurationED,SIGNAL(timeChanged(QTime)),this,SLOT(s_ShotTable_DurationChange(QTime)));

    connect(ui->ArrangeBT,SIGNAL(pressed()),this,SLOT(s_BlockSettings_Arrange()));
    connect(ui->TextEditBT,SIGNAL(clicked()),this,SLOT(s_BlockSettings_TextEditor()));                                  connect(ui->actionEditText,SIGNAL(triggered()),this,SLOT(s_BlockSettings_TextEditor()));
    connect(ui->ImageEditBT,SIGNAL(clicked()),this,SLOT(s_BlockSettings_ImageEditCorrect()));                           connect(ui->actionEditImage,SIGNAL(triggered()),this,SLOT(s_BlockSettings_ImageEditCorrect()));
    connect(ui->VideoEditBT,SIGNAL(clicked()),this,SLOT(s_BlockSettings_VideoEdit()));                                  connect(ui->actionEditVideo,SIGNAL(triggered()),this,SLOT(s_BlockSettings_VideoEdit()));
    connect(ui->InfoBt,SIGNAL(clicked()),this,SLOT(s_BlockSettings_Information()));                                     connect(ui->actionInfo,SIGNAL(triggered()),this,SLOT(s_BlockSettings_Information()));

    connect(ui->ShotTable,SIGNAL(itemSelectionChanged()),this,SLOT(s_ShotTable_SelectionChanged()));
    connect(ui->ShotTable,SIGNAL(DragMoveItem()),this,SLOT(s_ShotTable_DragMoveItem()));
    connect(ui->AddShot,SIGNAL(pressed()),this,SLOT(s_ShotTable_AddShot()));
    connect(ui->RemoveShot,SIGNAL(pressed()),this,SLOT(s_ShotTable_RemoveShot()));
    connect(ui->ShotTableMoveLeftBt,SIGNAL(clicked()),this,SLOT(s_ShotTable_MoveLeft()));
    connect(ui->ShotTableMoveRightBt,SIGNAL(clicked()),this,SLOT(s_ShotTable_MoveRight()));

    connect(ui->PosXEd,SIGNAL(valueChanged(double)),this,SLOT(s_BlockSettings_PosXValue(double)));
    connect(ui->PosYEd,SIGNAL(valueChanged(double)),this,SLOT(s_BlockSettings_PosYValue(double)));
    connect(ui->WidthEd,SIGNAL(valueChanged(double)),this,SLOT(s_BlockSettings_PosWidthValue(double)));
    connect(ui->HeightEd,SIGNAL(valueChanged(double)),this,SLOT(s_BlockSettings_PosHeightValue(double)));
    connect(ui->BackgroundFormCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_BlockSettings_ShapeBackgroundForm(int)));
    connect(ui->OpacityCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_BlockSettings_ShapeOpacity(int)));
    connect(ui->PenStyleCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_BlockSettings_ShapePenStyle(int)));
    connect(ui->ShadowEffectCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_BlockSettings_ShapeShadowFormValue(int)));
    connect(ui->ShadowEffectED,SIGNAL(valueChanged(int)),this,SLOT(s_BlockSettings_ShapeShadowDistanceValue(int)));

    connect(ui->PenColorCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_BlockSettings_ShapePenColor(int)));
    connect(ui->PenSizeEd,SIGNAL(valueChanged(int)),this,SLOT(s_BlockSettings_ShapePenSize(int)));
    connect(ui->ShadowColorCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_BlockSettings_ShapeShadowColor(int)));

    connect(ui->RotateXED,SIGNAL(valueChanged(int)),this,SLOT(s_BlockSettings_RotateXValue(int)));     connect(ui->RotateXSLD,SIGNAL(valueChanged(int)),this,SLOT(s_BlockSettings_RotateXValue(int)));        connect(ui->ResetRotateXBT,SIGNAL(released()),this,SLOT(s_BlockSettings_ResetRotateXValue()));
    connect(ui->RotateYED,SIGNAL(valueChanged(int)),this,SLOT(s_BlockSettings_RotateYValue(int)));     connect(ui->RotateYSLD,SIGNAL(valueChanged(int)),this,SLOT(s_BlockSettings_RotateYValue(int)));        connect(ui->ResetRotateYBT,SIGNAL(released()),this,SLOT(s_BlockSettings_ResetRotateYValue()));
    connect(ui->RotateZED,SIGNAL(valueChanged(int)),this,SLOT(s_BlockSettings_RotateZValue(int)));     connect(ui->RotateZSLD,SIGNAL(valueChanged(int)),this,SLOT(s_BlockSettings_RotateZValue(int)));        connect(ui->ResetRotateZBT,SIGNAL(released()),this,SLOT(s_BlockSettings_ResetRotateZValue()));

    // Block animation
    connect(ui->BlockAnimCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_BlockSettings_BlockAnimType(int)));
    connect(ui->TurnZED,SIGNAL(valueChanged(int)),this,SLOT(s_BlockSettings_BlockAnimTurnZValue(int)));         connect(ui->TurnZSlider,SIGNAL(valueChanged(int)),this,SLOT(s_BlockSettings_BlockAnimTurnZValue(int)));         connect(ui->ResetTurnZBT,SIGNAL(released()),this,SLOT(s_BlockSettings_BlockAnimTurnZReset()));
    connect(ui->TurnXED,SIGNAL(valueChanged(int)),this,SLOT(s_BlockSettings_BlockAnimTurnXValue(int)));         connect(ui->TurnXSlider,SIGNAL(valueChanged(int)),this,SLOT(s_BlockSettings_BlockAnimTurnXValue(int)));         connect(ui->ResetTurnXBT,SIGNAL(released()),this,SLOT(s_BlockSettings_BlockAnimTurnXReset()));
    connect(ui->TurnYED,SIGNAL(valueChanged(int)),this,SLOT(s_BlockSettings_BlockAnimTurnYValue(int)));         connect(ui->TurnYSlider,SIGNAL(valueChanged(int)),this,SLOT(s_BlockSettings_BlockAnimTurnYValue(int)));         connect(ui->ResetTurnYBT,SIGNAL(released()),this,SLOT(s_BlockSettings_BlockAnimTurnYReset()));
    connect(ui->DissolveCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_BlockSettings_BlockAnimDissolveType(int)));

    // Block table/scene part
    connect(ui->BlockTable,SIGNAL(itemSelectionChanged()),this,SLOT(s_BlockTable_SelectionChanged()));
    connect(ui->BlockTable,SIGNAL(DoubleClickEvent(QMouseEvent *)),this,SLOT(s_BlockTable_ItemDoubleClicked(QMouseEvent *)));
    connect(ui->BlockTable,SIGNAL(RightClickEvent(QMouseEvent *)),this,SLOT(s_BlockTable_ItemRightClicked(QMouseEvent *)));
    connect(ui->AddTextBlock,SIGNAL(pressed()),this,SLOT(s_BlockTable_AddNewTextBlock()));                                          connect(ui->actionAddTextBlock,SIGNAL(triggered()),this,SLOT(s_BlockTable_AddNewTextBlock()));
    connect(ui->AddFileBlock,SIGNAL(pressed()),this,SLOT(s_BlockTable_AddNewFileBlock()));                                          connect(ui->actionAddFile,SIGNAL(triggered()),this,SLOT(s_BlockTable_AddNewFileBlock()));
    connect(ui->RemoveBlock,SIGNAL(pressed()),this,SLOT(s_BlockTable_RemoveBlock()));                                               connect(ui->actionRemoveBlock,SIGNAL(triggered()),this,SLOT(s_BlockTable_RemoveBlock()));

    // Style buttons
    connect(ui->FramingStyleCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChangeFramingStyle(int)));
    connect(ui->CoordinateStyleBT,SIGNAL(pressed()),this,SLOT(s_CoordinateStyleBT()));
    connect(ui->BlockShapeStyleBT,SIGNAL(pressed()),this,SLOT(s_BlockShapeStyleBT()));

    // Text annimation
    connect(ui->ZoomSlider,SIGNAL(valueChanged(int)),this,SLOT(s_BlockSettings_TextAnimZoom(int)));
    connect(ui->ZoomED,SIGNAL(valueChanged(int)),this,SLOT(s_BlockSettings_TextAnimZoom(int)));
    connect(ui->ZoomResetBT,SIGNAL(released()),this,SLOT(s_BlockSettings_TextAnimZoomReset()));
    connect(ui->ScrollXSlider,SIGNAL(valueChanged(int)),this,SLOT(s_BlockSettings_TextAnimScrollX(int)));
    connect(ui->ScrollXED,SIGNAL(valueChanged(int)),this,SLOT(s_BlockSettings_TextAnimScrollX(int)));
    connect(ui->ScrollXResetBT,SIGNAL(released()),this,SLOT(s_BlockSettings_TextAnimScrollXReset()));
    connect(ui->ScrollYSlider,SIGNAL(valueChanged(int)),this,SLOT(s_BlockSettings_TextAnimScrollY(int)));
    connect(ui->ScrollYED,SIGNAL(valueChanged(int)),this,SLOT(s_BlockSettings_TextAnimScrollY(int)));
    connect(ui->ScrollYResetBT,SIGNAL(released()),this,SLOT(s_BlockSettings_TextAnimScrollYReset()));

    // Interactive zone
    connect(ui->InteractiveZone,SIGNAL(StartSelectionChange()),this,SLOT(s_BlockTable_StartSelectionChange()));
    connect(ui->InteractiveZone,SIGNAL(EndSelectionChange()),this,SLOT(s_BlockTable_EndSelectionChange()));
    connect(ui->InteractiveZone,SIGNAL(RightClickEvent(QMouseEvent *)),this,SLOT(s_BlockTable_ItemRightClicked(QMouseEvent *)));
    connect(ui->InteractiveZone,SIGNAL(DoubleClickEvent(QMouseEvent *)),this,SLOT(s_BlockTable_ItemDoubleClicked(QMouseEvent *)));
    connect(ui->InteractiveZone,SIGNAL(TransformBlock(double,double,double,double,double,double,double,double)),this,SLOT(s_BlockSettings_IntZoneTransformBlocks(double,double,double,double,double,double,double,double)));
    connect(ui->InteractiveZone,SIGNAL(DisplayTransformBlock(double,double,double,double,double,double,double,double)),this,SLOT(s_BlockSettings_IntZoneDisplayTransformBlocks(double,double,double,double,double,double,double,double)));

    s_Event_ClipboardChanged();           // Setup clipboard button state
    connect(QApplication::clipboard(),SIGNAL(dataChanged()),this,SLOT(s_Event_ClipboardChanged()));
}

//====================================================================================================================

void DlgSlideProperties::resizeEvent(QResizeEvent *) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::resizeEvent");

    ui->InteractiveZone->RefreshDisplay();
}

//====================================================================================================================

void DlgSlideProperties::showEvent(QShowEvent *ev) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::showEvent");

    QCustomDialog::showEvent(ev);

    if (CurrentShot==NULL) {
        // Init ShotTable
        ui->ShotTable->setUpdatesEnabled(false);
        for (int i=0;i<CurrentSlide->List.count();i++) {
            ui->ShotTable->insertColumn(i);
            wgt_QCustomThumbnails *Object=new wgt_QCustomThumbnails(ui->ShotTable,THUMBNAILTYPE_SHOT);
            Object->DiaporamaObject=CurrentSlide;
            ui->ShotTable->setCellWidget(0,i,Object);
            ui->ShotTable->setColumnWidth(i,CurrentSlide->Parent->GetWidthForHeight(ui->ShotTable->rowHeight(0)));
        }
        ui->ShotTable->setCurrentCell(0,0);
        ui->ShotTable->setUpdatesEnabled(true);
    }
}

//====================================================================================================================
// Initiale Undo

void DlgSlideProperties::PrepareGlobalUndo() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::PrepareGlobalUndo");

    // Save object before modification for cancel button
    Undo=new QDomDocument(APPLICATION_NAME);
    QDomElement root=Undo->createElement("UNDO-DLG");       // Create xml document and root
    CurrentSlide->SaveToXML(root,"UNDO-DLG-OBJECT",CurrentSlide->Parent->ProjectFileName,true);  // Save object
    Undo->appendChild(root);                                // Add object to xml document
}

//====================================================================================================================
// Apply Undo : call when user click on Cancel button

void DlgSlideProperties::DoGlobalUndo() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::DoGlobalUndo");

    QDomElement root=Undo->documentElement();
    if (root.tagName()=="UNDO-DLG") CurrentSlide->LoadFromXML(root,"UNDO-DLG-OBJECT","",NULL);
}

//====================================================================================================================

void DlgSlideProperties::MakeFormIcon(QComboBox *UICB) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::MakeFormIcon");

    for (int i=0;i<UICB->count();i++) {
        cCompositionObject Object(COMPOSITIONTYPE_BACKGROUND,0,((cApplicationConfig *)BaseApplicationConfig));
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
        Object.DrawCompositionObject(&Painter,1,0,0,UICB->iconSize().width(),UICB->iconSize().height(),true,0,0,NULL,1,NULL,false,0,false);
        Painter.end();
        UICB->setItemIcon(i,QIcon(Image));
    }
}

//====================================================================================================================

void DlgSlideProperties::MakeBorderStyleIcon(QComboBox *UICB) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::MakeBorderStyleIcon");

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

void DlgSlideProperties::SaveWindowState() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomDialog::SaveWindowState");

    // Save Window size and position
    if (DlgWSP) ((cSaveDlgSlideProperties *)DlgWSP)->SaveWindowState(this,ui->SplitterTop);
}

//====================================================================================================================

void DlgSlideProperties::RestoreWindowState() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomDialog::RestoreWindowState");

    // Restore window size and position
    if (DlgWSP) ((cSaveDlgSlideProperties *)DlgWSP)->ApplyToWindow(this,ui->SplitterTop);
}

//====================================================================================================================

void DlgSlideProperties::GetForDisplayUnit(double &DisplayW,double &DisplayH) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::GetForDisplayUnit");

    if (CurrentSlide->Parent->ImageGeometry==GEOMETRY_4_3)        { DisplayW=1440; DisplayH=1080; }
    else if (CurrentSlide->Parent->ImageGeometry==GEOMETRY_16_9)  { DisplayW=1920; DisplayH=1080; }
    else if (CurrentSlide->Parent->ImageGeometry==GEOMETRY_40_17) { DisplayW=1920; DisplayH=816;  }
    else { DisplayW=0; DisplayH=0; }
}

//====================================================================================================================
// Call when user click on Ok button

void DlgSlideProperties::OKPrevious() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::OKPrevious");

    SaveWindowState();  // Save Window size and position
    done(2);            // Close the box
}

void DlgSlideProperties::OKNext() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::OKNext");

    SaveWindowState();  // Save Window size and position
    done(3);            // Close the box
}

//====================================================================================================================

void DlgSlideProperties::s_Event_ClipboardChanged() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_Event_ClipboardChanged");

    ui->PasteBlockBT->setEnabled((QApplication::clipboard())&&(QApplication::clipboard()->mimeData())&&(QApplication::clipboard()->mimeData()->hasFormat("ffDiaporama/block")));
    ui->actionPaste->setEnabled((QApplication::clipboard())&&(QApplication::clipboard()->mimeData())&&(QApplication::clipboard()->mimeData()->hasFormat("ffDiaporama/block")));
}

//====================================================================================================================

void DlgSlideProperties::RefreshStyleControls() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::RefreshStyleControls");

    if (InRefreshStyleControls) return;
    InRefreshStyleControls=true;

    bool IsVisible=(BlockSelectMode==SELECTMODE_ONE)&&(CurrentCompoObject->IsVisible);
    if (ui->CoordinateStyleBT->isEnabled()!=IsVisible)  ui->CoordinateStyleBT->setEnabled(IsVisible);
    if (ui->CoordinateStyleED->isEnabled()!=IsVisible)  ui->CoordinateStyleED->setEnabled(IsVisible);
    if (ui->BlockShapeStyleBT->isEnabled()!=IsVisible)  ui->BlockShapeStyleBT->setEnabled(IsVisible);
    if (ui->BlockShapeStyleED->isEnabled()!=IsVisible)  ui->BlockShapeStyleED->setEnabled(IsVisible);
    if (ui->FramingStyleCB->isEnabled()!=IsVisible)     ui->FramingStyleCB->setEnabled(IsVisible);
    if (ui->FramingStyleLabel->isEnabled()!=IsVisible)  ui->FramingStyleLabel->setEnabled(IsVisible);

    if (IsVisible) {
        ui->BlockShapeStyleED->setText(((cApplicationConfig *)BaseApplicationConfig)->StyleBlockShapeCollection.GetStyleName(CurrentCompoObject->GetBlockShapeStyle()));
        ui->CoordinateStyleED->setText(((cApplicationConfig *)BaseApplicationConfig)->StyleCoordinateCollection.GetStyleName(CurrentCompoObject->GetCoordinateStyle()));

        if ((CurrentCompoObject->BackgroundBrush->Video==NULL)&&(CurrentCompoObject->BackgroundBrush->Image==NULL)) {

            // It's a text block
            StopMajFramingStyle=true;
            ui->FramingStyleLabel->setPixmap(QPixmap(":/img/Geometry.png"));
            if (ui->FramingStyleCB->itemText(0)!=QApplication::translate("DlgImageCorrection","Unlock")) {
                ui->FramingStyleCB->setUpdatesEnabled(false);
                ui->FramingStyleCB->clear();
                ui->FramingStyleCB->addItem(QIcon(ICON_GEOMETRY_UNLOCKED),QApplication::translate("DlgImageCorrection","Unlock"));
                ui->FramingStyleCB->addItem(QIcon(ICON_GEOMETRY_LOCKED),  QApplication::translate("DlgImageCorrection","Lock to this geometry"));
                ui->FramingStyleCB->addItem(QIcon(ICON_GEOMETRY_PROJECT), QApplication::translate("DlgImageCorrection","Lock to project geometry"));
            }
            int Index=1;
            if (!CurrentCompoObject->BackgroundBrush->LockGeometry) Index=0; else if (CurrentCompoObject->BackgroundBrush->AspectRatio==ProjectGeometry) Index=2;
            if (ui->FramingStyleCB->currentIndex()!=Index) ui->FramingStyleCB->setCurrentIndex(Index);
            if (!ui->FramingStyleCB->updatesEnabled()) ui->FramingStyleCB->setUpdatesEnabled(true);
            StopMajFramingStyle=false;

        } else {

            // Image or video
            ui->FramingStyleLabel->setPixmap(QPixmap(":/img/EditImage.png"));
            CurrentCompoObject->BackgroundBrush->InitDefaultFramingStyle(CurrentCompoObject->BackgroundBrush->LockGeometry,CurrentCompoObject->BackgroundBrush->AspectRatio);
            if (CurrentCompoObject->BackgroundBrush->Image!=NULL)       ((cApplicationConfig *)BaseApplicationConfig)->StyleImageFramingCollection.SetImageGeometryFilter(CurrentSlide->Parent->ImageGeometry,CurrentCompoObject->BackgroundBrush->Image->ObjectGeometry);
            else if (CurrentCompoObject->BackgroundBrush->Video!=NULL)  ((cApplicationConfig *)BaseApplicationConfig)->StyleImageFramingCollection.SetImageGeometryFilter(CurrentSlide->Parent->ImageGeometry,CurrentCompoObject->BackgroundBrush->Video->ObjectGeometry);

            QString StyleDef =CurrentCompoObject->GetFramingStyle();
            QString StyleName=((cApplicationConfig *)BaseApplicationConfig)->StyleImageFramingCollection.GetStyleName(StyleDef);
            if (StyleName=="") {
                if (StyleDef==CurrentCompoObject->BackgroundBrush->DefaultFramingW)      StyleName=QApplication::translate("DlgManageStyle","Adjust to image width");
                else if (StyleDef==CurrentCompoObject->BackgroundBrush->DefaultFramingH) StyleName=QApplication::translate("DlgManageStyle","Adjust to image height");
                else if (StyleDef==CurrentCompoObject->BackgroundBrush->DefaultFramingF) StyleName=QApplication::translate("DlgManageStyle","Adjust to full image");
                else StyleName=QApplication::translate("DlgManageStyle","Custom");
            }
            StopMajFramingStyle=true;
            ((cApplicationConfig *)BaseApplicationConfig)->StyleImageFramingCollection.FillCollectionCB(ui->FramingStyleCB,StyleName,true);
            StopMajFramingStyle=false;
        }

    } else {
        ui->BlockShapeStyleED->setText("");
        ui->CoordinateStyleED->setText("");
        ui->FramingStyleLabel->setPixmap(QPixmap(":/img/EditImage.png"));
        if ((ui->FramingStyleCB->count()!=1)||(ui->FramingStyleCB->currentIndex()!=0)||(ui->FramingStyleCB->itemText(0)!=QApplication::translate("DlgManageStyle","Custom"))) {
            ui->FramingStyleCB->clear();
            ui->FramingStyleCB->addItem(QIcon(ICON_FRAMING_CUSTOM),QApplication::translate("DlgManageStyle","Custom"));
            ui->FramingStyleCB->setCurrentIndex(0);
        }
    }

    InRefreshStyleControls=false;
}

//====================================================================================================================

void DlgSlideProperties::RefreshControls(bool UpdateInteractiveZone) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::RefreshControls");

    if ((InRefreshControls)||(!CompositionList)) return;
    InRefreshControls=true;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    //***********************
    // Tools button
    //***********************
    ui->VisibleBT->         setEnabled((BlockSelectMode==SELECTMODE_ONE));
    ui->SoundBT->           setEnabled((BlockSelectMode==SELECTMODE_ONE)&&(CurrentCompoObject->IsVisible) &&(CurrentCompoObject->BackgroundBrush->Video!=NULL)&&(CurrentCompoObject->BackgroundBrush->SoundVolume==0));
    ui->ImageEditBT->       setEnabled((BlockSelectMode==SELECTMODE_ONE)&&(CurrentCompoObject->IsVisible) &&(CurrentCompoObject->BackgroundBrush->BrushType==BRUSHTYPE_IMAGEDISK));
    ui->VideoEditBT->       setEnabled((BlockSelectMode==SELECTMODE_ONE)&&(CurrentCompoObject->IsVisible) &&(CurrentCompoObject->BackgroundBrush->Video!=NULL));
    ui->TextEditBT->        setEnabled((BlockSelectMode==SELECTMODE_ONE)&&(CurrentCompoObject->IsVisible));
    ui->InfoBt->            setEnabled((BlockSelectMode==SELECTMODE_ONE)&&(CurrentCompoObject->IsVisible)&&(CurrentCompoObject->BackgroundBrush->BrushType==BRUSHTYPE_IMAGEDISK));
    ui->RemoveBlock->       setEnabled((BlockSelectMode==SELECTMODE_ONE)||(BlockSelectMode==SELECTMODE_MULTIPLE));
    ui->BlockUpBT->         setEnabled((BlockSelectMode==SELECTMODE_ONE)&&(CurrentCompoObjectNbr>0));
    ui->BlockDownBT->       setEnabled((BlockSelectMode==SELECTMODE_ONE)&&(CurrentCompoObjectNbr<ui->BlockTable->rowCount()-1));
    ui->CopyBlockBT->       setEnabled((BlockSelectMode==SELECTMODE_ONE)||(BlockSelectMode==SELECTMODE_MULTIPLE));
    ui->CutBlockBT->        setEnabled((BlockSelectMode==SELECTMODE_ONE)||(BlockSelectMode==SELECTMODE_MULTIPLE));

    ui->ArrangeBT->         setEnabled((BlockSelectMode==SELECTMODE_MULTIPLE));
    ui->actionTop->         setEnabled((BlockSelectMode==SELECTMODE_MULTIPLE));
    ui->actionMiddle->      setEnabled((BlockSelectMode==SELECTMODE_MULTIPLE));
    ui->actionBottom->      setEnabled((BlockSelectMode==SELECTMODE_MULTIPLE));
    ui->actionLeft->        setEnabled((BlockSelectMode==SELECTMODE_MULTIPLE));
    ui->actionCenter->      setEnabled((BlockSelectMode==SELECTMODE_MULTIPLE));
    ui->actionRight->       setEnabled((BlockSelectMode==SELECTMODE_MULTIPLE));

    // Synchronize action to their corresponding tool button
    ui->actionSetVisible->  setEnabled(ui->VisibleBT->isEnabled());
    ui->actionTakeSound->   setEnabled(ui->SoundBT->isEnabled());
    ui->actionEditImage->   setEnabled(ui->ImageEditBT->isEnabled());
    ui->actionEditVideo->   setEnabled(ui->VideoEditBT->isEnabled());
    ui->actionEditText->    setEnabled(ui->TextEditBT->isEnabled());
    ui->actionInfo->        setEnabled(ui->InfoBt->isEnabled());
    ui->actionRemoveBlock-> setEnabled(ui->RemoveBlock->isEnabled());
    ui->actionUpBlock->     setEnabled(ui->BlockUpBT->isEnabled());
    ui->actionDownBlock->   setEnabled(ui->BlockDownBT->isEnabled());
    ui->actionCopy->        setEnabled(ui->CopyBlockBT->isEnabled());
    ui->actionCut->         setEnabled(ui->CutBlockBT->isEnabled());

    // Change icons for visible and getsound buttons/actions
    if (BlockSelectMode==SELECTMODE_ONE) {
        ui->VisibleBT->setIcon(QIcon(QString(!CurrentCompoObject->IsVisible?ICON_VISIBLE_OK:ICON_VISIBLE_KO)));
        ui->actionSetVisible->setIcon(QIcon(QString(!CurrentCompoObject->IsVisible?ICON_VISIBLE_OK:ICON_VISIBLE_KO)));
        ui->SoundBT->setIcon(QIcon(QString((CurrentCompoObject==NULL)||(CurrentCompoObject->BackgroundBrush->Video==NULL)||(CurrentCompoObject->BackgroundBrush->SoundVolume==0)?ICON_SOUND_OK:ICON_SOUND_KO)));
        ui->actionTakeSound->setIcon(QIcon(QString((CurrentCompoObject==NULL)||(CurrentCompoObject->BackgroundBrush->Video==NULL)||(CurrentCompoObject->BackgroundBrush->SoundVolume==0)?ICON_SOUND_OK:ICON_SOUND_KO)));
    }

    //*****************************
    // Position, size and rotation
    //*****************************
    if ((BlockSelectMode==SELECTMODE_ONE)&&(CurrentCompoObject->IsVisible)) {

        ui->PosSize_X->     setEnabled(true);               ui->PosXEd->    setEnabled(true);
        ui->PosSize_Y->     setEnabled(true);               ui->PosYEd->    setEnabled(true);
        ui->PosSize_Width-> setEnabled(true);               ui->WidthEd->   setEnabled(true);
        ui->PosSize_Height->setEnabled(true);               ui->HeightEd->  setEnabled(true);

        if (CurrentSlide->Parent->ApplicationConfig->DisplayUnit==DISPLAYUNIT_PERCENT) {
            ui->PosXEd->setRange(-200,200);                 ui->PosXEd->    setValue(CurrentCompoObject->x*100);
            ui->PosYEd->setRange(-200,200);                 ui->PosYEd->    setValue(CurrentCompoObject->y*100);
            ui->WidthEd->setRange(1,200);                   ui->WidthEd->   setValue(CurrentCompoObject->w*100);
            ui->HeightEd->setRange(1,200);                  ui->HeightEd->  setValue(CurrentCompoObject->h*100);
        } else { // DisplayUnit==DISPLAYUNIT_PIXELS
            double DisplayW=0,DisplayH=0;
            GetForDisplayUnit(DisplayW,DisplayH);
            ui->PosXEd->setRange(-2*DisplayW,2*DisplayW);   ui->PosXEd->    setValue(CurrentCompoObject->x*DisplayW);
            ui->PosYEd->setRange(-2*DisplayH,2*DisplayH);   ui->PosYEd->    setValue(CurrentCompoObject->y*DisplayH);
            ui->WidthEd->setRange(3,2*DisplayW);            ui->WidthEd->   setValue(CurrentCompoObject->w*DisplayW);
            ui->HeightEd->setRange(3,2*DisplayH);           ui->HeightEd->  setValue(CurrentCompoObject->h*DisplayH);
        }
    } else {
        ui->PosSize_X->     setEnabled(false);              ui->PosXEd->    setEnabled(false);                                                  ui->PosXEd->  setValue(0);
        ui->PosSize_Y->     setEnabled(false);              ui->PosYEd->    setEnabled(false);                                                  ui->PosYEd->  setValue(0);
        ui->PosSize_Width-> setEnabled(false);              ui->WidthEd->   setEnabled(false);          ui->WidthEd->setRange(0,2*DisplayW);    ui->WidthEd-> setValue(0);
        ui->PosSize_Height->setEnabled(false);              ui->HeightEd->  setEnabled(false);          ui->HeightEd->setRange(0,2*DisplayH);   ui->HeightEd->setValue(0);
    }

    //***********************
    // Rotation
    //***********************
    if ((BlockSelectMode==SELECTMODE_ONE)&&(CurrentCompoObject->IsVisible)) {
        ui->Rotate_X-> setEnabled(true);       ui->RotateXED-> setEnabled(true);        ui->ResetRotateXBT->setEnabled(true);       ui->RotateXSLD->setEnabled(true);
        ui->Rotate_Y-> setEnabled(true);       ui->RotateYED-> setEnabled(true);        ui->ResetRotateYBT->setEnabled(true);       ui->RotateYSLD->setEnabled(true);
        ui->Rotate_Z-> setEnabled(true);       ui->RotateZED-> setEnabled(true);        ui->ResetRotateZBT->setEnabled(true);       ui->RotateZSLD->setEnabled(true);

        ui->RotateXED->setValue(CurrentCompoObject->RotateXAxis);                       ui->RotateXSLD->setValue(CurrentCompoObject->RotateXAxis);
        ui->RotateYED->setValue(CurrentCompoObject->RotateYAxis);                       ui->RotateYSLD->setValue(CurrentCompoObject->RotateYAxis);
        ui->RotateZED->setValue(CurrentCompoObject->RotateZAxis);                       ui->RotateZSLD->setValue(CurrentCompoObject->RotateZAxis);

    } else {
        ui->Rotate_X-> setEnabled(false);       ui->RotateXED-> setEnabled(false);      ui->ResetRotateXBT->setEnabled(false);       ui->RotateXSLD->setEnabled(false);
        ui->Rotate_Y-> setEnabled(false);       ui->RotateYED-> setEnabled(false);      ui->ResetRotateYBT->setEnabled(false);       ui->RotateYSLD->setEnabled(false);
        ui->Rotate_Z-> setEnabled(false);       ui->RotateZED-> setEnabled(false);      ui->ResetRotateZBT->setEnabled(false);       ui->RotateZSLD->setEnabled(false);

        ui->RotateXED->setValue(0);             ui->RotateXSLD->setValue(0);
        ui->RotateYED->setValue(0);             ui->RotateYSLD->setValue(0);
        ui->RotateZED->setValue(0);             ui->RotateZSLD->setValue(0);
    }

    //***********************
    // Shape part
    //***********************
    if ((BlockSelectMode==SELECTMODE_ONE)&&(CurrentCompoObject->IsVisible)) {
        ui->BackgroundFormCB->  setEnabled(true);
        ui->PenSizeEd->         setEnabled(true);
        ui->PenColorCB->        setEnabled(CurrentCompoObject->PenSize!=0);
        ui->PenStyleCB->        setEnabled(CurrentCompoObject->PenSize!=0);
        ui->OpacityCB->         setEnabled(true);
        ui->ShadowEffectCB->    setEnabled(true);
        ui->ShadowEffectED->    setEnabled(CurrentCompoObject->FormShadow!=0);
        ui->ShadowColorCB->     setEnabled(CurrentCompoObject->FormShadow!=0);

        ui->BackgroundFormCB->  setCurrentIndex(CurrentCompoObject->BackgroundForm-1);
        ui->PenSizeEd->         setValue(int(CurrentCompoObject->PenSize));
        ui->OpacityCB->         setCurrentIndex(CurrentCompoObject->Opacity);
        ui->ShadowEffectCB->    setCurrentIndex(CurrentCompoObject->FormShadow);
        ui->ShadowEffectED->    setValue(int(CurrentCompoObject->FormShadowDistance));
        ui->PenColorCB->        SetCurrentColor(&CurrentCompoObject->PenColor);
        ui->ShadowColorCB->     SetCurrentColor(&CurrentCompoObject->FormShadowColor);

        for (int i=0;i<ui->PenStyleCB->count();i++) if (ui->PenStyleCB->itemData(i).toInt()==CurrentCompoObject->PenStyle) {
            if (i!=ui->PenStyleCB->currentIndex()) ui->PenStyleCB->setCurrentIndex(i);
            break;
        }
    } else {
        ui->BackgroundFormCB->  setEnabled(false);
        ui->PenSizeEd->         setEnabled(false);
        ui->PenColorCB->        setEnabled(false);
        ui->PenStyleCB->        setEnabled(false);
        ui->OpacityCB->         setEnabled(false);
        ui->ShadowEffectCB->    setEnabled(false);
        ui->ShadowEffectED->    setEnabled(false);
        ui->ShadowColorCB->     setEnabled(false);

        ui->BackgroundFormCB->  setCurrentIndex(-1);
        ui->PenSizeEd->         setValue(0);
        ui->OpacityCB->         setCurrentIndex(-1);
        ui->ShadowEffectCB->    setCurrentIndex(-1);
        ui->ShadowEffectED->    setValue(0);
        ui->PenColorCB->        SetCurrentColor(NULL);
        ui->ShadowColorCB->     SetCurrentColor(NULL);
    }

    //**************************
    // Text animation controls
    //**************************
    if ((BlockSelectMode==SELECTMODE_ONE)&&(CurrentCompoObject->IsVisible)) {
        ui->ZoomLabel->   setEnabled(true);         ui->ZoomSlider->   setEnabled(true);        ui->ZoomED->   setEnabled(true);        ui->ZoomResetBT->   setEnabled(true);
        ui->ScrollXLabel->setEnabled(true);         ui->ScrollXSlider->setEnabled(true);        ui->ScrollXED->setEnabled(true);        ui->ScrollXResetBT->setEnabled(true);
        ui->ScrollYLabel->setEnabled(true);         ui->ScrollYSlider->setEnabled(true);        ui->ScrollYED->setEnabled(true);        ui->ScrollYResetBT->setEnabled(true);

        ui->ZoomSlider->setValue(CurrentCompoObject->TxtZoomLevel);            ui->ZoomED->setValue(CurrentCompoObject->TxtZoomLevel);
        ui->ScrollXSlider->setValue(CurrentCompoObject->TxtScrollX);           ui->ScrollXED->setValue(CurrentCompoObject->TxtScrollX);
        ui->ScrollYSlider->setValue(CurrentCompoObject->TxtScrollY);           ui->ScrollYED->setValue(CurrentCompoObject->TxtScrollY);
    } else {
        ui->ZoomLabel->   setEnabled(false);        ui->ZoomSlider->   setEnabled(false);       ui->ZoomED->   setEnabled(false);       ui->ZoomResetBT->   setEnabled(false);
        ui->ScrollXLabel->setEnabled(false);        ui->ScrollXSlider->setEnabled(false);       ui->ScrollXED->setEnabled(false);       ui->ScrollXResetBT->setEnabled(false);
        ui->ScrollYLabel->setEnabled(false);        ui->ScrollYSlider->setEnabled(false);       ui->ScrollYED->setEnabled(false);       ui->ScrollYResetBT->setEnabled(false);

        ui->ZoomSlider->setValue(100);              ui->ZoomED->setValue(100);
        ui->ScrollXSlider->setValue(0);             ui->ScrollXED->setValue(0);
        ui->ScrollYSlider->setValue(0);             ui->ScrollYED->setValue(0);
    }
    //**************************
    // Block animation controls
    //**************************
    if ((BlockSelectMode==SELECTMODE_ONE)&&(CurrentCompoObject->IsVisible)) {
        ui->BlockAnimLabel->setEnabled(true);
        ui->BlockAnimCB->setEnabled(true);
        ui->BlockAnimCB->setCurrentIndex(CurrentCompoObject->BlockAnimType);
    } else {
        ui->BlockAnimLabel->setEnabled(false);
        ui->BlockAnimCB->setEnabled(false);
        ui->BlockAnimCB->setCurrentIndex(-1);
    }

    // Multiple turn animation
    if ((BlockSelectMode==SELECTMODE_ONE)&&(CurrentCompoObject->IsVisible)&&(CurrentCompoObject->BlockAnimType==BLOCKANIMTYPE_MULTIPLETURN)) {
        ui->TurnZLabel->setVisible(true);       ui->TurnZSlider->setVisible(true);      ui->TurnZED->setVisible(true);      ui->ResetTurnZBT->setVisible(true);
        ui->TurnXLabel->setVisible(true);       ui->TurnXSlider->setVisible(true);      ui->TurnXED->setVisible(true);      ui->ResetTurnXBT->setVisible(true);
        ui->TurnYLabel->setVisible(true);       ui->TurnYSlider->setVisible(true);      ui->TurnYED->setVisible(true);      ui->ResetTurnYBT->setVisible(true);
        ui->TurnZED->setValue(CurrentCompoObject->TurnZAxis);                           ui->TurnZSlider->setValue(CurrentCompoObject->TurnZAxis);
        ui->TurnXED->setValue(CurrentCompoObject->TurnXAxis);                           ui->TurnXSlider->setValue(CurrentCompoObject->TurnXAxis);
        ui->TurnYED->setValue(CurrentCompoObject->TurnYAxis);                           ui->TurnYSlider->setValue(CurrentCompoObject->TurnYAxis);
    } else {
        ui->TurnZLabel->setVisible(false);      ui->TurnZSlider->setVisible(false);     ui->TurnZED->setVisible(false);     ui->ResetTurnZBT->setVisible(false);
        ui->TurnXLabel->setVisible(false);      ui->TurnXSlider->setVisible(false);     ui->TurnXED->setVisible(false);     ui->ResetTurnXBT->setVisible(false);
        ui->TurnYLabel->setVisible(false);      ui->TurnYSlider->setVisible(false);     ui->TurnYED->setVisible(false);     ui->ResetTurnYBT->setVisible(false);
    }

    // Dissolve animation
    if ((BlockSelectMode==SELECTMODE_ONE)&&(CurrentCompoObject->IsVisible)&&(CurrentCompoObject->BlockAnimType==BLOCKANIMTYPE_DISSOLVE)) {
        ui->DissolveLabel->setEnabled(true);      ui->DissolveLabel->setVisible(true);
        ui->DissolveCB->   setEnabled(true);      ui->DissolveCB->   setVisible(true);
        ui->DissolveCB->setCurrentIndex(CurrentCompoObject->Dissolve);
    } else {
        ui->DissolveLabel->setEnabled(false);     ui->DissolveLabel->setVisible(false);
        ui->DissolveCB->   setEnabled(false);     ui->DissolveCB->   setVisible(false);
    }

    RefreshStyleControls();

    QApplication::restoreOverrideCursor();
    InRefreshControls=false;

    // Refresh interactive zone display and shot thumbnail
    if (UpdateInteractiveZone) {

        ui->ShotTable->cellWidget(0,CurrentShotNbr)->repaint();
        ui->InteractiveZone->RefreshDisplay();

    } else ui->InteractiveZone->repaint(); // else refresh selection only
}

//====================================================================================================================
// Refresh background image of the scene

void DlgSlideProperties::s_RefreshSceneImage() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_RefreshSceneImage");

    ui->InteractiveZone->RefreshDisplay();
}

//====================================================================================================================
// Display or hide TV Margins
//====================================================================================================================

void DlgSlideProperties::s_TVMarginsBt() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_TVMarginsBt");

    DlgRulerDef Dlg(&ui->InteractiveZone->MagneticRuler,HELPFILE_DlgRulerDef,BaseApplicationConfig,((cApplicationConfig *)BaseApplicationConfig)->DlgRulerDef,this);
    Dlg.InitDialog();
    connect(&Dlg,SIGNAL(RefreshDisplay()),this,SLOT(s_RefreshSceneImage()));
    if (Dlg.exec()==0) {
        CurrentSlide->Parent->ApplicationConfig->SlideRuler=ui->InteractiveZone->MagneticRuler;
        ui->TVMarginsBT->setIcon(QIcon(QString(ui->InteractiveZone->MagneticRuler!=0?ICON_RULER_ON:ICON_RULER_OFF)));
    }
    ui->InteractiveZone->RefreshDisplay();
}

//====================================================================================================================

cCompositionObject *DlgSlideProperties::GetGlobalCompositionObject(int IndexKey) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::GetSelectedGlobalCompositionObject");

    int CurrentBlock=ui->BlockTable->currentRow();
    if ((CurrentBlock<0)||(CurrentBlock>=CompositionList->List.count())) return NULL;
    int i=0;
    while ((i<CurrentSlide->ObjectComposition.List.count())&&(CurrentSlide->ObjectComposition.List[i]->IndexKey!=IndexKey)) i++;
    if (i<CurrentSlide->ObjectComposition.List.count()) return CurrentSlide->ObjectComposition.List[i]; else return NULL;
}

cCompositionObject *DlgSlideProperties::GetSelectedGlobalCompositionObject() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::GetSelectedGlobalCompositionObject");

    int CurrentBlock=ui->BlockTable->currentRow();
    if ((CurrentBlock<0)||(CurrentBlock>=CompositionList->List.count())) return NULL;
    return GetGlobalCompositionObject(CompositionList->List[CurrentBlock]->IndexKey);
}

//====================================================================================================================

cCompositionObject *DlgSlideProperties::GetSelectedCompositionObject() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::GetSelectedCompositionObject");

    int CurrentBlock=ui->BlockTable->currentRow();
    if ((CurrentBlock<0)||(CurrentBlock>=CompositionList->List.count())) return NULL; else return CompositionList->List[CurrentBlock];
}

//========= Apply modification
void DlgSlideProperties::ApplyToContexte(bool ApplyGlobal) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::ApplyToContexte");

    if (ApplyGlobal) ApplyGlobalPropertiesToAllShots(CurrentCompoObject);
    RefreshControls();
}

//====================================================================================================================
// User select a block in the scene widget

void DlgSlideProperties::s_Scene_SelectionChanged() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_Scene_SelectionChanged");
}

//====================================================================================================================

void DlgSlideProperties::CopyBlockProperties(cCompositionObject *SourceBlock,cCompositionObject *DestBlock) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::CopyBlockProperties");

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
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::ApplyGlobalPropertiesToAllShots");

    // Apply to GlobalComposition objects
    for (int j=0;j<CurrentSlide->ObjectComposition.List.count();j++) if (GlobalBlock->IndexKey==CurrentSlide->ObjectComposition.List[j]->IndexKey)
        CopyBlockProperties(GlobalBlock,CurrentSlide->ObjectComposition.List[j]);

    // Apply to Shots Composition objects
    for (int i=0;i<CurrentSlide->List.count();i++) for (int j=0;j<CurrentSlide->List[i]->ShotComposition.List.count();j++) if (GlobalBlock->IndexKey==CurrentSlide->List[i]->ShotComposition.List[j]->IndexKey)
        CopyBlockProperties(GlobalBlock,CurrentSlide->List[i]->ShotComposition.List[j]);
}

//====================================================================================================================
// SLIDE SETTINGS
//====================================================================================================================

void DlgSlideProperties::s_SlideSet_NewChapter(int) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_SlideSet_NewChapter");
    if (InRefreshControls) return;  // No action if in control setup

    CurrentSlide->StartNewChapter=ui->NewChapterCB->isChecked();
}

//===================================================================================

void DlgSlideProperties::s_SlideSet_SlideNameChange(QString NewText) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_SlideSet_SlideNameChange");
    if (InRefreshControls) return;  // No action if in control setup

    CurrentSlide->SlideName=NewText;
}

//====================================================================================================================
// SHOT SETTINGS
//====================================================================================================================

void DlgSlideProperties::s_ShotTable_DurationChange(QTime NewValue) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_ShotTable_DurationChange");

    if ((InRefreshControls)||(CurrentShot==NULL)) return;  // No action if in control setup or if no shot was selected

    CurrentShot->StaticDuration=QTime(0,0,0,0).msecsTo(NewValue);
    s_ShotTable_DisplayDuration();
}

//===================================================================================

void DlgSlideProperties::s_ShotTable_AddShot() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_ShotTable_AddShot");

    if ((InRefreshControls)||(CurrentShot==NULL)) return;  // No action if in control setup or if no shot was selected

    // Insert a new empty shot after current shot in the current slide
    CurrentSlide->List.insert(CurrentShotNbr+1,new cDiaporamaShot(CurrentSlide));

    // Fill this new shot with copy of all blocks of current shot
    cDiaporamaShot *imagesequence=CurrentSlide->List[CurrentShotNbr+1];
    for (int i=0;i<CompositionList->List.count();i++) {
        imagesequence->ShotComposition.List.append(new cCompositionObject(COMPOSITIONTYPE_SHOT,CompositionList->List[i]->IndexKey,BaseApplicationConfig));
        imagesequence->ShotComposition.List[i]->CopyFromCompositionObject(CompositionList->List[i]);
    }

    ui->ShotTable->setUpdatesEnabled(false);
    ui->ShotTable->insertColumn(CurrentShotNbr+1);
    wgt_QCustomThumbnails *Object=new wgt_QCustomThumbnails(ui->ShotTable,THUMBNAILTYPE_SHOT);
    Object->DiaporamaObject=CurrentSlide;
    ui->ShotTable->setCellWidget(0,CurrentShotNbr+1,Object);
    ui->ShotTable->setColumnWidth(CurrentShotNbr+1,CurrentSlide->Parent->GetWidthForHeight(ui->ShotTable->rowHeight(0)));

    // Change current shot to this new shot
    ui->ShotTable->SetCurrentCell(CurrentShotNbr+1);
    ui->ShotTable->setUpdatesEnabled(true);
}

//===================================================================================

void DlgSlideProperties::s_ShotTable_RemoveShot() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_ShotTable_RemoveShot");

    // Don't remove last shot
    if (CurrentSlide->List.count()<2) return;

    if ((InRefreshControls)||(CurrentShot==NULL)) return;  // No action if in control setup or if no shot was selected

    if ((((cApplicationConfig *)BaseApplicationConfig)->AskUserToRemove)&&(CustomMessageBox(this,QMessageBox::Question,QApplication::translate("DlgSlideProperties","Remove shot"),QApplication::translate("DlgSlideProperties","Are you sure to want to delete this shot?"),
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)==QMessageBox::No)) return;

    // Remove shot from slide
    delete CurrentSlide->List.takeAt(CurrentShotNbr);

    // Remove shot from shot table
    int ShotNbr=CurrentShotNbr;
    ui->ShotTable->setUpdatesEnabled(false);
    ui->ShotTable->removeColumn(ShotNbr);
    if (ShotNbr>=CurrentSlide->List.count()) ShotNbr--;
    ui->ShotTable->SetCurrentCell(ShotNbr);
    ui->ShotTable->setUpdatesEnabled(true);
}

//===================================================================================

void DlgSlideProperties::s_ShotTable_DragMoveItem() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_ShotTable_DragMoveItem");

    if (GlobalMainWindow->DragItemSource<GlobalMainWindow->DragItemDest) GlobalMainWindow->DragItemDest--;
    CurrentSlide->List.move(GlobalMainWindow->DragItemSource,GlobalMainWindow->DragItemDest);

    ui->ShotTable->setUpdatesEnabled(false);
    ui->ShotTable->SetCurrentCell(GlobalMainWindow->DragItemDest);
    ui->ShotTable->setUpdatesEnabled(true);  // Reset timeline painting
}

//===================================================================================

void DlgSlideProperties::s_ShotTable_MoveLeft() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_ShotTable_MoveLeft");

    if ((InRefreshControls)||(CurrentShot==NULL)) return;  // No action if in control setup or if no shot was selected

    if (CurrentShotNbr>0) {
        CurrentSlide->List.swap(CurrentShotNbr,CurrentShotNbr-1);
        ui->ShotTable->setUpdatesEnabled(false);
        ui->ShotTable->SetCurrentCell(CurrentShotNbr-1);
        ui->ShotTable->setUpdatesEnabled(true);  // Reset timeline painting
    }
}

//===================================================================================

void DlgSlideProperties::s_ShotTable_MoveRight() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_ShotTable_MoveRight");

    if ((InRefreshControls)||(CurrentShot==NULL)) return;  // No action if in control setup or if no shot was selected

    if (CurrentShotNbr<CurrentSlide->List.count()-1) {
        CurrentSlide->List.swap(CurrentShotNbr+1,CurrentShotNbr);
        ui->ShotTable->setUpdatesEnabled(false);
        ui->ShotTable->SetCurrentCell(CurrentShotNbr+1);
        ui->ShotTable->setUpdatesEnabled(true);  // Reset timeline painting
    }
}

//===================================================================================

void DlgSlideProperties::s_ShotTable_SelectionChanged() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_ShotTable_SelectionChanged");

    // Keep link to current shot
    CurrentShotNbr=ui->ShotTable->currentColumn();
    if ((CurrentShotNbr<0)||(CurrentShotNbr>=CurrentSlide->List.count())) {
        ShotSelectMode  =SELECTMODE_NONE;
        CurrentShot     =NULL;
        CompositionList =NULL;
        CurrentShotNbr  =-1;
    } else {
        ShotSelectMode  =SELECTMODE_ONE;
        CurrentShot     =CurrentSlide->List[CurrentShotNbr];
        CompositionList =&CurrentShot->ShotComposition;
    }

    // Because block order can change from a shot to another, try to keep current block by looking at indexkey
    int i       =ui->BlockTable->currentRow();
    int IndexKey=-1;
    if ((i>=0)&&(i<CompositionList->List.count())) IndexKey=CompositionList->List[i]->IndexKey;

    int CurrentBlockTableIndex=0;
    if (IndexKey!=-1) {
        while ((CurrentBlockTableIndex<CompositionList->List.count())&&(CompositionList->List[CurrentBlockTableIndex]->IndexKey!=IndexKey)) CurrentBlockTableIndex++;
        if (CurrentBlockTableIndex>=CompositionList->List.count()) CurrentBlockTableIndex=0;
    }

    ui->BlockTable->CompositionList     =CompositionList;
    ui->InteractiveZone->CurrentShotNbr =CurrentShotNbr;

    ui->ShotTableMoveLeftBt->setEnabled(CurrentShotNbr>0);
    ui->ShotTableMoveRightBt->setEnabled(CurrentShotNbr<ui->ShotTable->columnCount()-1);
    ui->RemoveShot->setEnabled(CurrentSlide->List.count()>1);

    s_ShotTable_DisplayDuration();
    RefreshBlockTable(CurrentBlockTableIndex);
}

//===================================================================================

void DlgSlideProperties::s_ShotTable_DisplayDuration() {
    // duration and shot duration
    ui->SlideDurationLabel->setText(QTime(0,0,0,0).addMSecs(CurrentSlide->GetDuration()).toString("hh:mm:ss.zzz"));
    ui->ShotDurationED->setTime(QTime(0,0,0,0).addMSecs(CurrentShot->StaticDuration));

    // Minimum shot duration display
    int  AddingDuration=0;
    if (CurrentShotNbr==(CurrentSlide->List.count()-1)) {   // If it's the last shot
        int TotalDuration=CurrentSlide->GetDuration();
        int Duration=0;
        for (int i=0;i<CurrentSlide->List.count();i++) Duration=Duration+CurrentSlide->List[i]->StaticDuration;
        if (Duration<TotalDuration) AddingDuration=TotalDuration-Duration;
    }
    if (AddingDuration==0) ui->MinShotDurationLabel->setText("");
        else ui->MinShotDurationLabel->setText(QString(QApplication::translate("DlgSlideProperties","Lengthened to %1 sec")).arg(double(CurrentShot->StaticDuration+AddingDuration)/1000));
}

//====================================================================================================================
// BLOCK TABLE SETTINGS
//====================================================================================================================

//=========  Refresh the BlockTable
void DlgSlideProperties::RefreshBlockTable(int SetCurrentIndex) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::RefreshBlockTable");

    ui->BlockTable->setUpdatesEnabled(false);
    ui->BlockTable->setRowCount(CompositionList->List.count());
    for (int i=0;i<ui->BlockTable->rowCount();i++) ui->BlockTable->setRowHeight(i,50);
    ui->BlockTable->setUpdatesEnabled(true);
    if (ui->BlockTable->currentRow()!=SetCurrentIndex) ui->BlockTable->setCurrentCell(SetCurrentIndex,0,QItemSelectionModel::Select|QItemSelectionModel::Current); else s_BlockTable_SelectionChanged();
    ui->InteractiveZone->RefreshDisplay();

}

//========= block selection change
void DlgSlideProperties::s_BlockTable_SelectionChanged() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_BlockTable_SelectionChanged");
    if (InSelectionChange) return;

    QModelIndexList SelList=ui->BlockTable->selectionModel()->selectedIndexes();

    IsSelected.clear();
    for (int i=0;i<ui->BlockTable->rowCount();i++)  IsSelected.append(false);
    for (int i=0;i<SelList.count();i++)             IsSelected[SelList.at(i).row()]=true;

    NbrSelected          =0;
    CurrentCompoObjectNbr=-1;
    CurrentCompoObject   =NULL;

    for (int i=0;i<IsSelected.count();i++) if (IsSelected[i]) {
        if (NbrSelected==0) {
            CurrentCompoObjectNbr=i;
            CurrentCompoObject   =CompositionList->List[CurrentCompoObjectNbr];
        }
        NbrSelected++;
    }
    if (NbrSelected==0)             BlockSelectMode=SELECTMODE_NONE;
        else if (NbrSelected==1)    BlockSelectMode=SELECTMODE_ONE;
        else                        BlockSelectMode=SELECTMODE_MULTIPLE;

    RefreshControls(false);
}

void DlgSlideProperties::s_BlockTable_StartSelectionChange() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_BlockTable_StartSelectionChange");
    InSelectionChange=true;
}

void DlgSlideProperties::s_BlockTable_EndSelectionChange() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_BlockTable_EndSelectionChange");
    InSelectionChange=false;
    s_BlockTable_SelectionChanged();
}

//====================================================================================================================
// User double click on a block in the BlockTable widget or in the scene

void DlgSlideProperties::s_BlockTable_ItemDoubleClicked(QMouseEvent *) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_BlockTable_ItemDoubleClicked");

    if (BlockSelectMode!=SELECTMODE_ONE) return;
    if (CurrentCompoObject->BackgroundBrush->BrushType!=BRUSHTYPE_IMAGEDISK)    s_BlockSettings_TextEditor();
    else if (CurrentCompoObject->BackgroundBrush->Image!=NULL)                  s_BlockSettings_ImageEditCorrect();
    else if (CurrentCompoObject->BackgroundBrush->Video!=NULL)                  s_BlockSettings_VideoEdit();
}


//====================================================================================================================

void DlgSlideProperties::s_BlockSettings_Arrange() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_BlockSettings_Arrange");


    QMenu *ContextMenu=new QMenu(this);
    ContextMenu->addAction(ui->actionTop);
    ContextMenu->addAction(ui->actionMiddle);
    ContextMenu->addAction(ui->actionBottom);
    ContextMenu->addSeparator();
    ContextMenu->addAction(ui->actionLeft);
    ContextMenu->addAction(ui->actionCenter);
    ContextMenu->addAction(ui->actionRight);
    ContextMenu->exec(QCursor::pos());
    delete ContextMenu;
    ui->ArrangeBT->setDown(false);
}

//====================================================================================================================
// User right click on a block in the BlockTable widget or in the scene

void DlgSlideProperties::s_BlockTable_ItemRightClicked(QMouseEvent *) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_BlockTable_ItemRightClicked");

    if (BlockSelectMode==SELECTMODE_NONE) {
        QMenu *ContextMenu=new QMenu(this);
        ContextMenu->addAction(ui->actionCut);
        ContextMenu->addAction(ui->actionCopy);
        ContextMenu->addAction(ui->actionPaste);
        ContextMenu->addSeparator();
        ContextMenu->addAction(ui->actionAddTextBlock);
        ContextMenu->addAction(ui->actionAddFile);
        ContextMenu->exec(QCursor::pos());
        delete ContextMenu;

    } else if (BlockSelectMode==SELECTMODE_ONE) {
        QMenu *ContextMenu=new QMenu(this);
        ContextMenu->addAction(ui->actionEditText);
        ContextMenu->addAction(ui->actionEditImage);
        ContextMenu->addAction(ui->actionEditVideo);
        ContextMenu->addSeparator();
        ContextMenu->addAction(ui->actionUpBlock);
        ContextMenu->addAction(ui->actionDownBlock);
        ContextMenu->addSeparator();
        ContextMenu->addAction(ui->actionSetVisible);
        ContextMenu->addAction(ui->actionTakeSound);
        ContextMenu->addSeparator();
        ContextMenu->addAction(ui->actionCut);
        ContextMenu->addAction(ui->actionCopy);
        ContextMenu->addAction(ui->actionPaste);
        ContextMenu->addSeparator();
        ContextMenu->addAction(ui->actionRemoveBlock);
        ContextMenu->addSeparator();
        ContextMenu->addAction(ui->actionInfo);
        ContextMenu->exec(QCursor::pos());
        delete ContextMenu;
    } else if (BlockSelectMode==SELECTMODE_MULTIPLE) {
        QMenu *ContextMenu=new QMenu(this);
        ContextMenu->addAction(ui->actionTop);
        ContextMenu->addAction(ui->actionMiddle);
        ContextMenu->addAction(ui->actionBottom);
        ContextMenu->addSeparator();
        ContextMenu->addAction(ui->actionLeft);
        ContextMenu->addAction(ui->actionCenter);
        ContextMenu->addAction(ui->actionRight);
        ContextMenu->addSeparator();
        ContextMenu->addAction(ui->actionCut);
        ContextMenu->addAction(ui->actionCopy);
        ContextMenu->addAction(ui->actionPaste);
        ContextMenu->addSeparator();
        ContextMenu->addAction(ui->actionRemoveBlock);
        ContextMenu->exec(QCursor::pos());
        delete ContextMenu;
    }
}

//====================================================================================================================

void DlgSlideProperties::s_BlockTable_AddNewTextBlock() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_BlockTable_AddNewTextBlock");

    int CurrentShotNbr=ui->ShotTable->currentColumn();

    // Create and append a composition block to the object list
    CurrentSlide->ObjectComposition.List.append(new cCompositionObject(COMPOSITIONTYPE_OBJECT,CurrentSlide->NextIndexKey,((cApplicationConfig *)BaseApplicationConfig)));
    cCompositionObject *CompositionObject=CurrentSlide->ObjectComposition.List[CurrentSlide->ObjectComposition.List.count()-1];
    CompositionObject->BackgroundBrush->LockGeometry=false;
    CompositionObject->BackgroundBrush->AspectRatio=1;

    // Apply Styles
    CompositionObject->ApplyTextStyle(CurrentSlide->Parent->ApplicationConfig->StyleTextCollection.GetStyleDef(CurrentSlide->Parent->ApplicationConfig->StyleTextCollection.DecodeString(CurrentSlide->Parent->ApplicationConfig->DefaultBlock_Text_TextST)));
    CompositionObject->ApplyBackgroundStyle(CurrentSlide->Parent->ApplicationConfig->StyleTextBackgroundCollection.GetStyleDef(CurrentSlide->Parent->ApplicationConfig->StyleTextBackgroundCollection.DecodeString(CurrentSlide->Parent->ApplicationConfig->DefaultBlock_Text_BackGST)));
    CompositionObject->ApplyBlockShapeStyle(CurrentSlide->Parent->ApplicationConfig->StyleBlockShapeCollection.GetStyleDef(CurrentSlide->Parent->ApplicationConfig->StyleBlockShapeCollection.DecodeString(CurrentSlide->Parent->ApplicationConfig->DefaultBlock_Text_ShapeST)));
    // Force filtering for CoordinateStyle
    CurrentSlide->Parent->ApplicationConfig->StyleCoordinateCollection.SetProjectGeometryFilter(CurrentSlide->Parent->ImageGeometry);
    CompositionObject->ApplyCoordinateStyle(CurrentSlide->Parent->ApplicationConfig->StyleCoordinateCollection.GetStyleDef(CurrentSlide->Parent->ApplicationConfig->StyleCoordinateCollection.DecodeString(CurrentSlide->Parent->ApplicationConfig->DefaultBlock_Text_CoordST[CurrentSlide->Parent->ImageGeometry])));

    // Create default text
    QTextDocument       TextDoc(QApplication::translate("DlgSlideProperties","Text","Default text value"));
    QFont               Font=QFont(CompositionObject->FontName,CompositionObject->FontSize,CompositionObject->IsBold?QFont::Bold:QFont::Normal,CompositionObject->IsItalic?QFont::StyleItalic:QFont::StyleNormal);
    QTextOption         OptionText((CompositionObject->HAlign==0)?Qt::AlignLeft:(CompositionObject->HAlign==1)?Qt::AlignHCenter:(CompositionObject->HAlign==2)?Qt::AlignRight:Qt::AlignJustify);
    QTextCursor         Cursor(&TextDoc);
    QTextCharFormat     TCF;
    QTextBlockFormat    TBF;
    Cursor.select(QTextCursor::Document);
    OptionText.setWrapMode(QTextOption::WordWrap);
    Font.setUnderline(CompositionObject->IsUnderline);
    TextDoc.setDefaultFont(Font);
    TextDoc.setDefaultTextOption(OptionText);
    TCF.setFont(Font);
    TCF.setFontWeight(CompositionObject->IsBold?QFont::Bold:QFont::Normal);
    TCF.setFontItalic(CompositionObject->IsItalic);
    TCF.setFontUnderline(CompositionObject->IsUnderline);
    TCF.setForeground(QBrush(QColor(CompositionObject->FontColor)));
    TBF.setAlignment((CompositionObject->HAlign==0)?Qt::AlignLeft:(CompositionObject->HAlign==1)?Qt::AlignHCenter:(CompositionObject->HAlign==2)?Qt::AlignRight:Qt::AlignJustify);
    Cursor.setCharFormat(TCF);
    Cursor.setBlockFormat(TBF);
    CompositionObject->Text=TextDoc.toHtml();

    // Now create and append a shot composition block to all shot
    for (int i=0;i<CurrentSlide->List.count();i++) {
        CurrentSlide->List[i]->ShotComposition.List.append(new cCompositionObject(COMPOSITIONTYPE_SHOT,CompositionObject->IndexKey,((cApplicationConfig *)BaseApplicationConfig)));
        CurrentSlide->List[i]->ShotComposition.List[CurrentSlide->List[i]->ShotComposition.List.count()-1]->CopyFromCompositionObject(CompositionObject);
        // Ensure new object is not visible in previous shot
        if (i<CurrentShotNbr) CurrentSlide->List[i]->ShotComposition.List[CurrentSlide->List[i]->ShotComposition.List.count()-1]->IsVisible=false;
    }

    // Inc NextIndexKey
    CurrentSlide->NextIndexKey++;

    RefreshBlockTable(CompositionList->List.count()-1);

    QTimer::singleShot(250,this,SLOT(s_BlockSettings_TextEditor()));    // Append "Open text editor" to the message queue
}

//====================================================================================================================

void DlgSlideProperties::s_BlockTable_AddNewFileBlock() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_BlockTable_AddNewFileBlock");

    QStringList FileList=QFileDialog::getOpenFileNames(this,QApplication::translate("DlgSlideProperties","Add files"),
                                                       CurrentSlide->Parent->ApplicationConfig->RememberLastDirectories?CurrentSlide->Parent->ApplicationConfig->LastMediaPath:"",
                                                       CurrentSlide->Parent->ApplicationConfig->GetFilterForMediaFile(cBaseApplicationConfig::ALLFILE));

    // Sort files in the fileList
    if (CurrentSlide->Parent->ApplicationConfig->SortFile) {
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

        if (((cApplicationConfig *)BaseApplicationConfig)->RememberLastDirectories) ((cApplicationConfig *)BaseApplicationConfig)->LastMediaPath=QFileInfo(NewFile).absolutePath();     // Keep folder for next use

        // Create and append a composition block to the object list
        CurrentSlide->ObjectComposition.List.append(new cCompositionObject(COMPOSITIONTYPE_OBJECT,CurrentSlide->NextIndexKey,((cApplicationConfig *)BaseApplicationConfig)));
        cCompositionObject  *CompositionObject  =CurrentSlide->ObjectComposition.List[CurrentSlide->ObjectComposition.List.count()-1];
        cBrushDefinition    *CurrentBrush       =CompositionObject->BackgroundBrush;

        CompositionObject->Text     ="";
        CompositionObject->PenSize  =0;
        CurrentBrush->BrushType     =BRUSHTYPE_IMAGEDISK;

        QString BrushFileName =QFileInfo(NewFile).absoluteFilePath();

        bool    IsValide =false;
        QString Extension=QFileInfo(BrushFileName).suffix().toLower();

        // Search if file is an image
        for (int i=0;i<((cApplicationConfig *)BaseApplicationConfig)->AllowImageExtension.count();i++) if (((cApplicationConfig *)BaseApplicationConfig)->AllowImageExtension[i]==Extension) {
            // Create an image wrapper
            CurrentBrush->Image=new cImageFile(((cApplicationConfig *)BaseApplicationConfig));
            bool ModifyFlag=false;
            IsValide=CurrentBrush->Image->GetInformationFromFile(BrushFileName,&AliasList,&ModifyFlag);
            if (!IsValide) {
                delete CurrentBrush->Image;
                CurrentBrush->Image=NULL;
            } else if (ModifyFlag) GlobalMainWindow->SetModifyFlag(true);
            break;
        }
        // If it's not an image : search if file is a video
        if (CurrentBrush->Image==NULL) for (int i=0;i<((cApplicationConfig *)BaseApplicationConfig)->AllowVideoExtension.count();i++) if (((cApplicationConfig *)BaseApplicationConfig)->AllowVideoExtension[i]==Extension) {
            // Create a video wrapper
            CurrentBrush->Video=new cVideoFile(OBJECTTYPE_VIDEOFILE,((cApplicationConfig *)BaseApplicationConfig));
            bool ModifyFlag=false;
            IsValide=(CurrentBrush->Video->GetInformationFromFile(BrushFileName,&AliasList,&ModifyFlag))&&(CurrentBrush->Video->OpenCodecAndFile());
            if (IsValide) {
                // Check if file have at least one sound track compatible
                if ((CurrentBrush->Video->AudioStreamNumber!=-1)&&(!(
                    (CurrentBrush->Video->ffmpegAudioFile->streams[CurrentBrush->Video->AudioStreamNumber]->codec->sample_fmt!=AV_SAMPLE_FMT_S16)||
                    (CurrentBrush->Video->ffmpegAudioFile->streams[CurrentBrush->Video->AudioStreamNumber]->codec->sample_fmt!=AV_SAMPLE_FMT_U8)
                ))) {
                    ErrorMessage=ErrorMessage+"\n"+QApplication::translate("MainWindow","This application support only audio track with unsigned 8 bits or signed 16 bits sample format","Error message");
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
                //CurrentSlide->List[0].StaticDuration=CurrentBrush->Video->StartPos.msecsTo(CurrentBrush->Video->EndPos);
            }
            break;
        }
        if (IsValide) {

            QImage *Image=(CurrentBrush->Image?CurrentBrush->Image->ImageAt(true,&CurrentBrush->Image->BrushFileTransform):
                           CurrentBrush->Video?CurrentBrush->Video->ImageAt(true,0,QTime(0,0,0,0).msecsTo(CurrentBrush->Video->StartPos),NULL,1,false,&CurrentBrush->Video->BrushFileTransform,false):
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
                CompositionObject->ApplyTextStyle(CurrentSlide->Parent->ApplicationConfig->StyleTextCollection.GetStyleDef(CurrentSlide->Parent->ApplicationConfig->StyleTextCollection.DecodeString(CurrentSlide->Parent->ApplicationConfig->DefaultBlockBA_IMG_TextST)));
                CompositionObject->ApplyBlockShapeStyle(CurrentSlide->Parent->ApplicationConfig->StyleBlockShapeCollection.GetStyleDef(CurrentSlide->Parent->ApplicationConfig->StyleBlockShapeCollection.DecodeString(CurrentSlide->Parent->ApplicationConfig->DefaultBlockBA_IMG_ShapeST)));
                // Force filtering for CoordinateStyle
                CurrentSlide->Parent->ApplicationConfig->StyleCoordinateCollection.SetImageGeometryFilter(CurrentSlide->Parent->ImageGeometry,CurrentBrush->Image?CurrentBrush->Image->ObjectGeometry:CurrentBrush->Video->ObjectGeometry);
                CompositionObject->ApplyCoordinateStyle(CurrentSlide->Parent->ApplicationConfig->StyleCoordinateCollection.GetStyleDef(CurrentSlide->Parent->ApplicationConfig->StyleCoordinateCollection.DecodeString(
                    CurrentSlide->Parent->ApplicationConfig->DefaultBlockBA_IMG_CoordST[CurrentBrush->Image?CurrentBrush->Image->ObjectGeometry:CurrentBrush->Video->ObjectGeometry][CurrentSlide->Parent->ImageGeometry])));

                // Special case for nonstandard image => force to image geometry constraint and adapt frame coordinates
                if ((CurrentBrush->Image?CurrentBrush->Image->ObjectGeometry:CurrentBrush->Video->ObjectGeometry)==IMAGE_GEOMETRY_UNKNOWN) {
                    double ImageGeometry=1;
                    double ProjectGeometry=1;
                    double NewW,NewH;

                    switch (CurrentSlide->Parent->ImageGeometry) {
                        case GEOMETRY_4_3   : ProjectGeometry=double(1440)/double(1920);  break;
                        case GEOMETRY_16_9  : ProjectGeometry=double(1080)/double(1920);  break;
                        case GEOMETRY_40_17 : ProjectGeometry=double(816)/double(1920);   break;

                    }
                    ProjectGeometry=QString("%1").arg(ProjectGeometry,0,'e').toDouble();  // Rounded to same number as style managment
                    switch (CurrentSlide->Parent->ApplicationConfig->DefaultBlockBA_CLIPARTLOCK[CurrentSlide->Parent->ImageGeometry]) {
                        case 0 :    // Adjust to Full in lock to image geometry mode
                            if (CurrentBrush->Image)            ImageGeometry=double(CurrentBrush->Image->ImageHeight)/double(CurrentBrush->Image->ImageWidth);
                                else if (CurrentBrush->Video)   ImageGeometry=double(CurrentBrush->Video->ImageHeight)/double(CurrentBrush->Video->ImageWidth);
                            CurrentBrush->InitDefaultFramingStyle(true,ImageGeometry);
                            CurrentBrush->ApplyStyle(true,CurrentBrush->DefaultFramingF);
                            NewW=CompositionObject->w*CurrentSlide->Parent->InternalWidth;
                            NewH=NewW*CurrentBrush->AspectRatio;
                            NewW=NewW/CurrentSlide->Parent->InternalWidth;
                            NewH=NewH/CurrentSlide->Parent->InternalHeight;
                            if (NewH>1) {
                                NewH=CompositionObject->h*CurrentSlide->Parent->InternalHeight;
                                NewW=NewH/CurrentBrush->AspectRatio;
                                NewW=NewW/CurrentSlide->Parent->InternalWidth;
                                NewH=NewH/CurrentSlide->Parent->InternalHeight;
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
            int CurrentShotNbr=ui->ShotTable->currentColumn();

            // Now create and append a shot composition block to all shot
            for (int i=0;i<CurrentSlide->List.count();i++) {
                CurrentSlide->List[i]->ShotComposition.List.append(new cCompositionObject(COMPOSITIONTYPE_SHOT,CompositionObject->IndexKey,((cApplicationConfig *)BaseApplicationConfig)));
                CurrentSlide->List[i]->ShotComposition.List[CurrentSlide->List[i]->ShotComposition.List.count()-1]->CopyFromCompositionObject(CompositionObject);
                // Ensure new object is not visible in previous shot
                if (i<CurrentShotNbr) CurrentSlide->List[i]->ShotComposition.List[CurrentSlide->List[i]->ShotComposition.List.count()-1]->IsVisible=false;
            }

            // If this object is a video will gain sound from this shots !
            if (CurrentBrush->Video!=NULL) for (int k=0;k<CurrentSlide->List.count();k++) for (int l=0;l<CurrentSlide->List[k]->ShotComposition.List.count();l++)
                if ((CurrentSlide->List[k]->ShotComposition.List[l]->BackgroundBrush->BrushType==BRUSHTYPE_IMAGEDISK)&&
                    (CurrentSlide->List[k]->ShotComposition.List[l]->BackgroundBrush->Video)) {
                    if (k<CurrentShotNbr) {
                        // Set this new block to SoundVolume=0
                        if (CurrentBrush->Video==CurrentSlide->List[k]->ShotComposition.List[l]->BackgroundBrush->Video)
                            CurrentSlide->List[k]->ShotComposition.List[l]->BackgroundBrush->SoundVolume=0;
                    } else {
                        // Set all other block to SoundVolume=0 and this block to SoundVolume=1
                        if (CurrentBrush->Video!=CurrentSlide->List[k]->ShotComposition.List[l]->BackgroundBrush->Video)
                            CurrentSlide->List[k]->ShotComposition.List[l]->BackgroundBrush->SoundVolume=0;
                        else CurrentSlide->List[k]->ShotComposition.List[l]->BackgroundBrush->SoundVolume=1;
                    }
            }

            // Inc NextIndexKey
            CurrentSlide->NextIndexKey++;

        } else {
            CustomMessageBox(this,QMessageBox::Critical,QApplication::translate("MainWindow","Error","Error message"),NewFile+"\n\n"+ErrorMessage,QMessageBox::Close);
        }
    }
    RefreshBlockTable(CompositionList->List.count()-1);
}

//====================================================================================================================

void DlgSlideProperties::s_BlockTable_RemoveBlock() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_BlockTable_RemoveBlock");

    if (BlockSelectMode==SELECTMODE_ONE) {
        if ((((cApplicationConfig *)BaseApplicationConfig)->AskUserToRemove)&&(CustomMessageBox(this,QMessageBox::Question,QApplication::translate("DlgSlideProperties","Remove block"),QApplication::translate("DlgSlideProperties","Are you sure to want to delete this block?"),
                                  QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)==QMessageBox::No)) return;
    } else if (BlockSelectMode==SELECTMODE_MULTIPLE) {
        if ((((cApplicationConfig *)BaseApplicationConfig)->AskUserToRemove)&&(CustomMessageBox(this,QMessageBox::Question,QApplication::translate("DlgSlideProperties","Remove blocks"),QApplication::translate("DlgSlideProperties","Are you sure to want to delete these blocks?"),
                                  QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)==QMessageBox::No)) return;
    }
    for (int i=CompositionList->List.count()-1;i>=0;i--) if (IsSelected[i]) {

        // Get indexkey of block
        int KeyToDelete=CompositionList->List[i]->IndexKey;

        // Delete block from all shots of the slide
        for (int j=0;j<CurrentSlide->List.count();j++) {
            int k=0;
            while (k<CurrentSlide->List[j]->ShotComposition.List.count()) {
                if (CurrentSlide->List[j]->ShotComposition.List[k]->IndexKey==KeyToDelete) delete CurrentSlide->List[j]->ShotComposition.List.takeAt(k);
                    else k++;
            }
        }

        // Delete block from global composition list of the slide
        int k=0;
        while (k<CurrentSlide->ObjectComposition.List.count()) {
            if (CurrentSlide->ObjectComposition.List[k]->IndexKey==KeyToDelete) delete CurrentSlide->ObjectComposition.List.takeAt(k);
                else k++;
        }
    }

    // Update display
    RefreshBlockTable(CurrentCompoObjectNbr>=CompositionList->List.count()?CurrentCompoObjectNbr-1:CurrentCompoObjectNbr);

    // Ensure nothing is selected
    ui->BlockTable->clearSelection();
}

//====================================================================================================================

void DlgSlideProperties::s_BlockTable_Cut() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_BlockTable_Cut");

    if ((BlockSelectMode!=SELECTMODE_ONE)&&(BlockSelectMode!=SELECTMODE_MULTIPLE)) return;
    s_BlockTable_Copy();
    s_BlockTable_RemoveBlock();
}

//====================================================================================================================

void DlgSlideProperties::s_BlockTable_Copy() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_BlockTable_Copy");

    if ((BlockSelectMode!=SELECTMODE_ONE)&&(BlockSelectMode!=SELECTMODE_MULTIPLE)) return;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    // Create xml document and root
    QDomDocument Object  =QDomDocument(APPLICATION_NAME);
    QDomElement  root    =Object.createElement("CLIPBOARD");
    int          BlockNum=0;
    for (int i=0;i<CompositionList->List.count();i++) if (IsSelected[i]) {
        QDomElement         Element=Object.createElement(QString("Block-%1").arg(BlockNum));
        cCompositionObject  *GlobalBlock=GetGlobalCompositionObject(CompositionList->List[i]->IndexKey);

        GlobalBlock->SaveToXML(Element,"CLIPBOARD-BLOCK-GLOBAL",CurrentSlide->Parent->ProjectFileName,true);                // Save global object
        CompositionList->List[i]->SaveToXML(Element,"CLIPBOARD-BLOCK-SHOT",CurrentSlide->Parent->ProjectFileName,true);     // Save shot object
        root.appendChild(Element);
        BlockNum++;
    }
    root.setAttribute("BlockNbr",BlockNum);
    Object.appendChild(root);

    // Transfert xml document to clipboard
    QMimeData *SlideData=new QMimeData();
    SlideData->setData("ffDiaporama/block",Object.toByteArray());
    QApplication::clipboard()->setMimeData(SlideData);

    RefreshControls();
    QApplication::restoreOverrideCursor();
}

//====================================================================================================================

void DlgSlideProperties::s_BlockTable_Paste() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_BlockTable_Paste");

    const QMimeData *SlideData=QApplication::clipboard()->mimeData();
    if (SlideData->hasFormat("ffDiaporama/block")) {
        QDomDocument Object=QDomDocument(APPLICATION_NAME);
        Object.setContent(SlideData->data("ffDiaporama/block"));
        if ((Object.elementsByTagName("CLIPBOARD").length()>0)&&(Object.elementsByTagName("CLIPBOARD").item(0).isElement()==true)) {
            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
            QDomElement root=Object.elementsByTagName("CLIPBOARD").item(0).toElement();
            int BlockNbr=root.attribute("BlockNbr").toInt();

            for (int BlockNum=0;BlockNum<BlockNbr;BlockNum++) if ((root.elementsByTagName(QString("Block-%1").arg(BlockNum)).length()>0)&&(root.elementsByTagName(QString("Block-%1").arg(BlockNum)).item(0).isElement()==true)) {
                QDomElement Element=root.elementsByTagName(QString("Block-%1").arg(BlockNum)).item(0).toElement();
                // Create and append a composition block to the object list
                CurrentSlide->ObjectComposition.List.append(new cCompositionObject(COMPOSITIONTYPE_OBJECT,CurrentSlide->NextIndexKey,((cApplicationConfig *)BaseApplicationConfig)));
                cCompositionObject *GlobalBlock=CurrentSlide->ObjectComposition.List[CurrentSlide->ObjectComposition.List.count()-1];
                GlobalBlock->LoadFromXML(Element,"CLIPBOARD-BLOCK-GLOBAL","",NULL,NULL);
                GlobalBlock->IndexKey=CurrentSlide->NextIndexKey;

                cCompositionObject ShotBlock(COMPOSITIONTYPE_SHOT,CurrentSlide->NextIndexKey,((cApplicationConfig *)BaseApplicationConfig));
                ShotBlock.LoadFromXML(Element,"CLIPBOARD-BLOCK-SHOT","",NULL,NULL);
                ShotBlock.IndexKey=CurrentSlide->NextIndexKey;
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
                for (int i=0;i<CurrentSlide->List.count();i++) {
                    CurrentSlide->List[i]->ShotComposition.List.append(new cCompositionObject(COMPOSITIONTYPE_SHOT,CurrentSlide->NextIndexKey,((cApplicationConfig *)BaseApplicationConfig)));
                    CurrentSlide->List[i]->ShotComposition.List[CurrentSlide->List[i]->ShotComposition.List.count()-1]->CopyFromCompositionObject(&ShotBlock);
                    // Ensure new object is not visible in previous shot
                    if (i<CurrentShotNbr) {
                        CurrentSlide->List[i]->ShotComposition.List[CurrentSlide->List[i]->ShotComposition.List.count()-1]->IsVisible=false;
                        // Ensure unvisible video have no sound !
                        if (CurrentSlide->List[i]->ShotComposition.List[CurrentSlide->List[i]->ShotComposition.List.count()-1]->BackgroundBrush->Video!=NULL)
                            CurrentSlide->List[i]->ShotComposition.List[CurrentSlide->List[i]->ShotComposition.List.count()-1]->BackgroundBrush->SoundVolume=0;
                    }
                }
                // Inc NextIndexKey
                CurrentSlide->NextIndexKey++;
            }

            // Refresh shot table
            ui->ShotTable->setUpdatesEnabled(false);
            ui->ShotTable->setUpdatesEnabled(true);

            // Refresh block table
            RefreshBlockTable(CompositionList->List.count()-1);

            // Select blocks we just added
            s_BlockTable_StartSelectionChange();
            ui->BlockTable->clearSelection();
            for (int i=0;i<BlockNbr;i++) ui->BlockTable->setCurrentCell(CompositionList->List.count()-1-i,0,i==0?QItemSelectionModel::Select|QItemSelectionModel::Current:QItemSelectionModel::Select);
            s_BlockTable_EndSelectionChange();

            QApplication::restoreOverrideCursor();
        }
    }
}

//====================================================================================================================

void DlgSlideProperties::s_BlockTable_MoveBlockUp() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_BlockTable_MoveBlockUp");

    if (BlockSelectMode!=SELECTMODE_ONE) return;
    if (CurrentCompoObjectNbr<1) return;
    CompositionList->List.swap(CurrentCompoObjectNbr,CurrentCompoObjectNbr-1);
    RefreshBlockTable(CurrentCompoObjectNbr-1);
}

//====================================================================================================================

void DlgSlideProperties::s_BlockTable_MoveBlockDown() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_BlockTable_MoveBlockDown");

    if (BlockSelectMode!=SELECTMODE_ONE) return;
    if (CurrentCompoObjectNbr>=CompositionList->List.count()-1) return;
    CompositionList->List.swap(CurrentCompoObjectNbr+1,CurrentCompoObjectNbr);
    RefreshBlockTable(CurrentCompoObjectNbr+1);
}

//********************************************************************************************************************
//                                                  BLOCKS ALIGNMENT
//********************************************************************************************************************

void DlgSlideProperties::s_BlockTable_AlignTop() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_BlockTable_AlignTop");
    for (int i=0;i<IsSelected.count();i++) if (IsSelected[i]) CompositionList->List[i]->y=ui->InteractiveZone->Sel_Y;
    RefreshControls(true);
}

void DlgSlideProperties::s_BlockTable_AlignMiddle() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_BlockTable_AlignMiddle");
    for (int i=0;i<IsSelected.count();i++) if (IsSelected[i]) CompositionList->List[i]->y=(ui->InteractiveZone->Sel_Y+ui->InteractiveZone->Sel_H/2)-CompositionList->List[i]->h/2;
    RefreshControls(true);
}

void DlgSlideProperties::s_BlockTable_AlignBottom() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_BlockTable_AlignBottom");
    for (int i=0;i<IsSelected.count();i++) if (IsSelected[i]) CompositionList->List[i]->y=(ui->InteractiveZone->Sel_Y+ui->InteractiveZone->Sel_H)-CompositionList->List[i]->h;
    RefreshControls(true);
}

void DlgSlideProperties::s_BlockTable_AlignLeft() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_BlockTable_AlignLeft");
    for (int i=0;i<IsSelected.count();i++) if (IsSelected[i]) CompositionList->List[i]->x=ui->InteractiveZone->Sel_X;
    RefreshControls(true);
}

void DlgSlideProperties::s_BlockTable_AlignCenter() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_BlockTable_AlignCenter");
    for (int i=0;i<IsSelected.count();i++) if (IsSelected[i]) CompositionList->List[i]->x=(ui->InteractiveZone->Sel_X+ui->InteractiveZone->Sel_W/2)-CompositionList->List[i]->w/2;
    RefreshControls(true);
}

void DlgSlideProperties::s_BlockTable_AlignRight() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_BlockTable_AlignRight");
    for (int i=0;i<IsSelected.count();i++) if (IsSelected[i]) CompositionList->List[i]->x=(ui->InteractiveZone->Sel_X+ui->InteractiveZone->Sel_W)-CompositionList->List[i]->w;
    RefreshControls(true);
}

//********************************************************************************************************************
//
//                                                  CURRENT BLOCK SETTINGS
//
//********************************************************************************************************************

//====================================================================================================================
// Buttons associated to a Dialog box
//====================================================================================================================

//========= Open text editor
void DlgSlideProperties::s_BlockSettings_TextEditor() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_BlockSettings_TextEditor");

    if ((InRefreshControls)||(BlockSelectMode!=SELECTMODE_ONE)||(!CurrentCompoObject)||(!CurrentCompoObject->IsVisible)) return;
    DlgTextEdit Dlg(CurrentCompoObject,HELPFILE_DlgTextEdit,((cApplicationConfig *)BaseApplicationConfig),((cApplicationConfig *)BaseApplicationConfig)->DlgTextEditWSP,
                    &((cApplicationConfig *)BaseApplicationConfig)->StyleTextCollection,&((cApplicationConfig *)BaseApplicationConfig)->StyleTextBackgroundCollection,this);
    Dlg.InitDialog();
    connect(&Dlg,SIGNAL(RefreshDisplay()),this,SLOT(s_RefreshSceneImage()));
    if (Dlg.exec()==0) {
        ApplyToContexte(true);
        int CurrentRow=ui->BlockTable->currentRow();
        RefreshBlockTable(CurrentRow>0?CurrentRow:0);
    } else RefreshControls();
}

//========= Open s_BlockSettings_Information dialog
void DlgSlideProperties::s_BlockSettings_Information() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_BlockSettings_Information");

    if ((InRefreshControls)||(BlockSelectMode!=SELECTMODE_ONE)||(!CurrentCompoObject)||(!CurrentCompoObject->IsVisible)||
        ((!CurrentCompoObject->BackgroundBrush->Video)&&(!CurrentCompoObject->BackgroundBrush->Image))) return;

    cBaseMediaFile *Media=NULL;
    if (CurrentCompoObject->BackgroundBrush->Image!=NULL)            Media=CurrentCompoObject->BackgroundBrush->Image;
        else if (CurrentCompoObject->BackgroundBrush->Video!=NULL)   Media=CurrentCompoObject->BackgroundBrush->Video;

    if (Media) {
        DlgInfoFile Dlg(Media,HELPFILE_DlgInfoFile,((cApplicationConfig *)BaseApplicationConfig),((cApplicationConfig *)BaseApplicationConfig)->DlgInfoFileWSP,this);
        Dlg.InitDialog();
        Dlg.exec();
    }
}

//========= Open video editor
void DlgSlideProperties::s_BlockSettings_VideoEdit() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_BlockSettings_VideoEdit");

    if ((InRefreshControls)||(BlockSelectMode!=SELECTMODE_ONE)||(!CurrentCompoObject)||(!CurrentCompoObject->IsVisible)||(!CurrentCompoObject->BackgroundBrush->Video)) return;
    DlgVideoEdit(CurrentCompoObject->BackgroundBrush,this).exec();
    ApplyToContexte(true);
}

//========= Open image correction editor
void DlgSlideProperties::s_BlockSettings_ImageEditCorrect() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_BlockSettings_ImageEditCorrect");

    if ((InRefreshControls)||(BlockSelectMode!=SELECTMODE_ONE)||(!CurrentCompoObject)||(!CurrentCompoObject->IsVisible)||
        ((!CurrentCompoObject->BackgroundBrush->Video)&&(!CurrentCompoObject->BackgroundBrush->Image))) return;

    cBrushDefinition *CurrentBrush=CurrentCompoObject->BackgroundBrush;

    int Position=0;
    // Compute position of video
    if (CurrentBrush->Video) for (int i=0;i<ui->ShotTable->currentColumn();i++) for (int j=0;j<CurrentSlide->List[i]->ShotComposition.List.count();j++)
        if (CurrentSlide->List[i]->ShotComposition.List[j]->IndexKey==CurrentCompoObject->IndexKey) {
            if (CurrentSlide->List[i]->ShotComposition.List[j]->IsVisible) Position+=CurrentSlide->List[i]->StaticDuration;
    }

    QString FileName    =QFileInfo(CurrentBrush->Image?CurrentBrush->Image->FileName:CurrentBrush->Video->FileName).fileName();
    bool UpdateSlideName=(CurrentSlide->SlideName==FileName);
    if (DlgImageCorrection(CurrentCompoObject,CurrentCompoObject->BackgroundForm,CurrentCompoObject->BackgroundBrush,Position,this).exec()==0) {

        CurrentBrush->AspectRatio=CurrentBrush->AspectRatio;
        CurrentCompoObject->h =(CurrentCompoObject->w*DisplayW*CurrentBrush->AspectRatio)/DisplayH;

        // Adjust height and width to image stay in screen
        if (((CurrentCompoObject->y+CurrentCompoObject->h)*DisplayH)>DisplayH) {
            CurrentCompoObject->h=1-CurrentCompoObject->y;
            CurrentCompoObject->w=((CurrentCompoObject->h*DisplayH)/CurrentBrush->AspectRatio)/DisplayW;
        }

        // if Slide name is name of this file
        if (UpdateSlideName) {
            CurrentSlide->SlideName=QFileInfo(CurrentBrush->Image?CurrentBrush->Image->FileName:CurrentBrush->Video->FileName).fileName();
            ui->SlideNameED->setText(CurrentSlide->SlideName);
        }

        // Lulo object for image and video must be remove
        if (CurrentCompoObject->BackgroundBrush->Video) CurrentSlide->Parent->ApplicationConfig->ImagesCache.RemoveVideoObject(CurrentCompoObject->BackgroundBrush->Video->FileName);
            else if (CurrentCompoObject->BackgroundBrush->Image) CurrentSlide->Parent->ApplicationConfig->ImagesCache.RemoveImageObject(CurrentCompoObject->BackgroundBrush->Image->FileName);

        ApplyToContexte(true);
        RefreshBlockTable(CurrentCompoObjectNbr);
    }
}

//========= Toggle visible state

void DlgSlideProperties::s_BlockSettings_ToggleVisibleState() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_BlockSettings_ToggleVisibleState");

    if ((InRefreshControls)||(BlockSelectMode!=SELECTMODE_ONE)||(!CurrentCompoObject)) return;

    CurrentCompoObject->IsVisible=!CurrentCompoObject->IsVisible;

    // Special case for video : ensure only this video have sound
    if (CurrentCompoObject->BackgroundBrush->Video!=NULL) {
        if (!CurrentCompoObject->IsVisible) {
            CurrentCompoObject->BackgroundBrush->SoundVolume=0;
        } else {
            bool SomeOneHaveSound=false;
            // Parse table to know if a block have sound for this shot
            for (int i=0;i<CompositionList->List.count();i++)
                if ((CompositionList->List[i]->BackgroundBrush->Video!=NULL)&&(CompositionList->List[i]->BackgroundBrush->SoundVolume!=0)) SomeOneHaveSound=true;
            // If no block have sound => get sound to this video
            if (!SomeOneHaveSound) CurrentCompoObject->BackgroundBrush->SoundVolume=1;
        }
    }
    RefreshBlockTable(CurrentCompoObjectNbr);
}

//========= Get sound

void DlgSlideProperties::s_BlockSettings_GetSound() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_BlockSettings_GetSound");

    if ((InRefreshControls)||(BlockSelectMode!=SELECTMODE_ONE)||(!CurrentCompoObject)||(!CurrentCompoObject->IsVisible)||(!CurrentCompoObject->BackgroundBrush->Video)) return;

    // Only if this block is a video and don't have sound yet
    if (CurrentCompoObject->BackgroundBrush->SoundVolume==0) {
        for (int i=0;i<CompositionList->List.count();i++)
            if ((CurrentCompoObject!=CompositionList->List[i])&&(CompositionList->List[i]->BackgroundBrush->Video)) CompositionList->List[i]->BackgroundBrush->SoundVolume=0;
        CurrentCompoObject->BackgroundBrush->SoundVolume=1;
        RefreshBlockTable(CurrentCompoObjectNbr);
    }
}

//====================================================================================================================
// Handler for position, size & rotation controls
//====================================================================================================================

//========= X position
void DlgSlideProperties::s_BlockSettings_PosXValue(double Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_BlockSettings_PosXValue");

    if ((InRefreshControls)||(BlockSelectMode!=SELECTMODE_ONE)||(!CurrentCompoObject)||(!CurrentCompoObject->IsVisible)) return;
    if (CurrentSlide->Parent->ApplicationConfig->DisplayUnit==DISPLAYUNIT_PERCENT) CurrentCompoObject->x=Value/100;           // DisplayUnit==DISPLAYUNIT_PERCENT
        else                                                                       CurrentCompoObject->x=(Value/DisplayW);    // DisplayUnit==DISPLAYUNIT_PIXELS
    ApplyToContexte(false);
}

//========= Y position
void DlgSlideProperties::s_BlockSettings_PosYValue(double Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_BlockSettings_PosYValue");

    if ((InRefreshControls)||(BlockSelectMode!=SELECTMODE_ONE)||(!CurrentCompoObject)||(!CurrentCompoObject->IsVisible)) return;
    if (CurrentSlide->Parent->ApplicationConfig->DisplayUnit==DISPLAYUNIT_PERCENT) CurrentCompoObject->y=Value/100;           // DisplayUnit==DISPLAYUNIT_PERCENT
        else                                                                       CurrentCompoObject->y=(Value/DisplayH);    // DisplayUnit==DISPLAYUNIT_PIXELS
    ApplyToContexte(false);
}

//========= Width
void DlgSlideProperties::s_BlockSettings_PosWidthValue(double Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_BlockSettings_PosWidthValue");

    if ((InRefreshControls)||(BlockSelectMode!=SELECTMODE_ONE)||(!CurrentCompoObject)||(!CurrentCompoObject->IsVisible)) return;
    if (CurrentSlide->Parent->ApplicationConfig->DisplayUnit==DISPLAYUNIT_PERCENT) CurrentCompoObject->w=Value/100;           // DisplayUnit==DISPLAYUNIT_PERCENT
        else CurrentCompoObject->w=(Value/DisplayW);                                                                          // DisplayUnit==DISPLAYUNIT_PIXELS
    if (CurrentCompoObject->BackgroundBrush->LockGeometry)  CurrentCompoObject->h=((CurrentCompoObject->w*DisplayW)*CurrentCompoObject->BackgroundBrush->AspectRatio)/DisplayH;
        else                                                CurrentCompoObject->BackgroundBrush->AspectRatio=(CurrentCompoObject->h*DisplayH)/(CurrentCompoObject->w*DisplayW);
    ApplyToContexte(false);
}

//========= Height
void DlgSlideProperties::s_BlockSettings_PosHeightValue(double Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_BlockSettings_PosHeightValue");

    if ((InRefreshControls)||(BlockSelectMode!=SELECTMODE_ONE)||(!CurrentCompoObject)||(!CurrentCompoObject->IsVisible)) return;
    if (CurrentSlide->Parent->ApplicationConfig->DisplayUnit==DISPLAYUNIT_PERCENT) CurrentCompoObject->h=Value/100;           // DisplayUnit==DISPLAYUNIT_PERCENT
        else CurrentCompoObject->h=(Value/DisplayH);                                                                          // DisplayUnit==DISPLAYUNIT_PIXELS
    if (CurrentCompoObject->BackgroundBrush->LockGeometry)  CurrentCompoObject->w=((CurrentCompoObject->h*DisplayH)/CurrentCompoObject->BackgroundBrush->AspectRatio)/DisplayW;
        else                                                CurrentCompoObject->BackgroundBrush->AspectRatio=(CurrentCompoObject->h*DisplayH)/(CurrentCompoObject->w*DisplayW);
    ApplyToContexte(false);
}

//========= X Rotation value
void DlgSlideProperties::s_BlockSettings_RotateXValue(int Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_BlockSettings_RotateXValue");

    if ((InRefreshControls)||(BlockSelectMode!=SELECTMODE_ONE)||(!CurrentCompoObject)||(!CurrentCompoObject->IsVisible)) return;
    CurrentCompoObject->RotateXAxis=Value;
    ApplyToContexte(false);
}
void DlgSlideProperties::s_BlockSettings_ResetRotateXValue() {
    s_BlockSettings_RotateXValue(0);
}

//========= Y Rotation value
void DlgSlideProperties::s_BlockSettings_RotateYValue(int Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_BlockSettings_RotateYValue");

    if ((InRefreshControls)||(BlockSelectMode!=SELECTMODE_ONE)||(!CurrentCompoObject)||(!CurrentCompoObject->IsVisible)) return;
    CurrentCompoObject->RotateYAxis=Value;
    ApplyToContexte(false);
}
void DlgSlideProperties::s_BlockSettings_ResetRotateYValue() {
    s_BlockSettings_RotateYValue(0);
}

//========= Z Rotation value
void DlgSlideProperties::s_BlockSettings_RotateZValue(int Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_BlockSettings_RotateZValue");

    if ((InRefreshControls)||(BlockSelectMode!=SELECTMODE_ONE)||(!CurrentCompoObject)||(!CurrentCompoObject->IsVisible)) return;
    CurrentCompoObject->RotateZAxis=Value;
    ApplyToContexte(false);
}
void DlgSlideProperties::s_BlockSettings_ResetRotateZValue() {
    s_BlockSettings_RotateZValue(0);
}

//====================================================================================================================
// Handler for shape
//====================================================================================================================

//========= Background forme
void DlgSlideProperties::s_BlockSettings_ShapeBackgroundForm(int Style) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_BlockSettings_ShapeBackgroundForm");

    if ((InRefreshControls)||(BlockSelectMode!=SELECTMODE_ONE)||(!CurrentCompoObject)||(!CurrentCompoObject->IsVisible)) return;
    CurrentCompoObject->BackgroundForm=Style+1;
    ApplyToContexte(true);
}

//========= Opacity
void DlgSlideProperties::s_BlockSettings_ShapeOpacity(int Style) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_BlockSettings_ShapeOpacity");

    if ((InRefreshControls)||(BlockSelectMode!=SELECTMODE_ONE)||(!CurrentCompoObject)||(!CurrentCompoObject->IsVisible)) return;
    CurrentCompoObject->Opacity=Style;
    ApplyToContexte(true);
}

//========= Border pen size
void DlgSlideProperties::s_BlockSettings_ShapePenSize(int) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_BlockSettings_ShapePenSize");

    if ((InRefreshControls)||(BlockSelectMode!=SELECTMODE_ONE)||(!CurrentCompoObject)||(!CurrentCompoObject->IsVisible)) return;
    CurrentCompoObject->PenSize=ui->PenSizeEd->value();
    ui->PenColorCB->setEnabled(CurrentCompoObject->PenSize!=0);
    ui->PenStyleCB->setEnabled(CurrentCompoObject->PenSize!=0);
    ApplyToContexte(true);
}

//========= Border pen style
void DlgSlideProperties::s_BlockSettings_ShapePenStyle(int index) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_BlockSettings_ShapePenStyle");

    if ((InRefreshControls)||(BlockSelectMode!=SELECTMODE_ONE)||(!CurrentCompoObject)||(!CurrentCompoObject->IsVisible)) return;
    CurrentCompoObject->PenStyle=ui->PenStyleCB->itemData(index).toInt();
    ApplyToContexte(true);
}

//========= Border pen color
void DlgSlideProperties::s_BlockSettings_ShapePenColor(int) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_BlockSettings_ShapePenColor");

    if ((InRefreshControls)||(BlockSelectMode!=SELECTMODE_ONE)||(!CurrentCompoObject)||(!CurrentCompoObject->IsVisible)) return;
    CurrentCompoObject->PenColor=ui->PenColorCB->GetCurrentColor();
    ApplyToContexte(true);
}

//========= Shape shadow style
void DlgSlideProperties::s_BlockSettings_ShapeShadowFormValue(int value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_BlockSettings_ShapeShadowFormValue");

    if ((InRefreshControls)||(BlockSelectMode!=SELECTMODE_ONE)||(!CurrentCompoObject)||(!CurrentCompoObject->IsVisible)) return;
    CurrentCompoObject->FormShadow=value;
    ui->ShadowEffectED->setEnabled(CurrentCompoObject->FormShadow!=0);
    ui->ShadowColorCB->setEnabled(CurrentCompoObject->FormShadow!=0);
    ApplyToContexte(true);
}

//========= Shape shadow distance
void DlgSlideProperties::s_BlockSettings_ShapeShadowDistanceValue(int value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_BlockSettings_ShapeShadowDistanceValue");

    if ((InRefreshControls)||(BlockSelectMode!=SELECTMODE_ONE)||(!CurrentCompoObject)||(!CurrentCompoObject->IsVisible)) return;
    CurrentCompoObject->FormShadowDistance =value;
    ApplyToContexte(true);
}

//========= shadow color
void DlgSlideProperties::s_BlockSettings_ShapeShadowColor(int) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_BlockSettings_ShapeShadowColor");

    if ((InRefreshControls)||(BlockSelectMode!=SELECTMODE_ONE)||(!CurrentCompoObject)||(!CurrentCompoObject->IsVisible)) return;
    CurrentCompoObject->FormShadowColor=ui->ShadowColorCB->GetCurrentColor();
    ApplyToContexte(true);
}

//====================================================================================================================
// Text annimation
//====================================================================================================================

//========= Text Zoom Level
void DlgSlideProperties::s_BlockSettings_TextAnimZoom(int Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_BlockSettings_TextAnimZoom");

    if ((InRefreshControls)||(BlockSelectMode!=SELECTMODE_ONE)||(!CurrentCompoObject)||(!CurrentCompoObject->IsVisible)) return;
    CurrentCompoObject->TxtZoomLevel=Value;
    ui->ZoomSlider->setValue(CurrentCompoObject->TxtZoomLevel);
    ui->ZoomED->setValue(CurrentCompoObject->TxtZoomLevel);
    ApplyToContexte(false);
}

void DlgSlideProperties::s_BlockSettings_TextAnimZoomReset() {
    s_BlockSettings_TextAnimZoom(100);
}

//========= Text scrolling X
void DlgSlideProperties::s_BlockSettings_TextAnimScrollX(int Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_BlockSettings_TextAnimScrollX");

    if ((InRefreshControls)||(BlockSelectMode!=SELECTMODE_ONE)||(!CurrentCompoObject)||(!CurrentCompoObject->IsVisible)) return;
    CurrentCompoObject->TxtScrollX=Value;
    ui->ScrollXSlider->setValue(CurrentCompoObject->TxtScrollX);
    ui->ScrollXED->setValue(CurrentCompoObject->TxtScrollX);
    ApplyToContexte(false);
}

void DlgSlideProperties::s_BlockSettings_TextAnimScrollXReset() {
    s_BlockSettings_TextAnimScrollX(0);
}

//========= Text scrolling Y
void DlgSlideProperties::s_BlockSettings_TextAnimScrollY(int Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_BlockSettings_TextAnimScrollY");

    if ((InRefreshControls)||(BlockSelectMode!=SELECTMODE_ONE)||(!CurrentCompoObject)||(!CurrentCompoObject->IsVisible)) return;
    CurrentCompoObject->TxtScrollY=Value;
    ui->ScrollYSlider->setValue(CurrentCompoObject->TxtScrollY);
    ui->ScrollYED->setValue(CurrentCompoObject->TxtScrollY);
    ApplyToContexte(false);
}

void DlgSlideProperties::s_BlockSettings_TextAnimScrollYReset() {
    s_BlockSettings_TextAnimScrollY(0);
}

//====================================================================================================================
// Handler for block animation
//====================================================================================================================

void DlgSlideProperties::s_BlockSettings_BlockAnimType(int Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_BlockSettings_BlockAnimType");
    if ((InRefreshControls)||(BlockSelectMode!=SELECTMODE_ONE)||(!CurrentCompoObject)||(!CurrentCompoObject->IsVisible)) return;
    CurrentCompoObject->BlockAnimType=Value;
    ApplyToContexte(false);
}

//========= Multiple block turn X value
void DlgSlideProperties::s_BlockSettings_BlockAnimTurnXValue(int Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_BlockSettings_BlockAnimTurnXValue");

    if ((InRefreshControls)||(BlockSelectMode!=SELECTMODE_ONE)||(!CurrentCompoObject)||(!CurrentCompoObject->IsVisible)) return;
    CurrentCompoObject->TurnXAxis=Value;
    ApplyToContexte(false);
}

void DlgSlideProperties::s_BlockSettings_BlockAnimTurnXReset() {
    s_BlockSettings_BlockAnimTurnXValue(0);
}

//========= Multiple block turn Z value
void DlgSlideProperties::s_BlockSettings_BlockAnimTurnZValue(int Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_BlockSettings_BlockAnimTurnZValue");

    if ((InRefreshControls)||(BlockSelectMode!=SELECTMODE_ONE)||(!CurrentCompoObject)||(!CurrentCompoObject->IsVisible)) return;
    CurrentCompoObject->TurnZAxis=Value;
    ApplyToContexte(false);
}

void DlgSlideProperties::s_BlockSettings_BlockAnimTurnZReset() {
    s_BlockSettings_BlockAnimTurnZValue(0);
}

//========= Multiple block turn Y value
void DlgSlideProperties::s_BlockSettings_BlockAnimTurnYValue(int Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_BlockSettings_BlockAnimTurnYValue");

    if ((InRefreshControls)||(BlockSelectMode!=SELECTMODE_ONE)||(!CurrentCompoObject)||(!CurrentCompoObject->IsVisible)) return;
    CurrentCompoObject->TurnYAxis=Value;
    ApplyToContexte(false);
}

void DlgSlideProperties::s_BlockSettings_BlockAnimTurnYReset() {
    s_BlockSettings_BlockAnimTurnYValue(0);
}

//========= Dissolve animation
void DlgSlideProperties::s_BlockSettings_BlockAnimDissolveType(int Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_BlockSettings_BlockAnimDissolveType");

    if ((InRefreshControls)||(BlockSelectMode!=SELECTMODE_ONE)||(!CurrentCompoObject)||(!CurrentCompoObject->IsVisible)) return;
    CurrentCompoObject->Dissolve=Value;
    ApplyToContexte(false);
}

//====================================================================================================================
// Handler for interactive zone
//====================================================================================================================

void DlgSlideProperties::s_BlockSettings_IntZoneTransformBlocks(double Move_X,double Move_Y,double Scale_X,double Scale_Y,double Sel_X,double Sel_Y,double Sel_W,double Sel_H) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_BlockSettings_IntZoneTransformBlocks");
    for (int i=0;i<IsSelected.count();i++) if (IsSelected[i]) {
        CompositionList->List[i]->x=CompositionList->List[i]->x+Move_X-(CompositionList->List[i]->x-Sel_X)+(Sel_W!=0?((CompositionList->List[i]->x-Sel_X)/Sel_W)*(Sel_W+Scale_X):0);
        CompositionList->List[i]->y=CompositionList->List[i]->y+Move_Y-(CompositionList->List[i]->y-Sel_Y)+(Sel_H!=0?((CompositionList->List[i]->y-Sel_Y)/Sel_H)*(Sel_H+Scale_Y):0);
        CompositionList->List[i]->w=Sel_W!=0?(CompositionList->List[i]->w/Sel_W)*(Sel_W+Scale_X):0; if (CompositionList->List[i]->w<0.002) CompositionList->List[i]->w=0.002;
        if (CompositionList->List[i]->BackgroundBrush->LockGeometry) CompositionList->List[i]->h=((CompositionList->List[i]->w*DisplayW)*CompositionList->List[i]->BackgroundBrush->AspectRatio)/DisplayH;
            else CompositionList->List[i]->h=Sel_H!=0?(CompositionList->List[i]->h/Sel_H)*(Sel_H+Scale_Y):0; if (CompositionList->List[i]->h<0.002) CompositionList->List[i]->h=0.002;
    }
    RefreshControls(true);
}

void DlgSlideProperties::s_BlockSettings_IntZoneDisplayTransformBlocks(double Move_X,double Move_Y,double Scale_X,double Scale_Y,double Sel_X,double Sel_Y,double Sel_W,double Sel_H) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_BlockSettings_IntZoneDisplayMoveBlocks");

    InRefreshControls=true;
    int i=CurrentCompoObjectNbr;
    double x=CompositionList->List[i]->x+Move_X-(CompositionList->List[i]->x-Sel_X)+(Sel_W!=0?((CompositionList->List[i]->x-Sel_X)/Sel_W)*(Sel_W+Scale_X):0);
    double y=CompositionList->List[i]->y+Move_Y-(CompositionList->List[i]->y-Sel_Y)+(Sel_H!=0?((CompositionList->List[i]->y-Sel_Y)/Sel_H)*(Sel_H+Scale_Y):0);
    double w=(Sel_W!=0?(CompositionList->List[i]->w/Sel_W)*(Sel_W+Scale_X):Scale_X);    if (w<0.002) w=0.002;
    double h;
    if (CompositionList->List[i]->BackgroundBrush->LockGeometry) h=((w*DisplayW)*CompositionList->List[i]->BackgroundBrush->AspectRatio)/DisplayH; else {
        h=(Sel_H!=0?(CompositionList->List[i]->h/Sel_H)*(Sel_H+Scale_Y):Scale_Y);
        if (h<0.002) h=0.002;
    }
    if (CurrentSlide->Parent->ApplicationConfig->DisplayUnit==DISPLAYUNIT_PERCENT) {
        ui->PosXEd->  setValue(x*100);
        ui->PosYEd->  setValue(y*100);
        ui->WidthEd-> setValue(w*100);
        ui->HeightEd->setValue(h*100);
    } else { // DisplayUnit==DISPLAYUNIT_PIXELS
        double DisplayW=0,DisplayH=0;
        GetForDisplayUnit(DisplayW,DisplayH);
        ui->PosXEd->  setValue(x*DisplayW);
        ui->PosYEd->  setValue(y*DisplayH);
        ui->WidthEd-> setValue(w*DisplayW);
        ui->HeightEd->setValue(h*DisplayH);
    }
    InRefreshControls=false;
}

//====================================================================================================================
// Handler for style sheet management
//====================================================================================================================

void DlgSlideProperties::s_CoordinateStyleBT() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_CoordinateStyleBT");

    if ((InRefreshControls)||(BlockSelectMode!=SELECTMODE_ONE)||(!CurrentCompoObject)||(!CurrentCompoObject->IsVisible)) return;

    // Define StyleCoordinateCollection filter
    if (CurrentCompoObject->BackgroundBrush->Image!=NULL)       ((cApplicationConfig *)BaseApplicationConfig)->StyleCoordinateCollection.SetImageGeometryFilter(CurrentSlide->Parent->ImageGeometry,CurrentCompoObject->BackgroundBrush->Image->ObjectGeometry);
    else if (CurrentCompoObject->BackgroundBrush->Video!=NULL)  ((cApplicationConfig *)BaseApplicationConfig)->StyleCoordinateCollection.SetImageGeometryFilter(CurrentSlide->Parent->ImageGeometry,CurrentCompoObject->BackgroundBrush->Video->ObjectGeometry);
    else                                                        ((cApplicationConfig *)BaseApplicationConfig)->StyleCoordinateCollection.SetProjectGeometryFilter(CurrentSlide->Parent->ImageGeometry);

    // Get Actual style definition
    QString ActualStyle=CurrentCompoObject->GetCoordinateStyle();

    // Open popup menu
    QString Item=((cApplicationConfig *)BaseApplicationConfig)->StyleCoordinateCollection.PopupCollectionMenu(this,ActualStyle);
    ui->CoordinateStyleBT->setDown(false);
    if (Item!="") {
        CurrentCompoObject->ApplyCoordinateStyle(((cApplicationConfig *)BaseApplicationConfig)->StyleCoordinateCollection.GetStyleDef(Item));
        // Refresh block table
        RefreshBlockTable(CurrentCompoObjectNbr);
    }
    RefreshStyleControls();
}

//====================================================================================================================

void DlgSlideProperties::s_BlockShapeStyleBT() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_BlockShapeStyleBT");

    if ((InRefreshControls)||(BlockSelectMode!=SELECTMODE_ONE)||(!CurrentCompoObject)||(!CurrentCompoObject->IsVisible)) return;
    QString ActualStyle =CurrentCompoObject->GetBlockShapeStyle();
    QString Item        =((cApplicationConfig *)BaseApplicationConfig)->StyleBlockShapeCollection.PopupCollectionMenu(this,ActualStyle);
    ui->BlockShapeStyleBT->setDown(false);
    if (Item!="") {
        CurrentCompoObject->ApplyBlockShapeStyle(((cApplicationConfig *)BaseApplicationConfig)->StyleBlockShapeCollection.GetStyleDef(Item));
        RefreshBlockTable(CurrentCompoObjectNbr);
    }
    RefreshStyleControls();
}

//====================================================================================================================

void DlgSlideProperties::s_ChangeFramingStyle(int Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideProperties::s_ChangeFramingStyle");

    if (StopMajFramingStyle) return;
    if ((InRefreshControls)||(BlockSelectMode!=SELECTMODE_ONE)||(!CurrentCompoObject)||(!CurrentCompoObject->IsVisible)) return;

    cBrushDefinition *CurrentBrush=CurrentCompoObject->BackgroundBrush;

    if ((CurrentBrush->Image==NULL)&&(CurrentBrush->Video==NULL)) {
        // It's a text block
        switch (Value) {
            case 0: // Unlock
                CurrentBrush->LockGeometry=false;
                break;
            case 1: // Lock to this geometry
                CurrentBrush->LockGeometry=true;
                CurrentBrush->AspectRatio=(CurrentCompoObject->h*DisplayH)/(CurrentCompoObject->w*DisplayW);
                break;
            case 2: // Lock to project geometry
                CurrentBrush->LockGeometry=true;
                CurrentBrush->AspectRatio=ProjectGeometry;
                break;
            default:
                return;
        }
        if (CurrentCompoObject->h>(CurrentCompoObject->w*DisplayW*CurrentBrush->AspectRatio)/DisplayH) CurrentCompoObject->h=(CurrentCompoObject->w*DisplayW*CurrentBrush->AspectRatio)/DisplayH;
            else CurrentCompoObject->w=(CurrentCompoObject->h*DisplayH/CurrentBrush->AspectRatio)/DisplayW;
    } else {
        // It's an image or video block
        int         i;
        QStringList List;

        switch (Value) {
            case 0: // Adjust to width
                ((cApplicationConfig *)BaseApplicationConfig)->StyleImageFramingCollection.StringDefToStringList(CurrentBrush->DefaultFramingW,List);
                break;
            case 1: // Adjust to heigth
                ((cApplicationConfig *)BaseApplicationConfig)->StyleImageFramingCollection.StringDefToStringList(CurrentBrush->DefaultFramingH,List);
                break;
            case 2: // Adjust to full
                ((cApplicationConfig *)BaseApplicationConfig)->StyleImageFramingCollection.StringDefToStringList(CurrentBrush->DefaultFramingF,List);
                break;
            case 3: // None
                break;
            default:
                ((cApplicationConfig *)BaseApplicationConfig)->StyleImageFramingCollection.StringToStringList(ui->FramingStyleCB->itemText(Value),List);
                break;
        }
        if (List.count()>0) {
            for (i=0;i<List.count();i++) {
                if      (List[i].startsWith("X:"))              CurrentBrush->X             =List[i].mid(QString("X:").length()).toDouble();
                else if (List[i].startsWith("Y:"))              CurrentBrush->Y             =List[i].mid(QString("Y:").length()).toDouble();
                else if (List[i].startsWith("ZoomFactor:"))     CurrentBrush->ZoomFactor    =List[i].mid(QString("ZoomFactor:").length()).toDouble();
                else if (List[i].startsWith("LockGeometry:"))   CurrentBrush->LockGeometry  =List[i].mid(QString("LockGeometry:").length()).toInt()==1;
                else if (List[i].startsWith("AspectRatio:"))    CurrentBrush->AspectRatio   =List[i].mid(QString("AspectRatio:").length()).toDouble();
            }
            CurrentBrush->AspectRatio=CurrentBrush->AspectRatio;
            if (CurrentCompoObject->h>(CurrentCompoObject->w*DisplayW*CurrentBrush->AspectRatio)/DisplayH) CurrentCompoObject->h=(CurrentCompoObject->w*DisplayW*CurrentBrush->AspectRatio)/DisplayH;
                else CurrentCompoObject->w=(CurrentCompoObject->h*DisplayH/CurrentBrush->AspectRatio)/DisplayW;
        }

    }
    int CurrentRow=ui->BlockTable->currentRow();
    RefreshBlockTable(CurrentRow>0?CurrentRow:0);
}
