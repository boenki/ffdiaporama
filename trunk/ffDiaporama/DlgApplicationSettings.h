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

#ifndef DLGAPPLICATIONSETTINGS_H
#define DLGAPPLICATIONSETTINGS_H

// Basic inclusions (common to all files)
#include "_GlobalDefines.h"

// Specific inclusions
#include "_Diaporama.h"

namespace Ui {
    class DlgApplicationSettings;
}

class DlgApplicationSettings : public QDialog {
Q_OBJECT
public:
    cApplicationConfig *ApplicationConfig;
    bool                IsDeviceChanged;
    int                 CurrentDevice;
    int                 CurrentDeviceIndex;
    QComboBox           *CB_SL[9][3];
    QComboBox           *CB_BA[9][3];
    QComboBox           *CB_SL_CLIPARTST[3];
    QComboBox           *CB_BA_CLIPARTST[3];

    explicit DlgApplicationSettings(cApplicationConfig &ApplicationConfig,QWidget *parent = 0);
    ~DlgApplicationSettings();


protected:
    virtual void showEvent(QShowEvent *);
    virtual void reject();
    virtual void accept();

    void        AskApplyDBChange();

private slots:
    void        Help();
    void        TabChanged(int);

    void        SetSavedWindowGeometry();
    void        FileFormatCombo(int);
    void        InitVideoBitRateCB(int);
    void        InitAudioBitRateCB(int);
    void        InitImageSizeCombo(int);

    void        ChangeSmartphoneTypeCB(int);
    void        ChangeMMSystemTypeCB(int);
    void        ChangeForTheWTypeCB(int);

    // Device database tab
    void        DBApplyChange();
    void        DBAddDevice();
    void        DBRemoveBT();
    void        DBResetToDefaultBT();
    void        DBFillTableDevice(int);
    void        DBImageSizeCombo(int);
    void        DBFileFormatCombo(int);
    void        DBSelectionChanged();
    void        DBChImageSizeCB(int);
    void        DBVideoBitRateCB(int);
    void        DBAudioBitRateCB(int);
    void        DBChModel(QString);
    void        DBDeviceSubtypeCB(int);
    void        DBChVideoBitRateCB(int);
    void        DBChAudioBitRateCB(int);

private:
    Ui::DlgApplicationSettings *ui;
};

#endif // DLGAPPLICATIONSETTINGS_H
