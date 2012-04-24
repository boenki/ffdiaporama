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
#include "../../../sharedfiles/_GlobalDefines.h"
#include "../../../sharedfiles/_QCustomDialog.h"

// Specific inclusions
#include "../../_Diaporama.h"

namespace Ui {
    class DlgVideoEdit;
}

class DlgVideoEdit : public QCustomDialog {
Q_OBJECT
public:
    cBrushDefinition        *CurrentBrush;
    bool                    StopMaj;

    explicit DlgVideoEdit(cBrushDefinition *CurrentBrush,QString HelpURL,cBaseApplicationConfig *ApplicationConfig,cSaveWindowPosition *DlgWSP,QWidget *parent=0);
    ~DlgVideoEdit();

    void            RefreshControls();

    // function to be overloaded
    virtual void    DoInitDialog();                             // Initialise dialog
    virtual void    DoAccept() { /* Nothing to do */ }          // Call when user click on Ok button
    virtual void    DoRejet()  { /* Nothing to do */ }          // Call when user click on Cancel button
    virtual void    PrepareGlobalUndo();                        // Initiale Undo
    virtual void    DoGlobalUndo();                             // Apply Undo : call when user click on Cancel button

    virtual void    PreparePartialUndo(int ActionType,QDomElement root);
    virtual void    ApplyPartialUndo(int ActionType,QDomElement root);

    virtual void    RestoreWindowState();

private slots:
    void            s_Event_SaveImageEvent();
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
