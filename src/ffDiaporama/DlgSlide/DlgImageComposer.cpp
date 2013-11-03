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

#include "DlgImageComposer.h"
#include "ui_DlgImageComposer.h"

#include "../../engine/cTextFrame.h"
#include "../../CustomCtrl/cCTexteFrameComboBox.h"

#include "../DlgInfoFile/DlgInfoFile.h"
#include "../DlgImage/DlgImageCorrection.h"
#include "../DlgText/DlgTextEdit.h"
#include "../DlgFileExplorer/DlgFileExplorer.h"

#include "../DlgSlide/DlgRuler/DlgRulerDef.h"

#include <QClipboard>
#include <QMimeData>
#include <QMessageBox>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QTextBlockFormat>
#include <QMenu>

#define ICON_EDIT_IMAGE     ":/img/EditImage.png"
#define ICON_EDIT_MOVIE     ":/img/EditMovie.png"
#define ICON_RULER_ON       ":/img/ruler_ok.png"
#define ICON_RULER_OFF      ":/img/ruler_ko.png"

// Undo actions
enum UNDOACTION_ID {
    UNDOACTION_INTERACTIVEMOVERESIZE,
    UNDOACTION_EDITZONE_POSX,
    UNDOACTION_EDITZONE_POSY,
    UNDOACTION_EDITZONE_WIDTH,
    UNDOACTION_EDITZONE_HEIGHT,
    UNDOACTION_EDITZONE_ROTATEX,
    UNDOACTION_EDITZONE_ROTATEY,
    UNDOACTION_EDITZONE_ROTATEZ,
    UNDOACTION_EDITZONE_SHAPEFORM,
    UNDOACTION_EDITZONE_TEXTCLIPART,
    UNDOACTION_EDITZONE_SHAPEOPACITY,
    UNDOACTION_EDITZONE_SHAPEPENSIZE,
    UNDOACTION_EDITZONE_SHAPEPENSTYLE,
    UNDOACTION_EDITZONE_SHAPEPENCOLOR,
    UNDOACTION_EDITZONE_SHAPESHADOWFORM,
    UNDOACTION_EDITZONE_SHAPESHADOWDIST,
    UNDOACTION_EDITZONE_SHAPESHADOWCOLOR,
    UNDOACTION_STYLE_COORDINATES,
    UNDOACTION_STYLE_SHAPE,
    UNDOACTION_STYLE_FRAMING,
    UNDOACTION_EDITZONE_TEXTANIMZOOM,
    UNDOACTION_BLOCKTABLE_ADDTEXTBLOCK,
    UNDOACTION_BLOCKTABLE_ADDFILEBLOCK,
    UNDOACTION_BLOCKTABLE_REMOVEBLOCK,
    UNDOACTION_BLOCKTABLE_PASTEBLOCK,
    UNDOACTION_BLOCKTABLE_CHBLOCKORDER,
    UNDOACTION_BLOCKTABLE_ARRANGEBLOCK,
    UNDOACTION_BLOCKTABLE_EDITTEXT,
    UNDOACTION_BLOCKTABLE_EDITVIDEO,
    UNDOACTION_BLOCKTABLE_EDITIMAGE
};

//====================================================================================================================

DlgImageComposer::DlgImageComposer(cDiaporama *ffdProject,cBaseApplicationConfig *ApplicationConfig,QWidget *parent)
    :QCustomDialog(ApplicationConfig,parent),ui(new Ui::DlgImageComposer) {

    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::DlgImageComposer");

    ui->setupUi(this);
    TypeWindowState  =TypeWindowState_withsplitterpos;
    Splitter         =ui->SplitterTop;
    this->ffdProject =ffdProject;
    CancelBt        =ui->CancelBt;
    OkBt            =ui->OkBt;
    HelpBt          =ui->HelpBt;
    HelpFile        ="0117";

    InRefreshStyleControls              =false;
    InRefreshControls                   =false;
    StopMajFramingStyle                 =false;
    InSelectionChange                   =false;
    NoPrepUndo                          =false;
    CurrentCompoObject                  =NULL;
    CurrentCompoObjectNbr               =-1;
    FramingCB_CurrentBrush              =NULL;
    NbrSelected                         =0;
    CompositionList                     =&ffdProject->ProjectThumbnail->List[0]->ShotComposition;
    ui->InteractiveZone->DiaporamaObject=ffdProject->ProjectThumbnail;
    ui->BlockTable->ApplicationConfig   =ApplicationConfig;
    ui->BlockTable->CurrentSlide        =ffdProject->ProjectThumbnail;
    ui->BlockTable->CompositionList     =&ffdProject->ProjectThumbnail->List[0]->ShotComposition;
    ui->InteractiveZone->BlockTable     =ui->BlockTable;
    ui->InteractiveZone->MagneticRuler  =ApplicationConfig->ThumbRuler;
    ui->InteractiveZone->DisplayW       =THUMBWITH;
    ui->InteractiveZone->DisplayH       =THUMBHEIGHT;
    switch (ffdProject->ImageGeometry) {
        case GEOMETRY_4_3:      ProjectGeometry=1440/1080;     break;
        case GEOMETRY_40_17:    ProjectGeometry=1920/816;      break;
        case GEOMETRY_16_9:
        default:                ProjectGeometry=1920/1080;     break;

    }
}

//====================================================================================================================
// Initialise dialog

void DlgImageComposer::DoInitDialog() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::DoInitDialog");

    // Thumbnail part
    ui->SplitterTop->setCollapsible(0,false);
    ui->SplitterTop->setCollapsible(1,false);
    ui->TextClipArtCB->PrepareTable();
    ui->BlockTable->ProjectGeometry=ProjectGeometry;
    // Init combo box Background form
    for (int i=0;i<ShapeFormDefinition.count();i++) if (ShapeFormDefinition.at(i).Enable) ui->BackgroundFormCB->addItem(ShapeFormDefinition.at(i).Name,QVariant(i));
    MakeFormIcon(ui->BackgroundFormCB);
    if (ui->ShadowEffectCB->view()->width()<500)        ui->ShadowEffectCB->view()->setFixedWidth(500);

    // Init combo box Background shadow form
    ui->ShadowEffectCB->addItem(QApplication::translate("DlgImageComposer","None"));
    ui->ShadowEffectCB->addItem(QApplication::translate("DlgImageComposer","Shadow upper left"));
    ui->ShadowEffectCB->addItem(QApplication::translate("DlgImageComposer","Shadow upper right"));
    ui->ShadowEffectCB->addItem(QApplication::translate("DlgImageComposer","Shadow bottom left"));
    ui->ShadowEffectCB->addItem(QApplication::translate("DlgImageComposer","Shadow bottom right"));
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
    if (BaseApplicationConfig->DisplayUnit==DISPLAYUNIT_PERCENT) {
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
    // Force icon in contextual menu
    ui->actionAddTextBlock->setIconVisibleInMenu(true);
    ui->actionAddFile->setIconVisibleInMenu(true);
    ui->actionEditText->setIconVisibleInMenu(true);
    ui->actionEditImage->setIconVisibleInMenu(true);
    ui->actionUpBlock->setIconVisibleInMenu(true);
    ui->actionDownBlock->setIconVisibleInMenu(true);
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
    ui->actionDistributeHoriz->setIconVisibleInMenu(true);
    ui->actionDistributeVert->setIconVisibleInMenu(true);
    ui->actionAddSimpleTextBlock->setIconVisibleInMenu(true);
    ui->actionAddClipArtTextBlock->setIconVisibleInMenu(true);

    // Define handler
    connect(ui->RulersBT,SIGNAL(clicked()),this,SLOT(s_RulersBt()));

    connect(ui->actionCopy,SIGNAL(triggered()),this,SLOT(s_BlockTable_Copy()));
    connect(ui->actionCut,SIGNAL(triggered()),this,SLOT(s_BlockTable_Cut()));
    connect(ui->actionPaste,SIGNAL(triggered()),this,SLOT(s_BlockTable_Paste()));

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

    connect(ui->ArrangeBT,SIGNAL(pressed()),this,SLOT(s_BlockSettings_Arrange()));
    connect(ui->EditBT,SIGNAL(pressed()),this,SLOT(s_BlockSettings_Edit()));
    connect(ui->actionEditText,SIGNAL(triggered()),this,SLOT(s_BlockSettings_TextEditor()));
    connect(ui->actionEditImage,SIGNAL(triggered()),this,SLOT(s_BlockSettings_ImageEditCorrect()));
    connect(ui->InfoBlockBT,SIGNAL(clicked()),this,SLOT(s_BlockSettings_Information()));
    connect(ui->actionInfo,SIGNAL(triggered()),this,SLOT(s_BlockSettings_Information()));

    connect(ui->ZoomSlider,SIGNAL(valueChanged(int)),this,SLOT(s_BlockSettings_TextZoom(int)));
    connect(ui->ZoomED,SIGNAL(valueChanged(int)),this,SLOT(s_BlockSettings_TextZoom(int)));
    connect(ui->ZoomResetBT,SIGNAL(released()),this,SLOT(s_BlockSettings_TextZoomReset()));

    connect(ui->PosXEd,SIGNAL(valueChanged(double)),this,SLOT(s_BlockSettings_PosXValue(double)));
    connect(ui->PosYEd,SIGNAL(valueChanged(double)),this,SLOT(s_BlockSettings_PosYValue(double)));
    connect(ui->WidthEd,SIGNAL(valueChanged(double)),this,SLOT(s_BlockSettings_PosWidthValue(double)));
    connect(ui->HeightEd,SIGNAL(valueChanged(double)),this,SLOT(s_BlockSettings_PosHeightValue(double)));
    connect(ui->BackgroundFormCB,SIGNAL(itemSelectionHaveChanged()),this,SLOT(s_BlockSettings_ShapeBackgroundForm()));
    connect(ui->TextClipArtCB,SIGNAL(itemSelectionHaveChanged()),this,SLOT(s_BlockSettings_ShapeTextClipArtChIndex()));
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

    // Block table/scene part
    connect(ui->BlockTable,SIGNAL(itemSelectionChanged()),this,SLOT(s_BlockTable_SelectionChanged()));
    connect(ui->BlockTable,SIGNAL(DoubleClickEvent(QMouseEvent *)),this,SLOT(s_BlockTable_ItemDoubleClicked(QMouseEvent *)));
    connect(ui->BlockTable,SIGNAL(RightClickEvent(QMouseEvent *)),this,SLOT(s_BlockTable_ItemRightClicked(QMouseEvent *)));
    connect(ui->BlockTable,SIGNAL(DragMoveBlock(int,int)),this,SLOT(s_BlockTable_DragMoveBlock(int,int)));
    connect(ui->BlockTable,SIGNAL(DragDropFiles(QList<QUrl>)),this,SLOT(s_BlockTable_DragDropFiles(QList<QUrl>)));
    connect(ui->AddTextBlock,SIGNAL(pressed()),this,SLOT(s_BlockTable_AddNewTextBlock()));
    connect(ui->actionAddTextBlock,SIGNAL(triggered()),this,SLOT(s_BlockTable_AddNewTextBlock()));
    connect(ui->actionAddSimpleTextBlock,SIGNAL(triggered()),this,SLOT(s_BlockTable_AddNewSimpleTextBlock()));
    connect(ui->actionAddClipArtTextBlock,SIGNAL(triggered()),this,SLOT(s_BlockTable_AddNewClipArtTextBlock()));
    connect(ui->AddFileBlock,SIGNAL(pressed()),this,SLOT(s_BlockTable_AddNewFileBlock()));
    connect(ui->actionAddFile,SIGNAL(triggered()),this,SLOT(s_BlockTable_AddNewFileBlock()));
    connect(ui->actionRemoveBlock,SIGNAL(triggered()),this,SLOT(s_BlockTable_RemoveBlock()));

    // Style buttons
    connect(ui->BlockShapeStyleBT,SIGNAL(pressed()),this,SLOT(s_BlockShapeStyleBT()));

    // Interactive zone
    connect(ui->InteractiveZone,SIGNAL(StartSelectionChange()),this,SLOT(s_BlockTable_StartSelectionChange()));
    connect(ui->InteractiveZone,SIGNAL(EndSelectionChange()),this,SLOT(s_BlockTable_EndSelectionChange()));
    connect(ui->InteractiveZone,SIGNAL(RightClickEvent(QMouseEvent *)),this,SLOT(s_BlockTable_ItemRightClicked(QMouseEvent *)));
    connect(ui->InteractiveZone,SIGNAL(DoubleClickEvent(QMouseEvent *)),this,SLOT(s_BlockTable_ItemDoubleClicked(QMouseEvent *)));
    connect(ui->InteractiveZone,SIGNAL(TransformBlock(qreal,qreal,qreal,qreal,qreal,qreal,qreal,qreal)),this,SLOT(s_BlockSettings_IntZoneTransformBlocks(qreal,qreal,qreal,qreal,qreal,qreal,qreal,qreal)));
    connect(ui->InteractiveZone,SIGNAL(DisplayTransformBlock(qreal,qreal,qreal,qreal,qreal,qreal,qreal,qreal)),this,SLOT(s_BlockSettings_IntZoneDisplayTransformBlocks(qreal,qreal,qreal,qreal,qreal,qreal,qreal,qreal)));

    s_Event_ClipboardChanged();           // Setup clipboard button state
    connect(QApplication::clipboard(),SIGNAL(dataChanged()),this,SLOT(s_Event_ClipboardChanged()));
    RefreshBlockTable(0);
}

