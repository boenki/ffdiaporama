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

#include "cCustomComboBox.h"

#include <QHeaderView>
#include <QColorDialog>

//======================================
// Specific defines for this dialog box
//======================================
#define MAXCOLORREF             99
#define MAXBRUSHPATTERN         12
#define MAXGRADIENTORIENTATION  9
#define ICONWIDTH               64
#define ICONHEIGHT              16

//******************************************************************************************************************
// Custom Color ComboBox ITEM
//******************************************************************************************************************
QString  ColorRef[MAXCOLORREF]={
    "#f2f2f2","#d8d8d8","#bfbfbf","#a5a5a5","#7f7f7f",      // LIGHT-GRAY
    "#595959","#3f3f3f","#262626","#0c0c0c","#000000",      // DARK-GRAY
    "#dae1eb","#b5c4d7","#91a7c3","#3c526f","#28374a",      // BLUE-GRAY
    "#c8eefc","#91defa","#5acef8","#0578a2","#03506c",      // BLUE-1
    "#e0e6f5","#c1ceeb","#a2b5e2","#365bb0","#243c75",      // BLUE-2
    "#e8eeee","#d1dede","#b9cdce","#61888a","#405b5c",      // BLUE-3
    "#e5ecd8","#cbd9b2","#b2c78c","#5c7237","#3d4c25",      // GREEN-1
    "#e8efe8","#d2dfd1","#bbcfba","#648c60","#425d40",      // GREEN-2
    "#e1dca5","#d0c974","#a29a36","#514d1b","#201e0a",      // GREEN-3
    "#f5f2d8","#ece5b2","#e2d88c","#a39428","#6d621a",      // GREEN-4
    "#f2eee8","#e6ded1","#dacdba","#a38557","#6d593a",      // MARROON-1
    "#f6e6d5","#eeceaa","#e6b681","#a2641f","#6c4315",      // MARROON-2
    "#f2e0c6","#e6c28d","#daa454","#664515","#442e0e",      // MARROON-3
    "#fff7c1","#fff084","#ffe947","#998700","#665a00",      // YELLOW-1
    "#fde1d1","#fcc3a3","#fba576","#c94b05","#863203",      // ORANGE
    "#fbc7bc","#f78f7a","#f35838","#711806","#4b1004",      // RED-1
    "#e5e1f4","#cbc3e9","#b1a6de","#533da9","#372970",      // VIOLET-1
    "#ece4f1","#dac9e3","#c7aed6","#7d4d99","#533366",      // VIOLET-2

    "#000000","#ff0000","#00ff00","#0000ff","#ffffff",      // Full-colors
    "#ffff00","#ff00ff","#00ffff","#3a3a3a"
};

//========================================================================================================================

cCustomColorComboBoxItem::cCustomColorComboBoxItem(QObject *parent):QStyledItemDelegate(parent) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomColorComboBoxItem::cCustomColorComboBoxItem");
}

//========================================================================================================================

void cCustomColorComboBoxItem::paint(QPainter *painter,const QStyleOptionViewItem &option,const QModelIndex &index) const {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomColorComboBoxItem::paint");
    int ColorNum=index.row()*5+index.column();
    if (ColorNum<MAXCOLORREF) {
        painter->setPen(Qt::NoPen);
        painter->setBrush(QBrush(QColor(ColorRef[ColorNum])));
        painter->drawRect(option.rect);
    } else {
        if (ColorNum==MAXCOLORREF) {
            painter->fillRect(option.rect,QColor(ComboBox->SavedCustomColor));
            QImage  Img(":/img/colorize.png");
            painter->drawImage(QRectF(option.rect.x()+(option.rect.width()-16)/2,option.rect.y()+(option.rect.height()-16)/2,16,16),Img);
        } else {
            painter->fillRect(option.rect,Qt::white);
        }
    }
    if (option.state & QStyle::State_Selected) {
        painter->setPen(QPen(Qt::blue));
        painter->setBrush(QBrush(Qt::NoBrush));
        painter->drawRect(option.rect.x(),option.rect.y(),option.rect.width()-1,option.rect.height()-1);
        painter->drawRect(option.rect.x()+1,option.rect.y()+1,option.rect.width()-1-2,option.rect.height()-1-2);
        painter->setPen(QPen(Qt::black));
        painter->drawRect(option.rect.x()+2,option.rect.y()+2,option.rect.width()-1-4,option.rect.height()-1-4);
    }
}

//========================================================================================================================

QSize cCustomColorComboBoxItem::sizeHint(const QStyleOptionViewItem &/*option*/,const QModelIndex &/*index*/) const {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomColorComboBoxItem::sizeHint");
    return QSize(24,24);
}

//******************************************************************************************************************
// Custom Color ComboBox
//******************************************************************************************************************

cCustomColorComboBox::cCustomColorComboBox(QWidget *parent):QComboBox(parent) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomColorComboBox::cCustomColorComboBox");
    STOPMAJ=false;
    CurrentColor=NULL;

    QTableWidget    *Table=new QTableWidget();
    Table->horizontalHeader()->hide();
    Table->verticalHeader()->hide();
    Table->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    Table->insertColumn(Table->columnCount());  Table->setColumnWidth(Table->columnCount()-1,24);
    Table->insertColumn(Table->columnCount());  Table->setColumnWidth(Table->columnCount()-1,24);
    Table->insertColumn(Table->columnCount());  Table->setColumnWidth(Table->columnCount()-1,24);
    Table->insertColumn(Table->columnCount());  Table->setColumnWidth(Table->columnCount()-1,24);
    Table->insertColumn(Table->columnCount());  Table->setColumnWidth(Table->columnCount()-1,24);
    setModel(Table->model());
    setView(Table);
    int i=0;
    while (i<=MAXCOLORREF) {
        if ((i/5)>=Table->rowCount()) {
            addItem("");        // automaticaly do a Table->insertRow(Table->rowCount());
            Table->setRowHeight(Table->rowCount()-1,24);
        }
        i++;
    }
    ItemDelegate.ComboBox=this;
    setItemDelegate(&ItemDelegate);
    MakeIcons();
    this->view()->setFixedWidth(5*24+18);
    this->view()->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    connect(Table,SIGNAL(itemSelectionChanged()),this,SLOT(s_ItemSelectionChanged()));
    connect(Table,SIGNAL(cellPressed(int,int)),this,SLOT(s_ItemPressed(int,int)));
}

//========================================================================================================================

void cCustomColorComboBox::SetCurrentColor(QString *Color) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomColorComboBox::SetCurrentColor");
    CurrentColor=Color;
    if (!Color) return;
    int i=0;
    while ((i<MAXCOLORREF)&&(ColorRef[i]!=*CurrentColor)) i++;
    int Row=i/5;
    int Col=i-(i/5)*5;
    StandardColor=((i>=0)&&(i<MAXCOLORREF));
    if (STOPMAJ) return;
    if (!StandardColor) {
        SavedCustomColor=*CurrentColor;
        ((QTableWidget *)view())->setCurrentCell(Row,5);
        setCurrentIndex(Row);
    } else {
        //SavedCustomColor=ColorRef[i]/*"#fefefe"*/;
        ((QTableWidget *)view())->setCurrentCell(Row,Col);
        setCurrentIndex(Row);
    }
    MakeIcons();
}

//========================================================================================================================

