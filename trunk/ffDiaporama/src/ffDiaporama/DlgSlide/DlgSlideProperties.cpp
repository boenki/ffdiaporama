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

#include "cCustomShotTable.h"
#include "cCustomBlockTable.h"
#include "cInteractiveZone.h"

#include "DlgSlideProperties.h"
#include "ui_DlgSlideProperties.h"

#include "engine/cTextFrame.h"
#include "engine/cLocation.h"

#include "CustomCtrl/cCTexteFrameComboBox.h"

#include "DlgInfoFile/DlgInfoFile.h"
#include "DlgImage/DlgImageCorrection.h"
#include "DlgText/DlgTextEdit.h"
#include "DlgFileExplorer/DlgFileExplorer.h"
#include "DlgRuler/DlgRulerDef.h"
#include "DlgChapter/DlgChapter.h"

#include <QClipboard>
#include <QMimeData>
#include <QMessageBox>
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

#define ICON_EDIT_IMAGE                     ":/img/EditImage.png"
#define ICON_EDIT_MOVIE                     ":/img/EditMovie.png"
#define ICON_EDIT_GMAPS                     ":/img/EditGMaps.png"

#define ISVIDEO(OBJECT)         ((OBJECT->MediaObject)&&(OBJECT->MediaObject->ObjectType==OBJECTTYPE_VIDEOFILE))
#define ISBLOCKVALIDE()         ((!InRefreshControls)&&(BlockSelectMode==SELECTMODE_ONE)&&(CurrentCompoObject))
#define ISBLOCKVALIDEVISIBLE()  (ISBLOCKVALIDE()&&(CurrentCompoObject->IsVisible))

//********************************************************************************************************************************
// DlgSlideProperties : Slide Dialog
//********************************************************************************************************************************

DlgSlideProperties::DlgSlideProperties(cDiaporamaObject *DiaporamaObject,cBaseApplicationConfig *ApplicationConfig,QWidget *parent):cShotComposer(DiaporamaObject,ApplicationConfig,parent),ui(new Ui::DlgSlideProperties) {
    ui->setupUi(this);
    Splitter                        =ui->SplitterTop;
    OkBt                            =ui->OKBT;
    CancelBt                        =ui->CancelBt;
    HelpBt                          =ui->HelpBt;
    HelpFile                        ="0119";
    UndoBt                          =ui->UndoBT;
    InteractiveZone                 =ui->InteractiveZone;
    BlockTable                      =ui->BlockTable;
    ShotTable                       =ui->ShotTable;
    InteractiveZone->MagneticRuler  =ApplicationConfig->SlideRuler;
    InteractiveZone->DisplayW       =DisplayW;
    InteractiveZone->DisplayH       =DisplayH;
    RedoneStyleCoordinate           =false;         // specific flag for GMapsObject
    CurrentShot                     =NULL;
    FramingCB_CurrentBrush          =NULL;
    FramingCB_CurrentShot           =-1;
}

//====================================================================================================================

DlgSlideProperties::~DlgSlideProperties() {
    InRefreshControls=true; // To ensure no controls will change during delete
    delete ui;
    CurrentSlide->Parent->CloseUnusedLibAv(CurrentSlide->Parent->CurrentCol);
}

//====================================================================================================================
// Initialise dialog

void DlgSlideProperties::DoInitDialog() {
    cShotComposer::DoInitDialog();

    ui->TextClipArtCB->PrepareTable();

    setWindowTitle(windowTitle()+" - "+QApplication::translate("DlgSlideProperties","Slide")+QString(" %1/%2").arg(CurrentSlide->Parent->CurrentCol+1).arg(CurrentSlide->Parent->List.count()));
    ui->OKPreviousBT->setEnabled(CurrentSlide->Parent->CurrentCol>0);
    ui->OKNextBT->setEnabled(CurrentSlide->Parent->CurrentCol<CurrentSlide->Parent->List.count()-1);
    ui->RulersBT->setIcon(QIcon(QString(InteractiveZone->MagneticRuler!=0?ICON_RULER_ON:ICON_RULER_OFF)));

    BlockTable->ProjectGeometry=ProjectGeometry;

    //******************************

    CompositionList         = NULL;

    // Re-entrence flags
    InRefreshStyleControls  =false;
    StopMajFramingStyle     =false;
    InDisplayDuration       =false;
    NoPrepUndo              =false;

    ui->SlideNameED->setText(CurrentSlide->SlideName);

    if (ui->TextFramingStyleCB->view()->width()<500)    ui->TextFramingStyleCB->view()->setFixedWidth(500);

    // Speed wave
    ui->SpeedWaveCB->AddProjectDefault(CurrentSlide->Parent->BlockAnimSpeedWave);

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
    ui->actionUpBlock->setIconVisibleInMenu(true);
    ui->actionDownBlock->setIconVisibleInMenu(true);
    ui->actionSetVisible->setIconVisibleInMenu(true);
    ui->actionSetHide->setIconVisibleInMenu(true);
    ui->actionSameAsPreviousShot->setIconVisibleInMenu(true);
    ui->actionUnlockSameAsPreviousShot->setIconVisibleInMenu(true);
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
    ui->actionAddShot->setIconVisibleInMenu(true);
    ui->actionRemoveShot->setIconVisibleInMenu(true);
    ui->actionMoveLeft->setIconVisibleInMenu(true);
    ui->actionMoveRight->setIconVisibleInMenu(true);
    ui->actionDistributeHoriz->setIconVisibleInMenu(true);
    ui->actionDistributeVert->setIconVisibleInMenu(true);
    ui->actionAddSimpleTextBlock->setIconVisibleInMenu(true);
    ui->actionAddClipArtTextBlock->setIconVisibleInMenu(true);
    ui->actionSaveAsProjectTitleModel->setIconVisibleInMenu(true);
    ui->actionSaveAsCptTitleModels->setIconVisibleInMenu(true);
    ui->actionSaveAsCreditTitleModels->setIconVisibleInMenu(true);
    ui->actionAddGMapsMap->setIconVisibleInMenu(true);
    ui->actionAddImageClipboard->setIconVisibleInMenu(true);

    ShotTable->setRowCount(1);

    // Define handler
    connect(ui->OKPreviousBT,SIGNAL(clicked()),this,SLOT(OKPrevious()));
    connect(ui->OKNextBT,SIGNAL(clicked()),this,SLOT(OKNext()));
    connect(ui->RulersBT,SIGNAL(clicked()),this,SLOT(s_RulersBt()));
    connect(ui->SaveModelBT,SIGNAL(pressed()),this,SLOT(s_SlideSet_SaveModel()));
    connect(ui->actionSaveAsProjectTitleModel,SIGNAL(triggered()),this,SLOT(s_SlideSet_SaveAsProjectTitleModel()));
    connect(ui->actionSaveAsCptTitleModels,SIGNAL(triggered()),this,SLOT(s_SlideSet_SaveAsCptTitleModels()));
    connect(ui->actionSaveAsCreditTitleModels,SIGNAL(triggered()),this,SLOT(s_SlideSet_SaveAsCreditTitleModels()));

    connect(ui->actionCopy,SIGNAL(triggered()),this,SLOT(s_BlockTable_Copy()));
    connect(ui->actionCut,SIGNAL(triggered()),this,SLOT(s_BlockTable_Cut()));
    connect(ui->actionPaste,SIGNAL(triggered()),this,SLOT(s_BlockTable_Paste()));

    connect(ui->actionSetVisible,SIGNAL(triggered()),this,SLOT(s_BlockSettings_ToggleVisibleState()));
    connect(ui->actionSetHide,SIGNAL(triggered()),this,SLOT(s_BlockSettings_ToggleVisibleState()));
    connect(ui->actionSameAsPreviousShot,SIGNAL(triggered()),this,SLOT(s_BlockSettings_SetSameAsPreviousShot()));
    connect(ui->actionUnlockSameAsPreviousShot,SIGNAL(triggered()),this,SLOT(s_BlockSettings_UnsetSameAsPreviousShot()));
    connect(ui->actionTakeSound,SIGNAL(triggered()),this,SLOT(s_BlockSettings_GetSound()));

    connect(ui->actionUpBlock,SIGNAL(triggered()),this,SLOT(s_BlockTable_MoveBlockUp()));
    connect(ui->actionDownBlock,SIGNAL(triggered()),this,SLOT(s_BlockTable_MoveBlockDown()));

    connect(ui->actionTop,SIGNAL(triggered()),this,SLOT(s_BlockTable_AlignTop()));
    connect(ui->actionMiddle,SIGNAL(triggered()),this,SLOT(s_BlockTable_AlignMiddle()));
    connect(ui->actionBottom,SIGNAL(triggered()),this,SLOT(s_BlockTable_AlignBottom()));
    connect(ui->actionLeft,SIGNAL(triggered()),this,SLOT(s_BlockTable_AlignLeft()));
    connect(ui->actionCenter,SIGNAL(triggered()),this,SLOT(s_BlockTable_AlignCenter()));
    connect(ui->actionRight,SIGNAL(triggered()),this,SLOT(s_BlockTable_AlignRight()));
    connect(ui->actionDistributeHoriz,SIGNAL(triggered()),this,SLOT(s_BlockTable_DistributeHoriz()));
    connect(ui->actionDistributeVert,SIGNAL(triggered()),this,SLOT(s_BlockTable_DistributeVert()));

    connect(ui->SlideNameED,SIGNAL(textEdited(QString)),this,SLOT(s_SlideSet_SlideNameChange(QString)));
    connect(ui->ChapterBT,SIGNAL(clicked()),this,SLOT(s_SlideSet_ChapterInformation()));
    connect(ui->ShotDurationED,SIGNAL(timeChanged(QTime)),this,SLOT(s_ShotTable_DurationChange(QTime)));

    connect(ui->ArrangeBT,SIGNAL(pressed()),this,SLOT(s_BlockSettings_Arrange()));
    connect(ui->EditBT,SIGNAL(pressed()),this,SLOT(s_BlockSettings_Edit()));
    connect(ui->actionEditText,SIGNAL(triggered()),this,SLOT(s_BlockSettings_TextEditor()));
    connect(ui->actionEditImage,SIGNAL(triggered()),this,SLOT(s_BlockSettings_ImageEditCorrect()));
    connect(ui->InfoBlockBT,SIGNAL(clicked()),this,SLOT(s_BlockSettings_Information()));
    connect(ui->actionInfo,SIGNAL(triggered()),this,SLOT(s_BlockSettings_Information()));

    connect(ShotTable,SIGNAL(itemSelectionChanged()),this,SLOT(s_ShotTable_SelectionChanged()));
    connect(ShotTable,SIGNAL(RightClickEvent(QMouseEvent *)),this,SLOT(s_ShotTable_RightClickEvent(QMouseEvent *)));
    connect(ShotTable,SIGNAL(DragMoveItem()),this,SLOT(s_ShotTable_DragMoveItem()));
    connect(ui->AddShot,SIGNAL(pressed()),this,SLOT(s_ShotTable_AddShot()));
    connect(ui->actionAddShot,SIGNAL(triggered()),this,SLOT(s_ShotTable_AddShot()));
    connect(ui->RemoveShot,SIGNAL(pressed()),this,SLOT(s_ShotTable_RemoveShot()));
    connect(ui->actionRemoveShot,SIGNAL(triggered()),this,SLOT(s_ShotTable_RemoveShot()));
    connect(ui->actionMoveLeft,SIGNAL(triggered()),this,SLOT(s_ShotTable_MoveLeft()));
    connect(ui->actionMoveRight,SIGNAL(triggered()),this,SLOT(s_ShotTable_MoveRight()));

    connect(ui->SpeedWaveCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_BlockSettings_SpeedWave(int)));

    // Block animation
    connect(ui->BlockAnimCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_BlockSettings_BlockAnimType(int)));
    connect(ui->TurnZED,SIGNAL(valueChanged(int)),this,SLOT(s_BlockSettings_BlockAnimTurnZValue(int)));         connect(ui->TurnZSlider,SIGNAL(valueChanged(int)),this,SLOT(s_BlockSettings_BlockAnimTurnZValue(int)));         connect(ui->ResetTurnZBT,SIGNAL(released()),this,SLOT(s_BlockSettings_BlockAnimTurnZReset()));
    connect(ui->TurnXED,SIGNAL(valueChanged(int)),this,SLOT(s_BlockSettings_BlockAnimTurnXValue(int)));         connect(ui->TurnXSlider,SIGNAL(valueChanged(int)),this,SLOT(s_BlockSettings_BlockAnimTurnXValue(int)));         connect(ui->ResetTurnXBT,SIGNAL(released()),this,SLOT(s_BlockSettings_BlockAnimTurnXReset()));
    connect(ui->TurnYED,SIGNAL(valueChanged(int)),this,SLOT(s_BlockSettings_BlockAnimTurnYValue(int)));         connect(ui->TurnYSlider,SIGNAL(valueChanged(int)),this,SLOT(s_BlockSettings_BlockAnimTurnYValue(int)));         connect(ui->ResetTurnYBT,SIGNAL(released()),this,SLOT(s_BlockSettings_BlockAnimTurnYReset()));
    connect(ui->DissolveCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_BlockSettings_BlockAnimDissolveType(int)));

    // Block table/scene part
    connect(BlockTable,SIGNAL(itemSelectionChanged()),this,SLOT(s_BlockTable_SelectionChanged()));
    connect(BlockTable,SIGNAL(DoubleClickEvent(QMouseEvent *)),this,SLOT(s_BlockTable_ItemDoubleClicked(QMouseEvent *)));
    connect(BlockTable,SIGNAL(RightClickEvent(QMouseEvent *)),this,SLOT(s_BlockTable_ItemRightClicked(QMouseEvent *)));
    connect(BlockTable,SIGNAL(DragMoveBlock(int,int)),this,SLOT(s_BlockTable_DragMoveBlock(int,int)));
    connect(BlockTable,SIGNAL(DragDropFiles(QList<QUrl>)),this,SLOT(s_BlockTable_DragDropFiles(QList<QUrl>)));
    connect(ui->AddTextBlock,SIGNAL(pressed()),this,SLOT(s_BlockTable_AddNewTextBlock()));
    connect(ui->actionAddTextBlock,SIGNAL(triggered()),this,SLOT(s_BlockTable_AddNewTextBlock()));
    connect(ui->actionAddSimpleTextBlock,SIGNAL(triggered()),this,SLOT(s_BlockTable_AddNewSimpleTextBlock()));
    connect(ui->actionAddClipArtTextBlock,SIGNAL(triggered()),this,SLOT(s_BlockTable_AddNewClipArtTextBlock()));
    connect(ui->AddFileBlock,SIGNAL(pressed()),this,SLOT(s_BlockTable_AddNewFileBlock()));
    connect(ui->actionAddFile,SIGNAL(triggered()),this,SLOT(s_BlockTable_AddNewFileBlock()));
    connect(ui->actionRemoveBlock,SIGNAL(triggered()),this,SLOT(s_BlockTable_RemoveBlock()));

    connect(ui->AddSpecialBlock,SIGNAL(pressed()),this,SLOT(s_BlockTable_AddSpecialBlock()));
    connect(ui->actionAddImageClipboard,SIGNAL(triggered()),this,SLOT(s_BlockTable_AddImageClipboard()));
    connect(ui->actionAddGMapsMap,SIGNAL(triggered()),this,SLOT(s_BlockTable_AddGMapsMapBlock()));

    // Style buttons
    connect(ui->FramingStyleCB,SIGNAL(itemSelectionHaveChanged()),this,SLOT(s_ChangeFramingStyle()));
    connect(ui->TextFramingStyleCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChangeTextFramingStyle(int)));

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
    connect(InteractiveZone,SIGNAL(StartSelectionChange()),this,SLOT(s_BlockTable_StartSelectionChange()));
    connect(InteractiveZone,SIGNAL(EndSelectionChange()),this,SLOT(s_BlockTable_EndSelectionChange()));
    connect(InteractiveZone,SIGNAL(RightClickEvent(QMouseEvent *)),this,SLOT(s_BlockTable_ItemRightClicked(QMouseEvent *)));
    connect(InteractiveZone,SIGNAL(DoubleClickEvent(QMouseEvent *)),this,SLOT(s_BlockTable_ItemDoubleClicked(QMouseEvent *)));
    connect(InteractiveZone,SIGNAL(TransformBlock(qreal,qreal,qreal,qreal,qreal,qreal,qreal,qreal)),this,SLOT(s_BlockSettings_IntZoneTransformBlocks(qreal,qreal,qreal,qreal,qreal,qreal,qreal,qreal)));
    connect(InteractiveZone,SIGNAL(DisplayTransformBlock(qreal,qreal,qreal,qreal,qreal,qreal,qreal,qreal)),this,SLOT(s_BlockSettings_IntZoneDisplayTransformBlocks(qreal,qreal,qreal,qreal,qreal,qreal,qreal,qreal)));

    s_Event_ClipboardChanged();           // Setup clipboard button state
    connect(QApplication::clipboard(),SIGNAL(dataChanged()),this,SLOT(s_Event_ClipboardChanged()));
}

