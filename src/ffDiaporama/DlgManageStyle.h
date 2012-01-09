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

#ifndef DLGMANAGESTYLE_H
#define DLGMANAGESTYLE_H

// Basic inclusions (common to all files)
#include "../sharedfiles/_GlobalDefines.h"

// Include some additional standard class

// Basic inclusions (common to all files)
#include "_GlobalDefines.h"
#include "_StyleDefinitions.h"

//=====================
// Manage style dialog
//=====================

namespace Ui {
    class DlgManageStyle;
}

class DlgManageStyle : public QDialog {
Q_OBJECT
public:
    cStyleCollection    *Collection;
    cStyleCollection    *UndoCollection;

    explicit DlgManageStyle(cStyleCollection *Collection,QWidget *parent=0);
    ~DlgManageStyle();

    void        PopulateList(QString StyleToActivate);

protected:
    virtual void showEvent(QShowEvent *);
    virtual void reject();
    virtual void accept();

private slots:
    void        Help();
    void        SetSavedWindowGeometry();

    void        s_currentRowChanged(int);
    void        s_DBRename();
    void        s_DBRemove();
    void        s_DBReset();

private:
    Ui::DlgManageStyle *ui;
};

#endif // DLGMANAGESTYLE_H
