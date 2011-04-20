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

#ifndef WGT_QCUSTOMSCENE_H
#define WGT_QCUSTOMSCENE_H

// Basic inclusions (common to all files)
#include "_GlobalDefines.h"

// Specific inclusions
#include "_Diaporama.h"
#include "cCustomGraphicsRectItem.h"

namespace Ui {
    class wgt_QCustomScene;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------
// Custom Widget object
//---------------------------------------------------------------------------------------------------------------------------------------------------------------

class wgt_QCustomScene : public QWidget {
Q_OBJECT
public:
    bool                    FLAGSTOPED;             // Flag to stop spin box during settings
    QIcon                   *RulerOn,*RulerOff;
    sMagneticRuller         MagneticRuller;
    QGraphicsScene          *scene;
    cCustomGraphicsRectItem *cadre;
    QPixmap                 *CacheImage;
    double                   xmax,ymax;
    cDiaporamaShot          *DiaporamaScene;

    explicit wgt_QCustomScene(QWidget *parent = 0);
    ~wgt_QCustomScene();

    void SetDiaporamaScene(cDiaporamaShot *DiaporamaScene);
    void RefreshBackgroundImage(bool ResetCache);
    void RefreshControls();
    void RefreshWidget();

protected:
    virtual void resizeEvent(QResizeEvent *);
    virtual void showEvent(QShowEvent *);

private slots:
    void        s_RotationEDChanged(int Value);
    void        s_XValueEDChanged(double Value);
    void        s_YValueEDChanged(double Value);
    void        s_ZoomValueEDChanged(double Value);
    void        s_RotateLeft();
    void        s_RotateRight();
    void        s_AdjustW();
    void        s_AdjustH();
    void        s_AdjustWH();
    void        s_MagneticEdgeBt();

signals:
    void        ModifyDataSignal();

private:
    Ui::wgt_QCustomScene *ui;
};

#endif // WGT_QCUSTOMSCENE_H