//====================================================================================================================

DlgImageComposer::~DlgImageComposer() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::DoInitDialog");

    ffdProject->CloseUnusedLibAv(ffdProject->CurrentCol);
    delete ui;
}

//====================================================================================================================
// Call when user click on Ok button

bool DlgImageComposer::DoAccept() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::DoAccept");

    return true;
}

//====================================================================================================================

void DlgImageComposer::ComputeBlockRatio(cCompositionObject *Block,qreal &Ratio_X,qreal &Ratio_Y) {
    QRectF tmpRect=PolygonToRectF(ComputePolygon(Block->BackgroundForm,Block->x*THUMBWITH,Block->y*THUMBHEIGHT,Block->w*THUMBWITH,Block->h*THUMBHEIGHT));
    Ratio_X=(Block->w*THUMBWITH)/tmpRect.width();
    Ratio_Y=(Block->h*THUMBHEIGHT)/tmpRect.height();
}

//====================================================================================================================

void DlgImageComposer::MakeFormIcon(QComboBox *UICB) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::MakeFormIcon");

    for (int i=0;i<UICB->count();i++) {
        cCompositionObject Object(COMPOSITIONTYPE_BACKGROUND,0,BaseApplicationConfig);
        Object.x                        =0;
        Object.y                        =0;
        Object.w                        =1;
        Object.h                        =1;
        Object.BackgroundForm           =UICB->itemData(i).toInt();
        Object.Opacity                  =4;
        Object.PenSize                  =1;
        Object.PenStyle                 =Qt::SolidLine;
        Object.PenColor                 ="#000000";
        Object.BackgroundBrush->ColorD  ="#FFFFFF";
        Object.BackgroundBrush->BrushType=BRUSHTYPE_SOLID;
        QPixmap  Image(UICB->iconSize());
        QPainter Painter;
        Painter.begin(&Image);
        Painter.fillRect(QRect(0,0,UICB->iconSize().width(),UICB->iconSize().height()),"#ffffff");
        Object.DrawCompositionObject(NULL,&Painter,1,UICB->iconSize().width(),UICB->iconSize().height(),true,0,NULL,1,1,NULL,false,0,false);
        Painter.end();
        UICB->setItemIcon(i,QIcon(Image));
    }
}

//====================================================================================================================

void DlgImageComposer::MakeBorderStyleIcon(QComboBox *UICB) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::MakeBorderStyleIcon");

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

void DlgImageComposer::resizeEvent(QResizeEvent *) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::resizeEvent");

    ui->InteractiveZone->RefreshDisplay();
}

//====================================================================================================================

void DlgImageComposer::showEvent(QShowEvent *ev) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::showEvent");

    QCustomDialog::showEvent(ev);
    ui->InteractiveZone->RefreshDisplay();
}

//====================================================================================================================

void DlgImageComposer::s_Event_ClipboardChanged() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::s_Event_ClipboardChanged");

    ui->actionPaste->setEnabled((QApplication::clipboard())&&(QApplication::clipboard()->mimeData())&&(QApplication::clipboard()->mimeData()->hasFormat("ffDiaporama/block")));
}

//====================================================================================================================

void DlgImageComposer::keyReleaseEvent(QKeyEvent *event) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::keyReleaseEvent");

    if ((focusWidget()==ui->BlockTable)||(focusWidget()==ui->InteractiveZone)) {
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
                    if (ui->InteractiveZone->hasFocus()) s_BlockTable_RemoveBlock();
                    break;
                default :
                    QCustomDialog::keyReleaseEvent(event);
                    break;
            }
        } else QCustomDialog::keyReleaseEvent(event);
    } else QCustomDialog::keyReleaseEvent(event);
}

//====================================================================================================================
// Initiale Undo

void DlgImageComposer::PrepareGlobalUndo() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::PrepareGlobalUndo");

    // Save object before modification for cancel button
    Undo=new QDomDocument(APPLICATION_NAME);
    QDomElement root=Undo->createElement("UNDO-DLG"); // Create xml document and root
    ffdProject->ProjectThumbnail->SaveToXML(root,"UNDO-DLG-ProjectThumbnail",ffdProject->ProjectFileName,true,NULL);
    Undo->appendChild(root); // Add object to xml document
}

//====================================================================================================================
// Apply Undo : call when user click on Cancel button

void DlgImageComposer::DoGlobalUndo() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::DoGlobalUndo");

    QDomElement root=Undo->documentElement();
    if (root.tagName()=="UNDO-DLG") {
        ffdProject->ProjectThumbnail->LoadFromXML(root,"UNDO-DLG-ProjectThumbnail","",NULL);
    }
}

//====================================================================================================================
// Refresh background image of the scene

void DlgImageComposer::s_RefreshSceneImage() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::s_RefreshSceneImage");

    ui->InteractiveZone->RefreshDisplay();
}

void DlgImageComposer::CopyBlockProperties(cCompositionObject *SourceBlock,cCompositionObject *DestBlock) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::CopyBlockProperties");

    if ((!DestBlock)||(SourceBlock==DestBlock)) return;

    // Attribut of the text part
    DestBlock->Text                 =SourceBlock->Text;                     // Text of the object
    DestBlock->TextClipArtName      =SourceBlock->TextClipArtName;          // Text ClipArt of the object
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
    DestBlock->TMType               =SourceBlock->TMType;                   // Text margins type (0=full shape, 1=shape default, 2=custom)
    DestBlock->TMx                  =SourceBlock->TMx;                      // Text margins
    DestBlock->TMy                  =SourceBlock->TMy;                      // Text margins
    DestBlock->TMw                  =SourceBlock->TMw;                      // Text margins
    DestBlock->TMh                  =SourceBlock->TMh;                      // Text margins

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

//====================================================================================================================

cCompositionObject *DlgImageComposer::GetGlobalCompositionObject(int IndexKey) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::GetSelectedGlobalCompositionObject");

    int CurrentBlock=ui->BlockTable->currentRow();
    if ((CurrentBlock<0)||(CurrentBlock>=CompositionList->List.count())) return NULL;
    int i=0;
    while ((i<ffdProject->ProjectThumbnail->ObjectComposition.List.count())&&(ffdProject->ProjectThumbnail->ObjectComposition.List[i]->IndexKey!=IndexKey)) i++;
    if (i<ffdProject->ProjectThumbnail->ObjectComposition.List.count()) return ffdProject->ProjectThumbnail->ObjectComposition.List[i]; else return NULL;
}

//====================================================================================================================
// BLOCK TABLE SETTINGS
//====================================================================================================================

//=========  Refresh the BlockTable
void DlgImageComposer::RefreshBlockTable(int SetCurrentIndex) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::RefreshBlockTable");
    ui->BlockTable->setUpdatesEnabled(false);
    ui->BlockTable->setRowCount(CompositionList->List.count());
    for (int i=0;i<ui->BlockTable->rowCount();i++) ui->BlockTable->setRowHeight(i,48+2+((((cBaseApplicationConfig *)BaseApplicationConfig)->TimelineHeight-TIMELINEMINHEIGH)/20+1)*3);
    ui->BlockTable->setUpdatesEnabled(true);
    if (ui->BlockTable->currentRow()!=SetCurrentIndex) {
        ui->BlockTable->clearSelection();
        ui->BlockTable->setCurrentCell(SetCurrentIndex,0,QItemSelectionModel::Select|QItemSelectionModel::Current);
    } else s_BlockTable_SelectionChanged();
    if (ui->BlockTable->rowCount()==0) s_BlockTable_SelectionChanged();
    ui->InteractiveZone->RefreshDisplay();
}

//========= block selection change
void DlgImageComposer::s_BlockTable_SelectionChanged() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::s_BlockTable_SelectionChanged");
    if (InSelectionChange) return;

    QModelIndexList SelList=ui->BlockTable->selectionModel()->selectedIndexes();

    IsSelected.clear();
    for (int i=0;i<ui->BlockTable->rowCount();i++)  IsSelected.append(false);
    for (int i=0;i<SelList.count();i++)             IsSelected[SelList.at(i).row()]=true;

    NbrSelected             =0;
    CurrentCompoObjectNbr   =-1;
    CurrentCompoObject      =NULL;

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

void DlgImageComposer::s_BlockTable_StartSelectionChange() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::s_BlockTable_StartSelectionChange");
    InSelectionChange=true;
}

void DlgImageComposer::s_BlockTable_EndSelectionChange() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::s_BlockTable_EndSelectionChange");
    InSelectionChange=false;
    s_BlockTable_SelectionChanged();
}

//====================================================================================================================

