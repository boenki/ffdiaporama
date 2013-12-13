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
        UNDOACTION_EDITZONE_TEXTANIMZOOM,
        UNDOACTION_EDITZONE_TEXTANIMSCROLLX,
        UNDOACTION_EDITZONE_TEXTANIMSCROLLY,
        UNDOACTION_EDITZONE_BLOCKANIMTYPE,
        UNDOACTION_EDITZONE_BLOCKSPEEDWAVE,
        UNDOACTION_EDITZONE_BLOCKANIMTURNX,
        UNDOACTION_EDITZONE_BLOCKANIMTURNZ,
        UNDOACTION_EDITZONE_BLOCKANIMTURNY,
        UNDOACTION_EDITZONE_BLOCKANIMDISSOLVE,
        UNDOACTION_STYLE_COORDINATES,
        UNDOACTION_STYLE_SHAPE,
        UNDOACTION_STYLE_FRAMING,
        UNDOACTION_EDITZONE_SLIDENAME,
        UNDOACTION_EDITZONE_SHOTDURATION,
        UNDOACTION_SHOTTABLE_ADDSHOT,
        UNDOACTION_SHOTTABLE_REMOVESHOT,
        UNDOACTION_SHOTTABLE_CHORDER,
        UNDOACTION_BLOCKTABLE_ADDTEXTBLOCK,
        UNDOACTION_BLOCKTABLE_ADDFILEBLOCK,
        UNDOACTION_BLOCKTABLE_REMOVEBLOCK,
        UNDOACTION_BLOCKTABLE_PASTEBLOCK,
        UNDOACTION_BLOCKTABLE_CHBLOCKORDER,
        UNDOACTION_BLOCKTABLE_ARRANGEBLOCK,
        UNDOACTION_BLOCKTABLE_EDITTEXT,
        UNDOACTION_BLOCKTABLE_EDITVIDEO,
        UNDOACTION_BLOCKTABLE_EDITIMAGE,
        UNDOACTION_BLOCKTABLE_VISIBLESTATE,
        UNDOACTION_BLOCKTABLE_SOUNDSTATE,
        UNDOACTION_BLOCKTABLE_SAMEASPREVIOUSSTATE,
        UNDOACTION_BLOCKTABLE_EDITCHAPTERINFO
    };

    bool                    InRefreshControls;
    bool                    InSelectionChange;

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
    bool                    SelectionHaveLockBlock;     // If true, selection have at least one locked block
    cCompositionList        *CompositionList;           // Link to current block List
    QList<bool>             IsSelected;                 // Table of selection state in the current block list
    int                     NbrSelected;                // Number of selected blocks

    explicit                    cShotComposer(cDiaporamaObject *DiaporamaObject,cBaseApplicationConfig *ApplicationConfig,QWidget *parent = 0);
    virtual void                DoInitDialog();

    // Utility function used to apply modification from one shot to next shot and/or global composition
    virtual void                CopyBlockProperties(cCompositionObject *SourceBlock,cCompositionObject *DestBlock);
    virtual void                ApplyGlobalPropertiesToAllShots(cCompositionObject *GlobalBlock);
    virtual void                ApplyToContexte(bool ApplyGlobal);
    virtual cCompositionObject  *GetGlobalCompositionObject(int IndexKey);      // Return CompositionObject in the global composition list for specific IndexKey

    // Update embedded controls
    virtual void                RefreshControls(bool UpdateInteractiveZone=true);

protected slots:
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

    // Block table
    void                        s_BlockTable_SelectionChanged();            // User select a block in the BlocTable widget

protected:
    void                        ComputeBlockRatio(cCompositionObject *Block,qreal &Ratio_X,qreal &Ratio_Y);
    void                        RefreshBlockTable(int SetCurrentIndex);
    void                        MakeFormIcon(QComboBox *UICB);
    void                        MakeBorderStyleIcon(QComboBox *UICB);

};

#endif // CSHOTCOMPOSER_H