//====================================================================================================================

void DlgSlideProperties::resizeEvent(QResizeEvent *) {
    ShotTable->setFixedHeight(ApplicationConfig->TimelineHeight/2+(ShotTable->height()-ShotTable->viewport()->height()));
    InteractiveZone->RefreshDisplay();
}

//====================================================================================================================

void DlgSlideProperties::showEvent(QShowEvent *ev) {
    QCustomDialog::showEvent(ev);

    if (CurrentShot==NULL) {
        // Init ShotTable
        ShotTable->setUpdatesEnabled(false);

        ShotTable->setRowCount(1);
        ShotTable->setRowHeight(0,ShotTable->viewport()->height());
        ShotTable->setColumnCount(CurrentSlide->List.count()/*-1*/);

        for (int i=0;i<CurrentSlide->List.count();i++) ShotTable->setColumnWidth(i,CurrentSlide->Parent->GetWidthForHeight(ShotTable->rowHeight(0)));
        ShotTable->setCurrentCell(0,0);
        ShotTable->setUpdatesEnabled(true);

        ui->ZoomED->setFixedWidth(ui->ScrollXED->width());
    }
}

//====================================================================================================================
// Save object before modification for cancel button

void DlgSlideProperties::PrepareGlobalUndo() {
    Undo=new QDomDocument(APPLICATION_NAME);
    QDomElement root=Undo->createElement("UNDO-DLG");                                               // Create xml document and root
    CurrentSlide->SaveToXML(root,"UNDO-DLG-OBJECT",CurrentSlide->Parent->ProjectFileName,true,NULL,NULL,false);     // Save object
    Undo->appendChild(root);                                                                        // Add object to xml document
}

//====================================================================================================================
// Apply Undo : call when user click on Cancel button

void DlgSlideProperties::DoGlobalUndo() {
    QDomElement root=Undo->documentElement();
    if (root.tagName()=="UNDO-DLG") CurrentSlide->LoadFromXML(root,"UNDO-DLG-OBJECT","",NULL,NULL,false);
}

//====================================================================================================================

void DlgSlideProperties::keyReleaseEvent(QKeyEvent *event) {
    if ((focusWidget()==BlockTable)||(focusWidget()==InteractiveZone)||(focusWidget()==ShotTable)) {
        if (event->modifiers()==Qt::ControlModifier) {
            switch (event->key()) {
                case Qt::Key_C      : s_BlockTable_Copy();                      break;
                case Qt::Key_X      : s_BlockTable_Cut();                       break;
                case Qt::Key_V      : s_BlockTable_Paste();                     break;
                default             : QCustomDialog::keyReleaseEvent(event);    break;
            }
        } if (event->modifiers()==Qt::NoModifier) {
            switch (event->key()) {
                case Qt::Key_Delete :
                    if ((InteractiveZone->hasFocus())||(BlockTable->hasFocus())) s_BlockTable_RemoveBlock();
                    else if (ShotTable->hasFocus()) s_ShotTable_RemoveShot();
                    break;
                default :
                    QCustomDialog::keyReleaseEvent(event);
                    break;
            }
        } else QCustomDialog::keyReleaseEvent(event);
    } else QCustomDialog::keyReleaseEvent(event);
}

//====================================================================================================================
// Call when user click on Ok button

bool DlgSlideProperties::DoAccept() {
    InRefreshControls=true;
    CurrentSlide->Parent->UpdateChapterInformation();
    emit(SetModifyFlag());
    return true;
}

void DlgSlideProperties::OKPrevious() {
    InRefreshControls=true;
    SaveWindowState();  // Save Window size and position
    CurrentSlide->Parent->UpdateChapterInformation();
    emit(SetModifyFlag());
    done(2);            // Close the box
}

void DlgSlideProperties::OKNext() {
    InRefreshControls=true;
    SaveWindowState();  // Save Window size and position
    CurrentSlide->Parent->UpdateChapterInformation();
    emit(SetModifyFlag());
    done(3);            // Close the box
}

void DlgSlideProperties::DoRejet() {
    InRefreshControls=true;
}

//====================================================================================================================

void DlgSlideProperties::s_Event_ClipboardChanged() {
    ui->actionAddImageClipboard->setEnabled((QApplication::clipboard())&&(QApplication::clipboard()->mimeData())&&(QApplication::clipboard()->mimeData()->hasImage()));
    ui->actionPaste->setEnabled((QApplication::clipboard())&&(QApplication::clipboard()->mimeData())&&
                                ((QApplication::clipboard()->mimeData()->hasFormat("ffDiaporama/block"))||(QApplication::clipboard()->mimeData()->hasImage())));
}

//====================================================================================================================

void DlgSlideProperties::PreparePartialUndo(int ActionType,QDomElement root,bool) {
    QDomDocument DomDocument;

    root.setAttribute("BlockNbr",CompositionList->List.count());
    QModelIndexList SelList=BlockTable->selectionModel()->selectedIndexes();
    QString         Selection;
    for (int i=0;i<SelList.count();i++) Selection=Selection+(Selection.length()>0?"###":"")+QString("%1").arg(SelList.at(i).row());
    root.setAttribute("Selection",Selection);

    for (int i=0;i<CompositionList->List.count();i++) {
        QDomElement SubElement=DomDocument.createElement(QString("BLOCK-%1").arg(i));
        switch (ActionType) {
            case UNDOACTION_INTERACTIVEMOVERESIZE:
            case UNDOACTION_EDITZONE_POSX:
            case UNDOACTION_EDITZONE_POSY:
            case UNDOACTION_EDITZONE_WIDTH:
            case UNDOACTION_EDITZONE_HEIGHT:
            case UNDOACTION_EDITZONE_ROTATEZ:
            case UNDOACTION_EDITZONE_ROTATEX:
            case UNDOACTION_EDITZONE_ROTATEY:
            case UNDOACTION_BLOCKTABLE_ARRANGEBLOCK:
            case UNDOACTION_STYLE_COORDINATES:
                SubElement.setAttribute("x",     CompositionList->List[i]->x);
                SubElement.setAttribute("y",     CompositionList->List[i]->y);
                SubElement.setAttribute("w",     CompositionList->List[i]->w);
                SubElement.setAttribute("h",     CompositionList->List[i]->h);
                SubElement.setAttribute("RZAxis",CompositionList->List[i]->RotateZAxis);
                SubElement.setAttribute("RXAxis",CompositionList->List[i]->RotateXAxis);
                SubElement.setAttribute("RYAxis",CompositionList->List[i]->RotateYAxis);
                break;
            case UNDOACTION_EDITZONE_SHAPEOPACITY:          SubElement.setAttribute("BackgroundTransparent",CompositionList->List[i]->Opacity);             break;
            case UNDOACTION_EDITZONE_SHAPEFORM:             SubElement.setAttribute("BackgroundForm",CompositionList->List[i]->BackgroundForm);             break;
            case UNDOACTION_EDITZONE_TEXTCLIPART:           SubElement.setAttribute("TextClipArtName",CompositionList->List[i]->TextClipArtName);           break;
            case UNDOACTION_EDITZONE_SHAPEPENSIZE:          SubElement.setAttribute("PenSize",CompositionList->List[i]->PenSize);                           break;
            case UNDOACTION_EDITZONE_SHAPEPENSTYLE:         SubElement.setAttribute("PenStyle",CompositionList->List[i]->PenStyle);                         break;
            case UNDOACTION_EDITZONE_SHAPEPENCOLOR:         SubElement.setAttribute("PenColor",CompositionList->List[i]->PenColor);                         break;
            case UNDOACTION_EDITZONE_SHAPESHADOWFORM:       SubElement.setAttribute("FormShadow",CompositionList->List[i]->FormShadow);                     break;
            case UNDOACTION_EDITZONE_SHAPESHADOWDIST:       SubElement.setAttribute("FormShadowDistance",CompositionList->List[i]->FormShadowDistance);     break;
            case UNDOACTION_EDITZONE_SHAPESHADOWCOLOR:      SubElement.setAttribute("FormShadowColor",CompositionList->List[i]->FormShadowColor);           break;
            case UNDOACTION_EDITZONE_TEXTANIMZOOM:          SubElement.setAttribute("TxtZoomLevel",CompositionList->List[i]->TxtZoomLevel);                 break;
            case UNDOACTION_EDITZONE_TEXTANIMSCROLLX:       SubElement.setAttribute("TxtScrollX",CompositionList->List[i]->TxtScrollX);                     break;
            case UNDOACTION_EDITZONE_TEXTANIMSCROLLY:       SubElement.setAttribute("TxtScrollY",CompositionList->List[i]->TxtScrollY);                     break;
            case UNDOACTION_EDITZONE_BLOCKANIMTYPE:         SubElement.setAttribute("BlockAnimType",CompositionList->List[i]->BlockAnimType);               break;
            case UNDOACTION_EDITZONE_BLOCKSPEEDWAVE:        SubElement.setAttribute("BlockSpeedWave",CompositionList->List[i]->BlockSpeedWave);             break;
            case UNDOACTION_EDITZONE_BLOCKANIMTURNX:        SubElement.setAttribute("TurnXAxis",CompositionList->List[i]->TurnXAxis);                       break;
            case UNDOACTION_EDITZONE_BLOCKANIMTURNZ:        SubElement.setAttribute("TurnZAxis",CompositionList->List[i]->TurnZAxis);                       break;
            case UNDOACTION_EDITZONE_BLOCKANIMTURNY:        SubElement.setAttribute("TurnYAxis",CompositionList->List[i]->TurnYAxis);                       break;
            case UNDOACTION_EDITZONE_BLOCKANIMDISSOLVE:     SubElement.setAttribute("Dissolve",CompositionList->List[i]->Dissolve);                         break;
            case UNDOACTION_EDITZONE_SLIDENAME:             SubElement.setAttribute("SlideName",CurrentSlide->SlideName);                                   break;
            case UNDOACTION_EDITZONE_SHOTDURATION:          SubElement.setAttribute("StaticDuration",qlonglong(CurrentShot->StaticDuration));               break;
            case UNDOACTION_BLOCKTABLE_VISIBLESTATE:        SubElement.setAttribute("IsVisible",CompositionList->List[i]->IsVisible?"1":"0");               break;
            case UNDOACTION_BLOCKTABLE_SOUNDSTATE:          SubElement.setAttribute("SoundVolume",CompositionList->List[i]->BackgroundBrush->SoundVolume);
            case UNDOACTION_BLOCKTABLE_EDITVIDEO:           CompositionList->List[i]->BackgroundBrush->SaveToXML(&SubElement,"BRUSH","",false,NULL,NULL);    break;
            case UNDOACTION_BLOCKTABLE_EDITTEXT:
            case UNDOACTION_BLOCKTABLE_EDITIMAGE:
            case UNDOACTION_STYLE_SHAPE:
            case UNDOACTION_STYLE_FRAMING:
            case UNDOACTION_BLOCKTABLE_ADDTEXTBLOCK:
            case UNDOACTION_BLOCKTABLE_ADDFILEBLOCK:
            case UNDOACTION_BLOCKTABLE_REMOVEBLOCK:
            case UNDOACTION_BLOCKTABLE_PASTEBLOCK:
            case UNDOACTION_BLOCKTABLE_CHBLOCKORDER:
            case UNDOACTION_SHOTTABLE_ADDSHOT:
            case UNDOACTION_SHOTTABLE_REMOVESHOT:
            case UNDOACTION_SHOTTABLE_CHORDER:
            case UNDOACTION_BLOCKTABLE_SAMEASPREVIOUSSTATE:
            default:
                CurrentSlide->SaveToXML(root,"UNDO-DLG-OBJECT",CurrentSlide->Parent->ProjectFileName,true,NULL,NULL,false);     // Save all
                break;
        }
        root.appendChild(SubElement);
    }
}

//====================================================================================================================

