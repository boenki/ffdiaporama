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

#include "cShotComposer.h"
#include "CustomCtrl/cCTexteFrameComboBox.h"
#include "CustomCtrl/cCShapeComboBox.h"
#include "CustomCtrl/cCColorComboBox.h"

#define ISVIDEO(OBJECT)                 ((OBJECT->MediaObject)&&(OBJECT->MediaObject->ObjectType==OBJECTTYPE_VIDEOFILE))
#define ISBLOCKVALIDE()                 ((!InRefreshControls)&&(BlockSelectMode==SELECTMODE_ONE)&&(CurrentCompoObject))
#define ISBLOCKVALIDEVISIBLE()          (ISBLOCKVALIDE()&&(CurrentCompoObject->IsVisible))
#define GETUI(WIDGETNAME)               findChild<QWidget *>(WIDGETNAME)
#define GETDOUBLESPINBOX(WIDGETNAME)    findChild<QDoubleSpinBox *>(WIDGETNAME)
#define GETSPINBOX(WIDGETNAME)          findChild<QSpinBox *>(WIDGETNAME)
#define GETSLIDER(WIDGETNAME)           findChild<QSlider *>(WIDGETNAME)
#define GETCOMBOBOX(WIDGETNAME)         findChild<QComboBox *>(WIDGETNAME)
#define GETBUTTON(WIDGETNAME)           findChild<QToolButton *>(WIDGETNAME)

//====================================================================================================================

cShotComposer::cShotComposer(cDiaporamaObject *DiaporamaObject,cBaseApplicationConfig *ApplicationConfig,QWidget *parent):QCustomDialog(ApplicationConfig,parent) {
    switch (DiaporamaObject->Parent->ImageGeometry) {
        case GEOMETRY_4_3:      DisplayW=1440;    DisplayH=1080;     break;
        case GEOMETRY_40_17:    DisplayW=1920;    DisplayH=816;      break;
        case GEOMETRY_16_9:
        default:                DisplayW=1920;    DisplayH=1080;     break;
    }
    CurrentSlide            =DiaporamaObject;
    ProjectGeometry         =DisplayH/DisplayW;
    ProjectGeometry         =GetDoubleValue(QString("%1").arg(ProjectGeometry,0,'e'));  // Rounded to same number as style managment
    TypeWindowState         =TypeWindowState_withsplitterpos;
    CurrentShotNbr          =0;
    CurrentCompoObject      =NULL;
    CurrentCompoObjectNbr   =-1;
    InRefreshControls       =false;
    InSelectionChange       =false;
    BlockSelectMode         =SELECTMODE_NONE;
    SelectionHaveLockBlock  =false;
}