QString cCustomColorComboBox::GetCurrentColor() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomColorComboBox::GetCurrentColor");
    if (!CurrentColor) return SavedCustomColor;
    int i=((QTableWidget *)view())->currentRow()*5+((QTableWidget *)view())->currentColumn();
    StandardColor=((i>=0)&&(i<MAXCOLORREF));
    if ((i>=0)&&(i<MAXCOLORREF)) {
        *CurrentColor=ColorRef[i];
        return *CurrentColor;
    }
    return SavedCustomColor;
}

//========================================================================================================================

void cCustomColorComboBox::MakeIcons() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomColorComboBox::MakeIcons");
    int ColorNum=0;
    if (CurrentColor) {
        while ((ColorNum<MAXCOLORREF)&&(ColorRef[ColorNum]!=*CurrentColor)) ColorNum++;
    } else {
        int CurrentRow=((QTableWidget *)view())->currentRow();      if (CurrentRow<0) CurrentRow=0;
        int CurrentCol=((QTableWidget *)view())->currentColumn();   if (CurrentCol<0) CurrentCol=0;
        ColorNum=CurrentRow*5+CurrentCol;
    }
    //setIconSize(QSize(width()-16,16));
    setIconSize(QSize(24,12));
    QPixmap  Image(iconSize());
    QPainter Painter;
    Painter.begin(&Image);
    if (ColorNum<MAXCOLORREF) {
        Painter.fillRect(QRectF(0,0,iconSize().width(),iconSize().height()),QColor(ColorRef[ColorNum]));
    } else {
        Painter.fillRect(QRectF(0,0,iconSize().width(),iconSize().height()),QColor(SavedCustomColor));
        QImage  Img(":/img/colorize.png");
        //Painter.drawImage(QRectF((iconSize().width()-16)/2,(iconSize().height()-16)/2,16,16),Img,QRectF(0,0,Img.width(),Img.height()));
        Painter.drawImage(QRectF(0,0,8,8),Img,QRectF(0,0,Img.width(),Img.height()));
    }
    Painter.end();
    setItemIcon(currentIndex(),QIcon(Image));
}

//========================================================================================================================

void cCustomColorComboBox::s_ItemSelectionChanged() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomColorComboBox::s_ItemSelectionChanged");
    STOPMAJ=true;
    setCurrentIndex(((QTableWidget *)view())->currentRow());
    MakeIcons();
    emit currentIndexChanged(((QTableWidget *)view())->currentRow()*5+((QTableWidget *)view())->currentColumn());
    STOPMAJ=false;
}

//========================================================================================================================

void cCustomColorComboBox::s_ItemPressed(int,int) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomColorComboBox::s_ItemPressed");
    int CurrentRow=((QTableWidget *)view())->currentRow();      if (CurrentRow<0) CurrentRow=0;
    int CurrentCol=((QTableWidget *)view())->currentColumn();   if (CurrentCol<0) CurrentCol=0;
    int ColorNum=CurrentRow*5+CurrentCol;
    if (ColorNum>=MAXCOLORREF) {
        // Open box to select custom color
        QColor color=QColorDialog::getColor(SavedCustomColor);
        if (color.isValid()) {
            STOPMAJ=true;
            SavedCustomColor=color.name();
            MakeIcons();
            emit currentIndexChanged(((QTableWidget *)view())->currentRow()*5+((QTableWidget *)view())->currentColumn());
            STOPMAJ=false;
        }
    }
}

//******************************************************************************************************************
// Custom Brush ComboBox ITEM
//******************************************************************************************************************

cCustomBrushComboBoxItem::cCustomBrushComboBoxItem(QObject *parent):QStyledItemDelegate(parent) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomBrushComboBoxItem::cCustomBrushComboBoxItem");
}

//========================================================================================================================
void cCustomBrushComboBoxItem::paint(QPainter *painter,const QStyleOptionViewItem &option,const QModelIndex &index) const {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomBrushComboBoxItem::paint");
    if ((!ComboBox)||(!ComboBox->Brush)) return;
    int ColorNum=index.row()*4+index.column();
    if (ColorNum<MAXBRUSHPATTERN) {
        cBrushDefinition Brush(ComboBox->Brush->ApplicationConfig,&BackgroundList);
        Brush.ColorD     =ComboBox->Brush->ColorD;
        Brush.BrushType  =BRUSHTYPE_PATTERN;
        Brush.PatternType=ColorNum;
        painter->fillRect(option.rect,Brush.ColorD.toLower()!="#ffffff"?Qt::white:Qt::black);
        QBrush *BR=Brush.GetBrush(option.rect,true,0,0,NULL,1,NULL);
        if (BR) {
            painter->setBrush(*BR);
            delete BR;
        }
        painter->drawRect(option.rect);
    } else {
        painter->fillRect(option.rect,Qt::white);
    }
    if (option.state & QStyle::State_Selected) {
        painter->setPen(QPen(Qt::blue));
        painter->setBrush(QBrush(Qt::NoBrush));
        painter->drawRect(option.rect.x(),option.rect.y(),option.rect.width()-1,option.rect.height()-1);
        painter->drawRect(option.rect.x()+1,option.rect.y()+1,option.rect.width()-1-2,option.rect.height()-1-2);
        painter->setPen(QPen(Qt::black));
        painter->drawRect(option.rect.x()+2,option.rect.y()+2,option.rect.width()-1-4,option.rect.height()-1-4);
    }
}

//========================================================================================================================

QSize cCustomBrushComboBoxItem::sizeHint(const QStyleOptionViewItem &/*option*/,const QModelIndex &/*index*/) const {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomBrushComboBoxItem::sizeHint");
    return QSize(24,24);
}

//******************************************************************************************************************
// Custom Brush ComboBox
//******************************************************************************************************************

cCustomBrushComboBox::cCustomBrushComboBox(QWidget *parent):QComboBox(parent) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomBrushComboBox::cCustomBrushComboBox");
    STOPMAJ=false;
    Brush=NULL;
    QTableWidget    *Table=new QTableWidget();
    Table->horizontalHeader()->hide();
    Table->verticalHeader()->hide();
    Table->insertColumn(Table->columnCount());  Table->setColumnWidth(Table->columnCount()-1,24);
    Table->insertColumn(Table->columnCount());  Table->setColumnWidth(Table->columnCount()-1,24);
    Table->insertColumn(Table->columnCount());  Table->setColumnWidth(Table->columnCount()-1,24);
    Table->insertColumn(Table->columnCount());  Table->setColumnWidth(Table->columnCount()-1,24);
    setModel(Table->model());
    setView(Table);
    int i=0;
    while (i<MAXBRUSHPATTERN) {
        if ((i/4)>=Table->rowCount()) {
            addItem("");    //automaticaly do a Table->insertRow(Table->rowCount());
            Table->setRowHeight(Table->rowCount()-1,24);
        }
        i++;
    }
    ItemDelegate.ComboBox=this;
    setItemDelegate(&ItemDelegate);
    MakeIcons();
    this->view()->setFixedWidth(4*24+18);
    this->view()->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    connect(Table,SIGNAL(itemSelectionChanged()),this,SLOT(s_ItemSelectionChanged()));
}

//========================================================================================================================

void cCustomBrushComboBox::SetCurrentBrush(cBrushDefinition *TheBrush) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomBrushComboBox::SetCurrentBrush");
    if (STOPMAJ) return;
    Brush=TheBrush;
    ((QTableWidget *)view())->setCurrentCell(Brush->PatternType/4,Brush->PatternType-(Brush->PatternType/4)*4);
    setCurrentIndex(Brush->PatternType/4);
    MakeIcons();
}

//========================================================================================================================

