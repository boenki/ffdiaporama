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

#ifndef DLGSLIDEPROPERTIES_H
#define DLGSLIDEPROPERTIES_H

// Basic inclusions (common to all files)
#include "../../CustomCtrl/_QCustomDialog.h"
#include "../../engine/_Diaporama.h"

namespace Ui {
    class DlgSlideProperties;
}

class DlgSlideProperties : public QCustomDialog {
Q_OBJECT
public:
    double                  DisplayW,DisplayH;
    cDiaporamaObject        *CurrentSlide;              // Current slide

    SELECTMODE              ShotSelectMode;             // Current shot selection mode
    cDiaporamaShot          *CurrentShot;               // Current shot (if selection mode = SELECTMODE_ONE)
    int                     CurrentShotNbr;             // Current shot number (if selection mode = SELECTMODE_ONE)

    cCompositionList        *CompositionList;           // Link to current block List
    QList<bool>             IsSelected;                 // Table of selection state in the current block list
    int                     NbrSelected;                // Number of selected blocks
    SELECTMODE              BlockSelectMode;            // Current block selection mode
    bool                    SelectionHaveLockBlock;     // If true, selection have at least one locked block
    cCompositionObject      *CurrentCompoObject;        // Current block object (if selection mode = SELECTMODE_ONE)
    int                     CurrentCompoObjectNbr;      // Number of Current block object (if selection mode = SELECTMODE_ONE)

    double                  ProjectGeometry;
    QString                 FramingStyleLabelPixmap;

    // Framing CB control
    cBrushDefinition        *FramingCB_CurrentBrush;
    int                     FramingCB_CurrentShot;

    // Re-entrence flags
    bool                    InRefreshStyleControls;
    bool                    InRefreshControls;
    bool                    InSelectionChange;
    bool                    StopMajFramingStyle;
    bool                    InDisplayDuration;
    bool                    NoPrepUndo;

    explicit DlgSlideProperties(cDiaporamaObject *DiaporamaObject,cBaseApplicationConfig *ApplicationConfig,cSaveWindowPosition *DlgWSP,QWidget *parent = 0);
    virtual                 ~DlgSlideProperties();

    // function to be overloaded
    virtual void            DoInitDialog();                             // Initialise dialog
    virtual bool            DoAccept();                                 // Call when user click on Ok button
    virtual void            DoRejet();                                  // Call when user click on Cancel button
    virtual void            PrepareGlobalUndo();                        // Initiale Undo
    virtual void            DoGlobalUndo();                             // Apply Undo : call when user click on Cancel button
    virtual void            SaveWindowState();
    virtual void            RestoreWindowState();

    void                    RefreshStyleControls();
    void                    RefreshControls(bool UpdateInteractiveZone=true);

    // Utility functions
    cCompositionObject      *GetSelectedCompositionObject();                // Return selected CompositionObject
    cCompositionObject      *GetGlobalCompositionObject(int IndexKey);      // Return CompositionObject in the global composition list for specific IndexKey
    cCompositionObject      *GetSelectedGlobalCompositionObject();          // Return selected CompositionObject in the global composition list
    void                    ApplyGlobalPropertiesToAllShots(cCompositionObject *GlobalBlock);
    void                    CopyBlockProperties(cCompositionObject *SourceBlock,cCompositionObject *DestBlock);

    void                    ApplyToContexte(bool ApplyGlobal);

    virtual void            PreparePartialUndo(int ActionType,QDomElement root);
    virtual void            ApplyPartialUndo(int ActionType,QDomElement root);

protected:
    virtual void            resizeEvent(QResizeEvent *);
    virtual void            showEvent(QShowEvent *);
    virtual void            keyReleaseEvent(QKeyEvent *event);

signals:
    void            SetModifyFlag();

private slots:
    void            s_Event_ClipboardChanged();
    void            s_RefreshSceneImage();
    void            s_RulersBt();

    void            OKPrevious();
    void            OKNext();

    // Slide settings
    void            s_SlideSet_SlideNameChange(QString NewText);
    void            s_SlideSet_ChapterInformation();
    void            s_SlideSet_SaveModel();
    void            s_SlideSet_SaveAsProjectTitleModel();
    void            s_SlideSet_SaveAsCptTitleModels();
    void            s_SlideSet_SaveAsCreditTitleModels();

    // Shot table & settings
    void            s_ShotTable_SelectionChanged();             // User select a shot in the ShotTable widget
    void            s_ShotTable_AddShot();
    void            s_ShotTable_RemoveShot();
    void            s_ShotTable_DragMoveItem();
    void            s_ShotTable_MoveLeft();
    void            s_ShotTable_MoveRight();
    void            s_ShotTable_DurationChange(QTime NewValue);
    void            s_ShotTable_RightClickEvent(QMouseEvent *);

