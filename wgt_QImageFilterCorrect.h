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

#ifndef wgt_QImageFilterCorrect_H
#define wgt_QImageFilterCorrect_H

#include <QWidget>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include "cdiaporama.h"

class   cDiaporamaObject;

namespace Ui {
    class wgt_QImageFilterCorrect;
}

class wgt_QImageFilterCorrect : public QWidget {
Q_OBJECT
public:
    QGraphicsScene          *scene;
    QGraphicsRectItem       *cadre;
    double                   xmax,ymax;
    cFilterCorrectObject    *Filter;
    cDiaporamaShot          *DiaporamaShotObject;

    explicit wgt_QImageFilterCorrect(QWidget *parent = 0);
    ~wgt_QImageFilterCorrect();

    void    SetFilter(cFilterCorrectObject *Filter,cDiaporamaShot *DiaporamaShotObject);

protected:
    virtual void resizeEvent(QResizeEvent *);
    virtual void showEvent(QShowEvent *);

private slots:
    void    RefreshBackgroundImage();
    void    s_BrightnessSliderMoved(int Value);
    void    s_ContrastSliderMoved(int Value);
    void    s_GammaSliderMoved(int Value);
    void    s_GammaValueED(double Value);
    void    s_RedSliderMoved(int Value);
    void    s_GreenSliderMoved(int Value);
    void    s_BlueSliderMoved(int Value);

signals:
    void        ModifyDataSignal();

private:
    Ui::wgt_QImageFilterCorrect *ui;
};

#endif // wgt_QImageFilterCorrect_H