cBrushDefinition *cCustomBrushComboBox::GetCurrentBrush() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomBrushComboBox::GetCurrentBrush");
    Brush->BrushType  =BRUSHTYPE_PATTERN;
    Brush->PatternType=currentIndex()*4+((QTableWidget *)view())->currentColumn();
    MakeIcons();
    return Brush;
}

//========================================================================================================================

void cCustomBrushComboBox::MakeIcons() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomBrushComboBox::MakeIcons");
    if (!Brush) return;
    int CurrentRow=currentIndex();
    if (CurrentRow<0) return;
    int CurrentCol=((QTableWidget *)view())->currentColumn();
    if (CurrentCol<0) CurrentCol=0;
    int ColorNum=CurrentRow*4+CurrentCol;
    //setIconSize(QSize(width()-16,16));
    setIconSize(QSize(24,12));
    QPixmap  Image(iconSize());
    QPainter Painter;
    Painter.begin(&Image);
    if (ColorNum<MAXBRUSHPATTERN) {
        cBrushDefinition TheBrush(Brush->ApplicationConfig,&BackgroundList);
        TheBrush.ColorD=Brush->ColorD;
        TheBrush.BrushType  =BRUSHTYPE_PATTERN;
        TheBrush.PatternType=ColorNum;
        Painter.fillRect(QRectF(0,0,iconSize().width(),iconSize().height()),TheBrush.ColorD.toLower()!="#ffffff"?Qt::white:Qt::black);
        QBrush *BR=TheBrush.GetBrush(QRectF(0,0,iconSize().width(),iconSize().height()),true,0,0,NULL,1,NULL);
        if (BR) {
            Painter.setBrush(*BR);
            delete BR;
        }
        Painter.drawRect(QRectF(0,0,iconSize().width(),iconSize().height()));
    } else {
        Painter.setBrush(QBrush(Qt::white));
        Painter.drawRect(QRectF(0,0,iconSize().width(),iconSize().height()));
    }
    Painter.end();
    setItemIcon(CurrentRow,QIcon(Image));
}

//========================================================================================================================

void cCustomBrushComboBox::s_ItemSelectionChanged() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomBrushComboBox::s_ItemSelectionChanged");
    STOPMAJ=true;
    setCurrentIndex(((QTableWidget *)view())->currentRow());
    MakeIcons();
    emit currentIndexChanged(((QTableWidget *)view())->currentRow()*4+((QTableWidget *)view())->currentColumn());
    STOPMAJ=false;
}

//******************************************************************************************************************
// Custom Gradient Orientation ComboBox ITEM
//******************************************************************************************************************

cGradientOrientationComboBoxItem::cGradientOrientationComboBoxItem(QObject *parent):QStyledItemDelegate(parent) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cGradientOrientationComboBoxItem::cGradientOrientationComboBoxItem");
}

//========================================================================================================================
void cGradientOrientationComboBoxItem::paint(QPainter *painter,const QStyleOptionViewItem &option,const QModelIndex &index) const {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cGradientOrientationComboBoxItem::paint");
    if ((!ComboBox)||(!ComboBox->Brush)) return;
    int ColorNum=index.row()*3+index.column();
    if ((ColorNum>=0)&&(ColorNum<MAXGRADIENTORIENTATION)) {
        cBrushDefinition Brush(ComboBox->Brush->ApplicationConfig,&BackgroundList);
        Brush.BrushType=ComboBox->Brush->BrushType;
        Brush.ColorD=ComboBox->Brush->ColorD;
        Brush.ColorF=ComboBox->Brush->ColorF;
        Brush.ColorIntermed=ComboBox->Brush->ColorIntermed;
        Brush.Intermediate=ComboBox->Brush->Intermediate;
        Brush.GradientOrientation=ColorNum;
        QBrush *BR=Brush.GetBrush(option.rect,true,0,0,NULL,1,NULL);
        if (BR) {
            painter->setBrush(*BR);
            delete BR;
        }
        painter->drawRect(option.rect);
    } else {
        painter->fillRect(option.rect,Qt::white);
    }
    if (option.state & QStyle::State_Selected) {
        painter->setPen(QPen(Qt::blue));
        painter->setBrush(QBrush(Qt::NoBrush));
        painter->drawRect(option.rect.x(),option.rect.y(),option.rect.width()-1,option.rect.height()-1);
        painter->drawRect(option.rect.x()+1,option.rect.y()+1,option.rect.width()-1-2,option.rect.height()-1-2);
        painter->setPen(QPen(Qt::black));
        painter->drawRect(option.rect.x()+2,option.rect.y()+2,option.rect.width()-1-4,option.rect.height()-1-4);
    }
}

//========================================================================================================================

QSize cGradientOrientationComboBoxItem::sizeHint(const QStyleOptionViewItem &/*option*/,const QModelIndex &/*index*/) const {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cGradientOrientationComboBoxItem::sizeHint");
    return QSize(32,32);
}

//******************************************************************************************************************
// Custom Gradient Orientation ComboBox
//******************************************************************************************************************

cGradientOrientationComboBox::cGradientOrientationComboBox(QWidget *parent):QComboBox(parent) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cGradientOrientationComboBox::cGradientOrientationComboBox");
    STOPMAJ=false;
    Brush=NULL;
    QTableWidget    *Table=new QTableWidget();
    Table->horizontalHeader()->hide();
    Table->verticalHeader()->hide();
    Table->insertColumn(Table->columnCount());  Table->setColumnWidth(Table->columnCount()-1,32);
    Table->insertColumn(Table->columnCount());  Table->setColumnWidth(Table->columnCount()-1,32);
    Table->insertColumn(Table->columnCount());  Table->setColumnWidth(Table->columnCount()-1,32);
    setModel(Table->model());
    setView(Table);
    int i=0;
    while (i<MAXGRADIENTORIENTATION) {
        if ((i/3)>=Table->rowCount()) {
            addItem("");    //automaticaly do a Table->insertRow(Table->rowCount());
            Table->setRowHeight(Table->rowCount()-1,32);
        }
        i++;
    }
    ItemDelegate.ComboBox=this;
    setItemDelegate(&ItemDelegate);
    MakeIcons();
    this->view()->setFixedWidth(3*32+18);
    this->view()->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    connect(Table,SIGNAL(itemSelectionChanged()),this,SLOT(s_ItemSelectionChanged()));
}

//========================================================================================================================

void cGradientOrientationComboBox::SetCurrentBrush(cBrushDefinition *TheBrush) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cGradientOrientationComboBox::SetCurrentBrush");
    if (STOPMAJ) return;
    Brush=TheBrush;
    setCurrentIndex(Brush->GradientOrientation/3);
    ((QTableWidget *)view())->setCurrentCell(Brush->GradientOrientation/3,Brush->GradientOrientation-(Brush->GradientOrientation/3)*3);
    MakeIcons();
}

//========================================================================================================================

cBrushDefinition *cGradientOrientationComboBox::GetCurrentBrush() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cGradientOrientationComboBox::GetCurrentBrush");
    if (Brush) {
        Brush->GradientOrientation=currentIndex()*3+((QTableWidget *)view())->currentColumn();
        MakeIcons();
    }
    return Brush;
}

//========================================================================================================================