void DlgSlideProperties::ApplyPartialUndo(int ActionType,QDomElement root) {
    int BlockNbr=0;
    if (root.hasAttribute("BlockNbr")) {
        BlockNbr=root.attribute("BlockNbr").toInt();
        QStringList SelList=root.attribute("Selection").split("###");
        for (int i=0;i<BlockNbr;i++) if ((root.elementsByTagName(QString("BLOCK-%1").arg(i)).length()>0)&&(root.elementsByTagName(QString("BLOCK-%1").arg(i)).item(0).isElement()==true)) {
            QDomElement SubElement=root.elementsByTagName(QString("BLOCK-%1").arg(i)).item(0).toElement();

            switch (ActionType) {
                case UNDOACTION_INTERACTIVEMOVERESIZE:
                case UNDOACTION_EDITZONE_POSX:
                case UNDOACTION_EDITZONE_POSY:
                case UNDOACTION_EDITZONE_WIDTH:
                case UNDOACTION_EDITZONE_HEIGHT:
                case UNDOACTION_BLOCKTABLE_ARRANGEBLOCK:
                case UNDOACTION_STYLE_COORDINATES:
                case UNDOACTION_EDITZONE_ROTATEZ:
                case UNDOACTION_EDITZONE_ROTATEX:
                case UNDOACTION_EDITZONE_ROTATEY:
                    if (SubElement.hasAttribute("x"))      CompositionList->List[i]->x          =GetDoubleValue(SubElement,"x");
                    if (SubElement.hasAttribute("y"))      CompositionList->List[i]->y          =GetDoubleValue(SubElement,"y");
                    if (SubElement.hasAttribute("w"))      CompositionList->List[i]->w          =GetDoubleValue(SubElement,"w");
                    if (SubElement.hasAttribute("h"))      CompositionList->List[i]->h          =GetDoubleValue(SubElement,"h");
                    if (SubElement.hasAttribute("RZAxis")) CompositionList->List[i]->RotateZAxis=GetDoubleValue(SubElement,"RZAxis");
                    if (SubElement.hasAttribute("RXAxis")) CompositionList->List[i]->RotateXAxis=GetDoubleValue(SubElement,"RXAxis");
                    if (SubElement.hasAttribute("RYAxis")) CompositionList->List[i]->RotateYAxis=GetDoubleValue(SubElement,"RYAxis");
                    break;
                case UNDOACTION_EDITZONE_SHAPEOPACITY:      if (SubElement.hasAttribute("BackgroundTransparent"))   CompositionList->List[i]->Opacity=                      SubElement.attribute("BackgroundTransparent").toInt();  break;
                case UNDOACTION_EDITZONE_SHAPEFORM:         if (SubElement.hasAttribute("BackgroundForm"))          CompositionList->List[i]->BackgroundForm=               SubElement.attribute("BackgroundForm").toInt();         break;
                case UNDOACTION_EDITZONE_TEXTCLIPART:       if (SubElement.hasAttribute("TextClipArtName"))         CompositionList->List[i]->TextClipArtName=              SubElement.attribute("TextClipArtName");                break;
                case UNDOACTION_EDITZONE_SHAPEPENSIZE:      if (SubElement.hasAttribute("PenSize"))                 CompositionList->List[i]->PenSize=                      SubElement.attribute("PenSize").toInt();                break;
                case UNDOACTION_EDITZONE_SHAPEPENSTYLE:     if (SubElement.hasAttribute("PenStyle"))                CompositionList->List[i]->PenStyle=                     SubElement.attribute("PenStyle").toInt();               break;
                case UNDOACTION_EDITZONE_SHAPEPENCOLOR:     if (SubElement.hasAttribute("PenColor"))                CompositionList->List[i]->PenColor=                     SubElement.attribute("PenColor");                       break;
                case UNDOACTION_EDITZONE_SHAPESHADOWFORM:   if (SubElement.hasAttribute("FormShadow"))              CompositionList->List[i]->FormShadow=                   SubElement.attribute("FormShadow").toInt();             break;
                case UNDOACTION_EDITZONE_SHAPESHADOWDIST:   if (SubElement.hasAttribute("FormShadowDistance"))      CompositionList->List[i]->FormShadowDistance=           SubElement.attribute("FormShadowDistance").toInt();     break;
                case UNDOACTION_EDITZONE_SHAPESHADOWCOLOR:  if (SubElement.hasAttribute("FormShadowColor"))         CompositionList->List[i]->FormShadowColor=              SubElement.attribute("FormShadowColor");                break;
                case UNDOACTION_EDITZONE_TEXTANIMZOOM:      if (SubElement.hasAttribute("TxtZoomLevel"))            CompositionList->List[i]->TxtZoomLevel=                 SubElement.attribute("TxtZoomLevel").toInt();           break;
                case UNDOACTION_EDITZONE_TEXTANIMSCROLLX:   if (SubElement.hasAttribute("TxtScrollX"))              CompositionList->List[i]->TxtScrollX=                   SubElement.attribute("TxtScrollX").toInt();             break;
                case UNDOACTION_EDITZONE_TEXTANIMSCROLLY:   if (SubElement.hasAttribute("TxtScrollY"))              CompositionList->List[i]->TxtScrollY=                   SubElement.attribute("TxtScrollY").toInt();             break;
                case UNDOACTION_EDITZONE_BLOCKANIMTYPE:     if (SubElement.hasAttribute("BlockAnimType"))           CompositionList->List[i]->BlockAnimType=                SubElement.attribute("BlockAnimType").toInt();          break;
                case UNDOACTION_EDITZONE_BLOCKSPEEDWAVE:    if (SubElement.hasAttribute("BlockSpeedWave"))          CompositionList->List[i]->BlockSpeedWave=               SubElement.attribute("BlockSpeedWave").toInt();         break;
                case UNDOACTION_EDITZONE_BLOCKANIMTURNX:    if (SubElement.hasAttribute("TurnXAxis"))               CompositionList->List[i]->TurnXAxis=                    SubElement.attribute("TurnXAxis").toInt();              break;
                case UNDOACTION_EDITZONE_BLOCKANIMTURNZ:    if (SubElement.hasAttribute("TurnZAxis"))               CompositionList->List[i]->TurnZAxis=                    SubElement.attribute("TurnZAxis").toInt();              break;
                case UNDOACTION_EDITZONE_BLOCKANIMTURNY:    if (SubElement.hasAttribute("TurnYAxis"))               CompositionList->List[i]->TurnYAxis=                    SubElement.attribute("TurnYAxis").toInt();              break;
                case UNDOACTION_EDITZONE_BLOCKANIMDISSOLVE: if (SubElement.hasAttribute("Dissolve"))                CompositionList->List[i]->Dissolve=                     SubElement.attribute("Dissolve").toInt();               break;
                case UNDOACTION_EDITZONE_SLIDENAME:         if (SubElement.hasAttribute("SlideName"))               CurrentSlide->SlideName=                                SubElement.attribute("SlideName");                      ui->SlideNameED->setText(CurrentSlide->SlideName);              break;
                case UNDOACTION_EDITZONE_SHOTDURATION:      if (SubElement.hasAttribute("StaticDuration"))          CurrentShot->StaticDuration=                            SubElement.attribute("StaticDuration").toLongLong();    s_ShotTable_DisplayDuration();                                  break;
                case UNDOACTION_BLOCKTABLE_VISIBLESTATE:    if (SubElement.hasAttribute("IsVisible"))               CompositionList->List[i]->IsVisible=                    SubElement.attribute("IsVisible")=="1";                 break;
                case UNDOACTION_BLOCKTABLE_SOUNDSTATE:      if (SubElement.hasAttribute("SoundVolume"))             CompositionList->List[i]->BackgroundBrush->SoundVolume= GetDoubleValue(SubElement,"SoundVolume");               break;
                case UNDOACTION_BLOCKTABLE_EDITVIDEO:       CompositionList->List[i]->BackgroundBrush->LoadFromXML(&SubElement,"BRUSH","",NULL,NULL,NULL,false);   break;
                case UNDOACTION_BLOCKTABLE_EDITTEXT:
                case UNDOACTION_BLOCKTABLE_EDITIMAGE:
                case UNDOACTION_STYLE_SHAPE:
                case UNDOACTION_STYLE_FRAMING:
                case UNDOACTION_BLOCKTABLE_ADDTEXTBLOCK:
                case UNDOACTION_BLOCKTABLE_ADDFILEBLOCK:
                case UNDOACTION_BLOCKTABLE_REMOVEBLOCK:
                case UNDOACTION_BLOCKTABLE_PASTEBLOCK:
                case UNDOACTION_BLOCKTABLE_CHBLOCKORDER:
                case UNDOACTION_BLOCKTABLE_SAMEASPREVIOUSSTATE:
                    CurrentSlide->LoadFromXML(root,"UNDO-DLG-OBJECT","",NULL,NULL,false);    // Restore all
                    i=BlockNbr; // Stop loop
                    ShotTable->setUpdatesEnabled(false);
                    ShotTable->setUpdatesEnabled(true);  // Reset timeline painting
                    s_ShotTable_SelectionChanged();
                    break;

                case UNDOACTION_SHOTTABLE_ADDSHOT:
                case UNDOACTION_SHOTTABLE_REMOVESHOT:
                case UNDOACTION_SHOTTABLE_CHORDER:
                default:
                    InRefreshControls=true;
                    CurrentSlide->LoadFromXML(root,"UNDO-DLG-OBJECT","",NULL,NULL,false);    // Restore all
                    i=BlockNbr; // Stop loop
                    ShotTable->setUpdatesEnabled(false);
                    while (ShotTable->columnCount()>0) ShotTable->removeColumn(ShotTable->columnCount()-1);
                    for (int i=0;i<CurrentSlide->List.count();i++) {
                        ShotTable->insertColumn(i);
                        ShotTable->setColumnWidth(i,CurrentSlide->Parent->GetWidthForHeight(ShotTable->rowHeight(0)));
                    }
                    ShotTable->setCurrentCell(0,0);
                    ShotTable->setUpdatesEnabled(true);
                    InRefreshControls=false;
                    s_ShotTable_SelectionChanged();
                    break;
            }
        }
        CurrentSlide->Parent->UpdateChapterInformation();
        ApplyToContexte(true);
        RefreshBlockTable(CurrentCompoObjectNbr);
        BlockTable->clearSelection();
        BlockTable->setCurrentCell(SelList[0].toInt(),0,QItemSelectionModel::Select|QItemSelectionModel::Current);
        for (int i=1;i<SelList.count();i++) BlockTable->setCurrentCell(SelList[i].toInt(),0,QItemSelectionModel::Select);
        ShotTable->setUpdatesEnabled(false);
        ShotTable->setUpdatesEnabled(true);
        //RefreshControls(true);
    }
}

//====================================================================================================================

void DlgSlideProperties::RefreshStyleControls() {
    if (InRefreshStyleControls) return;
    InRefreshStyleControls=true;

    bool IsVisible=(BlockSelectMode==SELECTMODE_ONE)&&(CurrentCompoObject);
    ui->FramingStyleCB    ->setEnabled(IsVisible && !SelectionHaveLockBlock);
    ui->TextFramingStyleCB->setEnabled(IsVisible && !SelectionHaveLockBlock);
    ui->ShapeSizePosCB    ->setEnabled(IsVisible && !SelectionHaveLockBlock);

    if (IsVisible) {
        ui->BlockShapeStyleED->setText(ApplicationConfig->StyleBlockShapeCollection.GetStyleName(CurrentCompoObject->GetBlockShapeStyle()));

        ui->ShapeSizePosCB->setUpdatesEnabled(false);
        ui->ShapeSizePosCB->clear();
        int ImageType=CurrentCompoObject->BackgroundBrush->GetImageType();
        int AutoCompo=CurrentCompoObject->GetAutoCompoSize(CurrentSlide->Parent->ImageGeometry);

        if (AutoCompo==AUTOCOMPOSIZE_CUSTOM) {                                                                                  // AUTOCOMPOSIZE_CUSTOM
            ui->ShapeSizePosCB->addItem(QApplication::translate("DlgImageCorrection","Custom size"),QVariant(AUTOCOMPOSIZE_CUSTOM));
            ui->ShapeSizePosCB->setCurrentIndex(ui->ShapeSizePosCB->count()-1);
        }
        if ((ImageType!=AUTOCOMPOSIZE_CUSTOM)&&(ImageType!=IMAGETYPE_PHOTOLANDSCAPE)&&(ImageType!=IMAGETYPE_PHOTOPORTRAIT)) {   // AUTOCOMPOSIZE_REALSIZE
            ui->ShapeSizePosCB->addItem(QApplication::translate("DlgImageCorrection","Real image size"),QVariant(AUTOCOMPOSIZE_REALSIZE));
            if (AutoCompo==AUTOCOMPOSIZE_REALSIZE) ui->ShapeSizePosCB->setCurrentIndex(ui->ShapeSizePosCB->count()-1);
        }
        ui->ShapeSizePosCB->addItem(QApplication::translate("DlgImageCorrection","Full screen size"),QVariant(AUTOCOMPOSIZE_FULLSCREEN));           // AUTOCOMPOSIZE_FULLSCREEN
        if (AutoCompo==AUTOCOMPOSIZE_FULLSCREEN) ui->ShapeSizePosCB->setCurrentIndex(ui->ShapeSizePosCB->count()-1);
        ui->ShapeSizePosCB->addItem(QApplication::translate("DlgImageCorrection","TV margins"),QVariant(AUTOCOMPOSIZE_TVMARGINS));                  // AUTOCOMPOSIZE_TVMARGINS
        if (AutoCompo==AUTOCOMPOSIZE_TVMARGINS) ui->ShapeSizePosCB->setCurrentIndex(ui->ShapeSizePosCB->count()-1);
        ui->ShapeSizePosCB->addItem(QApplication::translate("DlgImageCorrection","Two thirds screen"),QVariant(AUTOCOMPOSIZE_TWOTHIRDSSCREEN));     // AUTOCOMPOSIZE_TWOTHIRDSSCREEN
        if (AutoCompo==AUTOCOMPOSIZE_TWOTHIRDSSCREEN) ui->ShapeSizePosCB->setCurrentIndex(ui->ShapeSizePosCB->count()-1);
        ui->ShapeSizePosCB->addItem(QApplication::translate("DlgImageCorrection","Half screen"),QVariant(AUTOCOMPOSIZE_HALFSCREEN));                // AUTOCOMPOSIZE_HALFSCREEN
        if (AutoCompo==AUTOCOMPOSIZE_HALFSCREEN) ui->ShapeSizePosCB->setCurrentIndex(ui->ShapeSizePosCB->count()-1);
        ui->ShapeSizePosCB->addItem(QApplication::translate("DlgImageCorrection","Third screen"),QVariant(AUTOCOMPOSIZE_THIRDSCREEN));              // AUTOCOMPOSIZE_THIRDSCREEN
        if (AutoCompo==AUTOCOMPOSIZE_THIRDSCREEN) ui->ShapeSizePosCB->setCurrentIndex(ui->ShapeSizePosCB->count()-1);
        ui->ShapeSizePosCB->addItem(QApplication::translate("DlgImageCorrection","Quarter screen"),QVariant(AUTOCOMPOSIZE_QUARTERSCREEN));          // AUTOCOMPOSIZE_QUARTERSCREEN
        if (AutoCompo==AUTOCOMPOSIZE_QUARTERSCREEN) ui->ShapeSizePosCB->setCurrentIndex(ui->ShapeSizePosCB->count()-1);
        ui->ShapeSizePosCB->setUpdatesEnabled(true);

        StopMajFramingStyle=true;
        if ((!CurrentCompoObject->BackgroundBrush->MediaObject)||(CurrentCompoObject->BackgroundBrush->MediaObject->ObjectType==OBJECTTYPE_UNMANAGED)) {
            // It's a text block

            if (ui->FramingStyleCB->isVisible())         ui->FramingStyleCB->setVisible(false);
            if (!ui->TextFramingStyleCB->isVisible())    ui->TextFramingStyleCB->setVisible(true);
            if (!ui->TextFramingStyleLabel->isVisible()) ui->TextFramingStyleLabel->setVisible(true);

            if (ui->TextFramingStyleCB->itemText(0)!=QApplication::translate("DlgImageCorrection","Unlock")) {
                ui->TextFramingStyleCB->setUpdatesEnabled(false);
                ui->TextFramingStyleCB->clear();
                ui->TextFramingStyleCB->addItem(QIcon(ICON_GEOMETRY_UNLOCKED),QApplication::translate("DlgImageCorrection","Unlock"),QVariant(AUTOFRAMING_CUSTOMLOCK));
                ui->TextFramingStyleCB->addItem(QIcon(ICON_GEOMETRY_LOCKED),  QApplication::translate("DlgImageCorrection","Lock to this geometry"),QVariant(AUTOFRAMING_CUSTOMUNLOCK));
                ui->TextFramingStyleCB->addItem(QIcon(ICON_GEOMETRY_PROJECT), QApplication::translate("DlgImageCorrection","Lock to project geometry"),QVariant(AUTOFRAMING_CUSTOMPRJLOCK));
            }
            int Index=1;
            if (!CurrentCompoObject->BackgroundBrush->LockGeometry) Index=0; else if (CurrentCompoObject->BackgroundBrush->AspectRatio==ProjectGeometry) Index=2;
            if (ui->TextFramingStyleCB->currentIndex()!=Index) ui->TextFramingStyleCB->setCurrentIndex(Index);
            if (!ui->TextFramingStyleCB->updatesEnabled()) ui->TextFramingStyleCB->setUpdatesEnabled(true);

        } else {
            // It's Image or video or ...
            if (ui->TextFramingStyleCB->isVisible()) ui->TextFramingStyleCB->setVisible(false);
            if (!ui->FramingStyleCB->isVisible())    ui->FramingStyleCB->setVisible(true);

            if ((FramingCB_CurrentBrush!=CurrentCompoObject->BackgroundBrush)||(FramingCB_CurrentShot!=CurrentShotNbr)) {

                int Position=0;
                // Compute position of video
                if (ISVIDEO(CurrentCompoObject->BackgroundBrush)) {
                    for (int i=0;i<ShotTable->currentColumn();i++) for (int j=0;j<CurrentSlide->List[i]->ShotComposition.List.count();j++)
                        if (CurrentSlide->List[i]->ShotComposition.List[j]->IndexKey==CurrentCompoObject->IndexKey) {
                            if (CurrentSlide->List[i]->ShotComposition.List[j]->IsVisible) Position+=CurrentSlide->List[i]->StaticDuration;
                        }
                }

                QImage *CachedImage=NULL;
                if (CurrentCompoObject->BackgroundBrush->MediaObject) {
                    if (CurrentCompoObject->BackgroundBrush->MediaObject->ObjectType!=OBJECTTYPE_VIDEOFILE) CachedImage=CurrentCompoObject->BackgroundBrush->MediaObject->ImageAt(true);
                        else                                                                                CachedImage=((cVideoFile *)CurrentCompoObject->BackgroundBrush->MediaObject)->ImageAt(true,Position+QTime(0,0,0,0).msecsTo(((cVideoFile *)CurrentCompoObject->BackgroundBrush->MediaObject)->StartPos),NULL,CurrentCompoObject->BackgroundBrush->Deinterlace,1,false,false);
                }
                if ((CachedImage)&&(CachedImage->format()!=QImage::Format_ARGB32_Premultiplied)) {
                    QImage *NewCachedImage=new QImage(CachedImage->convertToFormat(QImage::Format_ARGB32_Premultiplied));
                    delete CachedImage;
                    CachedImage=NewCachedImage;
                }

                if (CachedImage) {
                    ui->FramingStyleCB->X=CurrentCompoObject->BackgroundBrush->X;
                    ui->FramingStyleCB->Y=CurrentCompoObject->BackgroundBrush->Y;
                    ui->FramingStyleCB->ZoomFactor=CurrentCompoObject->BackgroundBrush->ZoomFactor;
                    ui->FramingStyleCB->AspectRatio=CurrentCompoObject->BackgroundBrush->AspectRatio;
                    ui->FramingStyleCB->PrepareFramingStyleTable(true,/*FILTERFRAMING_ALL*/FILTERFRAMING_IMAGE|FILTERFRAMING_PROJECT,CurrentCompoObject->BackgroundBrush,CachedImage,CurrentCompoObject->BackgroundForm,ProjectGeometry);
                    delete CachedImage;
                }
            }
            ui->FramingStyleCB->SetCurrentFraming(CurrentCompoObject->BackgroundBrush->GetCurrentFramingStyle(ProjectGeometry));
        }
        ui->BackgroundFormCB->PrepareFrameShapeTable(true,0,CurrentCompoObject->BackgroundForm);
        ui->BackgroundFormCB->SetCurrentFrameShape(CurrentCompoObject->BackgroundForm);
        StopMajFramingStyle=false;

        FramingCB_CurrentBrush=CurrentCompoObject->BackgroundBrush;
        FramingCB_CurrentShot =CurrentShotNbr;

    } else {
        FramingCB_CurrentBrush=NULL;
        FramingCB_CurrentShot =CurrentShotNbr;
        ui->ShapeSizePosCB->clear();
        if (!ui->FramingStyleCB->isVisible())        ui->FramingStyleCB->setVisible(true);
        if (ui->TextFramingStyleCB->isVisible())     ui->TextFramingStyleCB->setVisible(false);
        if (!ui->TextFramingStyleLabel->isVisible()) ui->TextFramingStyleLabel->setVisible(false);
        ui->BlockShapeStyleED->setText("");
        ui->FramingStyleCB->clear();
        ui->BackgroundFormCB->clear();
    }

    InRefreshStyleControls=false;
}

//====================================================================================================================

