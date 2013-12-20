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

#ifndef CSHOTCOMPOSER_H
#define CSHOTCOMPOSER_H

// Basic inclusions (common to all files)
#include "CustomCtrl/_QCustomDialog.h"
#include "engine/_Diaporama.h"
#include "DlgSlide/cInteractiveZone.h"
#include "DlgSlide/cCustomBlockTable.h"
#include "DlgSlide/cCustomShotTable.h"

class cShotComposer : public QCustomDialog {
Q_OBJECT
public:
    // Undo actions
    enum UNDOACTION_ID {
        UNDOACTION_FULL_SLIDE
    };

    bool                    InRefreshControls;
    bool                    InSelectionChange;
    bool                    NoPrepUndo;

    double                  DisplayW,DisplayH;
    cDiaporamaObject        *CurrentSlide;              // Current slide
    double                  ProjectGeometry;
    cInteractiveZone        *InteractiveZone;
    cCustomBlockTable       *BlockTable;
    cCustomShotTable        *ShotTable;
    cCompositionObject      *CurrentCompoObject;        // Current block object (if selection mode = SELECTMODE_ONE)
    int                     CurrentCompoObjectNbr;      // Number of Current block object (if selection mode = SELECTMODE_ONE)
    int                     CurrentShotNbr;             // Current shot number (if selection mode = SELECTMODE_ONE)
    SELECTMODE              BlockSelectMode;            // Current block selection mode
    cCompositionList        *CompositionList;           // Link to current block List
    QList<bool>             IsSelected;                 // Table of selection state in the current block list
    int                     NbrSelected;                // Number of selected blocks

    explicit                    cShotComposer(cDiaporamaObject *DiaporamaObject,cBaseApplicationConfig *ApplicationConfig,QWidget *parent = 0);
    virtual void                DoInitDialog();

    // Utility function used to apply modification from one shot to next shot and/or global composition
    virtual void                CopyBlockProperties(cCompositionObject *SourceBlock,cCompositionObject *DestBlock);
    virtual void                ResetThumbs(bool ResetAllThumbs);
    virtual void                ApplyToContexte(bool ResetAllThumbs);
    virtual cCompositionObject  *GetGlobalCompositionObject(int IndexKey);      // Return CompositionObject in the global composition list for specific IndexKey

    // Update embedded controls
    virtual void                RefreshControls(bool UpdateInteractiveZone=true);

protected slots:
    void                        s_Event_ClipboardChanged();

    void                        s_BlockSettings_BlockInheritances();

    // Block settings : Coordinates
    void                        s_BlockSettings_PosXValue(double);
    void                        s_BlockSettings_PosYValue(double);
    void                        s_BlockSettings_PosWidthValue(double);
    void                        s_BlockSettings_PosHeightValue(double);

    // Block settings : Rotations
    void                        s_BlockSettings_RotateZValue(int);
    void                        s_BlockSettings_RotateXValue(int);
    void                        s_BlockSettings_RotateYValue(int);
    void                        s_BlockSettings_ResetRotateXValue()     { s_BlockSettings_RotateXValue(0); }
    void                        s_BlockSettings_ResetRotateYValue()     { s_BlockSettings_RotateYValue(0); }
    void                        s_BlockSettings_ResetRotateZValue()     { s_BlockSettings_RotateZValue(0); }

    // Block settings : Shape
    void                        s_BlockSettings_BlockShapeStyleBT();
    void                        s_BlockSettings_ShapeTextClipArtChIndex();
    void                        s_BlockSettings_ShapeBackgroundForm();
    void                        s_BlockSettings_ShapeOpacity(int);
    void                        s_BlockSettings_ShapeShadowFormValue(int);
    void                        s_BlockSettings_ShapeShadowDistanceValue(int);
    void                        s_BlockSettings_ShapePenSize(int);
    void                        s_BlockSettings_ShapePenColor(int);
    void                        s_BlockSettings_ShapePenStyle(int);
    void                        s_BlockSettings_ShapeShadowColor(int);

    // Block settings : Alignment
    void                        s_BlockTable_AlignTop();
    void                        s_BlockTable_AlignMiddle();
    void                        s_BlockTable_AlignBottom();
    void                        s_BlockTable_AlignLeft();
    void                        s_BlockTable_AlignCenter();
    void                        s_BlockTable_AlignRight();
    void                        s_BlockTable_DistributeHoriz();
    void                        s_BlockTable_DistributeVert();

    // Block table
    void                        s_BlockTable_SelectionChanged();            // User select a block in the BlocTable widget
    void                        s_BlockTable_MoveBlockUp();
    void                        s_BlockTable_MoveBlockDown();
    void                        s_BlockTable_DragMoveBlock(int,int);
    void                        s_BlockTable_RemoveBlock();