void cGradientOrientationComboBox::MakeIcons() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cGradientOrientationComboBox::MakeIcons");
    if (!Brush) return;
    int CurrentRow=currentIndex();
    if (CurrentRow<0) return;
    int CurrentCol=((QTableWidget *)view())->currentColumn();
    if (CurrentCol<0) CurrentCol=0;
    int ColorNum=CurrentRow*3+CurrentCol;
    //setIconSize(QSize(width()-16,16));
    setIconSize(QSize(24,12));
    QPixmap  Image(iconSize());
    QPainter Painter;
    Painter.begin(&Image);
    if ((ColorNum>=0)&&(ColorNum<MAXGRADIENTORIENTATION)) {
        cBrushDefinition TheBrush(Brush->ApplicationConfig,&BackgroundList);
        TheBrush.BrushType=Brush->BrushType;
        TheBrush.ColorD=Brush->ColorD;
        TheBrush.ColorF=Brush->ColorF;
        TheBrush.ColorIntermed=Brush->ColorIntermed;
        TheBrush.Intermediate=Brush->Intermediate;
        TheBrush.GradientOrientation=ColorNum;
        QBrush *BR=TheBrush.GetBrush(QRectF(0,0,iconSize().width(),iconSize().height()),true,0,0,NULL,1,NULL);
        if (BR) {
            Painter.setBrush(*BR);
            delete BR;
        }
        Painter.drawRect(QRectF(0,0,iconSize().width(),iconSize().height()));
    } else {
        Painter.setBrush(QBrush(Qt::white));
        Painter.drawRect(QRectF(0,0,iconSize().width(),iconSize().height()));
    }
    Painter.end();
    setItemIcon(CurrentRow,QIcon(Image));
}

//========================================================================================================================

void cGradientOrientationComboBox::s_ItemSelectionChanged() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cGradientOrientationComboBox::s_ItemSelectionChanged");
    STOPMAJ=true;
    setCurrentIndex(((QTableWidget *)view())->currentRow());
    MakeIcons();
    emit currentIndexChanged(((QTableWidget *)view())->currentRow()*3+((QTableWidget *)view())->currentColumn());
    STOPMAJ=false;
}

//******************************************************************************************************************
// Custom Brush ComboBox ITEM
//******************************************************************************************************************

cBackgroundComboBoxItem::cBackgroundComboBoxItem(QObject *parent):QStyledItemDelegate(parent) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBackgroundComboBoxItem::cBackgroundComboBoxItem");
}

//========================================================================================================================
void cBackgroundComboBoxItem::paint(QPainter *painter,const QStyleOptionViewItem &option,const QModelIndex &index) const {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBackgroundComboBoxItem::paint");
    int BackgroundNum=index.row();
    if (BackgroundNum<BackgroundList.List.count()) {
        painter->drawPixmap(option.rect.left(),option.rect.top(),BackgroundList.List[BackgroundNum].Icon);
    } else {
        painter->fillRect(option.rect,Qt::white);
    }
    if (option.state & QStyle::State_Selected) {
        painter->setPen(QPen(Qt::blue));
        painter->setBrush(QBrush(Qt::NoBrush));
        painter->drawRect(option.rect.x(),option.rect.y(),option.rect.width()-1,option.rect.height()-1);
        painter->drawRect(option.rect.x()+1,option.rect.y()+1,option.rect.width()-1-2,option.rect.height()-1-2);
        painter->setPen(QPen(Qt::black));
        painter->drawRect(option.rect.x()+2,option.rect.y()+2,option.rect.width()-1-4,option.rect.height()-1-4);
    }
}

//========================================================================================================================

QSize cBackgroundComboBoxItem::sizeHint(const QStyleOptionViewItem &/*option*/,const QModelIndex &/*index*/) const {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBackgroundComboBoxItem::sizeHint");
    if (BackgroundList.List.count()>0) return QSize(BackgroundList.List[0].Icon.width(),64);
        else return QSize(114,64);
}

//******************************************************************************************************************
// Custom Brush ComboBox
//******************************************************************************************************************

cBackgroundComboBox::cBackgroundComboBox(QWidget *parent):QComboBox(parent) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBackgroundComboBox::cBackgroundComboBox");
    STOPMAJ=false;
    QTableWidget    *Table=new QTableWidget();
    Table->horizontalHeader()->hide();
    Table->verticalHeader()->hide();
    int ThumbWith=(BackgroundList.List.count()>0)?BackgroundList.List[0].Icon.width():114;
    setIconSize(QSize(ThumbWith,64));
    Table->insertColumn(Table->columnCount());  Table->setColumnWidth(Table->columnCount()-1,ThumbWith);
    setModel(Table->model());
    setView(Table);
    int i=0;
    while (i<BackgroundList.List.count()) {
        addItem("");    //automaticaly do a Table->insertRow(Table->rowCount());
        Table->setRowHeight(Table->rowCount()-1,64);
        i++;
    }
    ItemDelegate.ComboBox=this;
    setItemDelegate(&ItemDelegate);
    MakeIcons();
    this->view()->setFixedWidth(114+18);
    this->view()->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    connect(Table,SIGNAL(itemSelectionChanged()),this,SLOT(s_ItemSelectionChanged()));
}

//========================================================================================================================

void cBackgroundComboBox::SetCurrentBackground(QString BrushImage) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBackgroundComboBox::SetCurrentBackground");
    if (STOPMAJ) return;
    int i=0;
    while ((i<BackgroundList.List.count())&&(BrushImage!=BackgroundList.List[i].Name)) i++;
    if (i>=BackgroundList.List.count()) {
        i=0;
        ((QTableWidget *)view())->setCurrentCell(0,i);
        s_ItemSelectionChanged();
    }
    ((QTableWidget *)view())->setCurrentCell(0,i);
    setCurrentIndex(i);
    MakeIcons();
}

//========================================================================================================================

QString cBackgroundComboBox::GetCurrentBackground() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBackgroundComboBox::GetCurrentBackground");
    int i=currentIndex();
    MakeIcons();
    return BackgroundList.List[i].Name;
}

//========================================================================================================================

void cBackgroundComboBox::MakeIcons() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBackgroundComboBox::MakeIcons");
    int CurrentRow=currentIndex();
    if (CurrentRow<0) return;
    int CurrentCol=((QTableWidget *)view())->currentColumn();
    if (CurrentCol<0) CurrentCol=0;
    int BackgroundNum=CurrentRow;
    int ThumbWith=(BackgroundList.List.count()>0)?BackgroundList.List[0].Icon.width():114;
    QPixmap  Image(ThumbWith,64);
    QPainter Painter;
    Painter.begin(&Image);
    if (BackgroundNum<BackgroundList.List.count()) {
        Painter.drawPixmap(0,0,BackgroundList.List[BackgroundNum].Icon);
    } else {
        Painter.setBrush(QBrush(Qt::white));
        Painter.drawRect(QRectF(0,0,ThumbWith,64));
    }
    Painter.end();
    setItemIcon(CurrentRow,QIcon(Image));
}

//========================================================================================================================

void cBackgroundComboBox::s_ItemSelectionChanged() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBackgroundComboBox::s_ItemSelectionChanged");
    STOPMAJ=true;
    setCurrentIndex(((QTableWidget *)view())->currentRow());
    MakeIcons();
    emit currentIndexChanged(((QTableWidget *)view())->currentRow());
    STOPMAJ=false;
}

//========================================================================================================================

cCustomLabel::cCustomLabel(QWidget *parent):QLabel(parent) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomLabel::cCustomLabel");
    setAttribute(Qt::WA_PaintOutsidePaintEvent, true);
}

void cCustomLabel::paintEvent(QPaintEvent *) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomLabel::paintEvent");
    DisplayCustomText(text());
}