void DlgImageComposer::RefreshStyleControls() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::RefreshStyleControls");

    if (InRefreshStyleControls) return;
    InRefreshStyleControls=true;

    ui->BlockShapeStyleBT ->setEnabled(BlockSelectMode==SELECTMODE_ONE);
    ui->BlockShapeStyleED ->setEnabled(BlockSelectMode==SELECTMODE_ONE);

    if (BlockSelectMode==SELECTMODE_ONE) {
        ui->BlockShapeStyleED->setText(BaseApplicationConfig->StyleBlockShapeCollection.GetStyleName(CurrentCompoObject->GetBlockShapeStyle()));

        StopMajFramingStyle=true;
        ui->BackgroundFormCB->PrepareFrameShapeTable(true,0,CurrentCompoObject->BackgroundForm);
        ui->BackgroundFormCB->SetCurrentFrameShape(CurrentCompoObject->BackgroundForm);
        StopMajFramingStyle=false;

        FramingCB_CurrentBrush=CurrentCompoObject->BackgroundBrush;

    } else {
        FramingCB_CurrentBrush=NULL;
        ui->BlockShapeStyleED->setText("");
        ui->BackgroundFormCB->clear();
    }

    InRefreshStyleControls=false;
}

//====================================================================================================================

void DlgImageComposer::RefreshControls(bool UpdateInteractiveZone) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::RefreshControls");

    if ((InRefreshControls)||(!CompositionList)) return;
    InRefreshControls=true;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    //***********************
    // Tools button
    //***********************
    ui->EditBT->                setEnabled((BlockSelectMode==SELECTMODE_ONE));
    ui->ArrangeBT->             setEnabled((BlockSelectMode==SELECTMODE_ONE)||(BlockSelectMode==SELECTMODE_MULTIPLE));
    ui->InfoBlockBT->           setEnabled((BlockSelectMode==SELECTMODE_ONE)&&(CurrentCompoObject->BackgroundBrush->BrushType==BRUSHTYPE_IMAGEDISK));

    // actions
    ui->actionTop->             setEnabled(BlockSelectMode==SELECTMODE_MULTIPLE);
    ui->actionMiddle->          setEnabled(BlockSelectMode==SELECTMODE_MULTIPLE);
    ui->actionBottom->          setEnabled(BlockSelectMode==SELECTMODE_MULTIPLE);
    ui->actionLeft->            setEnabled(BlockSelectMode==SELECTMODE_MULTIPLE);
    ui->actionCenter->          setEnabled(BlockSelectMode==SELECTMODE_MULTIPLE);
    ui->actionRight->           setEnabled(BlockSelectMode==SELECTMODE_MULTIPLE);
    ui->actionDistributeHoriz-> setEnabled(NbrSelected>=3);
    ui->actionDistributeVert->  setEnabled(NbrSelected>=3);
    ui->actionEditImage->       setEnabled((BlockSelectMode==SELECTMODE_ONE)&&(CurrentCompoObject->BackgroundBrush->BrushType==BRUSHTYPE_IMAGEDISK)&&((CurrentCompoObject->BackgroundBrush->Video)||((CurrentCompoObject->BackgroundBrush->Image)&&(!CurrentCompoObject->BackgroundBrush->Image->IsVectorImg))));
    ui->actionEditText->        setEnabled(BlockSelectMode==SELECTMODE_ONE);
    ui->actionInfo->            setEnabled((BlockSelectMode==SELECTMODE_ONE)&&(CurrentCompoObject->BackgroundBrush->BrushType==BRUSHTYPE_IMAGEDISK));
    ui->actionRemoveBlock->     setEnabled((BlockSelectMode==SELECTMODE_ONE)||(BlockSelectMode==SELECTMODE_MULTIPLE));
    ui->actionUpBlock->         setEnabled((BlockSelectMode==SELECTMODE_ONE)&&(CurrentCompoObjectNbr>0));
    ui->actionDownBlock->       setEnabled((BlockSelectMode==SELECTMODE_ONE)&&(CurrentCompoObjectNbr<ui->BlockTable->rowCount()-1));
    ui->actionCopy->            setEnabled((BlockSelectMode==SELECTMODE_ONE)||(BlockSelectMode==SELECTMODE_MULTIPLE));
    ui->actionCut->             setEnabled((BlockSelectMode==SELECTMODE_ONE)||(BlockSelectMode==SELECTMODE_MULTIPLE));

    if (ui->actionEditImage->isEnabled()) {
        ui->actionEditImage->setIcon(QIcon(CurrentCompoObject->BackgroundBrush->Image!=NULL?ICON_EDIT_IMAGE:ICON_EDIT_MOVIE));
        ui->actionEditImage->setText(CurrentCompoObject->BackgroundBrush->Image!=NULL?ffDText(ffDSection_DlgImageCorrection,0):ffDText(ffDSection_DlgImageCorrection,1));
        ui->actionEditImage->setToolTip(CurrentCompoObject->BackgroundBrush->Image!=NULL?ffDText(ffDSection_DlgImageCorrection,2):ffDText(ffDSection_DlgImageCorrection,3));
    }

    //*****************************
    // Position, size and rotation
    //*****************************
    if (BlockSelectMode==SELECTMODE_ONE) {

        ui->PosSize_X->     setEnabled(true);               ui->PosXEd->     setEnabled(true);
        ui->PosSize_Y->     setEnabled(true);               ui->PosYEd->     setEnabled(true);
        ui->PosSize_Width-> setEnabled(true);               ui->WidthEd->    setEnabled(true);
        ui->PosSize_Height->setEnabled(true);               ui->HeightEd->   setEnabled(true);
        ui->ZoomLabel->     setEnabled(true);               ui->ZoomSlider-> setEnabled(true);
        ui->ZoomED->        setEnabled(true);               ui->ZoomResetBT->setEnabled(true);

        qreal Ratio_X,Ratio_Y;
        ComputeBlockRatio(CurrentCompoObject,Ratio_X,Ratio_Y);

        if (BaseApplicationConfig->DisplayUnit==DISPLAYUNIT_PERCENT) {
            ui->PosXEd->setRange(-200,200);                 ui->PosXEd->    setValue(CurrentCompoObject->x*100/Ratio_X);
            ui->PosYEd->setRange(-200,200);                 ui->PosYEd->    setValue(CurrentCompoObject->y*100/Ratio_Y);
            ui->WidthEd->setRange(1,200);                   ui->WidthEd->   setValue(CurrentCompoObject->w*100/Ratio_X);
            ui->HeightEd->setRange(1,200);                  ui->HeightEd->  setValue(CurrentCompoObject->h*100/Ratio_Y);
        } else { // DisplayUnit==DISPLAYUNIT_PIXELS
            ui->PosXEd->setRange(-2*THUMBWITH,2*THUMBWITH);   ui->PosXEd->    setValue(CurrentCompoObject->x*THUMBWITH/Ratio_X);
            ui->PosYEd->setRange(-2*THUMBHEIGHT,2*THUMBHEIGHT);   ui->PosYEd->    setValue(CurrentCompoObject->y*THUMBHEIGHT/Ratio_Y);
            ui->WidthEd->setRange(3,2*THUMBWITH);            ui->WidthEd->   setValue(CurrentCompoObject->w*THUMBWITH/Ratio_X);
            ui->HeightEd->setRange(3,2*THUMBHEIGHT);           ui->HeightEd->  setValue(CurrentCompoObject->h*THUMBHEIGHT/Ratio_Y);
        }
        ui->ZoomSlider->setValue(CurrentCompoObject->TxtZoomLevel);
        ui->ZoomED->setValue(CurrentCompoObject->TxtZoomLevel);

    } else {
        ui->PosSize_X->     setEnabled(false);              ui->PosXEd->    setEnabled(false);                                                  ui->PosXEd->  setValue(0);
        ui->PosSize_Y->     setEnabled(false);              ui->PosYEd->    setEnabled(false);                                                  ui->PosYEd->  setValue(0);
        ui->PosSize_Width-> setEnabled(false);              ui->WidthEd->   setEnabled(false);          ui->WidthEd->setRange(0,2*THUMBWITH);    ui->WidthEd-> setValue(0);
        ui->PosSize_Height->setEnabled(false);              ui->HeightEd->  setEnabled(false);          ui->HeightEd->setRange(0,2*THUMBHEIGHT);   ui->HeightEd->setValue(0);
        ui->ZoomLabel->     setEnabled(false);              ui->ZoomSlider-> setEnabled(false);
        ui->ZoomED->        setEnabled(false);              ui->ZoomResetBT->setEnabled(false);
    }

    //***********************
    // Rotation
    //***********************
    if (BlockSelectMode==SELECTMODE_ONE) {
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
    if (BlockSelectMode==SELECTMODE_ONE) {
        ui->BackgroundFormCB->  setEnabled(true);
        ui->PenSizeEd->         setEnabled(true);
        ui->PenColorCB->        setEnabled(CurrentCompoObject->PenSize!=0);
        ui->PenStyleCB->        setEnabled(CurrentCompoObject->PenSize!=0);
        ui->OpacityCB->         setEnabled(true);
        ui->ShadowEffectCB->    setEnabled(true);
        ui->ShadowEffectED->    setEnabled(CurrentCompoObject->FormShadow!=0);
        ui->ShadowColorCB->     setEnabled(CurrentCompoObject->FormShadow!=0);

        SetCBIndex(ui->BackgroundFormCB,CurrentCompoObject->BackgroundForm);
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

    // Set control visible or hide depending on TextClipArt
    bool IsVectorImg=((CurrentCompoObject)&&(CurrentCompoObject->BackgroundBrush)&&(CurrentCompoObject->BackgroundBrush->Image)&&(CurrentCompoObject->BackgroundBrush->Image->IsVectorImg));
    ui->BlockShapeStyleBT->setVisible(((!CurrentCompoObject)||(CurrentCompoObject->TextClipArtName==""))&&(!IsVectorImg));
    ui->BlockShapeStyleSpacer->setVisible(((!CurrentCompoObject)||(CurrentCompoObject->TextClipArtName==""))&&(!IsVectorImg));
    ui->BlockShapeStyleED->setVisible(((!CurrentCompoObject)||(CurrentCompoObject->TextClipArtName==""))&&(!IsVectorImg));
    ui->BackgroundFormCB->setVisible(((!CurrentCompoObject)||(CurrentCompoObject->TextClipArtName==""))&&(!IsVectorImg));
    ui->BackgroundFormLabel->setVisible(((!CurrentCompoObject)||(CurrentCompoObject->TextClipArtName==""))&&(!IsVectorImg));
    ui->PenSizeEd->setVisible(((!CurrentCompoObject)||(CurrentCompoObject->TextClipArtName==""))&&(!IsVectorImg));
    ui->PenColorCB->setVisible(((!CurrentCompoObject)||(CurrentCompoObject->TextClipArtName==""))&&(!IsVectorImg));
    ui->PenStyleCB->setVisible(((!CurrentCompoObject)||(CurrentCompoObject->TextClipArtName==""))&&(!IsVectorImg));
    ui->PenLabel->setVisible(((!CurrentCompoObject)||(CurrentCompoObject->TextClipArtName==""))&&(!IsVectorImg));
    ui->TextClipArtCB->setVisible((CurrentCompoObject)&&(CurrentCompoObject->TextClipArtName!=""));
    ui->TextClipArtLabel->setVisible((CurrentCompoObject)&&(CurrentCompoObject->TextClipArtName!=""));
    if ((CurrentCompoObject)&&(CurrentCompoObject->TextClipArtName!="")) ui->TextClipArtCB->SetCurrentTextFrame(CurrentCompoObject->TextClipArtName);

    RefreshStyleControls();

    QApplication::restoreOverrideCursor();
    InRefreshControls=false;

    // Refresh interactive zone display and shot thumbnail
    if (UpdateInteractiveZone) ui->InteractiveZone->RefreshDisplay(); else ui->InteractiveZone->repaint(); // else refresh selection only
}

//====================================================================================================================

void DlgImageComposer::s_BlockTable_Cut() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::s_BlockTable_Cut");

    if ((BlockSelectMode!=SELECTMODE_ONE)&&(BlockSelectMode!=SELECTMODE_MULTIPLE)) return;
    s_BlockTable_Copy();
    s_BlockTable_RemoveBlock();
}

