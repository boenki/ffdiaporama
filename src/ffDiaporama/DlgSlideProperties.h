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

#ifndef DLGSLIDEPROPERTIES_H
#define DLGSLIDEPROPERTIES_H

// Basic inclusions (common to all files)
#include "../sharedfiles/_GlobalDefines.h"
#include "../sharedfiles/_QCustomDialog.h"

// Specific inclusions
#include "_Diaporama.h"
#include "cCustomGraphicsRectItem.h"
#include "_ApplicationDefinitions.h"

namespace Ui {
    class DlgSlideProperties;
}

class DlgSlideProperties : public QCustomDialog {
Q_OBJECT
public:
    cDiaporamaObject        *DiaporamaObject;
    QDomDocument            *Undo;                      // Save object before modification for cancel button

    cCompositionList        *CompositionList;           // Link to Composition List
    double                  xmax,ymax;                  // Size of the scene
    int                     NextZValue;                 // Current zvalue
    QGraphicsScene          *scene;                     // Link to the scene
    QImage                  *BackgroundImage;           // Background Image

    sMagneticRuler          MagneticRuler;              // TV Margins ruler
    int                     WithPen;                    // With for ruler pen
    double                  ProjectGeometry;
    QString                 FramingStyleLabelPixmap;

    // Re-entrence flags
    bool                    InBlockTable_SelectionChanged;
    bool                    InScene_SelectionChanged;
    bool                    InShotTable_SelectionChanged;
    bool                    InRefreshStyleControls;
    bool                    InRefreshControls;
    bool                    InRefreshSceneImage;
    bool                    StopMAJSpinbox;                         // Use to avoid controls to send refreshcontrol
    bool                    StopMajFramingStyle;

    explicit DlgSlideProperties(cDiaporamaObject *DiaporamaObject,QString HelpURL,cBaseApplicationConfig *ApplicationConfig,cSaveWindowPosition *DlgWSP,QWidget *parent = 0);
    ~DlgSlideProperties();

    // function to be overloaded
    virtual void            DoInitDialog();                             // Initialise dialog
    virtual void            DoAccept() {/*Nothing to do*/}              // Call when user click on Ok button
    virtual void            DoRejet()  {/*Nothing to do*/}              // Call when user click on Cancel button
    virtual void            PrepareGlobalUndo();                        // Initiale Undo
    virtual void            DoGlobalUndo();                             // Apply Undo : call when user click on Cancel button
    virtual void            SaveWindowState();
    virtual void            RestoreWindowState();

    void                    RefreshStyleControls();
    void                    RefreshControls();
    void                    RefreshSceneImage();
    void                    AdjustApectRatio(cBrushDefinition *CurrentBrush,cCompositionObject *CurrentTextItem);
    void                    GetForDisplayUnit(double &DisplayW,double &DisplayH);
    void                    s_Scene_DoubleClick();                      // User double click on a block in the scene widget

    // Utility functions
    cCompositionObject      *GetSelectedCompositionObject();        // Return selected CompositionObject
    cCompositionObject      *GetSelectedGlobalCompositionObject();  // Return selected CompositionObject in the global composition list
    void                    ApplyGlobalPropertiesToAllShots(cCompositionObject *GlobalBlock);
    void                    CopyBlockProperties(cCompositionObject *SourceBlock,cCompositionObject *DestBlock);

    // Context for modifing values to current composition object
    cCompositionObject      *CompositionObject;
    cCustomGraphicsRectItem *RectItem;
    double                  DisplayW,DisplayH;

    bool                    PrepContexte();
    void                    ApplyToContexte(bool ReposRectItem,bool ApplyGlobal);

protected:
    virtual void    resizeEvent(QResizeEvent *);
    virtual void    showEvent(QShowEvent *);

private slots:
    void            s_Event_ClipboardChanged();
    void            s_RefreshSceneImage();

    void            OKPrevious();
    void            OKNext();
    void            s_SlideNameChange(QString NewText);
    void            s_NewChapter(int state);
    void            s_ShotDurationChange(QTime NewValue);
    void            ChangeVisibleState();
    void            s_TVMarginsBt();

    void            s_CopyBlockBT();
    void            s_CutBlockBT();
    void            s_PasteBlockBT();

    void            BlockUp();
    void            BlockDown();

    // Buttons associated to a Dialog box
    void            TextEditor();
    void            ImageEditCorrect();
    void            VideoEdit();
    void            Information();
    void            GetSound();

    // Shot table part
    void            s_ShotTable_SelectionChanged();             // User select a shot in the ShotTable widget
    void            s_ShotTable_AddShot();
    void            s_ShotTable_RemoveShot();
    void            s_ShotTableDragMoveItem();
    void            s_ShotTableMoveLeft();
    void            s_ShotTableMoveRight();

    // Block table/scene part
    void            s_BlockTable_SelectionChanged();            // User select a block in the BlocTable widget
    void            s_BlockTable_ItemDoubleClicked(QTableWidgetItem * item);
    void            s_Scene_SelectionChanged();                 // User select a block in the scene widget
    void            s_BlockTable_AddNewTextBlock();
    void            s_BlockTable_AddNewFileBlock();
    void            s_BlockTable_RemoveBlock();

    void            s_ChgPosXValue(double);
    void            s_ChgPosYValue(double);
    void            s_ChgWidthValue(double);
    void            s_ChgHeightValue(double);
    void            s_ChangeBackgroundForm(int);
    void            s_ChangeOpacity(int);

    void            s_ChgShadowFormValue(int);
    void            s_ChgShadowDistanceValue(int);
    void            s_ChgPenSize(int);
    void            s_ChPenColorCB(int);
    void            s_ChangePenStyle(int);
    void            s_ChgShadowColorCB(int);

    void            s_ChgRotateZValue(int);
    void            s_ResetRotateZBT();

    void            s_ChgRotateXValue(int);
    void            s_ResetRotateXBT();

    void            s_ChgRotateYValue(int);
    void            s_ResetRotateYBT();

    // Style
    void            s_ChangeFramingStyle(int);
    void            s_CoordinateStyleBT();
    void            s_BlockShapeStyleBT();

    // Text annimation
    void            s_ZoomED(int);
    void            s_ZoomResetBT();
    void            s_ScrollXED(int);
    void            s_ScrollXResetBT();
    void            s_ScrollYED(int);
    void            s_ScrollYResetBT();

    // Multilple turn annimation
    void            s_ChgTurnZValue(int);
    void            s_ResetTurnZBT();
    void            s_ChgTurnXValue(int);
    void            s_ResetTurnXBT();
    void            s_ChgTurnYValue(int);
    void            s_ResetTurnYBT();

private:
    void            Clean();
    void            MakeFormIcon(QComboBox *UICB);
    void            MakeBorderStyleIcon(QComboBox *UICB);

    void            RefreshShotTable(int SetCurrentIndex);
    void            RefreshBlockTable(int SetCurrentIndex);

    Ui::DlgSlideProperties *ui;
};

#endif // DLGSLIDEPROPERTIES_H