void cCustomLabel::DisplayCustomText(QString Text) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomLabel::DisplayCustomText");
    setText(Text);
    QPainter Painter(this);
    Painter.save();
    Painter.setBrush(QBrush(Qt::lightGray));
    Painter.drawRoundedRect(QRect(1,1,this->width()-3,this->height()-3),5,5);
    Painter.drawText(QRect(3,2,this->width()-5,this->height()-5),Text,QTextOption(Qt::AlignVCenter));
    Painter.restore();
}

//******************************************************************************************************************
// Custom QAbstractItemDelegate for AutoFraming ComboBox
//******************************************************************************************************************
#define AutoFramingComboBoxIMAGEWIDTH   128
#define AutoFramingComboBoxNBRCOLUMN    3

cFramingComboBoxItem::cFramingComboBoxItem(QObject *parent):QStyledItemDelegate(parent) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cFramingComboBoxItem::cFramingComboBoxItem");
}

//========================================================================================================================
void cFramingComboBoxItem::paint(QPainter *painter,const QStyleOptionViewItem &option,const QModelIndex &index) const {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cFramingComboBoxItem::paint");
    int CurIndex=index.row()*AutoFramingComboBoxNBRCOLUMN+index.column();
    int CurrentFramingStyle=0;
    while ((CurrentFramingStyle<ComboBox->FramingStyleTable.count())&&(ComboBox->CurrentFramingStyle!=ComboBox->FramingStyleTable.at(CurrentFramingStyle).FrameStyle)) CurrentFramingStyle++;

    if ((CurIndex>=0)&&(CurIndex<ComboBox->FramingStyleTable.count())) {
        painter->drawPixmap(option.rect.left(),option.rect.top(),QPixmap().fromImage(((cFramingStyleTableItem)ComboBox->FramingStyleTable.at(CurIndex)).Image));
    } else {
        painter->fillRect(option.rect,Qt::white);
    }
    if (index.row()*((QTableWidget *)ComboBox->view())->columnCount()+index.column()==CurrentFramingStyle) {
        painter->setPen(QPen(Qt::red));
        painter->setBrush(QBrush(Qt::NoBrush));
        painter->drawRect(option.rect.x()+3,option.rect.y()+3,option.rect.width()-6-1,option.rect.height()-6-1);
    }
    if (option.state & QStyle::State_Selected) {
        painter->setPen(QPen(Qt::blue));
        painter->setBrush(QBrush(Qt::NoBrush));
        painter->drawRect(option.rect.x(),option.rect.y(),option.rect.width()-1,option.rect.height()-1);
        painter->drawRect(option.rect.x()+1,option.rect.y()+1,option.rect.width()-1-2,option.rect.height()-1-2);
        painter->setPen(QPen(Qt::black));
        painter->drawRect(option.rect.x()+2,option.rect.y()+2,option.rect.width()-1-4,option.rect.height()-1-4);
    }
}

//========================================================================================================================

QSize cFramingComboBoxItem::sizeHint(const QStyleOptionViewItem &/*option*/,const QModelIndex &/*index*/) const {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBackgroundComboBoxItem::sizeHint");
    return QSize(AutoFramingComboBoxIMAGEWIDTH,AutoFramingComboBoxIMAGEWIDTH);
}

//========================================================================================================================

cFramingStyleTableItem::cFramingStyleTableItem(QImage *Image,int FrameStyle) {
    this->Image=*Image;
    this->FrameStyle=FrameStyle;
}

//******************************************************************************************************************
// Custom Brush ComboBox
//******************************************************************************************************************

cFramingComboBox::cFramingComboBox(QWidget *parent):QComboBox(parent) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cFramingComboBox::cFramingComboBox");
    CurrentFilter       =-1;
    CurrentFramingStyle =-1;
    CurrentNbrITem      =-1;
    STOPMAJ             =false;

    QTableWidget    *Table=new QTableWidget();
    Table->horizontalHeader()->hide();
    Table->verticalHeader()->hide();
    for (int i=0;i<AutoFramingComboBoxNBRCOLUMN;i++) {
        Table->insertColumn(0);
        Table->setColumnWidth(0,AutoFramingComboBoxIMAGEWIDTH);
    }
    setModel(Table->model());
    setView(Table);
    ItemDelegate.ComboBox=this;
    setItemDelegate(&ItemDelegate);
    this->view()->setFixedWidth(AutoFramingComboBoxIMAGEWIDTH*AutoFramingComboBoxNBRCOLUMN+18);
    this->view()->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    connect(Table,SIGNAL(itemSelectionChanged()),this,SLOT(s_ItemSelectionChanged()));
}

//========================================================================================================================

void cFramingComboBox::hidePopup() {
    QComboBox::hidePopup();
    emit itemSelectionHaveChanged();
    MakeIcons();
    CurrentFramingStyle=GetCurrentFraming();
}

//========================================================================================================================

void cFramingComboBox::keyReleaseEvent(QKeyEvent *event) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cFramingComboBox::keyReleaseEvent");
    int CurIndex=((QTableWidget *)view())->currentRow()*AutoFramingComboBoxNBRCOLUMN+((QTableWidget *)view())->currentColumn();
    int aCurIndex=CurIndex;
    if ((event->key()==Qt::Key_Right)||(event->key()==Qt::Key_Down)) {
        if (CurIndex<FramingStyleTable.count()-1) CurIndex++;
    } else if ((event->key()==Qt::Key_Left)||(event->key()==Qt::Key_Up)) {
        if (CurIndex>0) CurIndex--;
    } else QComboBox::keyReleaseEvent(event);
    if (aCurIndex!=CurIndex) {
        STOPMAJ=true;
        ((QTableWidget *)view())->setCurrentCell(CurIndex/AutoFramingComboBoxNBRCOLUMN,CurIndex-(CurIndex/AutoFramingComboBoxNBRCOLUMN)*AutoFramingComboBoxNBRCOLUMN);
        setCurrentIndex(CurIndex/AutoFramingComboBoxNBRCOLUMN);
        MakeIcons();
        STOPMAJ=false;
        emit itemSelectionHaveChanged();
    }
}

//========================================================================================================================

