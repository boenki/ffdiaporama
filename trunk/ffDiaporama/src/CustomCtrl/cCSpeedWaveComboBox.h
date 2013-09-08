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

#ifndef CCSPEEDWAVECOMBOBOX_H
#define CCSPEEDWAVECOMBOBOX_H

// Basic inclusions (common to all files)
#include "_QCustomDialog.h"
#include "_QCustomComboBox.h"
#include "../engine/_SpeedWave.h"

class cCSpeedWaveComboBox : public QCustomComboBox {
Q_OBJECT
public:
    explicit cCSpeedWaveComboBox(QWidget *parent = 0);

    virtual void AddProjectDefault(int DefaultSpeedWave);
    virtual void SetCurrentValue(int Value);
    virtual int  GetCurrentValue();

signals:
    
public slots:
    
};

#endif // CCSPEEDWAVECOMBOBOX_H
