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

#ifndef DLGSLIDEPROPERTIES_H
#define DLGSLIDEPROPERTIES_H

// Basic inclusions (common to all files)
#include "_GlobalDefines.h"

// Specific inclusions
#include "_Diaporama.h"
#include "cCustomGraphicsRectItem.h"

namespace Ui {
    class DlgSlideProperties;
}

class DlgSlideProperties : public QDialog {
Q_OBJECT
public:
    cDiaporamaObject        *DiaporamaObject;
    QDomDocument            *Undo;                      // Save object before modification for cancel button

    bool                    IsFirstInitDone;            // true when first show window was done
    bool                    InRefreshSceneImage;        // True if process is currently in RefreshSceneImage function

    bool                    StopMAJSpinbox;
    bool                    StopMajSelect;
    bool                    BLOCKCHSIZE;
    cCompositionList        *CompositionList;           // Link to Composition List
    double                  xmax,ymax;                  // Size of the scene
    int                     NextZValue;                 // Current zvalue
    QGraphicsScene          *scene;                     // Link to the scene
    QImage                  *BackgroundImage;           // Background Image

    sMagneticRuler          MagneticRuler;              // TV Margins ruler
    int                     WithPen;                    // With for ruler pen

    explicit DlgSlideProperties(cDiaporamaObject *DiaporamaObject,QWidget *parent = 0);
    ~DlgSlideProperties();

    void            RefreshControls();
    void            RefreshSceneImage();
    void            RefreshSceneImageAndCache(cCompositionObject  *CurrentTextItem,cBrushDefinition *CurrentBrush);
    void            UpdateDockInfo();
    void            AdjustApectRatio(cBrushDefinition *CurrentBrush,cCompositionObject *CurrentTextItem);
    void            GetForDisplayUnit(double &DisplayW,double &DisplayH);
    void            s_Scene_DoubleClick();                      // User double click on a block in the scene widget

    // Utility functions
    cCustomGraphicsRectItem *GetSelectItem();                       // Return selected item
    cCompositionObject      *GetSelectedCompositionObject();        // Return selected CompositionObject
    cCompositionObject      *GetSelectedGlobalCompositionObject();  // Return selected CompositionObject in the global composition list
    cBrushDefinition        *GetCurrentBrush();                     // Return selected brush
    cBrushDefinition        *GetCurrentGlobalBrush();               // Return brush of selected CompositionObject in the global composition list
    void                    ApplyGlobalPropertiesToAllShots(cCompositionObject *GlobalBlock);
    void                    CopyBlockProperties(cCompositionObject *SourceBlock,cCompositionObject *DestBlock);

protected:
    virtual void    resizeEvent(QResizeEvent *);
    virtual void    showEvent(QShowEvent *);
    virtual void    reject();
    virtual void    accept();

private slots:
    void            OKPrevious();
    void            OKNext();
    void            Help();
    void            SetSavedWindowGeometry();
    void            s_SlideNameChange(QString NewText);
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
    void            s_ChgRotateXValue(int);
    void            s_ChgRotateYValue(int);

    // Brush part
    void            s_ChangeBrushTypeCombo(int);

    // Intermediate position for gradient 3 colors
    void            s_IntermPosSliderMoved(int);
    void            s_IntermPosED(int);

    // Handler for custom color/brush/pattern/gradient combo box index change
    void            s_ChIndexPatternBrushCombo(int);
    void            s_ChIndexGradientOrientationCombo(int);
    void            s_ChIndexGradientFirstColorCombo(int);
    void            s_ChIndexGradientFinalColorCombo(int);
    void            s_ChIndexGradientIntermColorCombo(int);
    void            s_ChIndexBackgroundCombo(int);

    // Image part
    void            s_ChangeImageGeometry(int);

private:
    void            Clean();
    void            MakeFormIcon(QComboBox *UICB);
    void            MakeBorderStyleIcon(QComboBox *UICB);

    void            RefreshShotTable(int SetCurrentIndex);
    void            RefreshBlockTable(int SetCurrentIndex);

    Ui::DlgSlideProperties *ui;
};

#endif // DLGSLIDEPROPERTIES_H
