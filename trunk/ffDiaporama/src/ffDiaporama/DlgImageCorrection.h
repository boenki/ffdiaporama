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

#ifndef DLGIMAGECORRECTION_H
#define DLGIMAGECORRECTION_H

// Basic inclusions (common to all files)
#include "../sharedfiles/_GlobalDefines.h"
#include "../sharedfiles/_QCustomDialog.h"

// Specific inclusions
#include "_Diaporama.h"
#include "cCustomGraphicsRectItem.h"

namespace Ui {
    class DlgImageCorrection;
}

class DlgImageCorrection : public QDialog {
Q_OBJECT
public:
    cCompositionObject      *CurrentTextItem;               // Block to modify
    cBrushDefinition        *CurrentBrush;
    QDomDocument            *UndoSlide,*UndoShot;           // Save objects before modification for cancel button
    bool                    UndoReloadImage;                // True if image change and undo must reload it
    QString                 UndoBrushFileName;              // Name of previous file is undo
    bool                    IsFirstInitDone;                // true when first show window was done
    bool                    FLAGSTOPED;                     // Flag to stop spin box during settings
    bool                    FLAGSTOPSPIN;                   // Flag to stop spin box during blur change
    sMagneticRuler          MagneticRuler;
    QCustomGraphicsScene    *scene;
    cCustomGraphicsRectItem *cadre;
    int                     BackgroundForm;
    double                  xmax,ymax;
    QImage                  *CachedImage;                   // Link to the image to work
    int                     VideoPosition;
    double                  ImageGeometry;
    double                  ProjectGeometry;
    QString                 InitialFilteredString;

    explicit DlgImageCorrection(cCompositionObject *TheCurrentTextItem,int BackgroundForm,cBrushDefinition *CurrentBrush,int TheVideoPosition,QWidget *parent = 0);
    ~DlgImageCorrection();

    void            RefreshBackgroundImage();
    void            RefreshControls();

protected:
    virtual void    resizeEvent(QResizeEvent *);
    virtual void    showEvent(QShowEvent *);
    virtual void    reject();
    virtual void    accept();

private slots:
    void            Help();
    void            SetSavedWindowGeometry();

    void            s_RotationEDChanged(double Value);
    void            s_XValueEDChanged(double Value);
    void            s_YValueEDChanged(double Value);
    void            s_WValueEDChanged(double Value);
    void            s_HValueEDChanged(double Value);
    void            s_RotateLeft();
    void            s_RotateRight();
    void            s_AdjustW();
    void            s_AdjustH();
    void            s_AdjustWH();
    void            s_MagneticEdgeBt();
    void            s_BrightnessSliderMoved(int Value);
    void            s_ContrastSliderMoved(int Value);
    void            s_GammaSliderMoved(int Value);
    void            s_GammaValueED(double Value);
    void            s_RedSliderMoved(int Value);
    void            s_GreenSliderMoved(int Value);
    void            s_BlueSliderMoved(int Value);
    void            s_BrightnessReset();
    void            s_ContrastReset();
    void            s_GammaReset();
    void            s_RedReset();
    void            s_GreenReset();
    void            s_BlueReset();
    void            s_ChTransformationCB(int);
    void            s_BlurSigmaSliderMoved(int Value);
    void            s_BlurSigmaValueED(double Value);
    void            s_BlurRadiusSliderMoved(int Value);
    void            s_BlurSharpenReset();
    void            s_RadiusReset();
    void            ChangeBrushDiskFile();
    void            s_LockGeometryCB(int value);
    void            s_FramingStyleBT();

private:
    Ui::DlgImageCorrection *ui;
};

#endif // DLGIMAGECORRECTION_H
