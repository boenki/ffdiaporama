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

#ifndef DLGIMAGETRANSFORMATION_H
#define DLGIMAGETRANSFORMATION_H

// Basic inclusions (common to all files)
#include "_GlobalDefines.h"

// Specific inclusions
#include "_Diaporama.h"

namespace Ui {
    class DlgImageTransformation;
}

class DlgImageTransformation : public QDialog {
Q_OBJECT
public:
    cBrushDefinition        *CurrentBrush;
    QDomDocument            *Undo;                          // Save object before modification for cancel button
    QGraphicsScene          *scene;
    double                   xmax,ymax;
    cFilterTransformObject  *Filter;
    QImage                  *SourceImage;
    double                   ZoomLevel;

    explicit DlgImageTransformation(cBrushDefinition *CurrentBrush,QWidget *parent = 0);
    ~DlgImageTransformation();

protected:
    virtual void    resizeEvent(QResizeEvent *);
    virtual void    showEvent(QShowEvent *);
    virtual void    reject();
    virtual void    accept();

private slots:
    void            Help();
    void            SetSavedWindowGeometry();
    void            RefreshBackgroundImage();
    void            s_ChTransformationCB(int);

    void            s_ZoomPlus();
    void            s_ZoomMinus();

    void            s_BlurSigmaSliderMoved(int Value);
    void            s_BlurSigmaValueED(double Value);
    void            s_BlurRadiusSliderMoved(int Value);

private:
    Ui::DlgImageTransformation *ui;
};

#endif // DLGIMAGETRANSFORMATION_H
