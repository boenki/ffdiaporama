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

#ifndef DLGTEXTEDIT_H
#define DLGTEXTEDIT_H

// Basic inclusions (common to all files)
#include "../../sharedfiles/_GlobalDefines.h"
#include "../../sharedfiles/_QCustomDialog.h"
#include "../../sharedfiles/cBaseApplicationConfig.h"

// Include some additional standard class
#include <QDialog>
#include <QComboBox>
#include <QFont>
#include <QString>

// Specific inclusions
#include "../_Diaporama.h"

namespace Ui {
    class DlgTextEdit;
}

class DlgTextEdit : public QCustomDialog {
Q_OBJECT
public:
    cCompositionObject  *CurrentTextItem;                       // Text to modify
    cBrushDefinition    *CurrentBrush;                          // Brush to modify
    bool                StopMAJSpinbox;
    cStyleCollection    *StyleTextCollection;                   // Link to text style collection
    cStyleCollection    *StyleTextBackgroundCollection;         // Link to background text style collection
    QString             CurrentPlainText;

    explicit        DlgTextEdit(cCompositionObject *CurrentTextItem,QString HelpURL,cBaseApplicationConfig *ApplicationConfig,cSaveWindowPosition *DlgWSP,
                                cStyleCollection *StyleTextCollection,cStyleCollection *StyleTextBackgroundCollection,QWidget *parent=0);
                    ~DlgTextEdit();

    // function to be overloaded
    virtual void    DoInitDialog();                                         // Initialise dialog
    virtual void    DoAccept() {/*Nothing to do*/}                          // Call when user click on Ok button
    virtual void    DoRejet()  {/*Nothing to do*/}                          // Call when user click on Cancel button
    virtual void    PrepareGlobalUndo();                                    // Initiale Undo
    virtual void    DoGlobalUndo();                                         // Apply Undo : call when user click on Cancel button
    virtual void    PreparePartialUndo(int ActionType,QDomElement root);
    virtual void    ApplyPartialUndo(int ActionType,QDomElement root);

    // Other
    void            RefreshControls();
    void            MakeTextStyleIcon(QComboBox *UICB);

private slots:
    void            s_cursorPositionChanged();

    void            s_ChangeFont(QFont);
    void            s_ChangeSizeFont(QString);
    void            s_SetBold();
    void            s_SetItalic();
    void            s_SetUnderline();
    void            s_SetTextSuper();
    void            s_SetTextSub();

    void            s_SetTextLeft();
    void            s_SetTextCenter();
    void            s_SetTextRight();
    void            s_SetTextJustif();

    void            s_IndentInBt();
    void            s_IndentOutBt();

    void            s_ListBt();
    void            s_ListNbrBt();

    void            s_SetTextUp();
    void            s_SetTextVCenter();
    void            s_SetTextBottom();

    void            s_ChangeStyleFont(int);
    void            s_TextEditChange();
    void            s_ChIndexFontColorCombo(int);
    void            s_ChIndexFontShadowColorCombo(int);

    // Brush part
    void            s_ChangeBrushTypeCombo(int);

    // Intermediate position for gradient 3 colors
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
    void            RefreshDisplay();

private:
    Ui::DlgTextEdit *ui;
};

#endif // DLGTEXTEDIT_H
