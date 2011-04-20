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

#ifndef WGT_QCUSTOMBRUSH_H
#define WGT_QCUSTOMBRUSH_H

// Basic inclusions (common to all files)
#include "_GlobalDefines.h"

// Specific inclusions
#include "_Diaporama.h"

namespace Ui {
    class wgt_QCustomBrush;
}

class wgt_QCustomBrush : public QWidget {
Q_OBJECT
public:
    cBrushDefinition *CurrentBrush;
    bool             StopMAJSpinbox;

    explicit wgt_QCustomBrush(QWidget *parent = 0);
    ~wgt_QCustomBrush();

    void        InitWidget(bool AllowBrushTypeNoBrush,bool AllowBrushTypePattern);
    void        RefreshControls(cBrushDefinition *CurrentTextItem,bool Allowed);

private slots:
    void        s_BackgroundTableSelectionChanged();
    void        s_ChangeBrushTypeCombo(int);

    // Intermediate position for gradient 3 colors
    void        s_IntermPosSliderMoved(int);
    void        s_IntermPosED(int);

    // Handler for custom buttons of each custom color combo box
    void        s_CustomFirstColorBt();
    void        s_CustomFinalColorBt();
    void        s_CustomIntermColorBt();

    // Handler for custom color/brush/pattern/gradient combo box index change
    void        s_ChIndexPatternBrushCombo(int);
    void        s_ChIndexGradientOrientationCombo(int);
    void        s_ChIndexGradientFirstColorCombo(int);
    void        s_ChIndexGradientFinalColorCombo(int);
    void        s_ChIndexGradientIntermColorCombo(int);

signals:
    void        NeedRefreshControls();

private:
    Ui::wgt_QCustomBrush *ui;
};

#endif // WGT_QCUSTOMBRUSH_H