void DlgSlideProperties::RefreshControls(bool UpdateInteractiveZone) {
    if ((InRefreshControls)||(!CompositionList)) return;
    InRefreshControls=true;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    for (int Shot=0;Shot<CurrentSlide->List.count();Shot++) for (int Block=0;Block<CurrentSlide->List[Shot]->ShotComposition.List.count();Block++)
        CurrentSlide->List[Shot]->ShotComposition.List[Block]->ComputeOptimisationFlags(Shot>0?CurrentSlide->List[Shot-1]->ShotComposition.List[Block]:NULL);

    //***********************
    // Tools button
    //***********************
    ui->EditBT->                setEnabled((BlockSelectMode==SELECTMODE_ONE));
    ui->ArrangeBT->             setEnabled((BlockSelectMode==SELECTMODE_ONE)||(BlockSelectMode==SELECTMODE_MULTIPLE));
    ui->InfoBlockBT->           setEnabled((BlockSelectMode==SELECTMODE_ONE)&&(CurrentCompoObject->BackgroundBrush->BrushType==BRUSHTYPE_IMAGEDISK));

    // actions
    ui->actionTop->             setEnabled((BlockSelectMode==SELECTMODE_MULTIPLE)&&(!SelectionHaveLockBlock));
    ui->actionMiddle->          setEnabled((BlockSelectMode==SELECTMODE_MULTIPLE)&&(!SelectionHaveLockBlock));
    ui->actionBottom->          setEnabled((BlockSelectMode==SELECTMODE_MULTIPLE)&&(!SelectionHaveLockBlock));
    ui->actionLeft->            setEnabled((BlockSelectMode==SELECTMODE_MULTIPLE)&&(!SelectionHaveLockBlock));
    ui->actionCenter->          setEnabled((BlockSelectMode==SELECTMODE_MULTIPLE)&&(!SelectionHaveLockBlock));
    ui->actionRight->           setEnabled((BlockSelectMode==SELECTMODE_MULTIPLE)&&(!SelectionHaveLockBlock));
    ui->actionDistributeHoriz-> setEnabled((NbrSelected>=3)&&(!SelectionHaveLockBlock));
    ui->actionDistributeVert->  setEnabled((NbrSelected>=3)&&(!SelectionHaveLockBlock));
    ui->actionSetVisible->      setEnabled((BlockSelectMode==SELECTMODE_ONE)&&(!SelectionHaveLockBlock));
    ui->actionSetHide->         setEnabled((BlockSelectMode==SELECTMODE_ONE)&&(!SelectionHaveLockBlock));
    ui->actionTakeSound->       setEnabled((BlockSelectMode==SELECTMODE_ONE)&&(!SelectionHaveLockBlock)&&(CurrentCompoObject->IsVisible) &&(ISVIDEO(CurrentCompoObject->BackgroundBrush))&&(CurrentCompoObject->BackgroundBrush->SoundVolume==0));
    ui->actionEditImage->       setEnabled((BlockSelectMode==SELECTMODE_ONE)&&(!SelectionHaveLockBlock)&&(CurrentCompoObject->IsVisible) &&(CurrentCompoObject->BackgroundBrush->BrushType==BRUSHTYPE_IMAGEDISK));
    ui->actionEditText->        setEnabled((BlockSelectMode==SELECTMODE_ONE)&&(!SelectionHaveLockBlock)&&(CurrentCompoObject->IsVisible));
    ui->actionInfo->            setEnabled((BlockSelectMode==SELECTMODE_ONE)&&(CurrentCompoObject->BackgroundBrush->BrushType==BRUSHTYPE_IMAGEDISK));
    ui->actionRemoveBlock->     setEnabled((BlockSelectMode==SELECTMODE_ONE)||(BlockSelectMode==SELECTMODE_MULTIPLE));
    ui->actionUpBlock->         setEnabled((BlockSelectMode==SELECTMODE_ONE)&&(!SelectionHaveLockBlock)&&(CurrentCompoObjectNbr>0));
    ui->actionDownBlock->       setEnabled((BlockSelectMode==SELECTMODE_ONE)&&(!SelectionHaveLockBlock)&&(CurrentCompoObjectNbr<BlockTable->rowCount()-1));
    ui->actionCopy->            setEnabled((BlockSelectMode==SELECTMODE_ONE)||(BlockSelectMode==SELECTMODE_MULTIPLE));
    ui->actionCut->             setEnabled((BlockSelectMode==SELECTMODE_ONE)||(BlockSelectMode==SELECTMODE_MULTIPLE));

    if ((CurrentCompoObject)&&(CurrentCompoObject->BackgroundBrush->MediaObject)&&(ui->actionEditImage->isEnabled())) {
        switch (CurrentCompoObject->BackgroundBrush->MediaObject->ObjectType) {
            case OBJECTTYPE_VIDEOFILE:
                ui->actionEditImage->setText(QApplication::translate("DlgSlideProperties","Correct, reframe or cut video","Action title in slide edit dialog + dialog title of image edit dialog"));
                ui->actionEditImage->setToolTip(QApplication::translate("DlgSlideProperties","Define framing and correction of image or cut video","Tooltip"));
                ui->actionEditImage->setIcon(QIcon(ICON_EDIT_MOVIE));
                break;
            case OBJECTTYPE_IMAGEFILE:
            case OBJECTTYPE_IMAGECLIPBOARD:
                ui->actionEditImage->setText(QApplication::translate("DlgSlideProperties","Correct or reframe image","Action title in slide edit dialog + dialog title of image edit dialog"));
                ui->actionEditImage->setToolTip(QApplication::translate("DlgSlideProperties","Define framing and correction of image","Tooltip"));
                ui->actionEditImage->setIcon(QIcon(ICON_EDIT_IMAGE));
                break;
            case OBJECTTYPE_IMAGEVECTOR:
                ui->actionEditImage->setText(QApplication::translate("DlgSlideProperties","Reframe image vector","Action title in slide edit dialog + dialog title of image edit dialog"));
                ui->actionEditImage->setToolTip(QApplication::translate("DlgSlideProperties","Define framing and correction of image vector","Tooltip"));
                ui->actionEditImage->setIcon(QIcon(ICON_EDIT_IMAGE));
                break;
            case OBJECTTYPE_GMAPSMAP:
                ui->actionEditImage->setText(QApplication::translate("DlgSlideProperties","Modify Google Maps map","Action title in slide edit dialog + dialog title of image edit dialog"));
                ui->actionEditImage->setToolTip(QApplication::translate("DlgSlideProperties","Define Modify Google Maps map and framing and correction of map","Tooltip"));
                ui->actionEditImage->setIcon(QIcon(ICON_EDIT_GMAPS));
                break;
            default:    break;  // avoid warning
        }
    }

    // Change icons for visible and getsound buttons/actions
    if (BlockSelectMode==SELECTMODE_ONE) ui->actionTakeSound->setIcon(QIcon(QString(CurrentCompoObject->BackgroundBrush->SoundVolume==0?ICON_SOUND_OK:ICON_SOUND_KO)));


    //**************************
    // Speed wave
    //**************************
    if ((BlockSelectMode==SELECTMODE_ONE)&&(CurrentCompoObject)&&(CurrentCompoObject->IsVisible)) {
        ui->SpeedWaveCB->setEnabled(!SelectionHaveLockBlock);
        ui->SpeedWaveLabel->setEnabled(!SelectionHaveLockBlock);
        ui->SpeedWaveCB->SetCurrentValue(CurrentCompoObject->BlockSpeedWave);
    } else {
        ui->SpeedWaveCB->setEnabled(false);
        ui->SpeedWaveLabel->setEnabled(false);
        ui->SpeedWaveCB->SetCurrentValue(SPEEDWAVE_PROJECTDEFAULT);
    }

    //**************************
    // Text animation controls
    //**************************
    if ((BlockSelectMode==SELECTMODE_ONE)&&(CurrentCompoObject)&&(CurrentCompoObject->IsVisible)) {
        ui->ZoomLabel->   setEnabled(!SelectionHaveLockBlock);         ui->ZoomSlider->   setEnabled(!SelectionHaveLockBlock);        ui->ZoomED->   setEnabled(!SelectionHaveLockBlock);        ui->ZoomResetBT->   setEnabled(!SelectionHaveLockBlock);
        ui->ScrollXLabel->setEnabled(!SelectionHaveLockBlock);         ui->ScrollXSlider->setEnabled(!SelectionHaveLockBlock);        ui->ScrollXED->setEnabled(!SelectionHaveLockBlock);        ui->ScrollXResetBT->setEnabled(!SelectionHaveLockBlock);
        ui->ScrollYLabel->setEnabled(!SelectionHaveLockBlock);         ui->ScrollYSlider->setEnabled(!SelectionHaveLockBlock);        ui->ScrollYED->setEnabled(!SelectionHaveLockBlock);        ui->ScrollYResetBT->setEnabled(!SelectionHaveLockBlock);

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
    if ((BlockSelectMode==SELECTMODE_ONE)&&(CurrentCompoObject)&&(CurrentCompoObject->IsVisible)) {
        ui->BlockAnimLabel->setEnabled(!SelectionHaveLockBlock);
        ui->BlockAnimCB->setEnabled(!SelectionHaveLockBlock);
        ui->BlockAnimCB->setCurrentIndex(CurrentCompoObject->BlockAnimType);
    } else {
        ui->BlockAnimLabel->setEnabled(false);
        ui->BlockAnimCB->setEnabled(false);
        ui->BlockAnimCB->setCurrentIndex(-1);
    }

    // Multiple turn animation
    if ((BlockSelectMode==SELECTMODE_ONE)&&(CurrentCompoObject)&&(CurrentCompoObject->IsVisible)&&(CurrentCompoObject->BlockAnimType==BLOCKANIMTYPE_MULTIPLETURN)) {
        ui->TurnZLabel->setEnabled(!SelectionHaveLockBlock);    ui->TurnZSlider->setEnabled(!SelectionHaveLockBlock);       ui->TurnZED->setEnabled(!SelectionHaveLockBlock);       ui->ResetTurnZBT->setEnabled(!SelectionHaveLockBlock);
        ui->TurnZLabel->setVisible(true);                       ui->TurnZSlider->setVisible(true);                          ui->TurnZED->setVisible(true);                          ui->ResetTurnZBT->setVisible(true);
        ui->TurnXLabel->setEnabled(!SelectionHaveLockBlock);    ui->TurnXSlider->setEnabled(!SelectionHaveLockBlock);       ui->TurnXED->setEnabled(!SelectionHaveLockBlock);       ui->ResetTurnXBT->setEnabled(!SelectionHaveLockBlock);
        ui->TurnXLabel->setVisible(true);                       ui->TurnXSlider->setVisible(true);                          ui->TurnXED->setVisible(true);                          ui->ResetTurnXBT->setVisible(true);
        ui->TurnYLabel->setEnabled(!SelectionHaveLockBlock);    ui->TurnYSlider->setEnabled(!SelectionHaveLockBlock);       ui->TurnYED->setEnabled(!SelectionHaveLockBlock);       ui->ResetTurnYBT->setEnabled(!SelectionHaveLockBlock);
        ui->TurnYLabel->setVisible(true);                       ui->TurnYSlider->setVisible(true);                          ui->TurnYED->setVisible(true);                          ui->ResetTurnYBT->setVisible(true);
        ui->TurnZED->setValue(CurrentCompoObject->TurnZAxis);                           ui->TurnZSlider->setValue(CurrentCompoObject->TurnZAxis);
        ui->TurnXED->setValue(CurrentCompoObject->TurnXAxis);                           ui->TurnXSlider->setValue(CurrentCompoObject->TurnXAxis);
        ui->TurnYED->setValue(CurrentCompoObject->TurnYAxis);                           ui->TurnYSlider->setValue(CurrentCompoObject->TurnYAxis);
    } else {
        ui->TurnZLabel->setVisible(false);      ui->TurnZSlider->setVisible(false);     ui->TurnZED->setVisible(false);     ui->ResetTurnZBT->setVisible(false);
        ui->TurnXLabel->setVisible(false);      ui->TurnXSlider->setVisible(false);     ui->TurnXED->setVisible(false);     ui->ResetTurnXBT->setVisible(false);
        ui->TurnYLabel->setVisible(false);      ui->TurnYSlider->setVisible(false);     ui->TurnYED->setVisible(false);     ui->ResetTurnYBT->setVisible(false);
    }

    // Dissolve animation
    if ((BlockSelectMode==SELECTMODE_ONE)&&(CurrentCompoObject)&&(CurrentCompoObject->IsVisible)&&(CurrentCompoObject->BlockAnimType==BLOCKANIMTYPE_DISSOLVE)) {
        ui->DissolveLabel->setEnabled(!SelectionHaveLockBlock);      ui->DissolveLabel->setVisible(true);
        ui->DissolveCB->   setEnabled(!SelectionHaveLockBlock);      ui->DissolveCB->   setVisible(true);
        ui->DissolveCB->setCurrentIndex(CurrentCompoObject->Dissolve);
    } else {
        ui->DissolveLabel->setEnabled(false);     ui->DissolveLabel->setVisible(false);
        ui->DissolveCB->   setEnabled(false);     ui->DissolveCB->   setVisible(false);
    }

    RefreshStyleControls();

    QApplication::restoreOverrideCursor();
    InRefreshControls=false;

    cShotComposer::RefreshControls(UpdateInteractiveZone);
}

//====================================================================================================================
// Refresh background image of the scene

void DlgSlideProperties::s_RefreshSceneImage() {
    InteractiveZone->RefreshDisplay();
}

//====================================================================================================================
// Display or hide rulers
//====================================================================================================================

void DlgSlideProperties::s_RulersBt() {
    DlgRulerDef Dlg(&InteractiveZone->MagneticRuler,true,ApplicationConfig,this);
    Dlg.InitDialog();
    connect(&Dlg,SIGNAL(RefreshDisplay()),this,SLOT(s_RefreshSceneImage()));
    if (Dlg.exec()==0) {
        ApplicationConfig->SlideRuler=InteractiveZone->MagneticRuler;
        ui->RulersBT->setIcon(QIcon(QString(InteractiveZone->MagneticRuler!=0?ICON_RULER_ON:ICON_RULER_OFF)));
    }
    InteractiveZone->RefreshDisplay();
}

//====================================================================================================================

cCompositionObject *DlgSlideProperties::GetSelectedGlobalCompositionObject() {
    int CurrentBlock=BlockTable->currentRow();
    if ((CurrentBlock<0)||(CurrentBlock>=CompositionList->List.count())) return NULL;
    return GetGlobalCompositionObject(CompositionList->List[CurrentBlock]->IndexKey);
}

//====================================================================================================================

cCompositionObject *DlgSlideProperties::GetSelectedCompositionObject() {
    int CurrentBlock=BlockTable->currentRow();
    if ((CurrentBlock<0)||(CurrentBlock>=CompositionList->List.count())) return NULL; else return CompositionList->List[CurrentBlock];
}

//====================================================================================================================
// SLIDE SETTINGS
//====================================================================================================================

void DlgSlideProperties::s_SlideSet_ChapterInformation() {
    AppendPartialUndo(UNDOACTION_BLOCKTABLE_EDITCHAPTERINFO,InteractiveZone,true);
    DlgChapter Dlg(CurrentSlide,ApplicationConfig,this);
    Dlg.InitDialog();
    if (Dlg.exec()!=0) RemoveLastPartialUndo(); else CurrentSlide->Parent->UpdateChapterInformation();
    RefreshControls(true);
}

//===================================================================================

void DlgSlideProperties::s_SlideSet_SlideNameChange(QString NewText) {
    if (InRefreshControls) return;  // No action if in control setup
    AppendPartialUndo(UNDOACTION_EDITZONE_SLIDENAME,ui->SlideNameED,false);

    CurrentSlide->SlideName=NewText;
}

//===================================================================================

void DlgSlideProperties::s_SlideSet_SaveModel() {
    QMenu *ContextMenu=new QMenu(this);
    ContextMenu->addAction(ui->actionSaveAsProjectTitleModel);
    ContextMenu->addAction(ui->actionSaveAsCptTitleModels);
    ContextMenu->addAction(ui->actionSaveAsCreditTitleModels);
    ContextMenu->exec(QCursor::pos());
    delete ContextMenu;
    ui->SaveModelBT->setDown(false);
}

//===================================================================================

void DlgSlideProperties::s_SlideSet_SaveAsProjectTitleModel() {
    CurrentSlide->SaveAsNewCustomModelFile(ffd_MODELTYPE_PROJECTTITLE);
    CustomMessageBox(this,QMessageBox::Information,ui->actionSaveAsProjectTitleModel->text(),
                     QApplication::translate("DlgSlideProperties","Slide added in Project title models collection","Information message"),QMessageBox::Close);
}

//===================================================================================

void DlgSlideProperties::s_SlideSet_SaveAsCptTitleModels() {
    CurrentSlide->SaveAsNewCustomModelFile(ffd_MODELTYPE_CHAPTERTITLE);
    CustomMessageBox(this,QMessageBox::Information,ui->actionSaveAsCptTitleModels->text(),
                     QApplication::translate("DlgSlideProperties","Slide added in Chapter title models collection","Information message"),QMessageBox::Close);
}

//===================================================================================

void DlgSlideProperties::s_SlideSet_SaveAsCreditTitleModels() {
    CurrentSlide->SaveAsNewCustomModelFile(ffd_MODELTYPE_CREDITTITLE);
    CustomMessageBox(this,QMessageBox::Information,ui->actionSaveAsCreditTitleModels->text(),
                     QApplication::translate("DlgSlideProperties","Slide added in Credit title models collection","Information message"),QMessageBox::Close);
}

//====================================================================================================================
// SHOT SETTINGS
//====================================================================================================================

void DlgSlideProperties::s_ShotTable_DurationChange(QTime NewValue) {
    if ((InDisplayDuration)||(CurrentShot==NULL)) return;  // No action if in control setup or if no shot was selected
    AppendPartialUndo(UNDOACTION_EDITZONE_SHOTDURATION,ui->ShotDurationED,false);

    CurrentShot->StaticDuration=QTime(0,0,0,0).msecsTo(NewValue);
    s_ShotTable_DisplayDuration();
}

//===================================================================================

void DlgSlideProperties::s_ShotTable_AddShot() {
    if ((InRefreshControls)||(CurrentShot==NULL)) return;  // No action if in control setup or if no shot was selected
    AppendPartialUndo(UNDOACTION_SHOTTABLE_ADDSHOT,ShotTable,true);

    // Insert a new empty shot after current shot in the current slide
    CurrentSlide->List.insert(CurrentShotNbr+1,new cDiaporamaShot(CurrentSlide));

    // Fill this new shot with copy of all blocks of current shot
    cDiaporamaShot *imagesequence=CurrentSlide->List[CurrentShotNbr+1];
    for (int i=0;i<CompositionList->List.count();i++) {
        imagesequence->ShotComposition.List.append(new cCompositionObject(COMPOSITIONTYPE_SHOT,CompositionList->List[i]->IndexKey,ApplicationConfig,&imagesequence->ShotComposition));
        imagesequence->ShotComposition.List[i]->CopyFromCompositionObject(CompositionList->List[i]);
    }

    ShotTable->setUpdatesEnabled(false);
    ShotTable->insertColumn(CurrentShotNbr+1);
    ShotTable->setColumnWidth(CurrentShotNbr+1,CurrentSlide->Parent->GetWidthForHeight(ShotTable->rowHeight(0)));

    // Change current shot to this new shot
    ShotTable->SetCurrentCell(CurrentShotNbr+1);
    ShotTable->setUpdatesEnabled(true);
}

//===================================================================================

void DlgSlideProperties::s_ShotTable_RemoveShot() {
    if (CurrentSlide->List.count()<2) return;               // Don't remove last shot
    if ((InRefreshControls)||(CurrentShot==NULL)) return;   // No action if in control setup or if no shot was selected
    if ((ApplicationConfig->AskUserToRemove)&&(CustomMessageBox(this,QMessageBox::Question,QApplication::translate("DlgSlideProperties","Remove shot"),QApplication::translate("DlgSlideProperties","Are you sure you want to delete this shot?"),
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)==QMessageBox::No)) return;

    AppendPartialUndo(UNDOACTION_SHOTTABLE_REMOVESHOT,ShotTable,true);

    InRefreshControls=true;

    // Remove shot from slide
    delete CurrentSlide->List.takeAt(CurrentShotNbr);

    // Remove shot from shot table
    int ShotNbr=CurrentShotNbr;
    ShotTable->setUpdatesEnabled(false);
    ShotTable->removeColumn(ShotNbr);
    if (ShotNbr>=CurrentSlide->List.count()) ShotNbr--;
    ShotTable->SetCurrentCell(ShotNbr);
    ShotTable->setUpdatesEnabled(true);

    InRefreshControls=false;
    s_ShotTable_SelectionChanged();
}

//===================================================================================

void DlgSlideProperties::s_ShotTable_DragMoveItem() {
    AppendPartialUndo(UNDOACTION_SHOTTABLE_CHORDER,ShotTable,true);

    int Src=ShotTable->DragItemSource;
    int Dst=ShotTable->DragItemDest;
    if ((Src<0)||(Src>=CurrentSlide->List.count())||(Dst<0)||(Dst>CurrentSlide->List.count())) return;

    cDiaporamaShot *ObjSrc=CurrentSlide->List.takeAt(Src);
    if (Src<Dst) Dst--;
    CurrentSlide->List.insert(Dst,ObjSrc);

    ShotTable->setUpdatesEnabled(false);
    ShotTable->SetCurrentCell(Dst);
    ShotTable->setUpdatesEnabled(true);  // Reset timeline painting
}

//===================================================================================

void DlgSlideProperties::s_ShotTable_MoveLeft() {
    if ((InRefreshControls)||(CurrentShot==NULL)) return;  // No action if in control setup or if no shot was selected
    AppendPartialUndo(UNDOACTION_SHOTTABLE_CHORDER,ShotTable,true);

    if (CurrentShotNbr>0) {
        CurrentSlide->List.swap(CurrentShotNbr,CurrentShotNbr-1);
        ShotTable->setUpdatesEnabled(false);
        ShotTable->SetCurrentCell(CurrentShotNbr-1);
        ShotTable->setUpdatesEnabled(true);  // Reset timeline painting
    }
}

//===================================================================================

void DlgSlideProperties::s_ShotTable_MoveRight() {
    if ((InRefreshControls)||(CurrentShot==NULL)) return;  // No action if in control setup or if no shot was selected
    AppendPartialUndo(UNDOACTION_SHOTTABLE_CHORDER,ShotTable,true);

    if (CurrentShotNbr<CurrentSlide->List.count()-1) {
        CurrentSlide->List.swap(CurrentShotNbr+1,CurrentShotNbr);
        ShotTable->setUpdatesEnabled(false);
        ShotTable->SetCurrentCell(CurrentShotNbr+1);
        ShotTable->setUpdatesEnabled(true);  // Reset timeline painting
    }
}

//===================================================================================

void DlgSlideProperties::s_ShotTable_SelectionChanged() {
    if (InRefreshControls) return;  // No action if in control setup or if no shot was selected

    // Keep link to current shot
    CurrentShotNbr=ShotTable->currentColumn();
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
    int i       =BlockTable->currentRow();
    int IndexKey=-1;
    if ((CompositionList!=NULL)&&(i>=0)&&(i<CompositionList->List.count())) IndexKey=CompositionList->List[i]->IndexKey;

    int CurrentBlockTableIndex=0;
    if (IndexKey!=-1) {
        while ((CurrentBlockTableIndex<CompositionList->List.count())&&(CompositionList->List[CurrentBlockTableIndex]->IndexKey!=IndexKey)) CurrentBlockTableIndex++;
        if (CurrentBlockTableIndex>=CompositionList->List.count()) CurrentBlockTableIndex=0;
    }

    BlockTable->CompositionList     =CompositionList;
    BlockTable->CurrentShotNbr      =CurrentShotNbr;
    InteractiveZone->CurrentShotNbr =CurrentShotNbr;

    ui->actionMoveLeft->setEnabled(CurrentShotNbr>0);
    ui->actionMoveRight->setEnabled(CurrentShotNbr<ShotTable->columnCount()-1);
    ui->RemoveShot->setEnabled(CurrentSlide->List.count()>1);
    ui->actionRemoveShot->setEnabled(CurrentSlide->List.count()>1);

    s_ShotTable_DisplayDuration();
    RefreshBlockTable(CurrentBlockTableIndex);
}

//===================================================================================

void DlgSlideProperties::s_ShotTable_DisplayDuration() {
    // duration and shot duration
    InDisplayDuration=true;
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
    if (AddingDuration==0)  ui->MinShotDurationLabel->setText(""); else {
        int         qDuration=CurrentShot->StaticDuration+AddingDuration;
        int         msec     =qDuration % 1000;          qDuration=qDuration/1000;
        int         sec      =qDuration % 60;            qDuration=qDuration/60;
        int         mn       =qDuration % 60;            qDuration=qDuration/60;
        int         hours    =qDuration % 24;
        int         days     =qDuration / 24;
        QString     Duration;
        if (days>0) Duration=QString("%1.%2:%3:%4.%5").arg(ito2a(days)).arg(ito2a(hours)).arg(ito2a(mn)).arg(ito2a(sec)).arg(ito3a(msec));
            else    Duration=QString("%1:%2:%3.%4").arg(ito2a(hours)).arg(ito2a(mn)).arg(ito2a(sec)).arg(ito3a(msec));
        ui->MinShotDurationLabel->setText(QString(QApplication::translate("DlgSlideProperties","Lengthened to %1")).arg(Duration));
    }

    InDisplayDuration=false;
    for (int i=CurrentShotNbr;i<CurrentSlide->List.count();i++) ShotTable->RepaintCell(i);
    if (CurrentShotNbr!=CurrentSlide->List.count()-1) ShotTable->RepaintCell(CurrentSlide->List.count()-1);
}

//===================================================================================

void DlgSlideProperties::s_ShotTable_RightClickEvent(QMouseEvent *) {
    QMenu *ContextMenu=new QMenu(this);
    ContextMenu->addAction(ui->actionAddShot);
    ContextMenu->addAction(ui->actionRemoveShot);
    ContextMenu->addAction(ui->actionMoveLeft);
    ContextMenu->addAction(ui->actionMoveRight);
    ContextMenu->exec(QCursor::pos());
    delete ContextMenu;
}

//====================================================================================================================
// BLOCK TABLE SETTINGS
//====================================================================================================================

void DlgSlideProperties::s_BlockTable_StartSelectionChange() {
    InSelectionChange=true;
}

void DlgSlideProperties::s_BlockTable_EndSelectionChange() {
    InSelectionChange=false;
    s_BlockTable_SelectionChanged();
}

//====================================================================================================================
// User double click on a block in the BlockTable widget or in the scene

void DlgSlideProperties::s_BlockTable_ItemDoubleClicked(QMouseEvent *) {
    if ((BlockSelectMode!=SELECTMODE_ONE)||(SelectionHaveLockBlock)) return;
    if (CurrentCompoObject->BackgroundBrush->BrushType!=BRUSHTYPE_IMAGEDISK)    s_BlockSettings_TextEditor();
        else                                                                    s_BlockSettings_ImageEditCorrect();
}

//====================================================================================================================

void DlgSlideProperties::s_BlockSettings_Arrange() {
    QMenu *ContextMenu=new QMenu(this);
    ContextMenu->addAction(ui->actionTop);
    ContextMenu->addAction(ui->actionMiddle);
    ContextMenu->addAction(ui->actionBottom);
    ContextMenu->addSeparator();
    ContextMenu->addAction(ui->actionLeft);
    ContextMenu->addAction(ui->actionCenter);
    ContextMenu->addAction(ui->actionRight);
    ContextMenu->addSeparator();
    ContextMenu->addAction(ui->actionDistributeHoriz);
    ContextMenu->addAction(ui->actionDistributeVert);
    ContextMenu->addSeparator();
    ContextMenu->addAction(ui->actionUpBlock);
    ContextMenu->addAction(ui->actionDownBlock);
    ContextMenu->exec(QCursor::pos());
    delete ContextMenu;
    ui->ArrangeBT->setDown(false);
}

//====================================================================================================================

void DlgSlideProperties::s_BlockSettings_Edit() {
    QMenu *ContextMenu=new QMenu(this);
    ContextMenu->addAction(ui->actionCut);
    ContextMenu->addAction(ui->actionCopy);
    ContextMenu->addAction(ui->actionPaste);
    ContextMenu->addSeparator();
    ContextMenu->addAction(ui->actionEditText);
    if (ui->actionEditImage->isEnabled()) ContextMenu->addAction(ui->actionEditImage);
    ContextMenu->addSeparator();
    ContextMenu->addAction(ui->actionRemoveBlock);
    if (NbrSelected==1) {   // Single selection
        ContextMenu->addSeparator();
        if (CurrentCompoObject->IsVisible) ContextMenu->addAction(ui->actionSetHide);
            else ContextMenu->addAction(ui->actionSetVisible);
        if (CurrentShotNbr) {
            if (CurrentCompoObject->SameAsPrevShot) ContextMenu->addAction(ui->actionUnlockSameAsPreviousShot);
                else ContextMenu->addAction(ui->actionSameAsPreviousShot);
        }
        if (ISVIDEO(CurrentCompoObject->BackgroundBrush)) ContextMenu->addAction(ui->actionTakeSound);
    }
    ContextMenu->exec(QCursor::pos());
    delete ContextMenu;
    ui->EditBT->setDown(false);
}

//====================================================================================================================
// User right click on a block in the BlockTable widget or in the scene

void DlgSlideProperties::s_BlockTable_ItemRightClicked(QMouseEvent *) {
    if (BlockSelectMode==SELECTMODE_NONE) {
        QMenu *ContextMenu=new QMenu(this);
        ContextMenu->addAction(ui->actionCut);
        ContextMenu->addAction(ui->actionCopy);
        ContextMenu->addAction(ui->actionPaste);
        ContextMenu->addSeparator();
        ContextMenu->addAction(ui->actionAddTextBlock);
        ContextMenu->addAction(ui->actionAddFile);
        ContextMenu->addAction(ui->actionAddImageClipboard);
        ContextMenu->addAction(ui->actionAddGMapsMap);
        ContextMenu->exec(QCursor::pos());
        delete ContextMenu;
    } else if (BlockSelectMode==SELECTMODE_ONE) {
        QMenu *ContextMenu=new QMenu(this);
        ContextMenu->addAction(ui->actionCut);
        ContextMenu->addAction(ui->actionCopy);
        ContextMenu->addAction(ui->actionPaste);
        ContextMenu->addSeparator();
        ContextMenu->addAction(ui->actionEditText);
        if (ui->actionEditImage->isEnabled()) ContextMenu->addAction(ui->actionEditImage);
        ContextMenu->addSeparator();
        ContextMenu->addAction(ui->actionRemoveBlock);
        ContextMenu->addSeparator();
        if (CurrentCompoObject->IsVisible) ContextMenu->addAction(ui->actionSetHide);
            else ContextMenu->addAction(ui->actionSetVisible);
        if (CurrentShotNbr>0) {
            if (CurrentCompoObject->SameAsPrevShot) ContextMenu->addAction(ui->actionUnlockSameAsPreviousShot);
            else ContextMenu->addAction(ui->actionSameAsPreviousShot);
        }
        if (ISVIDEO(CurrentCompoObject->BackgroundBrush)) ContextMenu->addAction(ui->actionTakeSound);
        ContextMenu->addSeparator();
        ContextMenu->addAction(ui->actionInfo);
        ContextMenu->addSeparator();
        ContextMenu->addAction(ui->actionUpBlock);
        ContextMenu->addAction(ui->actionDownBlock);
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
        ContextMenu->addAction(ui->actionDistributeHoriz);
        ContextMenu->addAction(ui->actionDistributeVert);
        ContextMenu->addSeparator();
        ContextMenu->addAction(ui->actionCut);
        ContextMenu->addAction(ui->actionCopy);
        ContextMenu->addAction(ui->actionPaste);
        ContextMenu->addSeparator();
        ContextMenu->addAction(ui->actionRemoveBlock);
        if (CurrentShotNbr>0) {
            ContextMenu->addSeparator();
            ContextMenu->addAction(ui->actionSameAsPreviousShot);
            ContextMenu->addAction(ui->actionUnlockSameAsPreviousShot);
        }
        ContextMenu->exec(QCursor::pos());
        delete ContextMenu;
    }
}

//====================================================================================================================

void DlgSlideProperties::s_BlockTable_AddNewTextBlock() {
    AppendPartialUndo(UNDOACTION_BLOCKTABLE_ADDTEXTBLOCK,BlockTable,true);

    QMenu *ContextMenu=new QMenu(this);
    ContextMenu->addAction(ui->actionAddSimpleTextBlock);
    ContextMenu->addAction(ui->actionAddClipArtTextBlock);
    ContextMenu->exec(QCursor::pos());
    delete ContextMenu;
    ui->AddTextBlock->setDown(false);
}

//====================================================================================================================

void DlgSlideProperties::s_BlockTable_AddNewSimpleTextBlock() {
    int CurrentShotNbr=ShotTable->currentColumn();

    // Create and append a composition block to the object list
    CurrentSlide->ObjectComposition.List.append(new cCompositionObject(COMPOSITIONTYPE_OBJECT,CurrentSlide->NextIndexKey,ApplicationConfig,&CurrentSlide->ObjectComposition));
    cCompositionObject *CompositionObject=CurrentSlide->ObjectComposition.List[CurrentSlide->ObjectComposition.List.count()-1];

    // Apply Styles
    CompositionObject->ApplyTextStyle(ApplicationConfig->StyleTextCollection.GetStyleDef(ApplicationConfig->StyleTextCollection.DecodeString(ApplicationConfig->DefaultBlock_Text_TextST)));
    CompositionObject->ApplyBackgroundStyle(ApplicationConfig->StyleTextBackgroundCollection.GetStyleDef(ApplicationConfig->StyleTextBackgroundCollection.DecodeString(ApplicationConfig->DefaultBlock_Text_BackGST)));
    CompositionObject->ApplyBlockShapeStyle(ApplicationConfig->StyleBlockShapeCollection.GetStyleDef(ApplicationConfig->StyleBlockShapeCollection.DecodeString(ApplicationConfig->DefaultBlock_Text_ShapeST)));
    CompositionObject->BackgroundBrush->LockGeometry=false; // For ApplyAutoCompoSize don't use it
    CompositionObject->ApplyAutoCompoSize(ApplicationConfig->DefaultBlock_AutoSizePos,CurrentSlide->Parent->ImageGeometry);
    CompositionObject->BackgroundBrush->LockGeometry=(ApplicationConfig->DefaultBlock_AutoLocking==AUTOFRAMING_CUSTOMPRJLOCK);
    CompositionObject->BackgroundBrush->AspectRatio=(CompositionObject->h*(CurrentSlide->Parent->ImageGeometry==GEOMETRY_4_3?1440:CurrentSlide->Parent->ImageGeometry==GEOMETRY_16_9?1080:CurrentSlide->Parent->ImageGeometry==GEOMETRY_40_17?816:1920))/(CompositionObject->w*1920);

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
        CurrentSlide->List[i]->ShotComposition.List.append(new cCompositionObject(COMPOSITIONTYPE_SHOT,CompositionObject->IndexKey,ApplicationConfig,&CurrentSlide->List[i]->ShotComposition));
        CurrentSlide->List[i]->ShotComposition.List[CurrentSlide->List[i]->ShotComposition.List.count()-1]->CopyFromCompositionObject(CompositionObject);
        // Ensure new object is not visible in previous shot
        if (i<CurrentShotNbr) CurrentSlide->List[i]->ShotComposition.List[CurrentSlide->List[i]->ShotComposition.List.count()-1]->IsVisible=false;
    }

    // Inc NextIndexKey
    CurrentSlide->NextIndexKey++;

    RefreshBlockTable(CompositionList->List.count()-1);
    NoPrepUndo=true;
    QTimer::singleShot(250,this,SLOT(s_BlockSettings_TextEditor()));    // Append "Open text editor" to the message queue
}

