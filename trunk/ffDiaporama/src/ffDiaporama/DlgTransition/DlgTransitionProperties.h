/* ======================================================================
    This file is part of ffDiaporama
    ffDiaporama is a tools to make diaporama as video
    Copyright (C) 2011-2013 Dominique Levray <levray.dominique@bbox.fr>

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

#ifndef DLGTRANSITIONPROPERTIES_H
#define DLGTRANSITIONPROPERTIES_H

// Basic inclusions (common to all files)
#include "../../CustomCtrl/_QCustomDialog.h"

// Specific inclusions
#include "../_Diaporama.h"

namespace Ui {
    class DlgTransitionProperties;
}

class DlgTransitionProperties : public QCustomDialog {
Q_OBJECT
public:
    cDiaporamaObject        *DiaporamaObject;
    QTimer                  Timer;
    int                     MaxItem;
    cDiaporamaObjectInfo    *PreviousFrame;
    int                     W,H;
    int                     TimePosition;
    int                     AnimationTime;
    bool                    IsMultiple;
    // Previous values
    int                     TransitionFamilly;
    int                     TransitionSubType;
    int                     TransitionDuration;

    explicit DlgTransitionProperties(bool IsMultiple,cDiaporamaObject *TheDiaporamaObject,QString HelpURL,cBaseApplicationConfig *ApplicationConfig,cSaveWindowPosition *DlgWSP,QWidget *parent=0);
    ~DlgTransitionProperties();

    // function to be overloaded
    virtual void    DoInitDialog();                             // Initialise dialog
    virtual bool    DoAccept();                                 // Call when user click on Ok button
    virtual void    DoRejet()           {/*Nothing to do*/}     // Call when user click on Cancel button
    virtual void    PrepareGlobalUndo();                        // Initiale Undo
    virtual void    DoGlobalUndo();                             // Apply Undo : call when user click on Cancel button

private slots:
    void            s_ChTransitionTypeCB(int);
    void            s_TimerEvent();
    void            s_TableCellChanged(int,int,int,int);
    void            s_ChTransitionCB(int);

private:
    Ui::DlgTransitionProperties *ui;
};

#endif // DLGTRANSITIONPROPERTIES_H