//====================================================================================================================

void DlgImageComposer::s_BlockTable_Copy() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::s_BlockTable_Copy");

    if ((BlockSelectMode!=SELECTMODE_ONE)&&(BlockSelectMode!=SELECTMODE_MULTIPLE)) return;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    // Create xml document and root
    QDomDocument Object  =QDomDocument(APPLICATION_NAME);
    QDomElement  root    =Object.createElement("CLIPBOARD");
    int          BlockNum=0;
    for (int i=0;i<CompositionList->List.count();i++) if (IsSelected[i]) {
        QDomElement         Element=Object.createElement(QString("Block-%1").arg(BlockNum));
        cCompositionObject  *GlobalBlock=GetGlobalCompositionObject(CompositionList->List[i]->IndexKey);

        GlobalBlock->SaveToXML(Element,"CLIPBOARD-BLOCK-GLOBAL",ffdProject->ProjectFileName,true,true,NULL);                // Save global object
        CompositionList->List[i]->SaveToXML(Element,"CLIPBOARD-BLOCK-SHOT",ffdProject->ProjectFileName,true,true,NULL);     // Save shot object
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

void DlgImageComposer::s_BlockTable_Paste() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::s_BlockTable_Paste");
    AppendPartialUndo(UNDOACTION_BLOCKTABLE_PASTEBLOCK,ui->BlockTable,true);

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
                ffdProject->ProjectThumbnail->ObjectComposition.List.append(new cCompositionObject(COMPOSITIONTYPE_OBJECT,ffdProject->ProjectThumbnail->NextIndexKey,BaseApplicationConfig));
                cCompositionObject *GlobalBlock=ffdProject->ProjectThumbnail->ObjectComposition.List[ffdProject->ProjectThumbnail->ObjectComposition.List.count()-1];
                GlobalBlock->LoadFromXML(Element,"CLIPBOARD-BLOCK-GLOBAL","",NULL,NULL);
                GlobalBlock->IndexKey=ffdProject->ProjectThumbnail->NextIndexKey;

                cCompositionObject ShotBlock(COMPOSITIONTYPE_SHOT,ffdProject->ProjectThumbnail->NextIndexKey,BaseApplicationConfig);
                ShotBlock.LoadFromXML(Element,"CLIPBOARD-BLOCK-SHOT","",NULL,NULL);
                ShotBlock.IndexKey=ffdProject->ProjectThumbnail->NextIndexKey;
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
                for (int i=0;i<ffdProject->ProjectThumbnail->List.count();i++) {
                    ffdProject->ProjectThumbnail->List[i]->ShotComposition.List.append(new cCompositionObject(COMPOSITIONTYPE_SHOT,ffdProject->ProjectThumbnail->NextIndexKey,BaseApplicationConfig));
                    ffdProject->ProjectThumbnail->List[i]->ShotComposition.List[ffdProject->ProjectThumbnail->List[i]->ShotComposition.List.count()-1]->CopyFromCompositionObject(&ShotBlock);
                }
                // Inc NextIndexKey
                ffdProject->ProjectThumbnail->NextIndexKey++;
            }

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

void DlgImageComposer::s_BlockTable_RemoveBlock() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::s_BlockTable_RemoveBlock");
    AppendPartialUndo(UNDOACTION_BLOCKTABLE_REMOVEBLOCK,ui->BlockTable,true);

    if (BlockSelectMode==SELECTMODE_ONE) {
        if ((BaseApplicationConfig->AskUserToRemove)&&(CustomMessageBox(this,QMessageBox::Question,QApplication::translate("DlgImageComposer","Remove block"),QApplication::translate("DlgImageComposer","Are you sure you want to delete this block?"),
                                  QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)==QMessageBox::No)) return;
    } else if (BlockSelectMode==SELECTMODE_MULTIPLE) {
        if ((BaseApplicationConfig->AskUserToRemove)&&(CustomMessageBox(this,QMessageBox::Question,QApplication::translate("DlgImageComposer","Remove blocks"),QApplication::translate("DlgImageComposer","Are you sure you want to delete these blocks?"),
                                  QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)==QMessageBox::No)) return;
    }
    for (int i=CompositionList->List.count()-1;i>=0;i--) if (IsSelected[i]) {

        // Get indexkey of block
        int KeyToDelete=CompositionList->List[i]->IndexKey;

        // Delete block from all shots of the slide
        for (int j=0;j<ffdProject->ProjectThumbnail->List.count();j++) {
            int k=0;
            while (k<ffdProject->ProjectThumbnail->List[j]->ShotComposition.List.count()) {
                if (ffdProject->ProjectThumbnail->List[j]->ShotComposition.List[k]->IndexKey==KeyToDelete) delete ffdProject->ProjectThumbnail->List[j]->ShotComposition.List.takeAt(k);
                    else k++;
            }
        }

        // Delete block from global composition list of the slide
        int k=0;
        while (k<ffdProject->ProjectThumbnail->ObjectComposition.List.count()) {
            if (ffdProject->ProjectThumbnail->ObjectComposition.List[k]->IndexKey==KeyToDelete) delete ffdProject->ProjectThumbnail->ObjectComposition.List.takeAt(k);
                else k++;
        }
    }

    // Update display
    RefreshBlockTable(CurrentCompoObjectNbr>=CompositionList->List.count()?CurrentCompoObjectNbr-1:CurrentCompoObjectNbr);

    // Ensure nothing is selected
    ui->BlockTable->clearSelection();
}

//====================================================================================================================

void DlgImageComposer::s_BlockTable_AddNewTextBlock() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::s_BlockTable_AddNewTextBlock");
    AppendPartialUndo(UNDOACTION_BLOCKTABLE_ADDTEXTBLOCK,ui->BlockTable,true);

    QMenu *ContextMenu=new QMenu(this);
    ContextMenu->addAction(ui->actionAddSimpleTextBlock);
    ContextMenu->addAction(ui->actionAddClipArtTextBlock);
    ContextMenu->exec(QCursor::pos());
    delete ContextMenu;
    ui->AddTextBlock->setDown(false);
}

//====================================================================================================================

void DlgImageComposer::s_BlockTable_AddNewSimpleTextBlock() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::s_BlockTable_AddNewTextBlock");

    // Create and append a composition block to the object list
    ffdProject->ProjectThumbnail->ObjectComposition.List.append(new cCompositionObject(COMPOSITIONTYPE_OBJECT,ffdProject->ProjectThumbnail->NextIndexKey,BaseApplicationConfig));
    cCompositionObject *CompositionObject=ffdProject->ProjectThumbnail->ObjectComposition.List[ffdProject->ProjectThumbnail->ObjectComposition.List.count()-1];

    // Apply Styles
    CompositionObject->ApplyTextStyle(BaseApplicationConfig->StyleTextCollection.GetStyleDef(BaseApplicationConfig->StyleTextCollection.DecodeString(BaseApplicationConfig->DefaultBlock_Text_TextST)));
    CompositionObject->ApplyBackgroundStyle(BaseApplicationConfig->StyleTextBackgroundCollection.GetStyleDef(BaseApplicationConfig->StyleTextBackgroundCollection.DecodeString(BaseApplicationConfig->DefaultBlock_Text_BackGST)));
    CompositionObject->ApplyBlockShapeStyle(BaseApplicationConfig->StyleBlockShapeCollection.GetStyleDef(BaseApplicationConfig->StyleBlockShapeCollection.DecodeString(BaseApplicationConfig->DefaultBlock_Text_ShapeST)));
    CompositionObject->BackgroundBrush->LockGeometry=false; // For ApplyAutoCompoSize don't use it
    CompositionObject->ApplyAutoCompoSize(BaseApplicationConfig->DefaultBlock_AutoSizePos,ffdProject->ImageGeometry);
    CompositionObject->BackgroundBrush->LockGeometry=(BaseApplicationConfig->DefaultBlock_AutoLocking==AUTOFRAMING_CUSTOMPRJLOCK);
    CompositionObject->BackgroundBrush->AspectRatio=(CompositionObject->h*(ffdProject->ImageGeometry==GEOMETRY_4_3?1440:ffdProject->ImageGeometry==GEOMETRY_16_9?1080:ffdProject->ImageGeometry==GEOMETRY_40_17?816:1920))/(CompositionObject->w*1920);

    // Create default text
    QTextDocument       TextDoc(QApplication::translate("DlgImageComposer","Text","Default text value"));
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
    for (int i=0;i<ffdProject->ProjectThumbnail->List.count();i++) {
        ffdProject->ProjectThumbnail->List[i]->ShotComposition.List.append(new cCompositionObject(COMPOSITIONTYPE_SHOT,CompositionObject->IndexKey,BaseApplicationConfig));
        ffdProject->ProjectThumbnail->List[i]->ShotComposition.List[ffdProject->ProjectThumbnail->List[i]->ShotComposition.List.count()-1]->CopyFromCompositionObject(CompositionObject);
    }

    // Inc NextIndexKey
    ffdProject->ProjectThumbnail->NextIndexKey++;

    RefreshBlockTable(CompositionList->List.count()-1);
    NoPrepUndo=true;
    QTimer::singleShot(250,this,SLOT(s_BlockSettings_TextEditor()));    // Append "Open text editor" to the message queue
}

//====================================================================================================================