//====================================================================================================================
// Initialise dialog
void cShotComposer::DoInitDialog() {
    Splitter->setCollapsible(0,false);
    Splitter->setCollapsible(1,false);
    InteractiveZone->DiaporamaObject            =CurrentSlide;
    InteractiveZone->BlockTable                 =BlockTable;
    BlockTable->ApplicationConfig               =ApplicationConfig;
    BlockTable->CurrentSlide                    =CurrentSlide;
    if (ShotTable) ShotTable->DiaporamaObject   =CurrentSlide;

    // Block settings : Coordinates
    if (ApplicationConfig->DisplayUnit==DISPLAYUNIT_PERCENT) {
        GETDOUBLESPINBOX("PosXEd")->setDecimals(2);     GETDOUBLESPINBOX("PosXEd")->setSingleStep(1);       GETDOUBLESPINBOX("PosXEd")->setSuffix("%");
        GETDOUBLESPINBOX("PosYEd")->setDecimals(2);     GETDOUBLESPINBOX("PosYEd")->setSingleStep(1);       GETDOUBLESPINBOX("PosYEd")->setSuffix("%");
        GETDOUBLESPINBOX("WidthEd")->setDecimals(2);    GETDOUBLESPINBOX("WidthEd")->setSingleStep(1);      GETDOUBLESPINBOX("WidthEd")->setSuffix("%");
        GETDOUBLESPINBOX("HeightEd")->setDecimals(2);   GETDOUBLESPINBOX("HeightEd")->setSingleStep(1);     GETDOUBLESPINBOX("HeightEd")->setSuffix("%");
    } else { // DisplayUnit==DISPLAYUNIT_PIXELS
        GETDOUBLESPINBOX("PosXEd")->setDecimals(0);     GETDOUBLESPINBOX("PosXEd")->setSingleStep(1);       GETDOUBLESPINBOX("PosXEd")->setSuffix("");
        GETDOUBLESPINBOX("PosYEd")->setDecimals(0);     GETDOUBLESPINBOX("PosYEd")->setSingleStep(1);       GETDOUBLESPINBOX("PosYEd")->setSuffix("");
        GETDOUBLESPINBOX("WidthEd")->setDecimals(0);    GETDOUBLESPINBOX("WidthEd")->setSingleStep(1);      GETDOUBLESPINBOX("WidthEd")->setSuffix("");
        GETDOUBLESPINBOX("HeightEd")->setDecimals(0);   GETDOUBLESPINBOX("HeightEd")->setSingleStep(1);     GETDOUBLESPINBOX("HeightEd")->setSuffix("");
    }
    connect(GETUI("PosXEd"),    SIGNAL(valueChanged(double)),this,SLOT(s_BlockSettings_PosXValue(double)));
    connect(GETUI("PosYEd"),    SIGNAL(valueChanged(double)),this,SLOT(s_BlockSettings_PosYValue(double)));
    connect(GETUI("WidthEd"),   SIGNAL(valueChanged(double)),this,SLOT(s_BlockSettings_PosWidthValue(double)));
    connect(GETUI("HeightEd"),  SIGNAL(valueChanged(double)),this,SLOT(s_BlockSettings_PosHeightValue(double)));

    // Block settings : Rotations
    GETSPINBOX("RotateXED")->setRange(-180,180);      GETSLIDER("RotateXSLD")->setRange(-180,180);
    GETSPINBOX("RotateYED")->setRange(-180,180);      GETSLIDER("RotateYSLD")->setRange(-180,180);
    GETSPINBOX("RotateZED")->setRange(-180,180);      GETSLIDER("RotateZSLD")->setRange(-180,180);
    connect(GETUI("RotateXED"),SIGNAL(valueChanged(int)),this,SLOT(s_BlockSettings_RotateXValue(int)));     connect(GETUI("RotateXSLD"),SIGNAL(valueChanged(int)),this,SLOT(s_BlockSettings_RotateXValue(int)));        connect(GETUI("ResetRotateXBT"),SIGNAL(released()),this,SLOT(s_BlockSettings_ResetRotateXValue()));
    connect(GETUI("RotateYED"),SIGNAL(valueChanged(int)),this,SLOT(s_BlockSettings_RotateYValue(int)));     connect(GETUI("RotateYSLD"),SIGNAL(valueChanged(int)),this,SLOT(s_BlockSettings_RotateYValue(int)));        connect(GETUI("ResetRotateYBT"),SIGNAL(released()),this,SLOT(s_BlockSettings_ResetRotateYValue()));
    connect(GETUI("RotateZED"),SIGNAL(valueChanged(int)),this,SLOT(s_BlockSettings_RotateZValue(int)));     connect(GETUI("RotateZSLD"),SIGNAL(valueChanged(int)),this,SLOT(s_BlockSettings_RotateZValue(int)));        connect(GETUI("ResetRotateZBT"),SIGNAL(released()),this,SLOT(s_BlockSettings_ResetRotateZValue()));


    // Init combo box Background form
    for (int i=0;i<ShapeFormDefinition.count();i++) if (ShapeFormDefinition.at(i).Enable) GETCOMBOBOX("BackgroundFormCB")->addItem(ShapeFormDefinition.at(i).Name,QVariant(i));
    MakeFormIcon(GETCOMBOBOX("BackgroundFormCB"));

    if (GETCOMBOBOX("ShadowEffectCB")->view()->width()<500)        GETCOMBOBOX("ShadowEffectCB")->view()->setFixedWidth(500);

    // Init combo box Background shadow form
    GETCOMBOBOX("ShadowEffectCB")->addItem(QApplication::translate("DlgSlideProperties","None"));
    GETCOMBOBOX("ShadowEffectCB")->addItem(QApplication::translate("DlgSlideProperties","Shadow upper left"));
    GETCOMBOBOX("ShadowEffectCB")->addItem(QApplication::translate("DlgSlideProperties","Shadow upper right"));
    GETCOMBOBOX("ShadowEffectCB")->addItem(QApplication::translate("DlgSlideProperties","Shadow bottom left"));
    GETCOMBOBOX("ShadowEffectCB")->addItem(QApplication::translate("DlgSlideProperties","Shadow bottom right"));
    GETSPINBOX("ShadowEffectED")->setRange(1,100);

    // Init combo box external border style
    GETCOMBOBOX("PenStyleCB")->addItem("");    GETCOMBOBOX("PenStyleCB")->setItemData(GETCOMBOBOX("PenStyleCB")->count()-1,(int)Qt::SolidLine);
    GETCOMBOBOX("PenStyleCB")->addItem("");    GETCOMBOBOX("PenStyleCB")->setItemData(GETCOMBOBOX("PenStyleCB")->count()-1,(int)Qt::DashLine);
    GETCOMBOBOX("PenStyleCB")->addItem("");    GETCOMBOBOX("PenStyleCB")->setItemData(GETCOMBOBOX("PenStyleCB")->count()-1,(int)Qt::DotLine);
    GETCOMBOBOX("PenStyleCB")->addItem("");    GETCOMBOBOX("PenStyleCB")->setItemData(GETCOMBOBOX("PenStyleCB")->count()-1,(int)Qt::DashDotLine);
    GETCOMBOBOX("PenStyleCB")->addItem("");    GETCOMBOBOX("PenStyleCB")->setItemData(GETCOMBOBOX("PenStyleCB")->count()-1,(int)Qt::DashDotDotLine);
    MakeBorderStyleIcon(GETCOMBOBOX("PenStyleCB"));

    // Init shape Borders
    GETSPINBOX("PenSizeEd")->setMinimum(0);    GETSPINBOX("PenSizeEd")->setMaximum(30);

    // Init combo box Background opacity
    GETCOMBOBOX("OpacityCB")->addItem("100%");
    GETCOMBOBOX("OpacityCB")->addItem(" 75%");
    GETCOMBOBOX("OpacityCB")->addItem(" 50%");
    GETCOMBOBOX("OpacityCB")->addItem(" 25%");
    connect(GETUI("ShapeSizePosCB"),    SIGNAL(currentIndexChanged(int)),   this,SLOT(s_BlockSettings_ShapeSizePos(int)));
    connect(GETUI("BackgroundFormCB"),  SIGNAL(itemSelectionHaveChanged()), this,SLOT(s_BlockSettings_ShapeBackgroundForm()));
    connect(GETUI("TextClipArtCB"),     SIGNAL(itemSelectionHaveChanged()), this,SLOT(s_BlockSettings_ShapeTextClipArtChIndex()));
    connect(GETUI("OpacityCB"),         SIGNAL(currentIndexChanged(int)),   this,SLOT(s_BlockSettings_ShapeOpacity(int)));
    connect(GETUI("PenStyleCB"),        SIGNAL(currentIndexChanged(int)),   this,SLOT(s_BlockSettings_ShapePenStyle(int)));
    connect(GETUI("ShadowEffectCB"),    SIGNAL(currentIndexChanged(int)),   this,SLOT(s_BlockSettings_ShapeShadowFormValue(int)));
    connect(GETUI("ShadowEffectED"),    SIGNAL(valueChanged(int)),          this,SLOT(s_BlockSettings_ShapeShadowDistanceValue(int)));
    connect(GETUI("PenColorCB"),        SIGNAL(currentIndexChanged(int)),   this,SLOT(s_BlockSettings_ShapePenColor(int)));
    connect(GETUI("PenSizeEd"),         SIGNAL(valueChanged(int)),          this,SLOT(s_BlockSettings_ShapePenSize(int)));
    connect(GETUI("ShadowColorCB"),     SIGNAL(currentIndexChanged(int)),   this,SLOT(s_BlockSettings_ShapeShadowColor(int)));
    connect(GETUI("BlockShapeStyleBT"), SIGNAL(pressed()),                  this,SLOT(s_BlockSettings_BlockShapeStyleBT()));
}