//====================================================================================================================

void DlgSlideProperties::s_BlockTable_AddNewClipArtTextBlock() {
    cPopupTexteFrame* popup1 = new cPopupTexteFrame(this);
    QPoint Pos=mapToGlobal(ui->AddTextBlock->pos());
    popup1->DoInitPopup(QPoint(Pos.x()+ui->AddTextBlock->width(),Pos.y()+ui->AddTextBlock->height()));
    popup1->exec();
    QString RessourceName=popup1->GetCurrentTextFrame();
    if (RessourceName=="") return;

    int CurrentShotNbr=ShotTable->currentColumn();

    // Create and append a composition block to the object list
    CurrentSlide->ObjectComposition.List.append(new cCompositionObject(COMPOSITIONTYPE_OBJECT,CurrentSlide->NextIndexKey,ApplicationConfig,&CurrentSlide->ObjectComposition));
    cCompositionObject *CompositionObject=CurrentSlide->ObjectComposition.List[CurrentSlide->ObjectComposition.List.count()-1];

    // Apply Styles
    CompositionObject->ApplyTextStyle(TextFrameList.List[TextFrameList.SearchImage(RessourceName)].TextStyle);
    CompositionObject->BackgroundBrush->LockGeometry=false; // For ApplyAutoCompoSize don't use it
    CompositionObject->ApplyAutoCompoSize(ApplicationConfig->DefaultBlock_AutoSizePos,CurrentSlide->Parent->ImageGeometry);
    CompositionObject->BackgroundBrush->LockGeometry=(ApplicationConfig->DefaultBlock_AutoLocking==AUTOFRAMING_CUSTOMPRJLOCK);
    CompositionObject->BackgroundBrush->AspectRatio =(CompositionObject->h*(CurrentSlide->Parent->ImageGeometry==GEOMETRY_4_3?1440:CurrentSlide->Parent->ImageGeometry==GEOMETRY_16_9?1080:CurrentSlide->Parent->ImageGeometry==GEOMETRY_40_17?816:1920))/(CompositionObject->w*1920);
    CompositionObject->TextClipArtName              =RessourceName;
    CompositionObject->TMx                          =TextFrameList.List[TextFrameList.SearchImage(RessourceName)].TMx;
    CompositionObject->TMy                          =TextFrameList.List[TextFrameList.SearchImage(RessourceName)].TMy;
    CompositionObject->TMw                          =TextFrameList.List[TextFrameList.SearchImage(RessourceName)].TMw;
    CompositionObject->TMh                          =TextFrameList.List[TextFrameList.SearchImage(RessourceName)].TMh;

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
        CurrentSlide->List[i]->ShotComposition.List.append(new cCompositionObject(COMPOSITIONTYPE_SHOT,CompositionObject->IndexKey,ApplicationConfig,&CurrentSlide->List[i]->ShotComposition));
        CurrentSlide->List[i]->ShotComposition.List[CurrentSlide->List[i]->ShotComposition.List.count()-1]->CopyFromCompositionObject(CompositionObject);
        // Ensure new object is not visible in previous shot
        if (i<CurrentShotNbr) CurrentSlide->List[i]->ShotComposition.List[CurrentSlide->List[i]->ShotComposition.List.count()-1]->IsVisible=false;
    }

    // Inc NextIndexKey
    CurrentSlide->NextIndexKey++;

    RefreshBlockTable(CompositionList->List.count()-1);
    NoPrepUndo=true;
    QTimer::singleShot(250,this,SLOT(s_BlockSettings_TextEditor()));    // Append "Open text editor" to the message queue
}

