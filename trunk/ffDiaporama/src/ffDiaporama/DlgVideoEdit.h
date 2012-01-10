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

#ifndef DLGVIDEOEDIT_H
#define DLGVIDEOEDIT_H

// Basic inclusions (common to all files)
#include "../sharedfiles/_GlobalDefines.h"

// Include some additional standard class

// Basic inclusions (common to all files)
#include "_GlobalDefines.h"

// Specific inclusions
#include "_Diaporama.h"

namespace Ui {
    class DlgVideoEdit;
}

class DlgVideoEdit : public QDialog {
Q_OBJECT
public:
    bool                    IsFirstInitDone;            // true when first show window was done
    cBrushDefinition        *CurrentBrush;
    QDomDocument            *Undo;                      // Save object before modification for cancel button

    explicit DlgVideoEdit(cBrushDefinition *CurrentBrush,QWidget *parent = 0);
    ~DlgVideoEdit();

    void            SetActualDuration();

protected:
    virtual void    showEvent(QShowEvent *);
    virtual void    reject();
    virtual void    accept();

private slots:
    void            Help();
    void            SetSavedWindowGeometry();
    void            s_DefStartPos();
    void            s_DefEndPos();
    void            s_SeekLeft();
    void            s_SeekRight();
    void            s_EditStartPos(QTime NewValue);
    void            s_EditEndPos(QTime NewValue);
    void            MusicReduceFactorChange(int);

private:
    Ui::DlgVideoEdit *ui;
};

#endif // DLGVIDEOEDIT_H