//====================================================================================================================
// Utility functions
//====================================================================================================================

// Fill background combobox
void cShotComposer::MakeFormIcon(QComboBox *UICB) {
    for (int i=0;i<UICB->count();i++) {
        cCompositionObject Object(COMPOSITIONTYPE_BACKGROUND,0,ApplicationConfig,this);
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

// Fill border combobox
void cShotComposer::MakeBorderStyleIcon(QComboBox *UICB) {
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

void cShotComposer::ComputeBlockRatio(cCompositionObject *Block,qreal &Ratio_X,qreal &Ratio_Y) {
    if (!Block) return;
    QRectF tmpRect=PolygonToRectF(ComputePolygon(Block->BackgroundForm,Block->x*InteractiveZone->DisplayW,Block->y*InteractiveZone->DisplayH,Block->w*InteractiveZone->DisplayW,Block->h*InteractiveZone->DisplayH));
    Ratio_X=(Block->w*InteractiveZone->DisplayW)/tmpRect.width();
    Ratio_Y=(Block->h*InteractiveZone->DisplayH)/tmpRect.height();
}

void cShotComposer::CopyBlockProperties(cCompositionObject *SourceBlock,cCompositionObject *DestBlock) {
    if (SourceBlock==DestBlock) return;

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

void cShotComposer::ApplyGlobalPropertiesToAllShots(cCompositionObject *GlobalBlock) {
    // Apply to GlobalComposition objects
    for (int j=0;j<CurrentSlide->ObjectComposition.List.count();j++) if (GlobalBlock->IndexKey==CurrentSlide->ObjectComposition.List[j]->IndexKey)
        CopyBlockProperties(GlobalBlock,CurrentSlide->ObjectComposition.List[j]);

    // Apply to Shots Composition objects
    for (int i=0;i<CurrentSlide->List.count();i++) for (int j=0;j<CurrentSlide->List[i]->ShotComposition.List.count();j++) if (GlobalBlock->IndexKey==CurrentSlide->List[i]->ShotComposition.List[j]->IndexKey)
        CopyBlockProperties(GlobalBlock,CurrentSlide->List[i]->ShotComposition.List[j]);
}

void cShotComposer::ApplyToContexte(bool ApplyGlobal) {
    if ((ApplyGlobal)&&(CurrentCompoObject)) ApplyGlobalPropertiesToAllShots(CurrentCompoObject);

    // Apply values of previous shot to all shot for all objects
    for (int ShotNum=CurrentShotNbr+1;ShotNum<CurrentSlide->List.count();ShotNum++) for (int Block=0;Block<CurrentSlide->List[CurrentShotNbr]->ShotComposition.List.count();Block++) {
        cCompositionObject *ShotObject=NULL;
        for (int i=0;i<CurrentSlide->List[ShotNum]->ShotComposition.List.count();i++)
            if (CurrentSlide->List[ShotNum]->ShotComposition.List[i]->IndexKey==CurrentSlide->List[CurrentShotNbr]->ShotComposition.List[Block]->IndexKey)
                ShotObject=CurrentSlide->List[ShotNum]->ShotComposition.List[i];
        if ((ShotObject!=NULL)&&(ShotObject->SameAsPrevShot)) ShotObject->CopyFromCompositionObject(CurrentSlide->List[CurrentShotNbr]->ShotComposition.List[Block]);
    }
    for (int i=CurrentShotNbr;i<CurrentSlide->List.count();i++) {
        if (i==0) ApplicationConfig->SlideThumbsTable->ClearThumbs(CurrentSlide->ThumbnailKey);
        if (ShotTable) ShotTable->RepaintCell(i);
    }
    RefreshControls(true);
}

cCompositionObject *cShotComposer::GetGlobalCompositionObject(int IndexKey) {
    int CurrentBlock=BlockTable->currentRow();
    if ((CurrentBlock<0)||(CurrentBlock>=CompositionList->List.count())) return NULL;
    int i=0;
    while ((i<CurrentSlide->ObjectComposition.List.count())&&(CurrentSlide->ObjectComposition.List[i]->IndexKey!=IndexKey)) i++;
    if (i<CurrentSlide->ObjectComposition.List.count()) return CurrentSlide->ObjectComposition.List[i]; else return NULL;
}

//====================================================================================================================

void cShotComposer::RefreshControls(bool UpdateInteractiveZone) {
    InRefreshControls=true;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    if ((BlockSelectMode==SELECTMODE_ONE)&&(CurrentCompoObject)&&(CurrentCompoObject->IsVisible)) {

        qreal Ratio_X,Ratio_Y;
        ComputeBlockRatio(CurrentCompoObject,Ratio_X,Ratio_Y);

        // Position, size and rotation
        GETUI("PosSize_X")->     setEnabled(true);
        GETUI("PosSize_Y")->     setEnabled(true);
        GETUI("PosSize_Width")-> setEnabled(true);
        GETUI("PosSize_Height")->setEnabled(true);
        GETDOUBLESPINBOX("PosXEd")->        setEnabled(!SelectionHaveLockBlock);
        GETDOUBLESPINBOX("PosYEd")->        setEnabled(!SelectionHaveLockBlock);
        GETDOUBLESPINBOX("WidthEd")->       setEnabled(!SelectionHaveLockBlock);
        GETDOUBLESPINBOX("HeightEd")->      setEnabled(!SelectionHaveLockBlock);

        if (ApplicationConfig->DisplayUnit==DISPLAYUNIT_PERCENT) {
            GETDOUBLESPINBOX("PosXEd")->  setRange(-200,200);                                                  GETDOUBLESPINBOX("PosXEd")->  setValue(CurrentCompoObject->x*100/Ratio_X);
            GETDOUBLESPINBOX("PosYEd")->  setRange(-200,200);                                                  GETDOUBLESPINBOX("PosYEd")->  setValue(CurrentCompoObject->y*100/Ratio_Y);
            GETDOUBLESPINBOX("WidthEd")-> setRange(1,200);                                                     GETDOUBLESPINBOX("WidthEd")-> setValue(CurrentCompoObject->w*100/Ratio_X);
            GETDOUBLESPINBOX("HeightEd")->setRange(1,200);                                                     GETDOUBLESPINBOX("HeightEd")->setValue(CurrentCompoObject->h*100/Ratio_Y);
        } else { // DisplayUnit==DISPLAYUNIT_PIXELS
            GETDOUBLESPINBOX("PosXEd")->  setRange(-2*InteractiveZone->DisplayW,2*InteractiveZone->DisplayW);  GETDOUBLESPINBOX("PosXEd")->  setValue(CurrentCompoObject->x*InteractiveZone->DisplayW/Ratio_X);
            GETDOUBLESPINBOX("PosYEd")->  setRange(-2*InteractiveZone->DisplayH,2*InteractiveZone->DisplayH);  GETDOUBLESPINBOX("PosYEd")->  setValue(CurrentCompoObject->y*InteractiveZone->DisplayH/Ratio_Y);
            GETDOUBLESPINBOX("WidthEd")-> setRange(3,2*InteractiveZone->DisplayW);                             GETDOUBLESPINBOX("WidthEd")-> setValue(CurrentCompoObject->w*InteractiveZone->DisplayW/Ratio_X);
            GETDOUBLESPINBOX("HeightEd")->setRange(3,2*InteractiveZone->DisplayH);                             GETDOUBLESPINBOX("HeightEd")->setValue(CurrentCompoObject->h*InteractiveZone->DisplayH/Ratio_Y);
        }

        // Rotation
        GETUI("Rotate_X")-> setEnabled(!SelectionHaveLockBlock);
        GETSPINBOX("RotateXED")-> setEnabled(!SelectionHaveLockBlock);
        GETUI("ResetRotateXBT")->setEnabled(!SelectionHaveLockBlock);
        GETSLIDER("RotateXSLD")->setEnabled(!SelectionHaveLockBlock);
        GETUI("Rotate_Y")-> setEnabled(!SelectionHaveLockBlock);
        GETSPINBOX("RotateYED")-> setEnabled(!SelectionHaveLockBlock);
        GETUI("ResetRotateYBT")->setEnabled(!SelectionHaveLockBlock);
        GETSLIDER("RotateYSLD")->setEnabled(!SelectionHaveLockBlock);
        GETUI("Rotate_Z")-> setEnabled(!SelectionHaveLockBlock);
        GETSPINBOX("RotateZED")-> setEnabled(!SelectionHaveLockBlock);
        GETUI("ResetRotateZBT")->setEnabled(!SelectionHaveLockBlock);
        GETSLIDER("RotateZSLD")->setEnabled(!SelectionHaveLockBlock);

        GETSPINBOX("RotateXED")->setValue(CurrentCompoObject->RotateXAxis);                       GETSLIDER("RotateXSLD")->setValue(CurrentCompoObject->RotateXAxis);
        GETSPINBOX("RotateYED")->setValue(CurrentCompoObject->RotateYAxis);                       GETSLIDER("RotateYSLD")->setValue(CurrentCompoObject->RotateYAxis);
        GETSPINBOX("RotateZED")->setValue(CurrentCompoObject->RotateZAxis);                       GETSLIDER("RotateZSLD")->setValue(CurrentCompoObject->RotateZAxis);

        // Shape part
        GETUI("BlockShapeStyleBT")->setEnabled(!SelectionHaveLockBlock);
        GETUI("BlockShapeStyleED")->setEnabled(!SelectionHaveLockBlock);
        GETUI("BackgroundFormCB")->setEnabled(!SelectionHaveLockBlock);
        GETUI("PenSizeEd")->       setEnabled(!SelectionHaveLockBlock);
        GETUI("PenColorCB")->      setEnabled(!SelectionHaveLockBlock && (CurrentCompoObject->PenSize!=0));
        GETUI("PenStyleCB")->      setEnabled(!SelectionHaveLockBlock && (CurrentCompoObject->PenSize!=0));
        GETUI("OpacityCB")->       setEnabled(!SelectionHaveLockBlock);
        GETUI("ShadowEffectCB")->  setEnabled(!SelectionHaveLockBlock);
        GETUI("ShadowEffectED")->  setEnabled(!SelectionHaveLockBlock && (CurrentCompoObject->FormShadow!=0));
        GETUI("ShadowColorCB")->   setEnabled(!SelectionHaveLockBlock && (CurrentCompoObject->FormShadow!=0));

        SetCBIndex(GETCOMBOBOX("BackgroundFormCB"),CurrentCompoObject->BackgroundForm);
        GETSPINBOX("PenSizeEd")->      setValue(int(CurrentCompoObject->PenSize));
        GETCOMBOBOX("OpacityCB")->     setCurrentIndex(CurrentCompoObject->Opacity);
        GETCOMBOBOX("ShadowEffectCB")->setCurrentIndex(CurrentCompoObject->FormShadow);
        GETSPINBOX("ShadowEffectED")-> setValue(int(CurrentCompoObject->FormShadowDistance));
        ((cCColorComboBox *)GETCOMBOBOX("PenColorCB"))->    SetCurrentColor(&CurrentCompoObject->PenColor);
        ((cCColorComboBox *)GETCOMBOBOX("ShadowColorCB"))-> SetCurrentColor(&CurrentCompoObject->FormShadowColor);

        for (int i=0;i<GETCOMBOBOX("PenStyleCB")->count();i++)
          if (GETCOMBOBOX("PenStyleCB")->itemData(i).toInt()==CurrentCompoObject->PenStyle) {
            if (i!=GETCOMBOBOX("PenStyleCB")->currentIndex()) GETCOMBOBOX("PenStyleCB")->setCurrentIndex(i);
            break;
        }

    } else {

        // Position, size and rotation
        GETUI("PosSize_X")->          setEnabled(false);
        GETUI("PosSize_Y")->          setEnabled(false);
        GETUI("PosSize_Width")->      setEnabled(false);
        GETUI("PosSize_Height")->     setEnabled(false);
        GETDOUBLESPINBOX("PosXEd")->  setEnabled(false);          GETDOUBLESPINBOX("PosXEd")->  setValue(0);
        GETDOUBLESPINBOX("PosYEd")->  setEnabled(false);          GETDOUBLESPINBOX("PosYEd")->  setValue(0);
        GETDOUBLESPINBOX("WidthEd")-> setEnabled(false);          GETDOUBLESPINBOX("WidthEd")-> setValue(0);      GETDOUBLESPINBOX("WidthEd")->setRange(0,2*InteractiveZone->DisplayW);
        GETDOUBLESPINBOX("HeightEd")->setEnabled(false);          GETDOUBLESPINBOX("HeightEd")->setValue(0);      GETDOUBLESPINBOX("HeightEd")->setRange(0,2*InteractiveZone->DisplayH);


        // Rotation
        GETUI("Rotate_X")-> setEnabled(false);                  GETSPINBOX("RotateXED")-> setEnabled(false);      GETUI("ResetRotateXBT")->setEnabled(false);       GETSLIDER("RotateXSLD")->setEnabled(false);
        GETUI("Rotate_Y")-> setEnabled(false);                  GETSPINBOX("RotateYED")-> setEnabled(false);      GETUI("ResetRotateYBT")->setEnabled(false);       GETSLIDER("RotateYSLD")->setEnabled(false);
        GETUI("Rotate_Z")-> setEnabled(false);                  GETSPINBOX("RotateZED")-> setEnabled(false);      GETUI("ResetRotateZBT")->setEnabled(false);       GETSLIDER("RotateZSLD")->setEnabled(false);

        GETSPINBOX("RotateXED")->setValue(0);                   GETSLIDER("RotateXSLD")->setValue(0);
        GETSPINBOX("RotateYED")->setValue(0);                   GETSLIDER("RotateYSLD")->setValue(0);
        GETSPINBOX("RotateZED")->setValue(0);                   GETSLIDER("RotateZSLD")->setValue(0);

        // Shape part
        GETUI("BlockShapeStyleBT")-> setEnabled(false);
        GETUI("BlockShapeStyleED")-> setEnabled(false);
        GETUI("BackgroundFormCB")->  setEnabled(false);         GETCOMBOBOX("BackgroundFormCB")->  setCurrentIndex(-1);
        GETUI("PenSizeEd")->         setEnabled(false);         GETSPINBOX("PenSizeEd")->setValue(0);
        GETUI("PenColorCB")->        setEnabled(false);         ((cCColorComboBox *)GETCOMBOBOX("PenColorCB"))->SetCurrentColor(NULL);
        GETUI("PenStyleCB")->        setEnabled(false);
        GETUI("OpacityCB")->         setEnabled(false);         GETCOMBOBOX("OpacityCB")->     setCurrentIndex(-1);
        GETUI("ShadowEffectCB")->    setEnabled(false);         GETCOMBOBOX("ShadowEffectCB")->setCurrentIndex(-1);
        GETUI("ShadowEffectED")->    setEnabled(false);         GETSPINBOX("ShadowEffectED")-> setValue(0);
        GETUI("ShadowColorCB")->     setEnabled(false);         ((cCColorComboBox *)GETCOMBOBOX("ShadowColorCB"))-> SetCurrentColor(NULL);
    }

    // Set control visible or hide depending on TextClipArt
    GETUI("BlockShapeStyleBT")->setVisible(((!CurrentCompoObject)||(CurrentCompoObject->TextClipArtName=="")));
    GETUI("BlockShapeStyleSpacer")->setVisible(((!CurrentCompoObject)||(CurrentCompoObject->TextClipArtName=="")));
    GETUI("BlockShapeStyleED")->setVisible(((!CurrentCompoObject)||(CurrentCompoObject->TextClipArtName=="")));
    GETUI("BackgroundFormCB")->setVisible(((!CurrentCompoObject)||(CurrentCompoObject->TextClipArtName=="")));
    GETUI("BackgroundFormLabel")->setVisible(((!CurrentCompoObject)||(CurrentCompoObject->TextClipArtName=="")));
    GETUI("PenSizeEd")->setVisible(((!CurrentCompoObject)||(CurrentCompoObject->TextClipArtName=="")));
    GETUI("PenColorCB")->setVisible(((!CurrentCompoObject)||(CurrentCompoObject->TextClipArtName=="")));
    GETUI("PenStyleCB")->setVisible(((!CurrentCompoObject)||(CurrentCompoObject->TextClipArtName=="")));
    GETUI("PenLabel")->setVisible(((!CurrentCompoObject)||(CurrentCompoObject->TextClipArtName=="")));
    GETUI("TextClipArtCB")->setVisible((CurrentCompoObject)&&(CurrentCompoObject->TextClipArtName!=""));
    GETUI("TextClipArtLabel")->setVisible((CurrentCompoObject)&&(CurrentCompoObject->TextClipArtName!=""));
    if ((CurrentCompoObject)&&(CurrentCompoObject->TextClipArtName!="")) ((cCTexteFrameComboBox *)GETCOMBOBOX("TextClipArtCB"))->SetCurrentTextFrame(CurrentCompoObject->TextClipArtName);

    InRefreshControls=false;
    QApplication::restoreOverrideCursor();

    // Refresh interactive zone display and shot thumbnail
    if (UpdateInteractiveZone)  InteractiveZone->RefreshDisplay();
        else                    InteractiveZone->repaint(); // else refresh selection only
}

//====================================================================================================================
// Handler for position, size & rotation controls
//====================================================================================================================

//========= X position
void cShotComposer::s_BlockSettings_PosXValue(double Value) {
    if (!ISBLOCKVALIDEVISIBLE()) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_POSX,"PosXEd",false);
    if (ApplicationConfig->DisplayUnit==DISPLAYUNIT_PERCENT)    CurrentCompoObject->x=Value/100;           // DisplayUnit==DISPLAYUNIT_PERCENT
        else                                                    CurrentCompoObject->x=(Value/InteractiveZone->DisplayW);    // DisplayUnit==DISPLAYUNIT_PIXELS
    ApplyToContexte(false);
}

//========= Y position
void cShotComposer::s_BlockSettings_PosYValue(double Value) {
    if (!ISBLOCKVALIDEVISIBLE()) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_POSY,"PosYEd",false);
    if (ApplicationConfig->DisplayUnit==DISPLAYUNIT_PERCENT)    CurrentCompoObject->y=Value/100;           // DisplayUnit==DISPLAYUNIT_PERCENT
        else                                                    CurrentCompoObject->y=(Value/InteractiveZone->DisplayH);    // DisplayUnit==DISPLAYUNIT_PIXELS
    ApplyToContexte(false);
}

//========= Width
void cShotComposer::s_BlockSettings_PosWidthValue(double Value) {
    if (!ISBLOCKVALIDEVISIBLE()) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_WIDTH,"WidthEd",false);

    qreal Ratio_X,Ratio_Y;
    ComputeBlockRatio(CurrentCompoObject,Ratio_X,Ratio_Y);

    if (ApplicationConfig->DisplayUnit==DISPLAYUNIT_PERCENT)    CurrentCompoObject->w=(Value/100)*Ratio_X;
        else                                                    CurrentCompoObject->w=(Value/InteractiveZone->DisplayW)*Ratio_X;
    if (CurrentCompoObject->BackgroundBrush->LockGeometry)      CurrentCompoObject->h=((CurrentCompoObject->w*InteractiveZone->DisplayW)*CurrentCompoObject->BackgroundBrush->AspectRatio)/InteractiveZone->DisplayH;
        else                                                    CurrentCompoObject->BackgroundBrush->AspectRatio=(CurrentCompoObject->h*InteractiveZone->DisplayH)/(CurrentCompoObject->w*InteractiveZone->DisplayW);
    ApplyToContexte(false);
}

//========= Height
void cShotComposer::s_BlockSettings_PosHeightValue(double Value) {
    if (!ISBLOCKVALIDEVISIBLE()) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_HEIGHT,"HeightEd",false);

    qreal Ratio_X,Ratio_Y;
    ComputeBlockRatio(CurrentCompoObject,Ratio_X,Ratio_Y);

    if (ApplicationConfig->DisplayUnit==DISPLAYUNIT_PERCENT)    CurrentCompoObject->h=(Value/100)*Ratio_Y;
        else                                                    CurrentCompoObject->h=(Value/InteractiveZone->DisplayH)*Ratio_Y;
    if (CurrentCompoObject->BackgroundBrush->LockGeometry)      CurrentCompoObject->w=((CurrentCompoObject->h*InteractiveZone->DisplayH)/CurrentCompoObject->BackgroundBrush->AspectRatio)/InteractiveZone->DisplayW;
        else                                                    CurrentCompoObject->BackgroundBrush->AspectRatio=(CurrentCompoObject->h*InteractiveZone->DisplayH)/(CurrentCompoObject->w*InteractiveZone->DisplayW);
    ApplyToContexte(false);
}

