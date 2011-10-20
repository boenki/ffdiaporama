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

#ifndef DLGTEXTEDIT_H
#define DLGTEXTEDIT_H

// Basic inclusions (common to all files)
#include "_GlobalDefines.h"

// Specific inclusions
#include "_Diaporama.h"

namespace Ui {
    class DlgTextEdit;
}

class DlgTextEdit : public QDialog {
Q_OBJECT
public:
    QDomDocument        *Undo;                      // Save object before modification for cancel button
    cCompositionObject  *CurrentTextItem;           // Text to modify
    cBrushDefinition    *CurrentBrush;              // Brush to modify
    bool                StopMAJSpinbox;
    bool                IsFirstInitDone;
    QWidget             *ParentWindow;


    explicit DlgTextEdit(cCompositionObject *CurrentTextItem,cBrushDefinition *CurrentBrush,QWidget *parent = 0);
    ~DlgTextEdit();

    void            RefreshControls();
    void            MakeTextStyleIcon(QComboBox *UICB);

protected:
    virtual void    resizeEvent(QResizeEvent *);
    virtual void    showEvent(QShowEvent *);
    virtual void    reject();
    virtual void    accept();

private slots:
    void            Help();
    void            SetSavedWindowGeometry();

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
    void            s_plainTextEditChange();
    void            s_ChIndexFontColorCombo(int);
    void            s_ChIndexFontShadowColorCombo(int);

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

    void            s_TextStyleBT();
    void            s_BackgroundStyleBT();

signals:
    void     RefreshDisplay();

private:
    Ui::DlgTextEdit *ui;
};

#endif // DLGTEXTEDIT_H