void DlgImageComposer::s_BlockTable_AddNewClipArtTextBlock() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::s_BlockTable_AddNewClipArtTextBlock");

    cPopupTexteFrame* popup1 = new cPopupTexteFrame(this);
    QPoint Pos=mapToGlobal(ui->AddTextBlock->pos());
    popup1->DoInitPopup(QPoint(Pos.x()+ui->AddTextBlock->width(),Pos.y()+ui->AddTextBlock->height()));
    popup1->exec();
    QString RessourceName=popup1->GetCurrentTextFrame();
    if (RessourceName=="") return;

    // Create and append a composition block to the object list
    ffdProject->ProjectThumbnail->ObjectComposition.List.append(new cCompositionObject(COMPOSITIONTYPE_OBJECT,ffdProject->ProjectThumbnail->NextIndexKey,BaseApplicationConfig));
    cCompositionObject *CompositionObject=ffdProject->ProjectThumbnail->ObjectComposition.List[ffdProject->ProjectThumbnail->ObjectComposition.List.count()-1];

    // Apply Styles
    CompositionObject->ApplyTextStyle(TextFrameList.List[TextFrameList.SearchImage(RessourceName)].TextStyle);
    CompositionObject->BackgroundBrush->LockGeometry=false; // For ApplyAutoCompoSize don't use it
    CompositionObject->ApplyAutoCompoSize(BaseApplicationConfig->DefaultBlock_AutoSizePos,ffdProject->ImageGeometry);
    CompositionObject->BackgroundBrush->LockGeometry=(BaseApplicationConfig->DefaultBlock_AutoLocking==AUTOFRAMING_CUSTOMPRJLOCK);
    CompositionObject->BackgroundBrush->AspectRatio =(CompositionObject->h*(ffdProject->ImageGeometry==GEOMETRY_4_3?1440:ffdProject->ImageGeometry==GEOMETRY_16_9?1080:ffdProject->ImageGeometry==GEOMETRY_40_17?816:1920))/(CompositionObject->w*1920);
    CompositionObject->TextClipArtName              =RessourceName;
    CompositionObject->TMx                          =TextFrameList.List[TextFrameList.SearchImage(RessourceName)].TMx;
    CompositionObject->TMy                          =TextFrameList.List[TextFrameList.SearchImage(RessourceName)].TMy;
    CompositionObject->TMw                          =TextFrameList.List[TextFrameList.SearchImage(RessourceName)].TMw;
    CompositionObject->TMh                          =TextFrameList.List[TextFrameList.SearchImage(RessourceName)].TMh;

    // Create default text
    QTextDocument       TextDoc(QApplication::translate("DlgImageComposer","Text","Default text value"));
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
    for (int i=0;i<ffdProject->ProjectThumbnail->List.count();i++) {
        ffdProject->ProjectThumbnail->List[i]->ShotComposition.List.append(new cCompositionObject(COMPOSITIONTYPE_SHOT,CompositionObject->IndexKey,BaseApplicationConfig));
        ffdProject->ProjectThumbnail->List[i]->ShotComposition.List[ffdProject->ProjectThumbnail->List[i]->ShotComposition.List.count()-1]->CopyFromCompositionObject(CompositionObject);
    }

    // Inc NextIndexKey
    ffdProject->ProjectThumbnail->NextIndexKey++;

    RefreshBlockTable(CompositionList->List.count()-1);
    NoPrepUndo=true;
    QTimer::singleShot(250,this,SLOT(s_BlockSettings_TextEditor()));    // Append "Open text editor" to the message queue
}

//====================================================================================================================

void DlgImageComposer::s_BlockTable_AddNewFileBlock() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::s_BlockTable_AddNewFileBlock");

    ui->AddFileBlock->setDown(false);
    QStringList FileList;
    DlgFileExplorer Dlg(FILTERALLOW_OBJECTTYPE_FOLDER|FILTERALLOW_OBJECTTYPE_MANAGED|FILTERALLOW_OBJECTTYPE_IMAGEFILE|FILTERALLOW_OBJECTTYPE_VIDEOFILE|FILTERALLOW_OBJECTTYPE_IMAGEVECTORFILE,
                        OBJECTTYPE_MANAGED,true,false,BaseApplicationConfig->RememberLastDirectories?BaseApplicationConfig->LastMediaPath:"",
                        QApplication::translate("MainWindow","Add files"),BaseApplicationConfig,this);
    Dlg.InitDialog();
    if (Dlg.exec()==0) FileList=Dlg.GetCurrentSelectedFiles();
    if (FileList.count()==0) return;

    // Sort files in the fileList
    if (BaseApplicationConfig->SortFile) {
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
    s_BlockTable_AddFilesBlock(FileList,ui->BlockTable->rowCount());
}

//====================================================================================================================

void DlgImageComposer::s_BlockTable_AddFilesBlock(QStringList FileList,int PositionToInsert) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::s_BlockTable_AddNewFileBlock");
    AppendPartialUndo(UNDOACTION_BLOCKTABLE_ADDFILEBLOCK,ui->BlockTable,true);

    // Add files
    QStringList AliasList;
    for (int i=0;i<FileList.count();i++) {
        QString NewFile=FileList[i];
        QString ErrorMessage=QApplication::translate("MainWindow","Format not supported","Error message");

        if (BaseApplicationConfig->RememberLastDirectories) BaseApplicationConfig->LastMediaPath=QFileInfo(NewFile).absolutePath();     // Keep folder for next use

        // Create and append a composition block to the object list
        ffdProject->ProjectThumbnail->ObjectComposition.List.insert(PositionToInsert,new cCompositionObject(COMPOSITIONTYPE_OBJECT,ffdProject->ProjectThumbnail->NextIndexKey,BaseApplicationConfig));
        cCompositionObject  *CompositionObject=ffdProject->ProjectThumbnail->ObjectComposition.List[PositionToInsert];
        cBrushDefinition    *CurrentBrush     =CompositionObject->BackgroundBrush;

        CompositionObject->Text     ="";
        CompositionObject->PenSize  =0;
        CurrentBrush->BrushType     =BRUSHTYPE_IMAGEDISK;

        QString BrushFileName =QFileInfo(NewFile).absoluteFilePath();

        bool    IsValide =false;
        QString Extension=QFileInfo(BrushFileName).suffix().toLower();

        // Search if file is an image
        for (int i=0;i<BaseApplicationConfig->AllowImageExtension.count();i++) if (BaseApplicationConfig->AllowImageExtension[i]==Extension) {
            // Create an image wrapper
            CurrentBrush->Image=new cImageFile(BaseApplicationConfig);
            IsValide=CurrentBrush->Image->GetInformationFromFile(BrushFileName,&AliasList,NULL,-1);
            if (!IsValide) {
                delete CurrentBrush->Image;
                CurrentBrush->Image=NULL;
            }
            break;
        }
        // If it's not an image : search if file is a video
        if (CurrentBrush->Image==NULL) for (int i=0;i<BaseApplicationConfig->AllowVideoExtension.count();i++) if (BaseApplicationConfig->AllowVideoExtension[i]==Extension) {
            // Create a video wrapper
            CurrentBrush->Video=new cVideoFile(OBJECTTYPE_VIDEOFILE,BaseApplicationConfig);
            IsValide=(CurrentBrush->Video->GetInformationFromFile(BrushFileName,&AliasList,NULL,-1))&&(CurrentBrush->Video->OpenCodecAndFile());
            if (IsValide) {
                // Check if file have at least one sound track compatible
                if ((CurrentBrush->Video->AudioStreamNumber!=-1)&&(!(
                    (CurrentBrush->Video->LibavAudioFile->streams[CurrentBrush->Video->AudioStreamNumber]->codec->sample_fmt!=AV_SAMPLE_FMT_S16)||
                    (CurrentBrush->Video->LibavAudioFile->streams[CurrentBrush->Video->AudioStreamNumber]->codec->sample_fmt!=AV_SAMPLE_FMT_U8)
                ))) {
                    ErrorMessage=ErrorMessage+"\n"+QApplication::translate("MainWindow","This application support only audio track with unsigned 8 bits or signed 16 bits sample format","Error message");
                    IsValide=false;
                }
                #if defined(LIBAV) && (LIBAVVERSIONINT<=8)
                if ((CurrentBrush->Video->AudioStreamNumber!=-1)&&(CurrentBrush->Video->LibavAudioFile->streams[CurrentBrush->Video->AudioStreamNumber]->codec->channels>2)) {
                    ErrorMessage=ErrorMessage+"\n"+QApplication::translate("MainWindow","This application support only mono or stereo audio track","Error message");
                    IsValide=false;
                }
                #endif
            }
            if (!IsValide) {
                delete CurrentBrush->Video;
                CurrentBrush->Video=NULL;
            } else {
                CurrentBrush->Video->EndPos=CurrentBrush->Video->Duration;
                if (CurrentBrush->Video->LibavVideoFile->start_time>0) CurrentBrush->Video->StartPos=QTime(0,0,0,0).addMSecs(int64_t((double(CurrentBrush->Video->LibavVideoFile->start_time)/AV_TIME_BASE)*1000));
            }
            break;
        }
        if (IsValide) {

            QImage *Image=(CurrentBrush->Image?CurrentBrush->Image->ImageAt(true):
                           CurrentBrush->Video?CurrentBrush->Video->ImageAt(true,QTime(0,0,0,0).msecsTo(CurrentBrush->Video->StartPos),NULL,CurrentBrush->Deinterlace,1,false,false):
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
                CompositionObject->ApplyTextStyle(BaseApplicationConfig->StyleTextCollection.GetStyleDef(BaseApplicationConfig->StyleTextCollection.DecodeString(BaseApplicationConfig->DefaultBlockBA_IMG_TextST)));
                CompositionObject->ApplyBlockShapeStyle(BaseApplicationConfig->StyleBlockShapeCollection.GetStyleDef(BaseApplicationConfig->StyleBlockShapeCollection.DecodeString(BaseApplicationConfig->DefaultBlockBA_IMG_ShapeST)));
                // Apply styles for coordinates
                CompositionObject->BackgroundBrush->ApplyAutoFraming(AUTOFRAMING_FULLMAX,ProjectGeometry);
                if ((CurrentBrush->Image)&&(CurrentBrush->Image->IsVectorImg)) CompositionObject->ApplyAutoCompoSize(AUTOCOMPOSIZE_HALFSCREEN,GEOMETRY_THUMBNAIL);
                    else CompositionObject->ApplyAutoCompoSize(AUTOCOMPOSIZE_HALFSCREEN,GEOMETRY_THUMBNAIL);
                delete Image;
            }
        }
        if (IsValide) {
            // Now create and append a shot composition block to all shot
            for (int i=0;i<ffdProject->ProjectThumbnail->List.count();i++) {
                ffdProject->ProjectThumbnail->List[i]->ShotComposition.List.insert(PositionToInsert,new cCompositionObject(COMPOSITIONTYPE_SHOT,CompositionObject->IndexKey,BaseApplicationConfig));
                ffdProject->ProjectThumbnail->List[i]->ShotComposition.List[PositionToInsert]->CopyFromCompositionObject(CompositionObject);
            }
            // Inc NextIndexKey
            ffdProject->ProjectThumbnail->NextIndexKey++;
            PositionToInsert++;

        } else {
            delete ffdProject->ProjectThumbnail->ObjectComposition.List.takeAt(PositionToInsert);
            CustomMessageBox(this,QMessageBox::Critical,QApplication::translate("MainWindow","Error","Error message"),NewFile+"\n\n"+ErrorMessage,QMessageBox::Close);
        }
    }
    RefreshBlockTable(PositionToInsert-1);
}

//====================================================================================================================
// Handler for interactive zone
//====================================================================================================================