//========= X Rotation value
void cShotComposer::s_BlockSettings_RotateXValue(int Value) {
    if (!ISBLOCKVALIDEVISIBLE()) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_ROTATEX,"RotateXED",false);
    CurrentCompoObject->RotateXAxis=Value;
    ApplyToContexte(false);
}

//========= Y Rotation value
void cShotComposer::s_BlockSettings_RotateYValue(int Value) {
    if (!ISBLOCKVALIDEVISIBLE()) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_ROTATEY,"RotateYED",false);
    CurrentCompoObject->RotateYAxis=Value;
    ApplyToContexte(false);
}

//========= Z Rotation value
void cShotComposer::s_BlockSettings_RotateZValue(int Value) {
    if (!ISBLOCKVALIDEVISIBLE()) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_ROTATEZ,"RotateZED",false);
    CurrentCompoObject->RotateZAxis=Value;
    ApplyToContexte(false);
}

//====================================================================================================================
// Handler for shape
//====================================================================================================================

void cShotComposer::s_BlockSettings_BlockShapeStyleBT() {
    if (!ISBLOCKVALIDEVISIBLE()) return;
    AppendPartialUndo(UNDOACTION_STYLE_SHAPE,InteractiveZone,false);
    QString ActualStyle =CurrentCompoObject->GetBlockShapeStyle();
    QString Item        =ApplicationConfig->StyleBlockShapeCollection.PopupCollectionMenu(this,ApplicationConfig,ActualStyle);
    GETBUTTON("BlockShapeStyleBT")->setDown(false);
    if (Item!="") {
        CurrentCompoObject->ApplyBlockShapeStyle(ApplicationConfig->StyleBlockShapeCollection.GetStyleDef(Item));
        ApplyToContexte(true);
        RefreshBlockTable(CurrentCompoObjectNbr);
        if (ShotTable) {
            ShotTable->setUpdatesEnabled(false);
            ShotTable->setUpdatesEnabled(true);
        }
    }
}

