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

#ifndef DLGPROJECT_H
#define DLGPROJECT_H

#include <QDialog>
#include "cdiaporama.h"

namespace Ui {
    class DlgProject;
}

class DlgProject : public QDialog {
Q_OBJECT
public:
    cDiaporama *Diaporama;
    explicit DlgProject(cDiaporama &Diaporama,bool NewProjectMode,QWidget *parent = 0);
    ~DlgProject();

protected:
    virtual void showEvent(QShowEvent *);
    virtual void reject();
    virtual void accept();

private slots:
    void        SetSavedWindowGeometry();

private:
    Ui::DlgProject *ui;
};

#endif // DLGPROJECT_H
