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

#ifndef cCustomColorComboBox_H
#define cCustomColorComboBox_H

// Basic inclusions (common to all files)
#include "../engine/_GlobalDefines.h"

// Specific inclusions
#include "_Diaporama.h"

#include <QStyledItemDelegate>
#include <QLabel>

//======================================
// Specific defines for this dialog box
//======================================
#define COMBOTYPEITEM_COLOR     0
#define COMBOTYPEITEM_BRUSH     1

//******************************************************************************************************************
// Custom QAbstractItemDelegate for Background ComboBox
//******************************************************************************************************************

class cBackgroundComboBox;
class cBackgroundComboBoxItem : public QStyledItemDelegate {
Q_OBJECT
public:
    cBackgroundComboBox    *ComboBox;

    explicit cBackgroundComboBoxItem(QObject *parent=0);

    virtual void    paint(QPainter *painter,const QStyleOptionViewItem &option,const QModelIndex &index) const;
    virtual QSize   sizeHint(const QStyleOptionViewItem &option,const QModelIndex &index) const;
};

//******************************************************************************************************************
// Custom Brush ComboBox
//******************************************************************************************************************

class cBackgroundComboBox : public QComboBox {
Q_OBJECT
public:
    bool                       STOPMAJ;
    QString                    BrushImage;
    cBackgroundComboBoxItem    ItemDelegate;

    explicit            cBackgroundComboBox(QWidget *parent = 0);
    void                MakeIcons();
    void                SetCurrentBackground(QString BrushImage);
    QString             GetCurrentBackground();

protected:

signals:

public slots:
    void s_ItemSelectionChanged();
};

#endif // cCustomColorComboBox_H