//========= Text ClipArt
void cShotComposer::s_BlockSettings_ShapeTextClipArtChIndex() {
    if (!ISBLOCKVALIDEVISIBLE()) return;
    if (CurrentCompoObject->TextClipArtName=="") return;
    AppendPartialUndo(UNDOACTION_EDITZONE_TEXTCLIPART,"TextClipArtCB",false);
    CurrentCompoObject->TextClipArtName=((cCTexteFrameComboBox *)GETCOMBOBOX("TextClipArtCB"))->GetCurrentTextFrame();
    cTextFrameObject *TFO=&TextFrameList.List[TextFrameList.SearchImage(CurrentCompoObject->TextClipArtName)];
    CurrentCompoObject->TMx=TFO->TMx;
    CurrentCompoObject->TMy=TFO->TMy;
    CurrentCompoObject->TMw=TFO->TMw;
    CurrentCompoObject->TMh=TFO->TMh;
    CurrentCompoObject->ApplyTextStyle(TFO->TextStyle);
    ApplyToContexte(true);
    RefreshBlockTable(CurrentCompoObjectNbr);
    if (ShotTable) {
        ShotTable->setUpdatesEnabled(false);
        ShotTable->setUpdatesEnabled(true);
    }
}

//========= Background forme
void cShotComposer::s_BlockSettings_ShapeBackgroundForm() {
    if (!ISBLOCKVALIDEVISIBLE()) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_SHAPEFORM,"BackgroundFormCB",false);
    CurrentCompoObject->BackgroundForm=((cCShapeComboBox *)GETCOMBOBOX("BackgroundFormCB"))->GetCurrentFrameShape();
    ApplyToContexte(true);
    RefreshBlockTable(CurrentCompoObjectNbr);
    if (ShotTable) {
        ShotTable->setUpdatesEnabled(false);
        ShotTable->setUpdatesEnabled(true);
    }
}

