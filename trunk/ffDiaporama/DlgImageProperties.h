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

#ifndef DLGIMAGEPROPERTIES_H
#define DLGIMAGEPROPERTIES_H

#include <QDialog>
#include <QCheckBox>
#include <QGraphicsScene>
#include <QIcon>
#include "cdiaporama.h"
#include "wgt_QCustomScene.h"

namespace Ui {
    class DlgImageProperties;
}

class   DlgImageProperties;

//---------------------------------------------------------------------------------------------------------------------------------------------------------------
// Dialog box
//---------------------------------------------------------------------------------------------------------------------------------------------------------------

class DlgImageProperties : public QDialog {
Q_OBJECT
public:
    cDiaporamaObject    *DiaporamaObject;
    bool                IsTabCanvasInit;                // Flag for Tab Canvas
    bool                IsTabImageInit;                 // Flag for Tab Image
    bool                IsTabAnimationInit;             // Flag for Tab Animation
    bool                IsTabCompositionInit;           // Flag for Tab Composition

    QDomDocument        *Undo;                          // Save object before modification for cancel button

    explicit DlgImageProperties(cDiaporamaObject *DiaporamaObject,QWidget *parent = 0);
    ~DlgImageProperties();

protected:
    virtual void showEvent(QShowEvent *);
    virtual void reject();
    virtual void accept();

private slots:
    void        SetSavedWindowGeometry();
    void        s_TabCurrentChanged(int Page);

    //**************************************************************
    // Composition Tab
    //**************************************************************
    void        s_CompositionNeedRefreshBackgroundImage();

    //**************************************************************
    // Animation Tab
    //**************************************************************
    void        s_ItemSelectionChanged();
    void        s_StaticSetCustom();
    void        s_DefineCustom(int Value);
    void        s_MobilSetCustom();
    void        s_MobilCustom(int Value);
    void        s_addSequence();
    void        s_removeSequence();
    void        s_UpSequence();
    void        s_DownSequence();
    void        s_CustomSceneModifyFlag();
    void        s_FullCanvas();
    void        s_CompositionInShotNeedRefreshBackgroundImage();
    void        s_ShotTabWidgetCurrentChanged(int);
    void        s_BackgroundImageUpdated();

private:
    //**************************************************************
    // Canvas Tab
    //**************************************************************
    void    SetTabCanvas();

    //**************************************************************
    // Image Tab
    //**************************************************************
    void    SetTabImage();

    //**************************************************************
    // Animation Tab
    //**************************************************************
    void    SetTabAnimation();
    void    RefreshControls();

    Ui::DlgImageProperties *ui;
};

#endif // DLGIMAGEPROPERTIES_H