void cFramingComboBox::PrepareFramingStyleTable(bool ResetContent,int Filter,cBrushDefinition *Brush,QImage *SourceImage,int BackgroundForm,qreal ProjectGeometry) {
    qreal   maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4;
    QImage  *Image=Brush->ImageToWorkspace(SourceImage,AutoFramingComboBoxIMAGEWIDTH,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4);

    int  AutoFraming  =Brush->GetCurrentFramingStyle(ProjectGeometry);
    bool AllowInternal=(int(Brush->ImageRotation/90)!=(Brush->ImageRotation/90));
    if (AllowInternal) Filter|=FILTERFRAMING_INTERNAL;
    qreal ImageGeometry=(qreal(SourceImage->height())/qreal(SourceImage->width()));
    if (ProjectGeometry==ImageGeometry) Filter=Filter & (~FILTERFRAMING_PROJECT);
    FramingStyleTable.clear();
    for (int i=0;i<NBR_AUTOFRAMING;i++) {
        bool ToAdd=(AutoFraming==i);
        if (!ToAdd) switch (i) {
            case AUTOFRAMING_CUSTOMUNLOCK   :   if ((Filter & FILTERFRAMING_CUSTOM)>0)                                                                      ToAdd=true;     break;
            case AUTOFRAMING_CUSTOMLOCK     :   if ((Filter & FILTERFRAMING_CUSTOM)>0)                                                                      ToAdd=true;     break;
            case AUTOFRAMING_CUSTOMIMGLOCK  :   if (((Filter & (FILTERFRAMING_IMAGE|FILTERFRAMING_CUSTOM))==(FILTERFRAMING_IMAGE|FILTERFRAMING_CUSTOM)))    ToAdd=true;     break;
            case AUTOFRAMING_CUSTOMPRJLOCK  :   if ((Filter & (FILTERFRAMING_PROJECT|FILTERFRAMING_CUSTOM))==(FILTERFRAMING_PROJECT|FILTERFRAMING_CUSTOM))  ToAdd=true;     break;
            case AUTOFRAMING_FULLMAX        :   if (((Filter & FILTERFRAMING_IMAGE)>0))                                                                     ToAdd=true;     break;
            case AUTOFRAMING_FULLMIN        :   if (((Filter & FILTERFRAMING_IMAGE)>0)&&(AllowInternal))                                                    ToAdd=true;     break;
            case AUTOFRAMING_HEIGHTLEFTMAX  :   if (((Filter & FILTERFRAMING_PROJECT)>0)&&(ProjectGeometry>ImageGeometry))                                  ToAdd=true;     break;
            case AUTOFRAMING_HEIGHTLEFTMIN  :   if (((Filter & FILTERFRAMING_PROJECT)>0)&&(ProjectGeometry>ImageGeometry)&&(AllowInternal))                 ToAdd=true;     break;
            case AUTOFRAMING_HEIGHTMIDLEMAX :   if (((Filter & FILTERFRAMING_PROJECT)>0))                                                                   ToAdd=true;     break;
            case AUTOFRAMING_HEIGHTMIDLEMIN :   if (((Filter & FILTERFRAMING_PROJECT)>0)&&(AllowInternal))                                                  ToAdd=true;     break;
            case AUTOFRAMING_HEIGHTRIGHTMAX :   if (((Filter & FILTERFRAMING_PROJECT)>0)&&(ProjectGeometry>ImageGeometry))                                  ToAdd=true;     break;
            case AUTOFRAMING_HEIGHTRIGHTMIN :   if (((Filter & FILTERFRAMING_PROJECT)>0)&&(ProjectGeometry>ImageGeometry)&&(AllowInternal))                 ToAdd=true;     break;
            case AUTOFRAMING_WIDTHTOPMAX    :   if (((Filter & FILTERFRAMING_PROJECT)>0)&&(ProjectGeometry<ImageGeometry))                                  ToAdd=true;     break;
            case AUTOFRAMING_WIDTHTOPMIN    :   if (((Filter & FILTERFRAMING_PROJECT)>0)&&(ProjectGeometry<ImageGeometry)&&(AllowInternal))                 ToAdd=true;     break;
            case AUTOFRAMING_WIDTHMIDLEMAX  :   if (((Filter & FILTERFRAMING_PROJECT)>0))                                                                   ToAdd=true;     break;
            case AUTOFRAMING_WIDTHMIDLEMIN  :   if (((Filter & FILTERFRAMING_PROJECT)>0)&&(AllowInternal))                                                  ToAdd=true;     break;
            case AUTOFRAMING_WIDTHBOTTOMMAX :   if (((Filter & FILTERFRAMING_PROJECT)>0)&&(ProjectGeometry<ImageGeometry))                                  ToAdd=true;     break;
            case AUTOFRAMING_WIDTHBOTTOMMIN :   if (((Filter & FILTERFRAMING_PROJECT)>0)&&(ProjectGeometry<ImageGeometry)&&(AllowInternal))                 ToAdd=true;     break;
        }
        if (ToAdd) {
            QImage CurImage=Image->copy();
            Brush->ApplyMaskToImageToWorkspace(&CurImage,i,BackgroundForm,AutoFramingComboBoxIMAGEWIDTH,maxw,maxh,minw,minh,X,Y,ZoomFactor,AspectRatio,ProjectGeometry);
            FramingStyleTable.append(cFramingStyleTableItem(&CurImage,i));
        }
    }

    if (ResetContent || (CurrentFilter!=Filter) || (CurrentNbrITem!=FramingStyleTable.count())) {
        CurrentFilter =Filter;
        CurrentNbrITem=FramingStyleTable.count();
        int CurIndex  =((QTableWidget *)view())->currentRow()*AutoFramingComboBoxNBRCOLUMN+((QTableWidget *)view())->currentColumn();
        while (count()>0) removeItem(count()-1);
        int NbrItem=FramingStyleTable.count();
        int NbrRow=NbrItem/AutoFramingComboBoxNBRCOLUMN;
        if (NbrRow*AutoFramingComboBoxNBRCOLUMN<NbrItem) NbrRow++;
        for (int i=0;i<NbrRow;i++) {
            addItem(QIcon(AUTOFRAMING_ICON_GEOMETRY_IMAGE),"");    //automaticaly do a Table->insertRow(Table->rowCount());
            ((QTableWidget *)view())->setRowHeight(((QTableWidget *)view())->rowCount()-1,AutoFramingComboBoxIMAGEWIDTH);
        }
        ((QTableWidget *)view())->setCurrentCell(CurIndex/AutoFramingComboBoxNBRCOLUMN,CurIndex-(CurIndex/AutoFramingComboBoxNBRCOLUMN)*AutoFramingComboBoxNBRCOLUMN);
        setCurrentIndex(CurIndex/AutoFramingComboBoxNBRCOLUMN);
    }
    delete Image;
}

//========================================================================================================================

void cFramingComboBox::SetCurrentFraming(int AutoFraming) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cFramingComboBox::SetCurrentFraming");
    if (STOPMAJ) return;
    CurrentFramingStyle=AutoFraming;
    int Index=0;
    while ((Index<FramingStyleTable.count())&&(AutoFraming!=FramingStyleTable.at(Index).FrameStyle)) Index++;
    if (Index<FramingStyleTable.count()) {
        STOPMAJ=true;
        ((QTableWidget *)view())->setCurrentCell(Index/AutoFramingComboBoxNBRCOLUMN,Index-(Index/AutoFramingComboBoxNBRCOLUMN)*AutoFramingComboBoxNBRCOLUMN);
        setCurrentIndex(Index/AutoFramingComboBoxNBRCOLUMN);
        MakeIcons();
        STOPMAJ=false;
    }
}

//========================================================================================================================

int cFramingComboBox::GetCurrentFraming() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cFramingComboBox::GetCurrentFraming");
    int CurIndex=((QTableWidget *)view())->currentRow()*AutoFramingComboBoxNBRCOLUMN+((QTableWidget *)view())->currentColumn();
    if ((CurIndex>=0)&&(CurIndex<FramingStyleTable.count())) return FramingStyleTable.at(CurIndex).FrameStyle;
    return -1;
}

//========================================================================================================================

void cFramingComboBox::MakeIcons() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cFramingComboBox::MakeIcons");
    int CurIndex=((QTableWidget *)view())->currentRow()*AutoFramingComboBoxNBRCOLUMN+((QTableWidget *)view())->currentColumn();
    if ((CurIndex>=0)&&(CurIndex<FramingStyleTable.count())) {
        QIcon IconGeoImage;
        switch (AUTOFRAMINGDEF[FramingStyleTable.at(CurIndex).FrameStyle].GeometryType) {
            case AUTOFRAMING_GEOMETRY_CUSTOM :  IconGeoImage=FramingStyleTable.at(CurIndex).FrameStyle==AUTOFRAMING_CUSTOMUNLOCK?QIcon(AUTOFRAMING_ICON_GEOMETRY_UNLOCKED):
                                                                                                        QIcon(AUTOFRAMING_ICON_GEOMETRY_LOCKED);       break;
            case AUTOFRAMING_GEOMETRY_PROJECT : IconGeoImage=QIcon(AUTOFRAMING_ICON_GEOMETRY_PROJECT);                                                 break;
            case AUTOFRAMING_GEOMETRY_IMAGE :   IconGeoImage=QIcon(AUTOFRAMING_ICON_GEOMETRY_IMAGE);                                                   break;
        }
        setItemIcon(((QTableWidget *)view())->currentRow(),IconGeoImage);
        setItemText(((QTableWidget *)view())->currentRow(),AUTOFRAMINGDEF[FramingStyleTable.at(CurIndex).FrameStyle].ToolTip);
    }
}