//========= Opacity
void cShotComposer::s_BlockSettings_ShapeOpacity(int Style) {
    if (!ISBLOCKVALIDEVISIBLE()) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_SHAPEOPACITY,"OpacityCB",false);
    CurrentCompoObject->Opacity=Style;
    ApplyToContexte(true);
    RefreshBlockTable(CurrentCompoObjectNbr);
    if (ShotTable) {
        ShotTable->setUpdatesEnabled(false);
        ShotTable->setUpdatesEnabled(true);
    }
}

//========= Border pen size
void cShotComposer::s_BlockSettings_ShapePenSize(int) {
    if (!ISBLOCKVALIDEVISIBLE()) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_SHAPEPENSIZE,"PenSizeEd",false);
    CurrentCompoObject->PenSize=GETSPINBOX("PenSizeEd")->value();
    GETCOMBOBOX("PenColorCB")->setEnabled(CurrentCompoObject->PenSize!=0);
    GETCOMBOBOX("PenStyleCB")->setEnabled(CurrentCompoObject->PenSize!=0);
    ApplyToContexte(true);
    RefreshBlockTable(CurrentCompoObjectNbr);
    if (ShotTable) {
        ShotTable->setUpdatesEnabled(false);
        ShotTable->setUpdatesEnabled(true);
    }
}

