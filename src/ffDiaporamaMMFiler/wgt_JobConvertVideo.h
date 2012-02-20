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

#ifndef wgt_JobConvertVideo_H
#define wgt_JobConvertVideo_H

#include <QWidget>

// Basic inclusions (common to all files)
#include "../sharedfiles/_GlobalDefines.h"

#include "wgt_JobBase.h"

namespace Ui {
    class wgt_JobConvertVideo;
}

class wgt_JobConvertVideo : public wgt_JobBase {
Q_OBJECT
public:
    explicit wgt_JobConvertVideo(QCustomDialog *Dialog,QWidget *parent = 0);
    ~wgt_JobConvertVideo();

    virtual void    DoInitDialog();
    virtual void    RefreshControls();
    virtual void    AppendJobSummary(int index,QString *JobSummary,cJobQueue *JobQueue);

private slots:
    void    s_DeviceCB(int);
    void    s_DeviceTypeCB(int);
    void    s_DeviceModelCB(int);

private:
    Ui::wgt_JobConvertVideo *ui;
};

#endif // wgt_JobConvertVideo_H
