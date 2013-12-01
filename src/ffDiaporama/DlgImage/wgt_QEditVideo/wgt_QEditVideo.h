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

#ifndef WGT_QEDITVIDEO_H
#define WGT_QEDITVIDEO_H

// Basic inclusions (common to all files)
#include "engine/_GlobalDefines.h"
#include "CustomCtrl/_QCustomDialog.h"
#include "engine/_Diaporama.h"

namespace Ui {
    class wgt_QEditVideo;
}

class wgt_QEditVideo : public QWidget {
Q_OBJECT

public:
    cBrushDefinition        *CurrentBrush;
    QCustomDialog           *ParentDialog;
    bool                    StopMaj;

    explicit                wgt_QEditVideo(QWidget *parent = 0);
                            ~wgt_QEditVideo();

    void                    DoInitWidget(QCustomDialog *ParentDialog,cBrushDefinition *CurrentBrush);
    void                    DoInitDialog();
    void                    RefreshControls(bool EmitToParent=true);
    void                    WinFocus();
    void                    LostFocus();

private slots:
    void                    s_Event_SaveImageEvent();
    void                    s_DefStartPos();
    void                    s_DefEndPos();
    void                    s_SeekLeft();
    void                    s_SeekRight();
    void                    s_EditStartPos(QTime NewValue);
    void                    s_EditEndPos(QTime NewValue);
    void                    s_MusicReduceFactorChange(int);
    void                    s_Deinterlace(int);

signals:
    void                    DoRefreshControls();

private:
    Ui::wgt_QEditVideo      *ui;
};

#endif // WGT_QEDITVIDEO_H