    // Block table
    void            s_BlockTable_SelectionChanged();            // User select a block in the BlocTable widget
    void            s_BlockTable_StartSelectionChange();
    void            s_BlockTable_EndSelectionChange();
    void            s_BlockTable_ItemDoubleClicked(QMouseEvent *);
    void            s_BlockTable_ItemRightClicked(QMouseEvent *);
    void            s_BlockTable_DragDropFiles(QList<QUrl> UrlList);
    void            s_BlockTable_AddNewTextBlock();
    void            s_BlockTable_AddNewSimpleTextBlock();
    void            s_BlockTable_AddNewClipArtTextBlock();
    void            s_BlockTable_AddNewFileBlock();
    void            s_BlockTable_RemoveBlock();
    void            s_BlockTable_Copy();
    void            s_BlockTable_Cut();
    void            s_BlockTable_Paste();
    void            s_BlockTable_MoveBlockUp();
    void            s_BlockTable_MoveBlockDown();
    void            s_BlockTable_DragMoveBlock(int,int);

    // Block settings : Call of other dialog
    void            s_BlockSettings_Arrange();
    void            s_BlockSettings_Edit();
    void            s_BlockSettings_TextEditor();
    void            s_BlockSettings_ImageEditCorrect();
    void            s_BlockSettings_Information();

    // Block settings : Basic values
    void            s_BlockSettings_ToggleVisibleState();
    void            s_BlockSettings_SetSameAsPreviousShot();
    void            s_BlockSettings_UnsetSameAsPreviousShot();
    void            s_BlockSettings_GetSound();

    // Block settings : Coordinates
    void            s_BlockSettings_ShapeSizePos(int);
    void            s_BlockSettings_PosXValue(double);
    void            s_BlockSettings_PosYValue(double);
    void            s_BlockSettings_PosWidthValue(double);
    void            s_BlockSettings_PosHeightValue(double);

    // Block settings : Rotations
    void            s_BlockSettings_RotateZValue(int);
    void            s_BlockSettings_ResetRotateZValue();
    void            s_BlockSettings_RotateXValue(int);
    void            s_BlockSettings_ResetRotateXValue();
    void            s_BlockSettings_RotateYValue(int);
    void            s_BlockSettings_ResetRotateYValue();

    // Block settings : Shape
    void            s_BlockSettings_ShapeTextClipArtChIndex();
    void            s_BlockSettings_ShapeBackgroundForm();
    void            s_BlockSettings_ShapeOpacity(int);
    void            s_BlockSettings_ShapeShadowFormValue(int);
    void            s_BlockSettings_ShapeShadowDistanceValue(int);
    void            s_BlockSettings_ShapePenSize(int);
    void            s_BlockSettings_ShapePenColor(int);
    void            s_BlockSettings_ShapePenStyle(int);
    void            s_BlockSettings_ShapeShadowColor(int);

    // Block settings : Style
    void            s_ChangeFramingStyle();
    void            s_ChangeTextFramingStyle(int Value);
    void            s_BlockShapeStyleBT();

    // Block settings : Alignment
    void            s_BlockTable_AlignTop();
    void            s_BlockTable_AlignMiddle();
    void            s_BlockTable_AlignBottom();
    void            s_BlockTable_AlignLeft();
    void            s_BlockTable_AlignCenter();
    void            s_BlockTable_AlignRight();
    void            s_BlockTable_DistributeHoriz();
    void            s_BlockTable_DistributeVert();

    // Block settings : Text animation
    void            s_BlockSettings_TextAnimZoom(int);
    void            s_BlockSettings_TextAnimZoomReset();
    void            s_BlockSettings_TextAnimScrollX(int);
    void            s_BlockSettings_TextAnimScrollXReset();
    void            s_BlockSettings_TextAnimScrollY(int);
    void            s_BlockSettings_TextAnimScrollYReset();

    // Block settings : Speed wave
    void            s_BlockSettings_SpeedWave(int);

    // Block settings : Block animation
    void            s_BlockSettings_BlockAnimType(int);

    // Block settings/Block animation : Multiple turn animation
    void            s_BlockSettings_BlockAnimTurnZValue(int);
    void            s_BlockSettings_BlockAnimTurnZReset();
    void            s_BlockSettings_BlockAnimTurnXValue(int);
    void            s_BlockSettings_BlockAnimTurnXReset();
    void            s_BlockSettings_BlockAnimTurnYValue(int);
    void            s_BlockSettings_BlockAnimTurnYReset();

    // Block settings/Block animation : Dissolve animation
    void            s_BlockSettings_BlockAnimDissolveType(int);

    // Block settings/Interactive zone messages
    void            s_BlockSettings_IntZoneTransformBlocks(qreal DeltaX,qreal DeltaY,qreal ScaleX,qreal ScaleY,qreal Sel_X,qreal Sel_Y,qreal Sel_W,qreal Sel_H);
    void            s_BlockSettings_IntZoneDisplayTransformBlocks(qreal DeltaX,qreal DeltaY,qreal ScaleX,qreal ScaleY,qreal Sel_X,qreal Sel_Y,qreal Sel_W,qreal Sel_H);

private:
    void            ComputeBlockRatio(cCompositionObject *Block,qreal &Ratio_X,qreal &Ratio_Y);
    void            MakeFormIcon(QComboBox *UICB);
    void            MakeBorderStyleIcon(QComboBox *UICB);

    void            s_ShotTable_DisplayDuration();
    void            RefreshBlockTable(int SetCurrentIndex);
    void            s_BlockTable_AddFilesBlock(QStringList FileList,int PositionToInsert);

    Ui::DlgSlideProperties *ui;
};

#endif // DLGSLIDEPROPERTIES_H