//====================================================================================================================

void DlgSlideProperties::s_BlockTable_DragDropFiles(QList<QUrl> urlList) {
    if (urlList.count()>0) {
        QString     fName;
        QFileInfo   info;
        QStringList FileList;
        for (int i=0;i<urlList.count();i++) {
            fName = urlList[i].toLocalFile();           // convert first QUrl to local path
            info.setFile(fName);                        // information about file
            if (info.isFile()) FileList.append(fName);  // append file
        }
        if (FileList.count()>0) {
            if (BlockTable->DragItemDest<0)                          BlockTable->DragItemDest=0;
            if (BlockTable->DragItemDest>BlockTable->rowCount()) BlockTable->DragItemDest=BlockTable->rowCount();
            s_BlockTable_AddFilesBlock(FileList,BlockTable->DragItemDest);
        }
    }
}

//====================================================================================================================

void DlgSlideProperties::s_BlockTable_AddNewFileBlock() {
    ui->AddFileBlock->setDown(false);
    QStringList FileList;
    DlgFileExplorer Dlg(BROWSER_TYPE_MEDIAFILES,true,false,true,QApplication::translate("MainWindow","Add files"),ApplicationConfig,this);
    Dlg.InitDialog();
    if (Dlg.exec()==0) FileList=Dlg.GetCurrentSelectedFiles();
    if (FileList.count()==0) return;

    QApplication::processEvents();
    s_BlockTable_AddFilesBlock(FileList,BlockTable->rowCount());
}

//====================================================================================================================

void DlgSlideProperties::s_BlockTable_AddFilesBlock(QStringList FileList,int PositionToInsert) {
    AppendPartialUndo(UNDOACTION_BLOCKTABLE_ADDFILEBLOCK,BlockTable,true);

    // Add files
    for (int i=0;i<FileList.count();i++) {
        QString         BrushFileName=QFileInfo(FileList[i]).absoluteFilePath();
        QString         Extension    =QFileInfo(BrushFileName).suffix().toLower();
        cBaseMediaFile  *MediaObject =NULL;

        if (ApplicationConfig->AllowImageExtension.contains(Extension))                 MediaObject=new cImageFile(ApplicationConfig);
            else if (ApplicationConfig->AllowImageVectorExtension.contains(Extension))  MediaObject=new cImageFile(ApplicationConfig);
            else if (ApplicationConfig->AllowVideoExtension.contains(Extension))        MediaObject=new cVideoFile(ApplicationConfig);

        if ((MediaObject)&&(MediaObject->GetInformationFromFile(BrushFileName,NULL,NULL,-1)&&(MediaObject->CheckFormatValide(this)))) {
            if (MediaObject->ObjectType==OBJECTTYPE_VIDEOFILE) {
                ((cVideoFile *)MediaObject)->EndPos=((cVideoFile *)MediaObject)->Duration;
                if (((cVideoFile *)MediaObject)->LibavStartTime>0) ((cVideoFile *)MediaObject)->StartPos=QTime(0,0,0,0).addMSecs(int64_t((double(((cVideoFile *)MediaObject)->LibavStartTime)/AV_TIME_BASE)*1000));
            }
            DoAddBlock(MediaObject,PositionToInsert);
            PositionToInsert++;

        } else {
            delete CurrentSlide->ObjectComposition.List.takeAt(PositionToInsert);
            CustomMessageBox(this,QMessageBox::Critical,QApplication::translate("MainWindow","Error","Error message"),FileList[i]+"\n\n"+QApplication::translate("MainWindow","Format not supported","Error message"),QMessageBox::Close);
        }
    }
    RefreshBlockTable(PositionToInsert-1);
}

//====================================================================================================================

void DlgSlideProperties::DoAddBlock(cBaseMediaFile *MediaObject,int PositionToInsert) {
    AppendPartialUndo(UNDOACTION_BLOCKTABLE_ADDFILEBLOCK,BlockTable,true);

    CurrentSlide->ObjectComposition.List.insert(PositionToInsert,new cCompositionObject(COMPOSITIONTYPE_OBJECT,CurrentSlide->NextIndexKey,ApplicationConfig,&CurrentSlide->ObjectComposition));
    cCompositionObject  *CompositionObject=CurrentSlide->ObjectComposition.List[PositionToInsert];
    cBrushDefinition    *CurrentBrush     =CompositionObject->BackgroundBrush;

    CompositionObject->Text     ="";
    CompositionObject->PenSize  =0;
    CurrentBrush->BrushType     =BRUSHTYPE_IMAGEDISK;

    // Create an cImageClipboard wrapper
    CurrentBrush->MediaObject=MediaObject;

    // Apply Styles
    CompositionObject->ApplyTextStyle(ApplicationConfig->StyleTextCollection.GetStyleDef(ApplicationConfig->StyleTextCollection.DecodeString(ApplicationConfig->DefaultBlockBA_IMG_TextST)));
    CompositionObject->ApplyBlockShapeStyle(ApplicationConfig->StyleBlockShapeCollection.GetStyleDef(ApplicationConfig->StyleBlockShapeCollection.DecodeString(ApplicationConfig->DefaultBlockBA_IMG_ShapeST)));

    // Apply styles for coordinates
    CurrentBrush->ApplyAutoFraming(ApplicationConfig->DefaultBlockBA[CurrentBrush->GetImageType()].AutoFraming,ProjectGeometry);

    if (MediaObject->ObjectType==OBJECTTYPE_IMAGEVECTOR) {
        CompositionObject->ApplyAutoCompoSize(AUTOCOMPOSIZE_REALSIZE,CurrentSlide->Parent->ImageGeometry);
        // adjust for image was not too small !
        if ((CompositionObject->w<0.2)&&(CompositionObject->h<0.2)) {
            while ((CompositionObject->w<0.2)&&(CompositionObject->h<0.2)) {
                CompositionObject->w=CompositionObject->w*2;
                CompositionObject->h=CompositionObject->h*2;
            }
            CompositionObject->x=(1-CompositionObject->w)/2;
            CompositionObject->y=(1-CompositionObject->h)/2;
        }
    } else CompositionObject->ApplyAutoCompoSize(ApplicationConfig->DefaultBlockBA[CurrentBrush->GetImageType()].AutoCompo,CurrentSlide->Parent->ImageGeometry);

    // Now create and append a shot composition block to all shot
    for (int i=0;i<CurrentSlide->List.count();i++) {
        CurrentSlide->List[i]->ShotComposition.List.insert(PositionToInsert,new cCompositionObject(COMPOSITIONTYPE_SHOT,CompositionObject->IndexKey,ApplicationConfig,&CurrentSlide->List[i]->ShotComposition));
        CurrentSlide->List[i]->ShotComposition.List[PositionToInsert]->CopyFromCompositionObject(CompositionObject);
        // Ensure new object is not visible in previous shot
        if (i<CurrentShotNbr) CurrentSlide->List[i]->ShotComposition.List[PositionToInsert]->IsVisible=false;
    }

    // If this object is a video will gain sound from this shots !
    if (MediaObject->ObjectType==OBJECTTYPE_VIDEOFILE) for (int k=0;k<CurrentSlide->List.count();k++) for (int l=0;l<CurrentSlide->List[k]->ShotComposition.List.count();l++)
        if (ISVIDEO(CurrentSlide->List[k]->ShotComposition.List[l]->BackgroundBrush)) {
            if (k<CurrentShotNbr) {
                // Set this new block to SoundVolume=0
                if (CurrentBrush->MediaObject->FileKey==CurrentSlide->List[k]->ShotComposition.List[l]->BackgroundBrush->MediaObject->FileKey)
                    CurrentSlide->List[k]->ShotComposition.List[l]->BackgroundBrush->SoundVolume=0;
            } else {
                // Set all other block to SoundVolume=0 and this block to SoundVolume=1
                if (CurrentBrush->MediaObject->FileKey!=CurrentSlide->List[k]->ShotComposition.List[l]->BackgroundBrush->MediaObject->FileKey)
                    CurrentSlide->List[k]->ShotComposition.List[l]->BackgroundBrush->SoundVolume=0;
                else CurrentSlide->List[k]->ShotComposition.List[l]->BackgroundBrush->SoundVolume=1;
            }
    }

    // Inc NextIndexKey
    CurrentSlide->NextIndexKey++;
}

//====================================================================================================================

void DlgSlideProperties::s_BlockTable_AddSpecialBlock() {
    QMenu *ContextMenu=new QMenu(this);
    ContextMenu->addAction(ui->actionAddImageClipboard);
    ContextMenu->addAction(ui->actionAddGMapsMap);
    ContextMenu->exec(QCursor::pos());
    delete ContextMenu;
    ui->AddSpecialBlock->setDown(false);
}

//====================================================================================================================

void DlgSlideProperties::s_BlockTable_AddGMapsMapBlock() {
    // Create an GMapsMap wrapper
    cGMapsMap   *MediaObject=new cGMapsMap(ApplicationConfig);
    MediaObject->CreatDateTime=QDateTime().currentDateTime();
    MediaObject->CreateDefaultImage();   // create default image
    MediaObject->GetInformationFromFile("",NULL,NULL,-1);
    DoAddBlock(MediaObject,CurrentSlide->ObjectComposition.List.count());
    RefreshBlockTable(CurrentSlide->ObjectComposition.List.count()-1);
    RedoneStyleCoordinate=true;
    QTimer::singleShot(250,this,SLOT(s_BlockSettings_ImageEditCorrect()));    // Append "Open image editor" to the message queue
}

//====================================================================================================================

void DlgSlideProperties::s_BlockTable_AddImageClipboard() {
    const QMimeData *SlideData=QApplication::clipboard()->mimeData();
    if (!SlideData->hasImage()) return;
    QImage ImageClipboard=qvariant_cast<QImage>(SlideData->imageData());
    if (ImageClipboard.isNull()) return;
    cImageClipboard *MediaObject=new cImageClipboard(ApplicationConfig);
    MediaObject->CreatDateTime=QDateTime().currentDateTime();
    ApplicationConfig->SlideThumbsTable->SetThumbs(&MediaObject->RessourceKey,ImageClipboard);
    MediaObject->GetInformationFromFile("",NULL,NULL,-1);
    DoAddBlock(MediaObject,BlockTable->rowCount());
    RefreshBlockTable(BlockTable->rowCount()-1);
}

//====================================================================================================================