    void                        s_BlockSettings_Information();
    void                        s_BlockSettings_TextEditor();

    // Block settings/Interactive zone messages
    void                        s_BlockSettings_IntZoneTransformBlocks(qreal DeltaX,qreal DeltaY,qreal ScaleX,qreal ScaleY,qreal Sel_X,qreal Sel_Y,qreal Sel_W,qreal Sel_H);
    void                        s_BlockSettings_IntZoneDisplayTransformBlocks(qreal DeltaX,qreal DeltaY,qreal ScaleX,qreal ScaleY,qreal Sel_X,qreal Sel_Y,qreal Sel_W,qreal Sel_H);

protected:
    void                        ComputeBlockRatio(cCompositionObject *Block,qreal &Ratio_X,qreal &Ratio_Y);
    void                        RefreshBlockTable(int SetCurrentIndex);
    void                        MakeFormIcon(QComboBox *UICB);
    void                        MakeBorderStyleIcon(QComboBox *UICB);

};

//====================================================================================================================
// Define some macros
//====================================================================================================================
#define ICON_RULER_ON                   ":/img/ruler_ok.png"
#define ICON_RULER_OFF                  ":/img/ruler_ko.png"
#define ICON_EDIT_IMAGE                 ":/img/EditImage.png"
#define ICON_EDIT_MOVIE                 ":/img/EditMovie.png"
#define ICON_EDIT_GMAPS                 ":/img/EditGMaps.png"

#define ISVIDEO(OBJECT)                 ((OBJECT->MediaObject)&&(OBJECT->MediaObject->ObjectType==OBJECTTYPE_VIDEOFILE))
#define ISBLOCKVALIDE()                 ((!InRefreshControls)&&(BlockSelectMode==SELECTMODE_ONE)&&(CurrentCompoObject))
#define ISBLOCKVALIDEVISIBLE()          (ISBLOCKVALIDE()&&(CurrentCompoObject->IsVisible))
#define GETUI(WIDGETNAME)               findChild<QWidget *>(WIDGETNAME)
#define GETDOUBLESPINBOX(WIDGETNAME)    findChild<QDoubleSpinBox *>(WIDGETNAME)
#define GETSPINBOX(WIDGETNAME)          findChild<QSpinBox *>(WIDGETNAME)
#define GETSLIDER(WIDGETNAME)           findChild<QSlider *>(WIDGETNAME)
#define GETCOMBOBOX(WIDGETNAME)         findChild<QComboBox *>(WIDGETNAME)
#define GETBUTTON(WIDGETNAME)           findChild<QToolButton *>(WIDGETNAME)
#define GETCHECKBOX(WIDGETNAME)         findChild<QCheckBox *>(WIDGETNAME)

#define APPLY1TONEXT(FIELD) ({                                                                                                          \
    bool ContAPPLY=true;                                                                                                                \
    int  ShotNum=CurrentShotNbr+1;                                                                                                      \
    while ((ContAPPLY)&&(ShotNum<CurrentSlide->List.count())) {                                                                         \
        for (int Block=0;ContAPPLY && Block<CurrentSlide->List[CurrentShotNbr]->ShotComposition.List.count();Block++)                   \
         for (int ToSearch=0;ContAPPLY && ToSearch<CurrentSlide->List[ShotNum]->ShotComposition.List.count();ToSearch++)                \
          if (CurrentSlide->List[ShotNum]->ShotComposition.List[ToSearch]->IndexKey==CurrentCompoObject->IndexKey) {                    \
            cCompositionObject *ShotObject=CurrentSlide->List[ShotNum]->ShotComposition.List[ToSearch];                                 \
            if (!ShotObject->BlockInheritance) ShotObject->FIELD=CurrentCompoObject->FIELD;                                               \
                else ContAPPLY=false;                                                                                                   \
        }                                                                                                                               \
        ShotNum++;                                                                                                                      \
    }                                                                                                                                   \
})

#define APPLY2TONEXT(FIELD1,FIELD2) ({                                                                                                  \
    bool ContAPPLY=true;                                                                                                                \
    int  ShotNum=CurrentShotNbr+1;                                                                                                      \
    while ((ContAPPLY)&&(ShotNum<CurrentSlide->List.count())) {                                                                         \
        for (int Block=0;ContAPPLY && Block<CurrentSlide->List[CurrentShotNbr]->ShotComposition.List.count();Block++)                   \
         for (int ToSearch=0;ContAPPLY && ToSearch<CurrentSlide->List[ShotNum]->ShotComposition.List.count();ToSearch++)                \
          if (CurrentSlide->List[ShotNum]->ShotComposition.List[ToSearch]->IndexKey==CurrentCompoObject->IndexKey) {                    \
            cCompositionObject *ShotObject=CurrentSlide->List[ShotNum]->ShotComposition.List[ToSearch];                                 \
            if (!ShotObject->BlockInheritance) {                                                                                          \
                ShotObject->FIELD1=CurrentCompoObject->FIELD1;                                                                          \
                ShotObject->FIELD2=CurrentCompoObject->FIELD2;                                                                          \
            } else ContAPPLY=false;                                                                                                     \
        }                                                                                                                               \
        ShotNum++;                                                                                                                      \
    }                                                                                                                                   \
})