//========= Border pen style
void cShotComposer::s_BlockSettings_ShapePenStyle(int index) {
    if (!ISBLOCKVALIDEVISIBLE()) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_SHAPEPENSTYLE,"PenStyleCB",false);
    CurrentCompoObject->PenStyle=GETCOMBOBOX("PenStyleCB")->itemData(index).toInt();
    ApplyToContexte(true);
    RefreshBlockTable(CurrentCompoObjectNbr);
    if (ShotTable) {
        ShotTable->setUpdatesEnabled(false);
        ShotTable->setUpdatesEnabled(true);
    }
}

//========= Border pen color
void cShotComposer::s_BlockSettings_ShapePenColor(int) {
    if (!ISBLOCKVALIDEVISIBLE()) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_SHAPEPENCOLOR,"PenColorCB",false);
    CurrentCompoObject->PenColor=((cCColorComboBox *)GETCOMBOBOX("PenColorCB"))->GetCurrentColor();
    ApplyToContexte(true);
    RefreshBlockTable(CurrentCompoObjectNbr);
    if (ShotTable) {
        ShotTable->setUpdatesEnabled(false);
        ShotTable->setUpdatesEnabled(true);
    }
}

//========= Shape shadow style
void cShotComposer::s_BlockSettings_ShapeShadowFormValue(int value) {
    if (!ISBLOCKVALIDEVISIBLE()) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_SHAPESHADOWFORM,"ShadowEffectCB",false);
    CurrentCompoObject->FormShadow=value;
    GETSPINBOX("ShadowEffectED")->setEnabled(CurrentCompoObject->FormShadow!=0);
    GETCOMBOBOX("ShadowColorCB")->setEnabled(CurrentCompoObject->FormShadow!=0);
    ApplyToContexte(true);
    RefreshBlockTable(CurrentCompoObjectNbr);
    if (ShotTable) {
        ShotTable->setUpdatesEnabled(false);
        ShotTable->setUpdatesEnabled(true);
    }
}

