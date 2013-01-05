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

#ifndef CBACKGROUNDCOMBOBOX_H
#define CBACKGROUNDCOMBOBOX_H

// Basic inclusions (common to all files)
#include "../engine/_GlobalDefines.h"
#include "../engine/cBrushDefinition.h"

#include <QComboBox>
#include <QStyledItemDelegate>

//******************************************************************************************************************
// Custom QAbstractItemDelegate for Brush ComboBox
//******************************************************************************************************************

class cBackgroundComboBox;
class cCBackgroundComboBoxItem : public QStyledItemDelegate {
Q_OBJECT
public:
    cBackgroundComboBox    *ComboBox;

    explicit cCBackgroundComboBoxItem(QObject *parent=0);

    virtual void    paint(QPainter *painter,const QStyleOptionViewItem &option,const QModelIndex &index) const;
    virtual QSize   sizeHint(const QStyleOptionViewItem &option,const QModelIndex &index) const;
};

class cBackgroundComboBox : public QComboBox {
Q_OBJECT
public:
    cBackgroundList             *BackgroundTable;
    int                         CurrentSel;
    int                         Geometry;
    bool                        STOPMAJ;
    int                         ImageWidth;
    cCBackgroundComboBoxItem    ItemDelegate;


    explicit            cBackgroundComboBox(QWidget *parent = 0);
    void                PrepareTable(int ProjectGeometry,cBackgroundList *Table);
    void                SetCurrentBackground(QString BackgroundName);
    QString             GetCurrentBackground();
    void                MakeIcons();

protected:
    virtual void        hidePopup();
    virtual void        keyReleaseEvent(QKeyEvent *event);

signals:
    void itemSelectionHaveChanged();

public slots:
    void s_ItemSelectionChanged();
};

#endif // CBACKGROUNDCOMBOBOX_H