#define APPLY3TONEXT(FIELD1,FIELD2,FIELD3) ({                                                                                           \
    bool ContAPPLY=true;                                                                                                                \
    int  ShotNum=CurrentShotNbr+1;                                                                                                      \
    while ((ContAPPLY)&&(ShotNum<CurrentSlide->List.count())) {                                                                         \
        for (int Block=0;ContAPPLY && Block<CurrentSlide->List[CurrentShotNbr]->ShotComposition.List.count();Block++)                   \
         for (int ToSearch=0;ContAPPLY && ToSearch<CurrentSlide->List[ShotNum]->ShotComposition.List.count();ToSearch++)                \
          if (CurrentSlide->List[ShotNum]->ShotComposition.List[ToSearch]->IndexKey==CurrentCompoObject->IndexKey) {                    \
            cCompositionObject *ShotObject=CurrentSlide->List[ShotNum]->ShotComposition.List[ToSearch];                                 \
            if (!ShotObject->BlockInheritance) {                                                                                          \
                ShotObject->FIELD1=CurrentCompoObject->FIELD1;                                                                          \
                ShotObject->FIELD2=CurrentCompoObject->FIELD2;                                                                          \
                ShotObject->FIELD3=CurrentCompoObject->FIELD3;                                                                          \
            } else ContAPPLY=false;                                                                                                     \
        }                                                                                                                               \
        ShotNum++;                                                                                                                      \
    }                                                                                                                                   \
})

#define APPLY4TONEXT(FIELD1,FIELD2,FIELD3,FIELD4) ({                                                                                    \
    bool ContAPPLY=true;                                                                                                                \
    int  ShotNum=CurrentShotNbr+1;                                                                                                      \
    while ((ContAPPLY)&&(ShotNum<CurrentSlide->List.count())) {                                                                         \
        for (int Block=0;ContAPPLY && Block<CurrentSlide->List[CurrentShotNbr]->ShotComposition.List.count();Block++)                   \
         for (int ToSearch=0;ContAPPLY && ToSearch<CurrentSlide->List[ShotNum]->ShotComposition.List.count();ToSearch++)                \
          if (CurrentSlide->List[ShotNum]->ShotComposition.List[ToSearch]->IndexKey==CurrentCompoObject->IndexKey) {                    \
            cCompositionObject *ShotObject=CurrentSlide->List[ShotNum]->ShotComposition.List[ToSearch];                                 \
            if (!ShotObject->BlockInheritance) {                                                                                          \
                ShotObject->FIELD1=CurrentCompoObject->FIELD1;                                                                          \
                ShotObject->FIELD2=CurrentCompoObject->FIELD2;                                                                          \
                ShotObject->FIELD3=CurrentCompoObject->FIELD3;                                                                          \
                ShotObject->FIELD4=CurrentCompoObject->FIELD4;                                                                          \
            } else ContAPPLY=false;                                                                                                     \
        }                                                                                                                               \
        ShotNum++;                                                                                                                      \
    }                                                                                                                                   \
})

#define APPLYBACKGROUNDBRUSH() ({                                                                                                       \
    bool ContAPPLY=true;                                                                                                                \
    int  ShotNum=CurrentShotNbr+1;                                                                                                      \
    while ((ContAPPLY)&&(ShotNum<CurrentSlide->List.count())) {                                                                         \
        for (int Block=0;ContAPPLY && Block<CurrentSlide->List[CurrentShotNbr]->ShotComposition.List.count();Block++)                   \
         for (int ToSearch=0;ContAPPLY && ToSearch<CurrentSlide->List[ShotNum]->ShotComposition.List.count();ToSearch++)                \
          if (CurrentSlide->List[ShotNum]->ShotComposition.List[ToSearch]->IndexKey==CurrentCompoObject->IndexKey) {                    \
            cCompositionObject *ShotObject=CurrentSlide->List[ShotNum]->ShotComposition.List[ToSearch];                                 \
            if (!ShotObject->BlockInheritance) {                                                                                          \
                ShotObject->BackgroundBrush->CopyFromBrushDefinition(CurrentCompoObject->BackgroundBrush);                              \
            } else ContAPPLY=false;                                                                                                     \
        }                                                                                                                               \
        ShotNum++;                                                                                                                      \
    }                                                                                                                                   \
})

#endif // CSHOTCOMPOSER_H
