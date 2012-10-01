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
#include "../../engine/_GlobalDefines.h"
#include "../../engine/_QCustomDialog.h"

// Specific inclusions
#include "../_Diaporama.h"
#include "../_ApplicationDefinitions.h"

namespace Ui {
    class DlgImageCorrection;
}

class DlgImageCorrection : public QCustomDialog {
Q_OBJECT
public:
    cBrushDefinition        *CurrentBrush;
    cCompositionObject      *CompoObject;
    int                     ffDPrjGeometry;                 // Project image geometry define in ffDiaporama project
    bool                    UndoReloadImage;                // True if image change and undo must reload it
    QString                 UndoBrushFileName;              // Name of previous file is undo
    bool                    FLAGSTOPED;                     // Flag to stop spin box during settings
    bool                    FLAGSTOPSPIN;                   // Flag to stop spin box during blur change
    qreal                   ImageGeometry;
    qreal                   ProjectGeometry;
    QString                 InitialFilteredString;
    int                     OnOffFilter;
    int                     CurrentFramingStyle;

    explicit DlgImageCorrection(cCompositionObject *TheCurrentTextItem,int BackgroundForm,cBrushDefinition *CurrentBrush,int TheVideoPosition,int ImageGeometry,QString HelpURL,cBaseApplicationConfig *ApplicationConfig,cSaveWindowPosition *DlgWSP,QWidget *parent=0);
    ~DlgImageCorrection();

    // function to be overloaded
    virtual void            DoInitDialog();                             // Initialise dialog
    virtual void            DoAccept();                                 // Call when user click on Ok button
    virtual void            DoRejet() { /* Nothing to do */ }           // Call when user click on Cancel button
    virtual void            PrepareGlobalUndo();                        // Initiale Undo
    virtual void            DoGlobalUndo();                             // Apply Undo : call when user click on Cancel button

    virtual void            PreparePartialUndo(int ActionType,QDomElement root);
    virtual void            ApplyPartialUndo(int ActionType,QDomElement root);

    void            RefreshControls();

protected:
    virtual void    resizeEvent(QResizeEvent *);
    virtual void    showEvent(QShowEvent *);

private slots:
    void            s_RulersBT();
    void            s_RotationEDChanged(double Value);
    void            s_XValueEDChanged(double Value);
    void            s_YValueEDChanged(double Value);
    void            s_WValueEDChanged(double Value);
    void            s_HValueEDChanged(double Value);
    void            s_RotateLeft();
    void            s_RotateRight();
    void            s_ItemSelectionHaveChanged();
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
    void            s_IntZoneTransformBlocks(qreal Move_X,qreal Move_Y,qreal Scale_X,qreal Scale_Y);
    void            s_DisplayIntZoneTransformBlocks(qreal Move_X,qreal Move_Y,qreal Scale_X,qreal Scale_Y);

private:
    void            UpdateFramingStyleCB();

    Ui::DlgImageCorrection *ui;
};

#endif // DLGIMAGECORRECTION_H