//========================================================================================================================

void cFramingComboBox::s_ItemSelectionChanged() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cFramingComboBox::s_ItemSelectionChanged");
    int CurIndex=((QTableWidget *)view())->currentRow()*AutoFramingComboBoxNBRCOLUMN+((QTableWidget *)view())->currentColumn();
    if ((CurIndex>=0)&&(CurIndex<FramingStyleTable.count())) {
        STOPMAJ=true;
        setCurrentIndex(((QTableWidget *)view())->currentRow());
        emit currentIndexChanged(((QTableWidget *)view())->currentRow());
        ((QTableWidget *)view())->setToolTip(AUTOFRAMINGDEF[FramingStyleTable.at(CurIndex).FrameStyle].ToolTip);
        STOPMAJ=false;
    }
}

//******************************************************************************************************************
// Custom QAbstractItemDelegate for AutoFraming ComboBox
//******************************************************************************************************************
#define AutoFrameShapeComboBoxIMAGEWIDTH   48
#define AutoFrameShapeComboBoxNBRCOLUMN    8

cFrameShapeBoxItem::cFrameShapeBoxItem(QObject *parent):QStyledItemDelegate(parent) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cFrameShapeBoxItem::cFrameShapeBoxItem");
}

//========================================================================================================================
void cFrameShapeBoxItem::paint(QPainter *painter,const QStyleOptionViewItem &option,const QModelIndex &index) const {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cFrameShapeBoxItem::paint");
    int CurIndex=index.row()*AutoFrameShapeComboBoxNBRCOLUMN+index.column();
    int CurrentFramingStyle=0;
    while ((CurrentFramingStyle<ComboBox->FrameShapeTable.count())&&(ComboBox->CurrentFramingStyle!=ComboBox->FrameShapeTable.at(CurrentFramingStyle).FrameStyle)) CurrentFramingStyle++;

    if ((CurIndex>=0)&&(CurIndex<ComboBox->FrameShapeTable.count())) {
        painter->drawPixmap(option.rect.left(),option.rect.top(),QPixmap().fromImage(((cFrameShapeTableItem)ComboBox->FrameShapeTable.at(CurIndex)).Image));
    } else {
        painter->fillRect(option.rect,Qt::white);
    }
    if (index.row()*((QTableWidget *)ComboBox->view())->columnCount()+index.column()==CurrentFramingStyle) {
        painter->setPen(QPen(Qt::red));
        painter->setBrush(QBrush(Qt::NoBrush));
        painter->drawRect(option.rect.x()+3,option.rect.y()+3,option.rect.width()-6-1,option.rect.height()-6-1);
    }
    if (option.state & QStyle::State_Selected) {
        painter->setPen(QPen(Qt::blue));
        painter->setBrush(QBrush(Qt::NoBrush));
        painter->drawRect(option.rect.x(),option.rect.y(),option.rect.width()-1,option.rect.height()-1);
        painter->drawRect(option.rect.x()+1,option.rect.y()+1,option.rect.width()-1-2,option.rect.height()-1-2);
        painter->setPen(QPen(Qt::black));
        painter->drawRect(option.rect.x()+2,option.rect.y()+2,option.rect.width()-1-4,option.rect.height()-1-4);
    }
}

//========================================================================================================================

QSize cFrameShapeBoxItem::sizeHint(const QStyleOptionViewItem &/*option*/,const QModelIndex &/*index*/) const {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cFrameShapeBoxItem::sizeHint");
    return QSize(AutoFrameShapeComboBoxIMAGEWIDTH,AutoFrameShapeComboBoxIMAGEWIDTH);
}

//========================================================================================================================

cFrameShapeTableItem::cFrameShapeTableItem(QImage *Image,int FrameStyle) {
    this->Image     =*Image;
    this->FrameStyle=FrameStyle;
}

//******************************************************************************************************************
// Custom Brush ComboBox
//******************************************************************************************************************

ccFrameShapeBox::ccFrameShapeBox(QWidget *parent):QComboBox(parent) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:ccFrameShapeBox::ccFrameShapeBox");
    CurrentFilter       =-1;
    CurrentFramingStyle =-1;
    CurrentNbrITem      =-1;
    STOPMAJ             =false;

    QTableWidget    *Table=new QTableWidget();
    Table->horizontalHeader()->hide();
    Table->verticalHeader()->hide();
    for (int i=0;i<AutoFrameShapeComboBoxNBRCOLUMN;i++) {
        Table->insertColumn(0);
        Table->setColumnWidth(0,AutoFrameShapeComboBoxIMAGEWIDTH);
    }
    setModel(Table->model());
    setView(Table);
    ItemDelegate.ComboBox=this;
    setItemDelegate(&ItemDelegate);
    this->view()->setFixedWidth(AutoFrameShapeComboBoxIMAGEWIDTH*AutoFrameShapeComboBoxNBRCOLUMN+18);
    this->view()->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    connect(Table,SIGNAL(itemSelectionChanged()),this,SLOT(s_ItemSelectionChanged()));
}

//========================================================================================================================

void ccFrameShapeBox::hidePopup() {
    QComboBox::hidePopup();
    emit itemSelectionHaveChanged();
    MakeIcons();
    CurrentFramingStyle=GetCurrentFrameShape();
}

//========================================================================================================================

void ccFrameShapeBox::keyReleaseEvent(QKeyEvent *event) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cFramingComboBox::keyReleaseEvent");
    int CurIndex=((QTableWidget *)view())->currentRow()*AutoFrameShapeComboBoxNBRCOLUMN+((QTableWidget *)view())->currentColumn();
    int aCurIndex=CurIndex;
    if ((event->key()==Qt::Key_Right)||(event->key()==Qt::Key_Down)) {
        if (CurIndex<FrameShapeTable.count()-1) CurIndex++;
    } else if ((event->key()==Qt::Key_Left)||(event->key()==Qt::Key_Up)) {
        if (CurIndex>0) CurIndex--;
    } else QComboBox::keyReleaseEvent(event);
    if (aCurIndex!=CurIndex) {
        STOPMAJ=true;
        ((QTableWidget *)view())->setCurrentCell(CurIndex/AutoFrameShapeComboBoxNBRCOLUMN,CurIndex-(CurIndex/AutoFrameShapeComboBoxNBRCOLUMN)*AutoFrameShapeComboBoxNBRCOLUMN);
        setCurrentIndex(CurIndex/AutoFrameShapeComboBoxNBRCOLUMN);
        MakeIcons();
        STOPMAJ=false;
        emit itemSelectionHaveChanged();
    }
}

//========================================================================================================================

