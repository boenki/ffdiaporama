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

#ifndef WGT_QBACKGROUNDDEFWIDGET_H
#define WGT_QBACKGROUNDDEFWIDGET_H

// Basic inclusions (common to all files)
#include "_GlobalDefines.h"

// Specific inclusions
#include "_Diaporama.h"

namespace Ui {
    class wgt_QBackgroundDefWidget;
}

class wgt_QBackgroundDefWidget : public QWidget {
Q_OBJECT
public:
    cDiaporamaObject    *DiaporamaObject;

    explicit    wgt_QBackgroundDefWidget(QWidget *parent = 0);
    ~wgt_QBackgroundDefWidget();

    void        SetDiaporamaObject(cDiaporamaObject *TheDiaporamaObject);

private slots:
    void        SetupUi();
    void        s_SameBackground();
    void        s_NewBackground();

signals:
    void        BackgroundTypeChange();

private:

    Ui::wgt_QBackgroundDefWidget *ui;
};

#endif // WGT_QBACKGROUNDDEFWIDGET_H