//========= Shape shadow distance
void cShotComposer::s_BlockSettings_ShapeShadowDistanceValue(int value) {
    if (!ISBLOCKVALIDEVISIBLE()) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_SHAPESHADOWDIST,GETSPINBOX("ShadowColorCB"),false);
    CurrentCompoObject->FormShadowDistance =value;
    ApplyToContexte(true);
    RefreshBlockTable(CurrentCompoObjectNbr);
    if (ShotTable) {
        ShotTable->setUpdatesEnabled(false);
        ShotTable->setUpdatesEnabled(true);
    }
}

//========= shadow color
void cShotComposer::s_BlockSettings_ShapeShadowColor(int) {
    if (!ISBLOCKVALIDEVISIBLE()) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_SHAPESHADOWCOLOR,"ShadowColorCB",false);
    CurrentCompoObject->FormShadowColor=((cCColorComboBox *)GETCOMBOBOX("ShadowColorCB"))->GetCurrentColor();
    ApplyToContexte(true);
    RefreshBlockTable(CurrentCompoObjectNbr);
    if (ShotTable) {
        ShotTable->setUpdatesEnabled(false);
        ShotTable->setUpdatesEnabled(true);
    }
}

//====================================================================================================================
// BLOCK TABLE SETTINGS
//====================================================================================================================

//=========  Refresh the BlockTable
void cShotComposer::RefreshBlockTable(int SetCurrentIndex) {
    BlockTable->setUpdatesEnabled(false);
    BlockTable->setRowCount(CompositionList->List.count());
    for (int i=0;i<BlockTable->rowCount();i++) BlockTable->setRowHeight(i,48+2+((((cBaseApplicationConfig *)ApplicationConfig)->TimelineHeight-TIMELINEMINHEIGH)/20+1)*3);
    BlockTable->setUpdatesEnabled(true);
    if (BlockTable->currentRow()!=SetCurrentIndex) {
        BlockTable->clearSelection();
        BlockTable->setCurrentCell(SetCurrentIndex,0,QItemSelectionModel::Select|QItemSelectionModel::Current);
    } else s_BlockTable_SelectionChanged();
    if (BlockTable->rowCount()==0) s_BlockTable_SelectionChanged();
    InteractiveZone->RefreshDisplay();
}

//========= block selection change
void cShotComposer::s_BlockTable_SelectionChanged() {
    if (InSelectionChange) return;

    QModelIndexList SelList=BlockTable->selectionModel()->selectedIndexes();

    IsSelected.clear();
    for (int i=0;i<BlockTable->rowCount();i++)  IsSelected.append(false);
    for (int i=0;i<SelList.count();i++)         IsSelected[SelList.at(i).row()]=true;

    NbrSelected             =0;
    CurrentCompoObjectNbr   =-1;
    CurrentCompoObject      =NULL;
    SelectionHaveLockBlock  =false;

    for (int i=0;i<IsSelected.count();i++) if (IsSelected[i]) {
        if (NbrSelected==0) {
            CurrentCompoObjectNbr=i;
            CurrentCompoObject   =CompositionList->List[CurrentCompoObjectNbr];
        }
        if ((CurrentShotNbr>0)&&(CurrentCompoObject->SameAsPrevShot)) SelectionHaveLockBlock=true;
        NbrSelected++;
    }
    if (NbrSelected==0)             BlockSelectMode=SELECTMODE_NONE;
        else if (NbrSelected==1)    BlockSelectMode=SELECTMODE_ONE;
        else                        BlockSelectMode=SELECTMODE_MULTIPLE;

    RefreshControls(false);
}