void DlgImageComposer::s_BlockSettings_IntZoneTransformBlocks(qreal Move_X,qreal Move_Y,qreal Scale_X,qreal Scale_Y,qreal RSel_X,qreal RSel_Y,qreal RSel_W,qreal RSel_H) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::s_BlockSettings_IntZoneTransformBlocks");
    AppendPartialUndo(UNDOACTION_INTERACTIVEMOVERESIZE,ui->InteractiveZone,true);

    for (int i=0;i<IsSelected.count();i++) if ((IsSelected[i])&&(CompositionList->List[i]->IsVisible)) {
        qreal   RatioScale_X=(RSel_W+Scale_X)/RSel_W;
        qreal   RatioScale_Y=(RSel_H+Scale_Y)/RSel_H;

        CompositionList->List[i]->x=RSel_X+Move_X+(CompositionList->List[i]->x-RSel_X)*RatioScale_X;
        CompositionList->List[i]->y=RSel_Y+Move_Y+(CompositionList->List[i]->y-RSel_Y)*RatioScale_Y;
        CompositionList->List[i]->w=CompositionList->List[i]->w*RatioScale_X;
        if (CompositionList->List[i]->w<0.002) CompositionList->List[i]->w=0.002;
        if (CompositionList->List[i]->BackgroundBrush->LockGeometry) CompositionList->List[i]->h=((CompositionList->List[i]->w*THUMBWITH)*CompositionList->List[i]->BackgroundBrush->AspectRatio)/THUMBHEIGHT;
            else CompositionList->List[i]->h=CompositionList->List[i]->h*RatioScale_Y;
        if (CompositionList->List[i]->h<0.002) CompositionList->List[i]->h=0.002;
    }
    RefreshControls();
}

void DlgImageComposer::s_BlockSettings_IntZoneDisplayTransformBlocks(qreal Move_X,qreal Move_Y,qreal Scale_X,qreal Scale_Y,qreal RSel_X,qreal RSel_Y,qreal RSel_W,qreal RSel_H) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::s_BlockSettings_IntZoneDisplayTransformBlocks");

    InRefreshControls=true;

    int     i           =CurrentCompoObjectNbr;
    qreal   RatioScale_X=(RSel_W+Scale_X)/RSel_W;
    qreal   RatioScale_Y=(RSel_H+Scale_Y)/RSel_H;
    QRectF  tmpRect     =PolygonToRectF(ComputePolygon(CompositionList->List[i]->BackgroundForm,
                                                       CompositionList->List[i]->x*THUMBWITH,CompositionList->List[i]->y*THUMBHEIGHT,
                                                       CompositionList->List[i]->w*THUMBWITH,CompositionList->List[i]->h*THUMBHEIGHT));
    qreal   Ratio_X     =(CompositionList->List[i]->x*THUMBWITH)/tmpRect.width();
    qreal   Ratio_Y     =(CompositionList->List[i]->h*THUMBHEIGHT)/tmpRect.height();
    qreal   x           =RSel_X+Move_X+(CompositionList->List[i]->x-RSel_X)*RatioScale_X;
    qreal   y           =RSel_Y+Move_Y+(CompositionList->List[i]->y-RSel_Y)*RatioScale_Y;
    qreal   w           =CompositionList->List[i]->w*RatioScale_X; if (w<0.002) w=0.002;
    qreal   h           =(CompositionList->List[i]->BackgroundBrush->LockGeometry?((w*THUMBWITH)*CompositionList->List[i]->BackgroundBrush->AspectRatio)/THUMBHEIGHT:CompositionList->List[i]->h*RatioScale_Y); if (h<0.002) h=0.002;

    if (BaseApplicationConfig->DisplayUnit==DISPLAYUNIT_PERCENT) {
        ui->PosXEd->  setValue(x*100/Ratio_X);
        ui->PosYEd->  setValue(y*100/Ratio_Y);
        ui->WidthEd-> setValue(w*100/Ratio_X);
        ui->HeightEd->setValue(h*100/Ratio_Y);
    } else { // DisplayUnit==DISPLAYUNIT_PIXELS
        ui->PosXEd->  setValue(x*THUMBWITH/Ratio_X);
        ui->PosYEd->  setValue(y*THUMBHEIGHT/Ratio_Y);
        ui->WidthEd-> setValue(w*THUMBWITH/Ratio_X);
        ui->HeightEd->setValue(h*THUMBHEIGHT/Ratio_Y);
    }
    InRefreshControls=false;
}

//====================================================================================================================

void DlgImageComposer::s_BlockTable_MoveBlockUp() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::s_BlockTable_MoveBlockUp");
    AppendPartialUndo(UNDOACTION_BLOCKTABLE_CHBLOCKORDER,ui->BlockTable,true);
    s_BlockTable_SelectionChanged(); // Refresh selection
    if (BlockSelectMode!=SELECTMODE_ONE) return;
    if (CurrentCompoObjectNbr<1) return;
    CompositionList->List.swap(CurrentCompoObjectNbr,CurrentCompoObjectNbr-1);
    RefreshBlockTable(CurrentCompoObjectNbr-1);
}

//====================================================================================================================

void DlgImageComposer::s_BlockTable_MoveBlockDown() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::s_BlockTable_MoveBlockDown");
    AppendPartialUndo(UNDOACTION_BLOCKTABLE_CHBLOCKORDER,ui->BlockTable,true);
    s_BlockTable_SelectionChanged(); // Refresh selection
    if (BlockSelectMode!=SELECTMODE_ONE) return;
    if (CurrentCompoObjectNbr>=CompositionList->List.count()-1) return;
    CompositionList->List.swap(CurrentCompoObjectNbr+1,CurrentCompoObjectNbr);
    RefreshBlockTable(CurrentCompoObjectNbr+1);
}

//====================================================================================================================

void DlgImageComposer::s_BlockTable_DragMoveBlock(int Src,int Dst) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::s_BlockTable_DragMoveBlock");
    if (Src>=CompositionList->List.count()) return;
    AppendPartialUndo(UNDOACTION_BLOCKTABLE_CHBLOCKORDER,ui->BlockTable,true);
    if (Src<Dst) Dst--;
    CompositionList->List.insert(Dst,CompositionList->List.takeAt(Src));
    RefreshBlockTable(Dst);
}

//====================================================================================================================
// User double click on a block in the BlockTable widget or in the scene

void DlgImageComposer::s_BlockTable_ItemDoubleClicked(QMouseEvent *) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::s_BlockTable_ItemDoubleClicked");

    if (BlockSelectMode!=SELECTMODE_ONE) return;
    if (CurrentCompoObject->BackgroundBrush->BrushType!=BRUSHTYPE_IMAGEDISK)                                            s_BlockSettings_TextEditor();
    else if ((CurrentCompoObject->BackgroundBrush->Image!=NULL)||(CurrentCompoObject->BackgroundBrush->Video!=NULL))    s_BlockSettings_ImageEditCorrect();
}

//====================================================================================================================
// User right click on a block in the BlockTable widget or in the scene

void DlgImageComposer::s_BlockTable_ItemRightClicked(QMouseEvent *) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::s_BlockTable_ItemRightClicked");

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
        ContextMenu->addAction(ui->actionCut);
        ContextMenu->addAction(ui->actionCopy);
        ContextMenu->addAction(ui->actionPaste);
        ContextMenu->addSeparator();
        ContextMenu->addAction(ui->actionEditText);
        if (ui->actionEditImage->isEnabled()) ContextMenu->addAction(ui->actionEditImage);
        ContextMenu->addSeparator();
        ContextMenu->addAction(ui->actionRemoveBlock);
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
        ContextMenu->exec(QCursor::pos());
        delete ContextMenu;
    }
}

//====================================================================================================================

void DlgImageComposer::s_BlockTable_DragDropFiles(QList<QUrl> urlList) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::s_BlockTable_DragDropFiles");
    if (urlList.count()>0) {
        QString     fName;
        QFileInfo   info;
        QStringList FileList;
        for (int i=0;i<urlList.count();i++) {
            fName = urlList[i].toLocalFile();           // convert first QUrl to local path
            info.setFile(fName);                        // information about file
            if (info.isFile()) FileList.append(fName);  // append file
        }
        if (FileList.count()>0) s_BlockTable_AddFilesBlock(FileList,ui->BlockTable->DragItemDest);
    }
}

//====================================================================================================================

void DlgImageComposer::s_BlockSettings_Arrange() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::s_BlockSettings_Arrange");

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

void DlgImageComposer::s_BlockSettings_Edit() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::s_BlockSettings_Arrange");

    QMenu *ContextMenu=new QMenu(this);
    ContextMenu->addAction(ui->actionCut);
    ContextMenu->addAction(ui->actionCopy);
    ContextMenu->addAction(ui->actionPaste);
    ContextMenu->addSeparator();
    ContextMenu->addAction(ui->actionEditText);
    if (ui->actionEditImage->isEnabled()) ContextMenu->addAction(ui->actionEditImage);
    ContextMenu->addSeparator();
    ContextMenu->addAction(ui->actionRemoveBlock);
    ContextMenu->exec(QCursor::pos());
    delete ContextMenu;
    ui->EditBT->setDown(false);
}

//====================================================================================================================
//========= Open text editor

void DlgImageComposer::s_BlockSettings_TextEditor() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::s_BlockSettings_TextEditor");

    if ((InRefreshControls)||(BlockSelectMode!=SELECTMODE_ONE)||(!CurrentCompoObject)) return;
    if (!NoPrepUndo) AppendPartialUndo(UNDOACTION_BLOCKTABLE_EDITTEXT,ui->InteractiveZone,true);
    NoPrepUndo=false;

    ui->InteractiveZone->DisplayMode=cInteractiveZone::DisplayMode_TextMargin;
    ui->InteractiveZone->RefreshDisplay();
    DlgTextEdit Dlg(ffdProject,CurrentCompoObject,BaseApplicationConfig,&BaseApplicationConfig->StyleTextCollection,&BaseApplicationConfig->StyleTextBackgroundCollection,this);
    Dlg.InitDialog();
    connect(&Dlg,SIGNAL(RefreshDisplay()),this,SLOT(s_RefreshSceneImage()));
    if (Dlg.exec()==0) {
        ui->InteractiveZone->DisplayMode=cInteractiveZone::DisplayMode_BlockShape;
        // Apply to GlobalComposition objects
        CopyBlockProperties(CurrentCompoObject,GetGlobalCompositionObject(CurrentCompoObject->IndexKey));
        int CurrentRow=ui->BlockTable->currentRow();
        RefreshBlockTable(CurrentRow>0?CurrentRow:0);
    } else {
        ui->InteractiveZone->DisplayMode=cInteractiveZone::DisplayMode_BlockShape;
        RemoveLastPartialUndo();
        RefreshControls();
    }
}

//====================================================================================================================
//========= Open Information dialog