void ccFrameShapeBox::PrepareFrameShapeTable(bool ResetContent,int Filter,int CurrentBackgroundForm,cApplicationConfig *ApplicationConfig) {
    this->ApplicationConfig=ApplicationConfig;
    if ((CurrentBackgroundForm>=SHAPEFORM_TRIANGLEUP)&&(CurrentBackgroundForm<=SHAPEFORM_TRIANGLELEFT)) Filter=Filter|FILTERFRAMESHAPE_OLDTRIANGLE;
    FrameShapeTable.clear();
    for (int i=0;i<NBR_SHAPEFORM;i++) {
        bool ToAdd=ShapeFormDefinition.at(i).Enable;
        if ((i>=SHAPEFORM_TRIANGLEUP)&&(i<=SHAPEFORM_TRIANGLELEFT)) ToAdd=((Filter & FILTERFRAMESHAPE_OLDTRIANGLE)>0);
        if (ToAdd) {
            cCompositionObject Object(COMPOSITIONTYPE_BACKGROUND,0,ApplicationConfig);
            Object.x                        =0.1;
            Object.y                        =0.1;
            Object.w                        =0.8;
            Object.h                        =0.8;
            Object.BackgroundForm           =i;
            Object.Opacity                  =4;
            Object.PenSize                  =1;
            Object.PenStyle                 =Qt::SolidLine;
            Object.PenColor                 ="#000000";
            Object.BackgroundBrush->ColorD  ="#FFFFFF";
            Object.BackgroundBrush->BrushType=BRUSHTYPE_SOLID;
            QImage   Image(QSize(AutoFrameShapeComboBoxIMAGEWIDTH,AutoFrameShapeComboBoxIMAGEWIDTH),QImage::Format_ARGB32);
            QPainter Painter;
            Painter.begin(&Image);
            Painter.fillRect(QRect(0,0,AutoFrameShapeComboBoxIMAGEWIDTH,AutoFrameShapeComboBoxIMAGEWIDTH),"#ffffff");
            Object.DrawCompositionObject(&Painter,1,0,0,AutoFrameShapeComboBoxIMAGEWIDTH,AutoFrameShapeComboBoxIMAGEWIDTH,true,0,0,NULL,1,NULL,false,0,false);
            Painter.end();
            FrameShapeTable.append(cFrameShapeTableItem(&Image,i));
        }
    }

    if (ResetContent || (CurrentFilter!=Filter) || (CurrentNbrITem!=FrameShapeTable.count())) {
        CurrentFilter =Filter;
        CurrentNbrITem=FrameShapeTable.count();
        int CurIndex  =((QTableWidget *)view())->currentRow()*AutoFrameShapeComboBoxNBRCOLUMN+((QTableWidget *)view())->currentColumn();
        while (count()>0) removeItem(count()-1);
        int NbrItem=FrameShapeTable.count();
        int NbrRow=NbrItem/AutoFrameShapeComboBoxNBRCOLUMN;
        if (NbrRow*AutoFrameShapeComboBoxNBRCOLUMN<NbrItem) NbrRow++;
        for (int i=0;i<NbrRow;i++) {
            addItem(QIcon(QPixmap().fromImage(FrameShapeTable.at(i).Image.scaled(this->iconSize().width(),this->iconSize().height()))),ShapeFormDefinition.at(FrameShapeTable.at(i).FrameStyle).Name);
            ((QTableWidget *)view())->setRowHeight(((QTableWidget *)view())->rowCount()-1,AutoFrameShapeComboBoxIMAGEWIDTH);
        }
        ((QTableWidget *)view())->setCurrentCell(CurIndex/AutoFrameShapeComboBoxNBRCOLUMN,CurIndex-(CurIndex/AutoFrameShapeComboBoxNBRCOLUMN)*AutoFrameShapeComboBoxNBRCOLUMN);
        setCurrentIndex(CurIndex/AutoFrameShapeComboBoxNBRCOLUMN);
    }
}

//========================================================================================================================

void ccFrameShapeBox::SetCurrentFrameShape(int FrameShape) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:ccFrameShapeBox::SetCurrentFraming");
    if (STOPMAJ) return;
    CurrentFramingStyle=FrameShape;
    int Index=0;
    while ((Index<FrameShapeTable.count())&&(FrameShape!=FrameShapeTable.at(Index).FrameStyle)) Index++;
    if (Index<FrameShapeTable.count()) {
        STOPMAJ=true;
        ((QTableWidget *)view())->setCurrentCell(Index/AutoFrameShapeComboBoxNBRCOLUMN,Index-(Index/AutoFrameShapeComboBoxNBRCOLUMN)*AutoFrameShapeComboBoxNBRCOLUMN);
        setCurrentIndex(Index/AutoFrameShapeComboBoxNBRCOLUMN);
        MakeIcons();
        STOPMAJ=false;
    }
}

//========================================================================================================================

int ccFrameShapeBox::GetCurrentFrameShape() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:ccFrameShapeBox::GetCurrentFrameShape");
    int CurIndex=((QTableWidget *)view())->currentRow()*AutoFrameShapeComboBoxNBRCOLUMN+((QTableWidget *)view())->currentColumn();
    if ((CurIndex>=0)&&(CurIndex<FrameShapeTable.count())) return FrameShapeTable.at(CurIndex).FrameStyle;
    return -1;
}

//========================================================================================================================

void ccFrameShapeBox::MakeIcons() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:ccFrameShapeBox::MakeIcons");
    int CurIndex=((QTableWidget *)view())->currentRow()*AutoFrameShapeComboBoxNBRCOLUMN+((QTableWidget *)view())->currentColumn();
    if ((CurIndex>=0)&&(CurIndex<FrameShapeTable.count())) {
        cCompositionObject Object(COMPOSITIONTYPE_BACKGROUND,0,ApplicationConfig);
        Object.x                        =0.05;
        Object.y                        =0.05;
        Object.w                        =0.9;
        Object.h                        =0.9;
        Object.BackgroundForm           =FrameShapeTable.at(CurIndex).FrameStyle;
        Object.Opacity                  =4;
        Object.PenSize                  =1;
        Object.PenStyle                 =Qt::SolidLine;
        Object.PenColor                 ="#000000";
        Object.BackgroundBrush->ColorD  ="#FFFFFF";
        Object.BackgroundBrush->BrushType=BRUSHTYPE_SOLID;
        QPixmap  Image(this->iconSize());
        QPainter Painter;
        Painter.begin(&Image);
        Painter.fillRect(QRect(0,0,this->iconSize().width(),this->iconSize().height()),"#ffffff");
        Object.DrawCompositionObject(&Painter,1,0,0,this->iconSize().width(),this->iconSize().height(),true,0,0,NULL,1,NULL,false,0,false);
        Painter.end();
        setItemIcon(((QTableWidget *)view())->currentRow(),QIcon(Image));
        setItemText(((QTableWidget *)view())->currentRow(),ShapeFormDefinition.at(FrameShapeTable.at(CurIndex).FrameStyle).Name);
    }
}

//========================================================================================================================

void ccFrameShapeBox::s_ItemSelectionChanged() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:ccFrameShapeBox::s_ItemSelectionChanged");
    if (STOPMAJ) return;
    int CurIndex=((QTableWidget *)view())->currentRow()*AutoFrameShapeComboBoxNBRCOLUMN+((QTableWidget *)view())->currentColumn();
    if ((CurIndex>=0)&&(CurIndex<FrameShapeTable.count())) {
        STOPMAJ=true;
        setCurrentIndex(((QTableWidget *)view())->currentRow());
        emit currentIndexChanged(((QTableWidget *)view())->currentRow());
        ((QTableWidget *)view())->setToolTip(ShapeFormDefinition.at(FrameShapeTable.at(CurIndex).FrameStyle).Name);
        STOPMAJ=false;
    }
}
