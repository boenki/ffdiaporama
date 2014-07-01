/* ======================================================================
    This file is part of ffDiaporama
    ffDiaporama is a tools to make diaporama as video
    Copyright (C) 2011-2014 Dominique Levray <domledom@laposte.net>

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

#ifndef CCCOLORCOMBOBOX_H
#define CCCOLORCOMBOBOX_H

#include "BasicDefines.h"
#include <QComboBox>
#include <QStyledItemDelegate>

//******************************************************************************************************************
// Custom QAbstractItemDelegate for Color ComboBox
//******************************************************************************************************************

class cCColorComboBox;
class cCColorComboBoxItem : public QStyledItemDelegate {
Q_OBJECT
public:
    cCColorComboBox    *ComboBox;

    explicit cCColorComboBoxItem(QObject *parent=0);

    virtual void    paint(QPainter *painter,const QStyleOptionViewItem &option,const QModelIndex &index) const;
    virtual QSize   sizeHint(const QStyleOptionViewItem &option,const QModelIndex &index) const;
};

//******************************************************************************************************************
// Custom Color ComboBox
//******************************************************************************************************************

class cCColorComboBox : public QComboBox {
Q_OBJECT
public:
    QString             *CurrentColor;
    QString             SavedCustomColor;
    bool                StandardColor;
    bool                STOPMAJ;
    bool                IsPopupOpen;
    cCColorComboBoxItem ItemDelegate;

    explicit            cCColorComboBox(QWidget *parent = 0);
    void                MakeIcons();
    void                SetCurrentColor(QString *Color);
    QString             GetCurrentColor();

protected:
    virtual void        hidePopup() { IsPopupOpen=false; QComboBox::hidePopup(); emit PopupClosed(0); }
    virtual void        showPopup() { IsPopupOpen=true;  QComboBox::showPopup(); }

signals:
    void                PopupClosed(int);

public slots:
    void                s_ItemSelectionChanged();
    void                s_ItemPressed(int Row,int Col);
};

#endif // CCCOLORCOMBOBOX_H