void DlgImageComposer::s_BlockSettings_Information() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::s_BlockSettings_Information");

    if ((InRefreshControls)||(BlockSelectMode!=SELECTMODE_ONE)||(!CurrentCompoObject)||
        ((!CurrentCompoObject->BackgroundBrush->Video)&&(!CurrentCompoObject->BackgroundBrush->Image))) return;

    cBaseMediaFile *Media=NULL;
    if (CurrentCompoObject->BackgroundBrush->Image!=NULL)            Media=CurrentCompoObject->BackgroundBrush->Image;
        else if (CurrentCompoObject->BackgroundBrush->Video!=NULL)   Media=CurrentCompoObject->BackgroundBrush->Video;

    if (Media) {
        DlgInfoFile Dlg(Media,BaseApplicationConfig,this);
        Dlg.InitDialog();
        Dlg.exec();
    }
}

//====================================================================================================================
//========= Open image/video correction editor

void DlgImageComposer::s_BlockSettings_ImageEditCorrect() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::s_BlockSettings_ImageEditCorrect");

    if ((InRefreshControls)||(BlockSelectMode!=SELECTMODE_ONE)||(!CurrentCompoObject)||
        ((!CurrentCompoObject->BackgroundBrush->Video)&&(!CurrentCompoObject->BackgroundBrush->Image))||(!ui->actionEditImage->isEnabled())) return;

    AppendPartialUndo(UNDOACTION_BLOCKTABLE_EDITIMAGE,ui->InteractiveZone,true);
    cBrushDefinition *CurrentBrush=CurrentCompoObject->BackgroundBrush;

    // Compute position of video
    int Position=CurrentBrush->Video?QTime(0,0,0,0).msecsTo(CurrentBrush->Video->StartPos):0;
    DlgImageCorrection Dlg(CurrentCompoObject,&CurrentCompoObject->BackgroundForm,CurrentCompoObject->BackgroundBrush,Position,ffdProject->ImageGeometry,ffdProject->ImageAnimSpeedWave,BaseApplicationConfig,this);
    Dlg.InitDialog();
    if (Dlg.exec()==0) {
        FramingCB_CurrentBrush   =NULL; // To force a refresh of ui->FramingCB !
        CurrentBrush->AspectRatio=CurrentBrush->AspectRatio;
        CurrentCompoObject->h    =(CurrentCompoObject->w*THUMBWITH*CurrentBrush->AspectRatio)/THUMBHEIGHT;

        // Adjust height and width to image stay in screen
        if (((CurrentCompoObject->y+CurrentCompoObject->h)*THUMBHEIGHT)>THUMBHEIGHT) {
            CurrentCompoObject->h=1-CurrentCompoObject->y;
            CurrentCompoObject->w=((CurrentCompoObject->h*THUMBHEIGHT)/CurrentBrush->AspectRatio)/THUMBWITH;
        }

        // Lulo object for image and video must be remove
        if (CurrentCompoObject->BackgroundBrush->Video) BaseApplicationConfig->ImagesCache.RemoveImageObject(CurrentCompoObject->BackgroundBrush->Video->FileKey);
        else if (CurrentCompoObject->BackgroundBrush->Image) BaseApplicationConfig->ImagesCache.RemoveImageObject(CurrentCompoObject->BackgroundBrush->Image->FileKey);

        RefreshBlockTable(CurrentCompoObjectNbr);
    } else {
        RemoveLastPartialUndo();
    }
}

//====================================================================================================================
// Handler for position, size & rotation controls
//====================================================================================================================

//========= X position
void DlgImageComposer::s_BlockSettings_PosXValue(double Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::s_BlockSettings_PosXValue");
    if ((InRefreshControls)||(BlockSelectMode!=SELECTMODE_ONE)||(!CurrentCompoObject)) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_POSX,ui->PosXEd,false);

    if (BaseApplicationConfig->DisplayUnit==DISPLAYUNIT_PERCENT) CurrentCompoObject->x=Value/100;           // DisplayUnit==DISPLAYUNIT_PERCENT
        else                                                     CurrentCompoObject->x=(Value/THUMBWITH);    // DisplayUnit==DISPLAYUNIT_PIXELS
    RefreshControls(true);
}

//========= Y position
void DlgImageComposer::s_BlockSettings_PosYValue(double Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::s_BlockSettings_PosYValue");
    if ((InRefreshControls)||(BlockSelectMode!=SELECTMODE_ONE)||(!CurrentCompoObject)) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_POSY,ui->PosYEd,false);

    if (BaseApplicationConfig->DisplayUnit==DISPLAYUNIT_PERCENT) CurrentCompoObject->y=Value/100;           // DisplayUnit==DISPLAYUNIT_PERCENT
        else                                                     CurrentCompoObject->y=(Value/THUMBHEIGHT);    // DisplayUnit==DISPLAYUNIT_PIXELS
    RefreshControls(true);
}

//========= Width
void DlgImageComposer::s_BlockSettings_PosWidthValue(double Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::s_BlockSettings_PosWidthValue");
    if ((InRefreshControls)||(BlockSelectMode!=SELECTMODE_ONE)||(!CurrentCompoObject)) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_WIDTH,ui->WidthEd,false);

    qreal Ratio_X,Ratio_Y;
    ComputeBlockRatio(CurrentCompoObject,Ratio_X,Ratio_Y);

    if (BaseApplicationConfig->DisplayUnit==DISPLAYUNIT_PERCENT) CurrentCompoObject->w=(Value/100)*Ratio_X;
        else CurrentCompoObject->w=(Value/THUMBWITH)*Ratio_X;
    if ((CurrentCompoObject->BackgroundBrush->LockGeometry)||(CurrentCompoObject->BackgroundBrush->Image!=NULL)||(CurrentCompoObject->BackgroundBrush->Video!=NULL))
            CurrentCompoObject->h=((CurrentCompoObject->w*THUMBWITH)*CurrentCompoObject->BackgroundBrush->AspectRatio)/THUMBHEIGHT;
        else CurrentCompoObject->BackgroundBrush->AspectRatio=(CurrentCompoObject->h*THUMBHEIGHT)/(CurrentCompoObject->w*THUMBWITH);
    RefreshControls(true);
}

//========= Height
void DlgImageComposer::s_BlockSettings_PosHeightValue(double Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::s_BlockSettings_PosHeightValue");
    if ((InRefreshControls)||(BlockSelectMode!=SELECTMODE_ONE)||(!CurrentCompoObject)) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_HEIGHT,ui->HeightEd,false);

    qreal Ratio_X,Ratio_Y;
    ComputeBlockRatio(CurrentCompoObject,Ratio_X,Ratio_Y);

    if (BaseApplicationConfig->DisplayUnit==DISPLAYUNIT_PERCENT) CurrentCompoObject->h=(Value/100)*Ratio_Y;
        else CurrentCompoObject->h=(Value/THUMBHEIGHT)*Ratio_Y;
    if ((CurrentCompoObject->BackgroundBrush->LockGeometry)||(CurrentCompoObject->BackgroundBrush->Image!=NULL)||(CurrentCompoObject->BackgroundBrush->Video!=NULL))
        CurrentCompoObject->w=((CurrentCompoObject->h*THUMBHEIGHT)/CurrentCompoObject->BackgroundBrush->AspectRatio)/THUMBWITH;
        else CurrentCompoObject->BackgroundBrush->AspectRatio=(CurrentCompoObject->h*THUMBHEIGHT)/(CurrentCompoObject->w*THUMBWITH);
    RefreshControls(true);
}

//========= X Rotation value
void DlgImageComposer::s_BlockSettings_RotateXValue(int Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::s_BlockSettings_RotateXValue");
    if ((InRefreshControls)||(BlockSelectMode!=SELECTMODE_ONE)||(!CurrentCompoObject)) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_ROTATEX,ui->RotateXED,false);
    CurrentCompoObject->RotateXAxis=Value;
    RefreshControls(true);
}
void DlgImageComposer::s_BlockSettings_ResetRotateXValue() {
    s_BlockSettings_RotateXValue(0);
}

//========= Y Rotation value
void DlgImageComposer::s_BlockSettings_RotateYValue(int Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::s_BlockSettings_RotateYValue");
    if ((InRefreshControls)||(BlockSelectMode!=SELECTMODE_ONE)||(!CurrentCompoObject)) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_ROTATEY,ui->RotateYED,false);
    CurrentCompoObject->RotateYAxis=Value;
    RefreshControls(true);
}
void DlgImageComposer::s_BlockSettings_ResetRotateYValue() {
    s_BlockSettings_RotateYValue(0);
}

//========= Z Rotation value
void DlgImageComposer::s_BlockSettings_RotateZValue(int Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::s_BlockSettings_RotateZValue");
    if ((InRefreshControls)||(BlockSelectMode!=SELECTMODE_ONE)||(!CurrentCompoObject)) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_ROTATEZ,ui->RotateZED,false);
    CurrentCompoObject->RotateZAxis=Value;
    RefreshControls(true);
}
void DlgImageComposer::s_BlockSettings_ResetRotateZValue() {
    s_BlockSettings_RotateZValue(0);
}

//====================================================================================================================
// Handler for Text
//====================================================================================================================

void DlgImageComposer::s_BlockSettings_TextZoom(int Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::s_BlockSettings_TextZoom");

    if ((InRefreshControls)||(BlockSelectMode!=SELECTMODE_ONE)||(!CurrentCompoObject)) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_TEXTANIMZOOM,ui->ZoomED,false);
    CurrentCompoObject->TxtZoomLevel=Value;
    ui->ZoomSlider->setValue(CurrentCompoObject->TxtZoomLevel);
    ui->ZoomED->setValue(CurrentCompoObject->TxtZoomLevel);
    CopyBlockProperties(CurrentCompoObject,GetGlobalCompositionObject(CurrentCompoObject->IndexKey));   // Apply to GlobalComposition objects
    RefreshControls(true);
}

void DlgImageComposer::s_BlockSettings_TextZoomReset() {
    s_BlockSettings_TextZoom(100);
}

//====================================================================================================================
// Handler for shape
//====================================================================================================================

//========= Text ClipArt
void DlgImageComposer::s_BlockSettings_ShapeTextClipArtChIndex() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::s_BlockSettings_ShapeTextClipArtChIndex");
    if ((InRefreshControls)||(BlockSelectMode!=SELECTMODE_ONE)||(!CurrentCompoObject)||(CurrentCompoObject->TextClipArtName=="")) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_TEXTCLIPART,ui->TextClipArtCB,false);
    CurrentCompoObject->TextClipArtName=ui->TextClipArtCB->GetCurrentTextFrame();
    cTextFrameObject *TFO=&TextFrameList.List[TextFrameList.SearchImage(CurrentCompoObject->TextClipArtName)];
    CurrentCompoObject->TMx=TFO->TMx;
    CurrentCompoObject->TMy=TFO->TMy;
    CurrentCompoObject->TMw=TFO->TMw;
    CurrentCompoObject->TMh=TFO->TMh;
    CurrentCompoObject->ApplyTextStyle(TFO->TextStyle);
    CopyBlockProperties(CurrentCompoObject,GetGlobalCompositionObject(CurrentCompoObject->IndexKey));   // Apply to GlobalComposition objects
    RefreshBlockTable(CurrentCompoObjectNbr);
}

