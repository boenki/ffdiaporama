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

#ifndef WGT_QCOMPOSITIONWIDGET_H
#define WGT_QCOMPOSITIONWIDGET_H

// Basic inclusions (common to all files)
#include "_GlobalDefines.h"

// Specific inclusions
#include "_Diaporama.h"
#include "cCustomGraphicsRectItem.h"

namespace Ui {
    class wgt_QCompositionWidget;
}

class wgt_QCompositionWidget : public QWidget {
Q_OBJECT
public:
    bool                    IsFirstInitDone;                // true when first show window was done
    bool                    StopMAJSpinbox;
    bool                    BLOCKCHSIZE;
    cCompositionList        *CompositionList;               // Link to Composition List
    double                   xmax,ymax;                     // Size of the scene
    int                     NextZValue;                     // Current zvalue
    QGraphicsScene          *scene;                         // Link to the scene
    QPixmap                 *BackgroundImage;               // Background Image

    explicit wgt_QCompositionWidget(QWidget *parent = 0);
    ~wgt_QCompositionWidget();

    void                    SetCompositionObject(cCompositionList *CompositionList,QPixmap *BackgroundImage);
    void                    RefreshBackgroundImage();
    cCustomGraphicsRectItem *GetSelectItem();
    cCompositionObject      *GetSelectedCompositionObject();
    QSize                   GetSceneBoxSize();

    void                    StartRefreshControls();

protected:
    virtual void            resizeEvent(QResizeEvent *);
    virtual void            showEvent(QShowEvent *);

private slots:
    void                    RefreshControls();
    void                    s_SelectionChangeEvent();
    void                    s_AddNewTextToImage();
    void                    s_RemoveTextFromImage();
    void                    s_ChangeFont(QFont);
    void                    s_ChangeSizeFont(QString);
    void                    s_SetBold();
    void                    s_SetItalic();
    void                    s_SetUnderline();
    void                    s_SetTextLeft();
    void                    s_SetTextCenter();
    void                    s_SetTextRight();
    void                    s_SetTextJustif();
    void                    s_SetTextUp();
    void                    s_SetTextVCenter();
    void                    s_SetTextBottom();
    void                    s_ChangeStyleFont(int);
    void                    s_ChgPosXValue(double);
    void                    s_ChgPosYValue(double);
    void                    s_ChgWidthValue(double);
    void                    s_ChgHeightValue(double);
    void                    s_ChangeBackgroundForm(int);
    void                    s_ChangeOpacity(int);
    void                    s_plainTextEditChange();
    void                    s_ChIndexFontColorCombo(int);
    void                    s_ChIndexFontShadowColorCombo(int);

    void                    s_ChgPenSize(int);
    void                    s_ChPenColorCB(int);

signals:
    void                    NeedRefreshBackgroundImage();
    void                    BackgroundImageUpdated();

private:
    void                    Clean();
    void                    MakeFormIcon(QComboBox *UICB);
    void                    MakeTextStyleIcon(QComboBox *UICB);
    Ui::wgt_QCompositionWidget *ui;
};

#endif // WGT_QCOMPOSITIONWIDGET_H
