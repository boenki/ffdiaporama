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
    bool                    StopMAJSpinbox;
    bool                    BLOCKCHSIZE;
    cCompositionList        *CompositionList;           // Link to Composition List
    double                  xmax,ymax;                  // Size of the scene
    int                     NextZValue;                 // Current zvalue
    QGraphicsScene          *scene;                     // Link to the scene
    QImage                  *BackgroundImage;           // Background Image

    explicit DlgSlideProperties(cDiaporamaObject *DiaporamaObject,QWidget *parent = 0);
    ~DlgSlideProperties();

    void            SetCompositionObject(cCompositionList *CompositionList);
    void            RefreshBackgroundImage();
    void            RefreshControls();

    // Utility functions
    cCustomGraphicsRectItem *GetSelectItem();                   // Return selected item
    cCompositionObject      *GetSelectedCompositionObject();    // Return selected CompositionObject
    cBrushDefinition        *GetCurrentBrush();                 // Return selected brush
    QSize                   GetSceneBoxSize();                  // Return current scene size

protected:
    virtual void    resizeEvent(QResizeEvent *);
    virtual void    showEvent(QShowEvent *);
    virtual void    reject();
    virtual void    accept();

private slots:
    void            Help();
    void            SetSavedWindowGeometry();
    void            ImageEditCorrect();
    void            ImageEditTransform();
    void            VideoEdit();

    void            s_CurrentShotSelectionChanged();
    void            s_addShot();
    void            s_removeShot();

    void            s_SelectionChangeEvent();

    void            s_AddNewTextBlock();
    void            s_AddNewFileBlock();
    void            s_RemoveBlock();

    void            s_ChangeFont(QFont);
    void            s_ChangeSizeFont(QString);
    void            s_SetBold();
    void            s_SetItalic();
    void            s_SetUnderline();
    void            s_SetTextLeft();
    void            s_SetTextCenter();
    void            s_SetTextRight();
    void            s_SetTextJustif();
    void            s_SetTextUp();
    void            s_SetTextVCenter();
    void            s_SetTextBottom();
    void            s_ChangeStyleFont(int);
    void            s_ChgPosXValue(double);
    void            s_ChgPosYValue(double);
    void            s_ChgWidthValue(double);
    void            s_ChgHeightValue(double);
    void            s_ChangeBackgroundForm(int);
    void            s_ChangeOpacity(int);
    void            s_plainTextEditChange();
    void            s_ChIndexFontColorCombo(int);
    void            s_ChIndexFontShadowColorCombo(int);

    void            s_ChgShadowFormValue(int);
    void            s_ChgShadowDistanceValue(int);
    void            s_ChgPenSize(int);
    void            s_ChPenColorCB(int);
    void            s_ChangePenStyle(int);

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
    void            MakeTextStyleIcon(QComboBox *UICB);
    void            MakeBorderStyleIcon(QComboBox *UICB);

    Ui::DlgSlideProperties *ui;
};

#endif // DLGSLIDEPROPERTIES_H