//========= Background forme
void DlgImageComposer::s_BlockSettings_ShapeBackgroundForm() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::s_BlockSettings_ShapeBackgroundForm");
    if ((InRefreshControls)||(BlockSelectMode!=SELECTMODE_ONE)||(!CurrentCompoObject)) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_SHAPEFORM,ui->BackgroundFormCB,false);
    CurrentCompoObject->BackgroundForm=ui->BackgroundFormCB->GetCurrentFrameShape();
    CopyBlockProperties(CurrentCompoObject,GetGlobalCompositionObject(CurrentCompoObject->IndexKey));   // Apply to GlobalComposition objects
    RefreshBlockTable(CurrentCompoObjectNbr);
}

//========= Opacity
void DlgImageComposer::s_BlockSettings_ShapeOpacity(int Style) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::s_BlockSettings_ShapeOpacity");

    if ((InRefreshControls)||(BlockSelectMode!=SELECTMODE_ONE)||(!CurrentCompoObject)) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_SHAPEOPACITY,ui->OpacityCB,false);
    CurrentCompoObject->Opacity=Style;
    CopyBlockProperties(CurrentCompoObject,GetGlobalCompositionObject(CurrentCompoObject->IndexKey));   // Apply to GlobalComposition objects
    RefreshControls(true);
}

//========= Border pen size
void DlgImageComposer::s_BlockSettings_ShapePenSize(int) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::s_BlockSettings_ShapePenSize");
    if ((InRefreshControls)||(BlockSelectMode!=SELECTMODE_ONE)||(!CurrentCompoObject)) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_SHAPEPENSIZE,ui->PenSizeEd,false);
    CurrentCompoObject->PenSize=ui->PenSizeEd->value();
    ui->PenColorCB->setEnabled(CurrentCompoObject->PenSize!=0);
    ui->PenStyleCB->setEnabled(CurrentCompoObject->PenSize!=0);
    CopyBlockProperties(CurrentCompoObject,GetGlobalCompositionObject(CurrentCompoObject->IndexKey));   // Apply to GlobalComposition objects
    RefreshControls(true);
}

//========= Border pen style
void DlgImageComposer::s_BlockSettings_ShapePenStyle(int index) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::s_BlockSettings_ShapePenStyle");
    if ((InRefreshControls)||(BlockSelectMode!=SELECTMODE_ONE)||(!CurrentCompoObject)) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_SHAPEPENSTYLE,ui->PenStyleCB,false);
    CurrentCompoObject->PenStyle=ui->PenStyleCB->itemData(index).toInt();
    CopyBlockProperties(CurrentCompoObject,GetGlobalCompositionObject(CurrentCompoObject->IndexKey));   // Apply to GlobalComposition objects
    RefreshControls(true);
}

//========= Border pen color
void DlgImageComposer::s_BlockSettings_ShapePenColor(int) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::s_BlockSettings_ShapePenColor");
    if ((InRefreshControls)||(BlockSelectMode!=SELECTMODE_ONE)||(!CurrentCompoObject)) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_SHAPEPENCOLOR,ui->PenColorCB,false);
    CurrentCompoObject->PenColor=ui->PenColorCB->GetCurrentColor();
    CopyBlockProperties(CurrentCompoObject,GetGlobalCompositionObject(CurrentCompoObject->IndexKey));   // Apply to GlobalComposition objects
    RefreshControls(true);
}

//========= Shape shadow style
void DlgImageComposer::s_BlockSettings_ShapeShadowFormValue(int value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::s_BlockSettings_ShapeShadowFormValue");
    if ((InRefreshControls)||(BlockSelectMode!=SELECTMODE_ONE)||(!CurrentCompoObject)) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_SHAPESHADOWFORM,ui->ShadowEffectCB,false);
    CurrentCompoObject->FormShadow=value;
    ui->ShadowEffectED->setEnabled(CurrentCompoObject->FormShadow!=0);
    ui->ShadowColorCB->setEnabled(CurrentCompoObject->FormShadow!=0);
    CopyBlockProperties(CurrentCompoObject,GetGlobalCompositionObject(CurrentCompoObject->IndexKey));   // Apply to GlobalComposition objects
    RefreshControls(true);
}

//========= Shape shadow distance
void DlgImageComposer::s_BlockSettings_ShapeShadowDistanceValue(int value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::s_BlockSettings_ShapeShadowDistanceValue");
    if ((InRefreshControls)||(BlockSelectMode!=SELECTMODE_ONE)||(!CurrentCompoObject)) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_SHAPESHADOWDIST,ui->ShadowEffectED,false);
    CurrentCompoObject->FormShadowDistance =value;
    CopyBlockProperties(CurrentCompoObject,GetGlobalCompositionObject(CurrentCompoObject->IndexKey));   // Apply to GlobalComposition objects
    RefreshControls(true);
}

//========= shadow color
void DlgImageComposer::s_BlockSettings_ShapeShadowColor(int) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::s_BlockSettings_ShapeShadowColor");
    if ((InRefreshControls)||(BlockSelectMode!=SELECTMODE_ONE)||(!CurrentCompoObject)) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_SHAPESHADOWCOLOR,ui->ShadowColorCB,false);
    CurrentCompoObject->FormShadowColor=ui->ShadowColorCB->GetCurrentColor();
    CopyBlockProperties(CurrentCompoObject,GetGlobalCompositionObject(CurrentCompoObject->IndexKey));   // Apply to GlobalComposition objects
    RefreshControls(true);
}

//====================================================================================================================
// Handler for style sheet management
//====================================================================================================================

void DlgImageComposer::s_BlockShapeStyleBT() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::s_BlockShapeStyleBT");
    if ((InRefreshControls)||(BlockSelectMode!=SELECTMODE_ONE)||(!CurrentCompoObject)) return;
    AppendPartialUndo(UNDOACTION_STYLE_SHAPE,ui->InteractiveZone,false);
    QString ActualStyle =CurrentCompoObject->GetBlockShapeStyle();
    QString Item        =BaseApplicationConfig->StyleBlockShapeCollection.PopupCollectionMenu(this,BaseApplicationConfig,ActualStyle);
    ui->BlockShapeStyleBT->setDown(false);
    if (Item!="") {
        InRefreshControls=true;
        CurrentCompoObject->ApplyBlockShapeStyle(BaseApplicationConfig->StyleBlockShapeCollection.GetStyleDef(Item));
        CopyBlockProperties(CurrentCompoObject,GetGlobalCompositionObject(CurrentCompoObject->IndexKey));   // Apply to GlobalComposition objects
        RefreshBlockTable(CurrentCompoObjectNbr);
    }
}

//********************************************************************************************************************
//                                                  BLOCKS ALIGNMENT
//********************************************************************************************************************

void DlgImageComposer::s_BlockTable_AlignTop() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::s_BlockTable_AlignTop");
    AppendPartialUndo(UNDOACTION_BLOCKTABLE_ARRANGEBLOCK,ui->InteractiveZone,true);
    for (int i=0;i<IsSelected.count();i++) if (IsSelected[i]) CompositionList->List[i]->y=ui->InteractiveZone->Sel_Y;
    RefreshControls(true);
}

void DlgImageComposer::s_BlockTable_AlignMiddle() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::s_BlockTable_AlignMiddle");
    AppendPartialUndo(UNDOACTION_BLOCKTABLE_ARRANGEBLOCK,ui->InteractiveZone,true);
    for (int i=0;i<IsSelected.count();i++) if (IsSelected[i]) CompositionList->List[i]->y=(ui->InteractiveZone->Sel_Y+ui->InteractiveZone->Sel_H/2)-CompositionList->List[i]->h/2;
    RefreshControls(true);
}

void DlgImageComposer::s_BlockTable_AlignBottom() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::s_BlockTable_AlignBottom");
    AppendPartialUndo(UNDOACTION_BLOCKTABLE_ARRANGEBLOCK,ui->InteractiveZone,true);
    for (int i=0;i<IsSelected.count();i++) if (IsSelected[i]) CompositionList->List[i]->y=(ui->InteractiveZone->Sel_Y+ui->InteractiveZone->Sel_H)-CompositionList->List[i]->h;
    RefreshControls(true);
}

void DlgImageComposer::s_BlockTable_AlignLeft() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::s_BlockTable_AlignLeft");
    AppendPartialUndo(UNDOACTION_BLOCKTABLE_ARRANGEBLOCK,ui->InteractiveZone,true);
    for (int i=0;i<IsSelected.count();i++) if (IsSelected[i]) CompositionList->List[i]->x=ui->InteractiveZone->Sel_X;
    RefreshControls(true);
}

void DlgImageComposer::s_BlockTable_AlignCenter() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::s_BlockTable_AlignCenter");
    AppendPartialUndo(UNDOACTION_BLOCKTABLE_ARRANGEBLOCK,ui->InteractiveZone,true);
    for (int i=0;i<IsSelected.count();i++) if (IsSelected[i]) CompositionList->List[i]->x=(ui->InteractiveZone->Sel_X+ui->InteractiveZone->Sel_W/2)-CompositionList->List[i]->w/2;
    RefreshControls(true);
}

void DlgImageComposer::s_BlockTable_AlignRight() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::s_BlockTable_AlignRight");
    AppendPartialUndo(UNDOACTION_BLOCKTABLE_ARRANGEBLOCK,ui->InteractiveZone,true);
    for (int i=0;i<IsSelected.count();i++) if (IsSelected[i]) CompositionList->List[i]->x=(ui->InteractiveZone->Sel_X+ui->InteractiveZone->Sel_W)-CompositionList->List[i]->w;
    RefreshControls(true);
}

void DlgImageComposer::s_BlockTable_DistributeHoriz() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::s_BlockTable_DistributeHoriz");
    AppendPartialUndo(UNDOACTION_BLOCKTABLE_ARRANGEBLOCK,ui->InteractiveZone,true);

    // 1st step : compute available space and create list
    QList<SortBlock> List;
    qreal           SpaceW   =ui->InteractiveZone->Sel_W;
    qreal           CurrentX =ui->InteractiveZone->Sel_X;
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

void DlgImageComposer::s_BlockTable_DistributeVert() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::s_BlockTable_DistributeVert");
    AppendPartialUndo(UNDOACTION_BLOCKTABLE_ARRANGEBLOCK,ui->InteractiveZone,true);

    // 1st step : compute available space and create list
    QList<SortBlock> List;
    qreal   SpaceH   =ui->InteractiveZone->Sel_H;
    qreal   CurrentY =ui->InteractiveZone->Sel_Y;
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

//====================================================================================================================
// Display or hide rulers
//====================================================================================================================

void DlgImageComposer::s_RulersBt() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageComposer::s_RulersBt");

    DlgRulerDef Dlg(&ui->InteractiveZone->MagneticRuler,false,BaseApplicationConfig,this);
    Dlg.InitDialog();
    connect(&Dlg,SIGNAL(RefreshDisplay()),this,SLOT(s_RefreshSceneImage()));
    if (Dlg.exec()==0) {
        BaseApplicationConfig->ThumbRuler=ui->InteractiveZone->MagneticRuler;
        ui->RulersBT->setIcon(QIcon(QString(ui->InteractiveZone->MagneticRuler!=0?ICON_RULER_ON:ICON_RULER_OFF)));
    }
    ui->InteractiveZone->RefreshDisplay();
}
