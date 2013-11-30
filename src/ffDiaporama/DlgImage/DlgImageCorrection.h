/* ======================================================================
    This file is part of ffDiaporama
    ffDiaporama is a tools to make diaporama as video
    Copyright (C) 2011-2013 Dominique Levray <domledom@laposte.net>

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
#include "CustomCtrl/_QCustomDialog.h"
#include "engine/_Diaporama.h"

#include "DlgImage/wgt_QEditVideo/wgt_QEditVideo.h"

namespace Ui {
    class DlgImageCorrection;
}

class DlgImageCorrection : public QCustomDialog {
Q_OBJECT
public:
    enum UNDOACTION_ID {
        UNDOACTION_INTERACTIVEMOVERESIZE,
        UNDOACTION_IMAGEEDITZONE,
        UNDOACTION_VIDEOPART
    };

    cBrushDefinition        *CurrentBrush;
    cCompositionObject      *CompoObject;
    ffd_GEOMETRY            ffDPrjGeometry;                 // Project image geometry define in ffDiaporama project
    bool                    UndoReloadImage;                // True if image change and undo must reload it
    QString                 UndoBrushFileName;              // Name of previous file is undo
    qreal                   ImageGeometry;
    qreal                   ProjectGeometry;
    QString                 InitialFilteredString;
    int                     CurrentFramingStyle;
    int                     *BackgroundForm;
    bool                    StopMaj;
    int                     DefaultSpeedWave;

    explicit DlgImageCorrection(cCompositionObject *TheCurrentTextItem,int *BackgroundForm,cBrushDefinition *CurrentBrush,int TheVideoPosition,ffd_GEOMETRY TheProjectGeometry,int DefaultSpeedWave,
                                cBaseApplicationConfig *ApplicationConfig,QWidget *parent=0);
    ~DlgImageCorrection();

    // function to be overloaded
    virtual void            DoInitDialog();                             // Initialise dialog
    virtual bool            DoAccept();                                 // Call when user click on Ok button
    virtual void            DoRejet() { /* Nothing to do */ }           // Call when user click on Cancel button
    virtual void            PrepareGlobalUndo();                        // Initiale Undo
    virtual void            DoGlobalUndo();                             // Apply Undo : call when user click on Cancel button

    virtual void            AppendPartialUndo(int ActionType,QWidget *FocusWindow,bool ForceAdd);
    virtual void            PreparePartialUndo(int ActionType,QDomElement root);
    virtual void            ApplyPartialUndo(int ActionType,QDomElement root);

    virtual void            RefreshControls();

protected:
    virtual void            resizeEvent(QResizeEvent *);
    virtual void            showEvent(QShowEvent *);

protected slots:
    virtual void            DoPartialUndo();

private slots:
    void            s_TabWidgetChanged(int);
    void            s_RulersBT();
    void            s_RotationEDChanged(double Value);
    void            s_XValueEDChanged(double Value);
    void            s_YValueEDChanged(double Value);
    void            s_WValueEDChanged(double Value);
    void            s_HValueEDChanged(double Value);
    void            s_RotateLeft();
    void            s_RotateRight();
    void            s_FramingStyleChanged();
    void            s_BrightnessSliderMoved(int Value);
    void            s_ContrastSliderMoved(int Value);
    void            s_GammaSliderMoved(int Value);
    void            s_GammaValueED(double Value);
    void            s_RedSliderMoved(int Value);
    void            s_GreenSliderMoved(int Value);
    void            s_BlueSliderMoved(int Value);
    void            s_DesatSliderMoved(int Value);
    void            s_OnOffFilter_Gray_Changed(int);
    void            s_OnOffFilter_Equalize_Changed(int);
    void            s_OnOffFilter_Despeckle_Changed(int);
    void            s_OnOffFilter_Negative_Changed(int);
    void            s_OnOffFilter_Emboss_Changed(int);
    void            s_OnOffFilter_Edge_Changed(int);
    void            s_OnOffFilter_Antialias_Changed(int);
    void            s_OnOffFilter_Normalize_Changed(int);
    void            s_OnOffFilter_Charcoal_Changed(int);
    void            s_OnOffFilter_Oil_Changed(int);
    void            s_SwirlSliderMoved(int Value);
    void            s_ImplodeSliderMoved(int Value);

    void            s_BrightnessReset()                     { s_BrightnessSliderMoved(0);   }
    void            s_ContrastReset()                       { s_ContrastSliderMoved(0);     }
    void            s_GammaReset()                          { s_GammaValueED(1);            }
    void            s_RedReset()                            { s_RedSliderMoved(0);          }
    void            s_GreenReset()                          { s_GreenSliderMoved(0);        }
    void            s_BlueReset()                           { s_BlueSliderMoved(0);         }
    void            s_DesatReset()                          { s_DesatSliderMoved(0);        }
    void            s_SwirlReset()                          { s_SwirlSliderMoved(0);        }
    void            s_ImplodeReset()                        { s_ImplodeSliderMoved(0);      }

    // BlurSharpen
    void            s_BlurSharpenTypeChanged(int);
    void            s_BlurSharpenSigmaSliderMoved(int Value);
    void            s_BlurSharpenSigmaValueED(double Value);
    void            s_QuickBlurSharpenSigmaSliderMoved(int Value);
    void            s_BlurSharpenRadiusSliderMoved(int Value);

    void            s_BlurSharpenSigmaReset()               { s_BlurSharpenSigmaSliderMoved(0);         }
    void            s_QuickBlurSharpenSigmaReset()          { s_QuickBlurSharpenSigmaSliderMoved(0);    }
    void            s_BlurSharpenRadiusReset()              { s_BlurSharpenRadiusSliderMoved(5);        }

    void            s_ChangeFile();
    void            s_IntZoneTransformBlocks(qreal Move_X,qreal Move_Y,qreal Scale_X,qreal Scale_Y);
    void            s_DisplayIntZoneTransformBlocks(qreal Move_X,qreal Move_Y,qreal Scale_X,qreal Scale_Y);
    void            s_ShapeBackgroundForm();

    void            s_SpeedWaveChanged(int);

private:
    void            MakeFormIcon(QComboBox *UICB);

    void            CreateVideoTag();

    Ui::DlgImageCorrection *ui;
    wgt_QEditVideo *VideoWidget;
};

#endif // DLGIMAGECORRECTION_H
