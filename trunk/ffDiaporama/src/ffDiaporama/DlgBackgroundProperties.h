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

#ifndef DLGBACKGROUNDPROPERTIES_H
#define DLGBACKGROUNDPROPERTIES_H

// Basic inclusions (common to all files)
#include "../sharedfiles/_GlobalDefines.h"
#include "../sharedfiles/_QCustomDialog.h"

// Specific inclusions
#include "_Diaporama.h"

namespace Ui {
    class DlgBackgroundProperties;
}

class DlgBackgroundProperties : public QDialog {
Q_OBJECT
public:
    cDiaporamaObject    *DiaporamaObject;
    QDomDocument        *Undo;                          // Save object before modification for cancel button
    bool                StopMAJSpinbox;
    bool                IsFirstInitDone;

    explicit DlgBackgroundProperties(cDiaporamaObject *DiaporamaObject,QWidget *parent = 0);
    ~DlgBackgroundProperties();

    void        RefreshControls(bool Allowed);

protected:
    virtual void showEvent(QShowEvent *);
    virtual void resizeEvent(QResizeEvent *);
    virtual void reject();
    virtual void accept();

private slots:
    void        Help();
    void        SetSavedWindowGeometry();
    void        s_SameBackground();
    void        s_NewBackground();
    void        SetupUi();

    void        s_ChangeBrushTypeCombo(int);

    // Intermediate position for gradient 3 colors
    void        s_IntermPosSliderMoved(int);
    void        s_IntermPosED(int);

    // Handler for custom color/brush/pattern/gradient combo box index change
    void        s_ChIndexPatternBrushCombo(int);
    void        s_ChIndexGradientOrientationCombo(int);
    void        s_ChIndexGradientFirstColorCombo(int);
    void        s_ChIndexGradientFinalColorCombo(int);
    void        s_ChIndexGradientIntermColorCombo(int);
    void        s_ChIndexBackgroundCombo(int);
    void        s_SelectFile();
    void        s_ImageEditCorrect();
    void        s_FullFill();
    void        s_KeepRatio();

private:
    Ui::DlgBackgroundProperties *ui;
};

#endif // DLGBACKGROUNDPROPERTIES_H