void DlgSlideProperties::s_BlockTable_RemoveBlock() {
    AppendPartialUndo(UNDOACTION_BLOCKTABLE_REMOVEBLOCK,BlockTable,true);

    if (BlockSelectMode==SELECTMODE_ONE) {
        if ((ApplicationConfig->AskUserToRemove)&&(CustomMessageBox(this,QMessageBox::Question,QApplication::translate("DlgSlideProperties","Remove block"),QApplication::translate("DlgSlideProperties","Are you sure you want to delete this block?"),
                                  QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)==QMessageBox::No)) return;
    } else if (BlockSelectMode==SELECTMODE_MULTIPLE) {
        if ((ApplicationConfig->AskUserToRemove)&&(CustomMessageBox(this,QMessageBox::Question,QApplication::translate("DlgSlideProperties","Remove blocks"),QApplication::translate("DlgSlideProperties","Are you sure you want to delete these blocks?"),
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
    BlockTable->clearSelection();
}

//====================================================================================================================

void DlgSlideProperties::s_BlockTable_Cut() {
    if ((BlockSelectMode!=SELECTMODE_ONE)&&(BlockSelectMode!=SELECTMODE_MULTIPLE)) return;
    s_BlockTable_Copy();
    s_BlockTable_RemoveBlock();
}

//====================================================================================================================

void DlgSlideProperties::s_BlockTable_Copy() {
    if ((BlockSelectMode!=SELECTMODE_ONE)&&(BlockSelectMode!=SELECTMODE_MULTIPLE)) return;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    // Create xml document and root
    QDomDocument Object  =QDomDocument(APPLICATION_NAME);
    QDomElement  root    =Object.createElement("CLIPBOARD");
    int          BlockNum=0;
    for (int i=0;i<CompositionList->List.count();i++) if (IsSelected[i]) {
        QDomElement         Element=Object.createElement(QString("Block-%1").arg(BlockNum));
        cCompositionObject  *GlobalBlock=GetGlobalCompositionObject(CompositionList->List[i]->IndexKey);
        GlobalBlock->SaveToXML(Element,"CLIPBOARD-BLOCK-GLOBAL",CurrentSlide->Parent->ProjectFileName,true,true,NULL,NULL);                // Save global object
        CompositionList->List[i]->SaveToXML(Element,"CLIPBOARD-BLOCK-SHOT",CurrentSlide->Parent->ProjectFileName,true,true,NULL,NULL);     // Save shot object
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
    const QMimeData *SlideData=QApplication::clipboard()->mimeData();
    if (SlideData->hasImage()) s_BlockTable_AddImageClipboard(); else
    if (SlideData->hasFormat("ffDiaporama/block")) {

        AppendPartialUndo(UNDOACTION_BLOCKTABLE_PASTEBLOCK,BlockTable,true);
        QDomDocument Object=QDomDocument(APPLICATION_NAME);
        Object.setContent(SlideData->data("ffDiaporama/block"));
        if ((Object.elementsByTagName("CLIPBOARD").length()>0)&&(Object.elementsByTagName("CLIPBOARD").item(0).isElement()==true)) {

            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

            QDomElement root=Object.elementsByTagName("CLIPBOARD").item(0).toElement();
            int BlockNbr=root.attribute("BlockNbr").toInt();

            for (int BlockNum=0;BlockNum<BlockNbr;BlockNum++) if ((root.elementsByTagName(QString("Block-%1").arg(BlockNum)).length()>0)&&(root.elementsByTagName(QString("Block-%1").arg(BlockNum)).item(0).isElement()==true)) {
                QDomElement Element=root.elementsByTagName(QString("Block-%1").arg(BlockNum)).item(0).toElement();
                // Create and append a composition block to the object list
                CurrentSlide->ObjectComposition.List.append(new cCompositionObject(COMPOSITIONTYPE_OBJECT,CurrentSlide->NextIndexKey,ApplicationConfig,&CurrentSlide->ObjectComposition));
                cCompositionObject *GlobalBlock=CurrentSlide->ObjectComposition.List[CurrentSlide->ObjectComposition.List.count()-1];
                GlobalBlock->LoadFromXML(Element,"CLIPBOARD-BLOCK-GLOBAL","",NULL,NULL,true,NULL,true);
                GlobalBlock->IndexKey=CurrentSlide->NextIndexKey;

                cCompositionObject ShotBlock(COMPOSITIONTYPE_SHOT,CurrentSlide->NextIndexKey,ApplicationConfig,this);
                ShotBlock.LoadFromXML(Element,"CLIPBOARD-BLOCK-SHOT","",NULL,NULL,true,NULL,true);

                ShotBlock.IndexKey=CurrentSlide->NextIndexKey;
                ShotBlock.BackgroundBrush->MediaObject=GlobalBlock->BackgroundBrush->MediaObject;
                ShotBlock.BackgroundBrush->DeleteMediaObject=false;
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
                    CurrentSlide->List[i]->ShotComposition.List.append(new cCompositionObject(COMPOSITIONTYPE_SHOT,CurrentSlide->NextIndexKey,ApplicationConfig,&CurrentSlide->List[i]->ShotComposition));
                    CurrentSlide->List[i]->ShotComposition.List[CurrentSlide->List[i]->ShotComposition.List.count()-1]->CopyFromCompositionObject(&ShotBlock);
                    // Ensure new object is not visible in previous shot
                    if (i<CurrentShotNbr) {
                        CurrentSlide->List[i]->ShotComposition.List[CurrentSlide->List[i]->ShotComposition.List.count()-1]->IsVisible=false;
                        // Ensure unvisible video have no sound !
                        if (ISVIDEO(CurrentSlide->List[i]->ShotComposition.List[CurrentSlide->List[i]->ShotComposition.List.count()-1]->BackgroundBrush))
                            CurrentSlide->List[i]->ShotComposition.List[CurrentSlide->List[i]->ShotComposition.List.count()-1]->BackgroundBrush->SoundVolume=0;
                    }
                }
                // Inc NextIndexKey
                CurrentSlide->NextIndexKey++;
            }

            // Refresh shot table
            ShotTable->setUpdatesEnabled(false);
            ShotTable->setUpdatesEnabled(true);

            // Refresh block table
            RefreshBlockTable(CompositionList->List.count()-1);

            // Select blocks we just added
            s_BlockTable_StartSelectionChange();
            BlockTable->clearSelection();
            for (int i=0;i<BlockNbr;i++) BlockTable->setCurrentCell(CompositionList->List.count()-1-i,0,i==0?QItemSelectionModel::Select|QItemSelectionModel::Current:QItemSelectionModel::Select);
            s_BlockTable_EndSelectionChange();

            QApplication::restoreOverrideCursor();
        }
    }
}

//====================================================================================================================

void DlgSlideProperties::s_BlockTable_MoveBlockUp() {
    AppendPartialUndo(UNDOACTION_BLOCKTABLE_CHBLOCKORDER,BlockTable,true);
    s_BlockTable_SelectionChanged(); // Refresh selection
    if (BlockSelectMode!=SELECTMODE_ONE) return;
    if (CurrentCompoObjectNbr<1) return;
    CompositionList->List.swap(CurrentCompoObjectNbr,CurrentCompoObjectNbr-1);
    RefreshBlockTable(CurrentCompoObjectNbr-1);
}

//====================================================================================================================

void DlgSlideProperties::s_BlockTable_MoveBlockDown() {
    AppendPartialUndo(UNDOACTION_BLOCKTABLE_CHBLOCKORDER,BlockTable,true);
    s_BlockTable_SelectionChanged(); // Refresh selection
    if (BlockSelectMode!=SELECTMODE_ONE) return;
    if (CurrentCompoObjectNbr>=CompositionList->List.count()-1) return;
    CompositionList->List.swap(CurrentCompoObjectNbr+1,CurrentCompoObjectNbr);
    RefreshBlockTable(CurrentCompoObjectNbr+1);
}

//====================================================================================================================

void DlgSlideProperties::s_BlockTable_DragMoveBlock(int Src,int Dst) {
    if (Src>=CompositionList->List.count()) return;
    AppendPartialUndo(UNDOACTION_BLOCKTABLE_CHBLOCKORDER,BlockTable,true);
    if (Src<Dst) Dst--;
    CompositionList->List.insert(Dst,CompositionList->List.takeAt(Src));
    RefreshBlockTable(Dst);
}

//********************************************************************************************************************
//                                                  BLOCKS ALIGNMENT
//********************************************************************************************************************

void DlgSlideProperties::s_BlockTable_AlignTop() {
    AppendPartialUndo(UNDOACTION_BLOCKTABLE_ARRANGEBLOCK,InteractiveZone,true);
    for (int i=0;i<IsSelected.count();i++) if (IsSelected[i]) CompositionList->List[i]->y=InteractiveZone->Sel_Y;
    RefreshControls(true);
}

void DlgSlideProperties::s_BlockTable_AlignMiddle() {
    AppendPartialUndo(UNDOACTION_BLOCKTABLE_ARRANGEBLOCK,InteractiveZone,true);
    for (int i=0;i<IsSelected.count();i++) if (IsSelected[i]) CompositionList->List[i]->y=(InteractiveZone->Sel_Y+InteractiveZone->Sel_H/2)-CompositionList->List[i]->h/2;
    RefreshControls(true);
}

void DlgSlideProperties::s_BlockTable_AlignBottom() {
    AppendPartialUndo(UNDOACTION_BLOCKTABLE_ARRANGEBLOCK,InteractiveZone,true);
    for (int i=0;i<IsSelected.count();i++) if (IsSelected[i]) CompositionList->List[i]->y=(InteractiveZone->Sel_Y+InteractiveZone->Sel_H)-CompositionList->List[i]->h;
    RefreshControls(true);
}

void DlgSlideProperties::s_BlockTable_AlignLeft() {
    AppendPartialUndo(UNDOACTION_BLOCKTABLE_ARRANGEBLOCK,InteractiveZone,true);
    for (int i=0;i<IsSelected.count();i++) if (IsSelected[i]) CompositionList->List[i]->x=InteractiveZone->Sel_X;
    RefreshControls(true);
}

void DlgSlideProperties::s_BlockTable_AlignCenter() {
    AppendPartialUndo(UNDOACTION_BLOCKTABLE_ARRANGEBLOCK,InteractiveZone,true);
    for (int i=0;i<IsSelected.count();i++) if (IsSelected[i]) CompositionList->List[i]->x=(InteractiveZone->Sel_X+InteractiveZone->Sel_W/2)-CompositionList->List[i]->w/2;
    RefreshControls(true);
}

void DlgSlideProperties::s_BlockTable_AlignRight() {
    AppendPartialUndo(UNDOACTION_BLOCKTABLE_ARRANGEBLOCK,InteractiveZone,true);
    for (int i=0;i<IsSelected.count();i++) if (IsSelected[i]) CompositionList->List[i]->x=(InteractiveZone->Sel_X+InteractiveZone->Sel_W)-CompositionList->List[i]->w;
    RefreshControls(true);
}

void DlgSlideProperties::s_BlockTable_DistributeHoriz() {
    AppendPartialUndo(UNDOACTION_BLOCKTABLE_ARRANGEBLOCK,InteractiveZone,true);

    // 1st step : compute available space and create list
    QList<SortBlock> List;
    qreal           SpaceW   =InteractiveZone->Sel_W;
    qreal           CurrentX =InteractiveZone->Sel_X;
    for (int i=0;i<IsSelected.count();i++) if (IsSelected[i]) {
        SpaceW=SpaceW-CompositionList->List[i]->w;
        List.append(MakeSortBlock(i,CompositionList->List[i]->x));
    }
    SpaceW=SpaceW/qreal(List.count()-1);

    // 2nd step : sort blocks
    for (int i=0;i<List.count();i++)
        for (int j=0;j<List.count()-1;j++)
            if (List[j].Position>List[j+1].Position) List.swap(j,j+1);

    // Last step : move blocks
    for (int i=0;i<List.count();i++) {
        CompositionList->List[List[i].Index]->x=CurrentX;
        CurrentX=CurrentX+CompositionList->List[List[i].Index]->w+SpaceW;
    }

    RefreshControls(true);
}

void DlgSlideProperties::s_BlockTable_DistributeVert() {
    AppendPartialUndo(UNDOACTION_BLOCKTABLE_ARRANGEBLOCK,InteractiveZone,true);

    // 1st step : compute available space and create list
    QList<SortBlock> List;
    qreal   SpaceH   =InteractiveZone->Sel_H;
    qreal   CurrentY =InteractiveZone->Sel_Y;
    for (int i=0;i<IsSelected.count();i++) if (IsSelected[i]) {
        SpaceH=SpaceH-CompositionList->List[i]->h;
        List.append(MakeSortBlock(i,CompositionList->List[i]->y));
    }
    SpaceH=SpaceH/qreal(List.count()-1);

    // 2nd step : sort blocks
    for (int i=0;i<List.count();i++)
        for (int j=0;j<List.count()-1;j++)
            if (List[j].Position>List[j+1].Position) List.swap(j,j+1);

    // Last step : move blocks
    for (int i=0;i<List.count();i++) {
        CompositionList->List[List[i].Index]->y=CurrentY;
        CurrentY=CurrentY+CompositionList->List[List[i].Index]->h+SpaceH;
    }

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
    if (!ISBLOCKVALIDEVISIBLE()) return;
    if (!NoPrepUndo) AppendPartialUndo(UNDOACTION_BLOCKTABLE_EDITTEXT,InteractiveZone,true);
    NoPrepUndo=false;

    InteractiveZone->DisplayMode=cInteractiveZone::DisplayMode_TextMargin;
    InteractiveZone->RefreshDisplay();
    DlgTextEdit Dlg(CurrentSlide->Parent,CurrentCompoObject,ApplicationConfig,&ApplicationConfig->StyleTextCollection,&ApplicationConfig->StyleTextBackgroundCollection,this);
    Dlg.InitDialog();
    connect(&Dlg,SIGNAL(RefreshDisplay()),this,SLOT(s_RefreshSceneImage()));
    if (Dlg.exec()==0) {
        InteractiveZone->DisplayMode=cInteractiveZone::DisplayMode_BlockShape;
        ApplyToContexte(true);
        int CurrentRow=BlockTable->currentRow();
        RefreshBlockTable(CurrentRow>0?CurrentRow:0);
    } else {
        InteractiveZone->DisplayMode=cInteractiveZone::DisplayMode_BlockShape;
        RemoveLastPartialUndo();
        RefreshControls();
    }
}

//========= Open s_BlockSettings_Information dialog
void DlgSlideProperties::s_BlockSettings_Information() {
    if ((InRefreshControls)||(BlockSelectMode!=SELECTMODE_ONE)||(!CurrentCompoObject)) return;

    DlgInfoFile Dlg(CurrentCompoObject->BackgroundBrush->MediaObject,ApplicationConfig,this);
    Dlg.InitDialog();
    Dlg.exec();
}

//========= Open image/video correction editor
void DlgSlideProperties::s_BlockSettings_ImageEditCorrect() {
    if ((InRefreshControls)||(BlockSelectMode!=SELECTMODE_ONE)||(!CurrentCompoObject)||(!CurrentCompoObject->IsVisible)||(!ui->actionEditImage->isEnabled()))
        return;

    AppendPartialUndo(UNDOACTION_BLOCKTABLE_EDITIMAGE,InteractiveZone,true);
    cBrushDefinition *CurrentBrush=CurrentCompoObject->BackgroundBrush;

    int Position=0;
    // Compute position of video
    if (ISVIDEO(CurrentBrush)) {
        Position=QTime(0,0,0,0).msecsTo(((cVideoFile*)&CurrentBrush->MediaObject)->StartPos);
        for (int i=0;i<ShotTable->currentColumn();i++) for (int j=0;j<CurrentSlide->List[i]->ShotComposition.List.count();j++)
          if ((CurrentSlide->List[i]->ShotComposition.List[j]->IndexKey==CurrentCompoObject->IndexKey)&&
              (CurrentSlide->List[i]->ShotComposition.List[j]->IsVisible)
             ) Position+=CurrentSlide->List[i]->StaticDuration;
    }

    QString FileName    =CurrentBrush->MediaObject->ShortName();
    bool UpdateSlideName=(CurrentSlide->SlideName==FileName);

    DlgImageCorrection Dlg(CurrentCompoObject,&CurrentCompoObject->BackgroundForm,CurrentCompoObject->BackgroundBrush,Position,
                           CurrentSlide->Parent->ImageGeometry,CurrentSlide->Parent->ImageAnimSpeedWave,ApplicationConfig,this);
    Dlg.InitDialog();
    if (Dlg.exec()==0) {

        // Redone application of styles for coordinates (specific to GMaps object when creating)
        if (RedoneStyleCoordinate) {
            CurrentCompoObject->ApplyAutoCompoSize(ApplicationConfig->DefaultBlockBA[CurrentCompoObject->BackgroundBrush->GetImageType()].AutoCompo,CurrentSlide->Parent->ImageGeometry);
            RedoneStyleCoordinate=false;
        }

        FramingCB_CurrentBrush   =NULL; // To force a refresh of ui->FramingCB !
        CurrentBrush->AspectRatio=CurrentBrush->AspectRatio;
        CurrentCompoObject->h    =(CurrentCompoObject->w*InteractiveZone->DisplayW*CurrentBrush->AspectRatio)/InteractiveZone->DisplayH;
        if ((CurrentShotNbr==0)&&(CurrentSlide->ThumbnailKey!=-1)) ApplicationConfig->SlideThumbsTable->ClearThumbs(CurrentSlide->ThumbnailKey);

        // Adjust height and width to image stay in screen
        if (((CurrentCompoObject->y+CurrentCompoObject->h)*InteractiveZone->DisplayH)>InteractiveZone->DisplayH) {
            CurrentCompoObject->h=1-CurrentCompoObject->y;
            CurrentCompoObject->w=((CurrentCompoObject->h*InteractiveZone->DisplayH)/CurrentBrush->AspectRatio)/InteractiveZone->DisplayW;
        }

        // if Slide name is name of this file
        if (UpdateSlideName) {
            CurrentSlide->SlideName=CurrentBrush->MediaObject->ShortName();
            ui->SlideNameED->setText(CurrentSlide->SlideName);
        }

        // Lulo object for image and video must be remove
        ApplicationConfig->ImagesCache.RemoveImageObject(CurrentCompoObject->BackgroundBrush->MediaObject->RessourceKey,CurrentCompoObject->BackgroundBrush->MediaObject->FileKey);

        // Apply settings to other shots
        ApplyToContexte(true,CurrentCompoObject->BackgroundBrush->MediaObject->ObjectType==OBJECTTYPE_GMAPSMAP);

        s_ShotTable_DisplayDuration();
        RefreshBlockTable(CurrentCompoObjectNbr);
    } else {
        RemoveLastPartialUndo();
    }
}

//========= Toggle visible state

void DlgSlideProperties::s_BlockSettings_ToggleVisibleState() {
    if (!ISBLOCKVALIDE()) return;
    AppendPartialUndo(UNDOACTION_BLOCKTABLE_VISIBLESTATE,InteractiveZone,true);

    CurrentCompoObject->IsVisible=!CurrentCompoObject->IsVisible;

    // Special case for video : ensure only this video have sound
    if (ISVIDEO(CurrentCompoObject->BackgroundBrush)) {
        if (!CurrentCompoObject->IsVisible) {
            CurrentCompoObject->BackgroundBrush->SoundVolume=0;
        } else {
            bool SomeOneHaveSound=false;
            // Parse table to know if a block have sound for this shot
            for (int i=0;i<CompositionList->List.count();i++)
                if ((ISVIDEO(CompositionList->List[i]->BackgroundBrush))&&(CompositionList->List[i]->BackgroundBrush->SoundVolume!=0)) SomeOneHaveSound=true;
            // If no block have sound => get sound to this video
            if (!SomeOneHaveSound) CurrentCompoObject->BackgroundBrush->SoundVolume=1;
        }
    }
    RefreshBlockTable(CurrentCompoObjectNbr);
    InteractiveZone->repaint();
}

//========= Set "Same As Previous Shot" state

void DlgSlideProperties::s_BlockSettings_SetSameAsPreviousShot() {
    if ((InRefreshControls)||(CurrentShotNbr==0)) return;
    AppendPartialUndo(UNDOACTION_BLOCKTABLE_SAMEASPREVIOUSSTATE,InteractiveZone,true);

    for (int Block=0;Block<IsSelected.count();Block++) if (IsSelected[Block] && (CompositionList->List[Block]->SameAsPrevShot==false)) {
        CompositionList->List[Block]->SameAsPrevShot=true;
        // Apply values of previous shot to all shot
        cCompositionObject *PreviousObject=NULL;
        for (int i=0;i<CurrentSlide->List[CurrentShotNbr-1]->ShotComposition.List.count();i++)
            if (CurrentSlide->List[CurrentShotNbr-1]->ShotComposition.List[i]->IndexKey==CompositionList->List[Block]->IndexKey) PreviousObject=CurrentSlide->List[CurrentShotNbr-1]->ShotComposition.List[i];

        int ShotNum=CurrentShotNbr;
        while (ShotNum<CurrentSlide->List.count())  {
            cCompositionObject *ShotObject=NULL;
            for (int i=0;i<CurrentSlide->List[ShotNum]->ShotComposition.List.count();i++) if (CurrentSlide->List[ShotNum]->ShotComposition.List[i]->IndexKey==CompositionList->List[Block]->IndexKey) ShotObject=CurrentSlide->List[ShotNum]->ShotComposition.List[i];
            if ((ShotObject!=NULL)&&(ShotObject->SameAsPrevShot)) {
                ShotObject->CopyFromCompositionObject(PreviousObject);
                ShotTable->RepaintCell(ShotNum);
                ShotNum++;
            } else ShotNum=CurrentSlide->List.count(); // Stop loop
        }
    }
    RefreshBlockTable(CurrentCompoObjectNbr);
    InteractiveZone->repaint();
}

//========= Unset "Same As Previous Shot" state

void DlgSlideProperties::s_BlockSettings_UnsetSameAsPreviousShot() {
    if ((InRefreshControls)||(CurrentShotNbr==0)) return;
    AppendPartialUndo(UNDOACTION_BLOCKTABLE_SAMEASPREVIOUSSTATE,InteractiveZone,true);

    for (int Block=0;Block<IsSelected.count();Block++) if (IsSelected[Block] && (CompositionList->List[Block]->SameAsPrevShot==true))
        CompositionList->List[Block]->SameAsPrevShot=false;

    RefreshBlockTable(CurrentCompoObjectNbr);
    InteractiveZone->repaint();
}

//========= Get sound

void DlgSlideProperties::s_BlockSettings_GetSound() {
    if (!ISBLOCKVALIDEVISIBLE()) return;
    AppendPartialUndo(UNDOACTION_BLOCKTABLE_SOUNDSTATE,InteractiveZone,true);

    // Only if this block is a video and don't have sound yet
    if (CurrentCompoObject->BackgroundBrush->SoundVolume==0) {
        for (int i=0;i<CompositionList->List.count();i++)
            if ((CurrentCompoObject!=CompositionList->List[i])&&(ISVIDEO(CompositionList->List[i]->BackgroundBrush))) CompositionList->List[i]->BackgroundBrush->SoundVolume=0;
        CurrentCompoObject->BackgroundBrush->SoundVolume=1;
        RefreshBlockTable(CurrentCompoObjectNbr);
    }
}

//====================================================================================================================
// Handler for position, size & rotation controls
//====================================================================================================================

//========= ShapeSizePos combo
void DlgSlideProperties::s_BlockSettings_ShapeSizePos(int Index) {
    if (!ISBLOCKVALIDEVISIBLE()) return;
    if ((Index<0)||(Index>=ui->ShapeSizePosCB->count())) return;
    AppendPartialUndo(UNDOACTION_STYLE_COORDINATES,ui->ShapeSizePosCB,true);
    CurrentCompoObject->ApplyAutoCompoSize(ui->ShapeSizePosCB->itemData(Index).toInt(),CurrentSlide->Parent->ImageGeometry);
    ApplyToContexte(false);
}

//====================================================================================================================
// Text annimation
//====================================================================================================================

//========= Text Zoom Level
void DlgSlideProperties::s_BlockSettings_TextAnimZoom(int Value) {
    if (!ISBLOCKVALIDEVISIBLE()) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_TEXTANIMZOOM,ui->ZoomED,false);
    CurrentCompoObject->TxtZoomLevel=Value;
    ui->ZoomSlider->setValue(CurrentCompoObject->TxtZoomLevel);
    ui->ZoomED->setValue(CurrentCompoObject->TxtZoomLevel);
    ApplyToContexte(false);
}

//========= Text scrolling X
void DlgSlideProperties::s_BlockSettings_TextAnimScrollX(int Value) {
    if (!ISBLOCKVALIDEVISIBLE()) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_TEXTANIMSCROLLX,ui->ScrollXED,false);
    CurrentCompoObject->TxtScrollX=Value;
    ui->ScrollXSlider->setValue(CurrentCompoObject->TxtScrollX);
    ui->ScrollXED->setValue(CurrentCompoObject->TxtScrollX);
    ApplyToContexte(false);
}

//========= Text scrolling Y
void DlgSlideProperties::s_BlockSettings_TextAnimScrollY(int Value) {
    if (!ISBLOCKVALIDEVISIBLE()) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_TEXTANIMSCROLLY,ui->ScrollYED,false);
    CurrentCompoObject->TxtScrollY=Value;
    ui->ScrollYSlider->setValue(CurrentCompoObject->TxtScrollY);
    ui->ScrollYED->setValue(CurrentCompoObject->TxtScrollY);
    ApplyToContexte(false);
}

//====================================================================================================================
// Handler for speed wave
//====================================================================================================================

void DlgSlideProperties::s_BlockSettings_SpeedWave(int) {
    if (!ISBLOCKVALIDEVISIBLE()) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_BLOCKSPEEDWAVE,ui->BlockAnimCB,false);
    CurrentCompoObject->BlockSpeedWave=ui->SpeedWaveCB->GetCurrentValue();
    ApplyToContexte(false);
}

//====================================================================================================================
// Handler for block animation
//====================================================================================================================

void DlgSlideProperties::s_BlockSettings_BlockAnimType(int Value) {
    if (!ISBLOCKVALIDEVISIBLE()) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_BLOCKANIMTYPE,ui->BlockAnimCB,false);
    CurrentCompoObject->BlockAnimType=Value;
    ApplyToContexte(false);
}

//========= Multiple block turn X value
void DlgSlideProperties::s_BlockSettings_BlockAnimTurnXValue(int Value) {
    if (!ISBLOCKVALIDEVISIBLE()) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_BLOCKANIMTURNX,ui->TurnXED,false);
    CurrentCompoObject->TurnXAxis=Value;
    ApplyToContexte(false);
}

//========= Multiple block turn Z value
void DlgSlideProperties::s_BlockSettings_BlockAnimTurnZValue(int Value) {
    if (!ISBLOCKVALIDEVISIBLE()) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_BLOCKANIMTURNZ,ui->TurnZED,false);
    CurrentCompoObject->TurnZAxis=Value;
    ApplyToContexte(false);
}

//========= Multiple block turn Y value
void DlgSlideProperties::s_BlockSettings_BlockAnimTurnYValue(int Value) {
    if (!ISBLOCKVALIDEVISIBLE()) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_BLOCKANIMTURNY,ui->TurnYED,false);
    CurrentCompoObject->TurnYAxis=Value;
    ApplyToContexte(false);
}

//========= Dissolve animation
void DlgSlideProperties::s_BlockSettings_BlockAnimDissolveType(int Value) {
    if (!ISBLOCKVALIDEVISIBLE()) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_BLOCKANIMDISSOLVE,ui->DissolveCB,false);
    CurrentCompoObject->Dissolve=Value;
    ApplyToContexte(false);
}

//====================================================================================================================
// Handler for interactive zone
//====================================================================================================================

void DlgSlideProperties::s_BlockSettings_IntZoneTransformBlocks(qreal Move_X,qreal Move_Y,qreal Scale_X,qreal Scale_Y,qreal RSel_X,qreal RSel_Y,qreal RSel_W,qreal RSel_H) {
    AppendPartialUndo(UNDOACTION_INTERACTIVEMOVERESIZE,InteractiveZone,true);

    for (int i=0;i<IsSelected.count();i++) if ((IsSelected[i])&&(CompositionList->List[i]->IsVisible)) {
        qreal   RatioScale_X=(RSel_W+Scale_X)/RSel_W;
        qreal   RatioScale_Y=(RSel_H+Scale_Y)/RSel_H;

        CompositionList->List[i]->x=RSel_X+Move_X+(CompositionList->List[i]->x-RSel_X)*RatioScale_X;
        CompositionList->List[i]->y=RSel_Y+Move_Y+(CompositionList->List[i]->y-RSel_Y)*RatioScale_Y;
        CompositionList->List[i]->w=CompositionList->List[i]->w*RatioScale_X;
        if (CompositionList->List[i]->w<0.002) CompositionList->List[i]->w=0.002;
        if (CompositionList->List[i]->BackgroundBrush->LockGeometry) CompositionList->List[i]->h=((CompositionList->List[i]->w*InteractiveZone->DisplayW)*CompositionList->List[i]->BackgroundBrush->AspectRatio)/InteractiveZone->DisplayH;
            else CompositionList->List[i]->h=CompositionList->List[i]->h*RatioScale_Y;
        if (CompositionList->List[i]->h<0.002) CompositionList->List[i]->h=0.002;
    }
    ApplyToContexte(false);
}

void DlgSlideProperties::s_BlockSettings_IntZoneDisplayTransformBlocks(qreal Move_X,qreal Move_Y,qreal Scale_X,qreal Scale_Y,qreal RSel_X,qreal RSel_Y,qreal RSel_W,qreal RSel_H) {
    InRefreshControls=true;

    int     i           =CurrentCompoObjectNbr;
    qreal   RatioScale_X=(RSel_W+Scale_X)/RSel_W;
    qreal   RatioScale_Y=(RSel_H+Scale_Y)/RSel_H;
    QRectF  tmpRect     =PolygonToRectF(ComputePolygon(CompositionList->List[i]->BackgroundForm,
                                                       CompositionList->List[i]->x*InteractiveZone->DisplayW,CompositionList->List[i]->y*InteractiveZone->DisplayH,
                                                       CompositionList->List[i]->w*InteractiveZone->DisplayW,CompositionList->List[i]->h*InteractiveZone->DisplayH));
    qreal   Ratio_X     =(CompositionList->List[i]->x*InteractiveZone->DisplayW)/tmpRect.width();
    qreal   Ratio_Y     =(CompositionList->List[i]->h*InteractiveZone->DisplayH)/tmpRect.height();
    qreal   x           =RSel_X+Move_X+(CompositionList->List[i]->x-RSel_X)*RatioScale_X;
    qreal   y           =RSel_Y+Move_Y+(CompositionList->List[i]->y-RSel_Y)*RatioScale_Y;
    qreal   w           =CompositionList->List[i]->w*RatioScale_X; if (w<0.002) w=0.002;
    qreal   h           =(CompositionList->List[i]->BackgroundBrush->LockGeometry?((w*InteractiveZone->DisplayW)*CompositionList->List[i]->BackgroundBrush->AspectRatio)/InteractiveZone->DisplayH:CompositionList->List[i]->h*RatioScale_Y); if (h<0.002) h=0.002;

    if (ApplicationConfig->DisplayUnit==DISPLAYUNIT_PERCENT) {
        ui->PosXEd->  setValue(x*100/Ratio_X);
        ui->PosYEd->  setValue(y*100/Ratio_Y);
        ui->WidthEd-> setValue(w*100/Ratio_X);
        ui->HeightEd->setValue(h*100/Ratio_Y);
    } else { // DisplayUnit==DISPLAYUNIT_PIXELS
        ui->PosXEd->  setValue(x*InteractiveZone->DisplayW/Ratio_X);
        ui->PosYEd->  setValue(y*InteractiveZone->DisplayH/Ratio_Y);
        ui->WidthEd-> setValue(w*InteractiveZone->DisplayW/Ratio_X);
        ui->HeightEd->setValue(h*InteractiveZone->DisplayH/Ratio_Y);
    }
    InRefreshControls=false;
}

//====================================================================================================================

void DlgSlideProperties::s_ChangeFramingStyle() {
    if (StopMajFramingStyle) return;
    if (!ISBLOCKVALIDEVISIBLE()) return;
    AppendPartialUndo(UNDOACTION_STYLE_FRAMING,InteractiveZone,false);

    cBrushDefinition *CurrentBrush=CurrentCompoObject->BackgroundBrush;
    int AutoCompo=CurrentCompoObject->GetAutoCompoSize(CurrentSlide->Parent->ImageGeometry);

    CurrentBrush->ApplyAutoFraming(ui->FramingStyleCB->GetCurrentFraming(),ProjectGeometry);
    CurrentCompoObject->ApplyAutoCompoSize(AutoCompo,CurrentSlide->Parent->ImageGeometry,false);
    //if (CurrentCompoObject->h>(CurrentCompoObject->w*InteractiveZone->DisplayW*CurrentBrush->AspectRatio)/InteractiveZone->DisplayH) CurrentCompoObject->h=(CurrentCompoObject->w*InteractiveZone->DisplayW*CurrentBrush->AspectRatio)/InteractiveZone->DisplayH;
    //    else CurrentCompoObject->w=(CurrentCompoObject->h*InteractiveZone->DisplayH/CurrentBrush->AspectRatio)/InteractiveZone->DisplayW;


    int CurrentRow=BlockTable->currentRow();
    FramingCB_CurrentBrush=NULL;
    RefreshBlockTable(CurrentRow>0?CurrentRow:0);
}
//====================================================================================================================

void DlgSlideProperties::s_ChangeTextFramingStyle(int Value) {
    if (StopMajFramingStyle) return;
    if (!ISBLOCKVALIDEVISIBLE()) return;
    AppendPartialUndo(UNDOACTION_STYLE_FRAMING,InteractiveZone,false);

    cBrushDefinition *CurrentBrush=CurrentCompoObject->BackgroundBrush;

    // It's a text block
    switch (Value) {
        case 0: // Unlock
            CurrentBrush->LockGeometry=false;
            break;
        case 1: // Lock to this geometry
            CurrentBrush->LockGeometry=true;
            CurrentBrush->AspectRatio=(CurrentCompoObject->h*InteractiveZone->DisplayH)/(CurrentCompoObject->w*InteractiveZone->DisplayW);
            break;
        case 2: // Lock to project geometry
            CurrentBrush->LockGeometry=true;
            CurrentBrush->AspectRatio=ProjectGeometry;
            break;
        default:
            return;
    }
    if (CurrentCompoObject->h>(CurrentCompoObject->w*InteractiveZone->DisplayW*CurrentBrush->AspectRatio)/InteractiveZone->DisplayH) CurrentCompoObject->h=(CurrentCompoObject->w*InteractiveZone->DisplayW*CurrentBrush->AspectRatio)/InteractiveZone->DisplayH;
        else CurrentCompoObject->w=(CurrentCompoObject->h*InteractiveZone->DisplayH/CurrentBrush->AspectRatio)/InteractiveZone->DisplayW;

    int CurrentRow=BlockTable->currentRow();
    RefreshBlockTable(CurrentRow>0?CurrentRow:0);
}
