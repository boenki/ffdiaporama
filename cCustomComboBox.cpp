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

#include "cCustomComboBox.h"

//======================================
// Specific defines for this dialog box
//======================================
#define MAXCOLORREF             99
#define MAXBRUSHPATTERN         12
#define MAXGRADIENTORIENTATION  9
#define MAXONOFFFILTER          8
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
}

//========================================================================================================================

void cCustomColorComboBoxItem::paint(QPainter *painter,const QStyleOptionViewItem &option,const QModelIndex &index) const {
    int ColorNum=index.row()*5+index.column();
    if (ColorNum<MAXCOLORREF) {
        painter->setPen(Qt::NoPen);
        painter->setBrush(QBrush(QColor(ColorRef[ColorNum])));
        painter->drawRect(option.rect);
    } else {
        if (ColorNum==MAXCOLORREF) {
            painter->fillRect(option.rect,QColor(ComboBox->SavedCustomColor));
            QImage  Img("icons/colorize.png");
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
    return QSize(24,24);
}

//******************************************************************************************************************
// Custom Color ComboBox
//******************************************************************************************************************

cCustomColorComboBox::cCustomColorComboBox(QWidget *parent):QComboBox(parent) {
    STOPMAJ=false;
    CurrentColor=NULL;
    setIconSize(QSize(24*5-30,16));
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
    connect(Table,SIGNAL(itemSelectionChanged()),this,SLOT(s_ItemSelectionChanged()));
    connect(Table,SIGNAL(cellPressed(int,int)),this,SLOT(s_ItemPressed(int,int)));
}

//========================================================================================================================

void cCustomColorComboBox::SetCurrentColor(QString *Color) {
    if (STOPMAJ) return;
    CurrentColor=Color;
    int i=0;
    while ((i<MAXCOLORREF)&&(ColorRef[i]!=*CurrentColor)) i++;
    int Row=i/5;
    int Col=i-(i/5)*5;
    ((QTableWidget *)view())->setCurrentCell(Row,Col);
    setCurrentIndex(Row);
    StandardColor=((i>=0)&&(i<MAXCOLORREF));
    if (!StandardColor) SavedCustomColor=*CurrentColor; else SavedCustomColor="#fefefe";
    MakeIcons();
}

//========================================================================================================================

QString cCustomColorComboBox::GetCurrentColor() {
    if (!CurrentColor) return SavedCustomColor;
    int i=((QTableWidget *)view())->currentRow()*5+((QTableWidget *)view())->currentColumn();
    StandardColor=((i>=0)&&(i<MAXCOLORREF));
    if ((i>=0)&&(i<MAXCOLORREF)) *CurrentColor=ColorRef[i];
    return *CurrentColor;
}

//========================================================================================================================

void cCustomColorComboBox::MakeIcons() {
    int CurrentRow=((QTableWidget *)view())->currentRow();      if (CurrentRow<0) CurrentRow=0;
    int CurrentCol=((QTableWidget *)view())->currentColumn();   if (CurrentCol<0) CurrentCol=0;
    int ColorNum=CurrentRow*5+CurrentCol;
    QPixmap  Image(iconSize());
    QPainter Painter;
    Painter.begin(&Image);
    if (ColorNum<MAXCOLORREF) Painter.fillRect(QRectF(0,0,iconSize().width(),iconSize().height()),QColor(ColorRef[ColorNum])); else {
        Painter.fillRect(QRectF(0,0,iconSize().width(),iconSize().height()),QColor(SavedCustomColor));
        QImage  Img("icons/colorize.png");
        Painter.drawImage(QRectF((iconSize().width()-16)/2,(iconSize().height()-16)/2,16,16),Img,QRectF(0,0,Img.width(),Img.height()));
    }
    Painter.end();
    setItemIcon(currentIndex(),QIcon(Image));
}

//========================================================================================================================

void cCustomColorComboBox::s_ItemSelectionChanged() {
    STOPMAJ=true;
    setCurrentIndex(((QTableWidget *)view())->currentRow());
    MakeIcons();
    emit currentIndexChanged(((QTableWidget *)view())->currentRow()*5+((QTableWidget *)view())->currentColumn());
    STOPMAJ=false;
}

//========================================================================================================================

void cCustomColorComboBox::s_ItemPressed(int,int) {
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
}

//========================================================================================================================
void cCustomBrushComboBoxItem::paint(QPainter *painter,const QStyleOptionViewItem &option,const QModelIndex &index) const {
    int ColorNum=index.row()*4+index.column();
    if (ColorNum<MAXBRUSHPATTERN) {
        cBrushDefinition Brush;
        Brush.ColorD     =ComboBox->Brush->ColorD;
        Brush.BrushType  =BRUSHTYPE_PATTERN;
        Brush.PatternType=ColorNum;
        painter->fillRect(option.rect,Brush.ColorD.toLower()!="#ffffff"?Qt::white:Qt::black);
        QBrush *BR=Brush.GetBrush(option.rect);
        painter->setBrush(*BR);
        delete BR;
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
    return QSize(24,24);
}

//******************************************************************************************************************
// Custom Brush ComboBox
//******************************************************************************************************************

cCustomBrushComboBox::cCustomBrushComboBox(QWidget *parent):QComboBox(parent) {
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
    connect(Table,SIGNAL(itemSelectionChanged()),this,SLOT(s_ItemSelectionChanged()));
}

//========================================================================================================================

void cCustomBrushComboBox::SetCurrentBrush(cBrushDefinition *TheBrush) {
    if (STOPMAJ) return;
    Brush=TheBrush;
    ((QTableWidget *)view())->setCurrentCell(Brush->PatternType/4,Brush->PatternType-(Brush->PatternType/4)*4);
    setCurrentIndex(Brush->PatternType/4);
    MakeIcons();
}

//========================================================================================================================

cBrushDefinition *cCustomBrushComboBox::GetCurrentBrush() {
    Brush->BrushType  =BRUSHTYPE_PATTERN;
    Brush->PatternType=currentIndex()*4+((QTableWidget *)view())->currentColumn();
    MakeIcons();
    return Brush;
}

//========================================================================================================================

void cCustomBrushComboBox::MakeIcons() {
    if (!Brush) return;
    int CurrentRow=currentIndex();
    if (CurrentRow<0) return;
    int CurrentCol=((QTableWidget *)view())->currentColumn();
    if (CurrentCol<0) CurrentCol=0;
    int ColorNum=CurrentRow*4+CurrentCol;
    QPixmap  Image(64,16);
    QPainter Painter;
    Painter.begin(&Image);
    if (ColorNum<MAXBRUSHPATTERN) {
        cBrushDefinition TheBrush;
        TheBrush.ColorD=Brush->ColorD;
        TheBrush.BrushType  =BRUSHTYPE_PATTERN;
        TheBrush.PatternType=ColorNum;
        Painter.fillRect(QRectF(0,0,64,16),TheBrush.ColorD.toLower()!="#ffffff"?Qt::white:Qt::black);
        QBrush *BR=TheBrush.GetBrush(QRectF(0,0,64,16));
        Painter.setBrush(*BR);
        delete BR;
        Painter.drawRect(QRectF(0,0,64,16));
    } else {
        Painter.setBrush(QBrush(Qt::white));
        Painter.drawRect(QRectF(0,0,64,16));
    }
    Painter.end();
    setItemIcon(CurrentRow,QIcon(Image));
}

//========================================================================================================================

void cCustomBrushComboBox::s_ItemSelectionChanged() {
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
}

//========================================================================================================================
void cGradientOrientationComboBoxItem::paint(QPainter *painter,const QStyleOptionViewItem &option,const QModelIndex &index) const {
    int ColorNum=index.row()*3+index.column();
    if ((ColorNum>=0)&&(ColorNum<MAXGRADIENTORIENTATION)) {
        cBrushDefinition Brush;
        Brush.BrushType=ComboBox->Brush->BrushType;
        Brush.ColorD=ComboBox->Brush->ColorD;
        Brush.ColorF=ComboBox->Brush->ColorF;
        Brush.ColorIntermed=ComboBox->Brush->ColorIntermed;
        Brush.Intermediate=ComboBox->Brush->Intermediate;
        Brush.GradientOrientation=ColorNum;
        QBrush *BR=Brush.GetBrush(option.rect);
        painter->setBrush(*BR);
        delete BR;
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
    return QSize(32,32);
}

//******************************************************************************************************************
// Custom Gradient Orientation ComboBox
//******************************************************************************************************************

cGradientOrientationComboBox::cGradientOrientationComboBox(QWidget *parent):QComboBox(parent) {
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
    connect(Table,SIGNAL(itemSelectionChanged()),this,SLOT(s_ItemSelectionChanged()));
}

//========================================================================================================================

void cGradientOrientationComboBox::SetCurrentBrush(cBrushDefinition *TheBrush) {
    if (STOPMAJ) return;
    Brush=TheBrush;
    setCurrentIndex(Brush->GradientOrientation/3);
    ((QTableWidget *)view())->setCurrentCell(Brush->GradientOrientation/3,Brush->GradientOrientation-(Brush->GradientOrientation/3)*3);
    MakeIcons();
}

//========================================================================================================================

cBrushDefinition *cGradientOrientationComboBox::GetCurrentBrush() {
    if (Brush) {
        Brush->GradientOrientation=currentIndex()*3+((QTableWidget *)view())->currentColumn();
        MakeIcons();
    }
    return Brush;
}

//========================================================================================================================

void cGradientOrientationComboBox::MakeIcons() {
    if (!Brush) return;
    int CurrentRow=currentIndex();
    if (CurrentRow<0) return;
    int CurrentCol=((QTableWidget *)view())->currentColumn();
    if (CurrentCol<0) CurrentCol=0;
    int ColorNum=CurrentRow*3+CurrentCol;
    QPixmap  Image(64,16);
    QPainter Painter;
    Painter.begin(&Image);
    if ((ColorNum>=0)&&(ColorNum<MAXGRADIENTORIENTATION)) {
        cBrushDefinition TheBrush;
        TheBrush.BrushType=Brush->BrushType;
        TheBrush.ColorD=Brush->ColorD;
        TheBrush.ColorF=Brush->ColorF;
        TheBrush.ColorIntermed=Brush->ColorIntermed;
        TheBrush.Intermediate=Brush->Intermediate;
        TheBrush.GradientOrientation=ColorNum;
        QBrush *BR=TheBrush.GetBrush(QRectF(0,0,64,16));
        Painter.setBrush(*BR);
        delete BR;
        Painter.drawRect(QRectF(0,0,64,16));
    } else {
        Painter.setBrush(QBrush(Qt::white));
        Painter.drawRect(QRectF(0,0,64,16));
    }
    Painter.end();
    setItemIcon(CurrentRow,QIcon(Image));
}

//========================================================================================================================

void cGradientOrientationComboBox::s_ItemSelectionChanged() {
    STOPMAJ=true;
    setCurrentIndex(((QTableWidget *)view())->currentRow());
    MakeIcons();
    emit currentIndexChanged(((QTableWidget *)view())->currentRow()*3+((QTableWidget *)view())->currentColumn());
    STOPMAJ=false;
}

//******************************************************************************************************************
// Custom QAbstractItemDelegate for OnOffFilter ComboBox
//******************************************************************************************************************

cOnOffFilterComboBoxItem::cOnOffFilterComboBoxItem(QObject *parent):QStyledItemDelegate(parent) {
}

//========================================================================================================================

void cOnOffFilterComboBoxItem::paint(QPainter *painter,const QStyleOptionViewItem &option,const QModelIndex &index) const {
    int ColorNum=index.row()*4+index.column();
    // Prepare OnOff filter pixmaps
    QImage              Image=ComboBox->SourceImage.copy();
    fmt_filters::image  Img(Image.bits(),Image.width(),Image.height());
    if ((ColorNum & FilterDespeckle)==FilterDespeckle)  fmt_filters::despeckle(Img);
    if ((ColorNum & FilterEqualize)==FilterEqualize)    fmt_filters::equalize(Img);
    if ((ColorNum & FilterGray)==FilterGray)            fmt_filters::gray(Img);
    painter->drawImage(option.rect,Image);
    QString Text="";
    if ((ColorNum & FilterGray)==FilterGray)            Text=Text+(Text!=""?"+":"")+QCoreApplication::translate("wgt_QImageFilterTransform","Gray");
    if ((ColorNum & FilterEqualize)==FilterEqualize)    Text=Text+(Text!=""?"+":"")+QCoreApplication::translate("wgt_QImageFilterTransform","Equalize");
    if ((ColorNum & FilterDespeckle)==FilterDespeckle)  Text=Text+(Text!=""?"+":"")+QCoreApplication::translate("wgt_QImageFilterTransform","Despeckle");
    if (Text=="") Text=QCoreApplication::translate("wgt_QImageFilterTransform","No transformation");
    painter->drawText(option.rect,Text);
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

QSize cOnOffFilterComboBoxItem::sizeHint(const QStyleOptionViewItem &/*option*/,const QModelIndex &/*index*/) const {
    return QSize(ComboBox->SourceImage.width(),ComboBox->SourceImage.height());
}

//******************************************************************************************************************
// Custom Color ComboBox
//******************************************************************************************************************

cOnOffFilterComboBox::cOnOffFilterComboBox(QWidget *parent):QComboBox(parent) {
    CurrentFilter=NULL;
    QTableWidget    *Table=new QTableWidget();
    Table->horizontalHeader()->hide();
    Table->verticalHeader()->hide();
    Table->insertColumn(Table->columnCount());  Table->setColumnWidth(Table->columnCount()-1,24);
    Table->insertColumn(Table->columnCount());  Table->setColumnWidth(Table->columnCount()-1,24);
    Table->insertColumn(Table->columnCount());  Table->setColumnWidth(Table->columnCount()-1,24);
    Table->insertColumn(Table->columnCount());  Table->setColumnWidth(Table->columnCount()-1,24);
    setModel(Table->model());
    setView(Table);
    QString Text="";
    int i=0;
    while (i<MAXONOFFFILTER) {
        if ((i/4)>=Table->rowCount()) {
            addItem("");        // automaticaly do a Table->insertRow(Table->rowCount());
            Table->setRowHeight(Table->rowCount()-1,24);
        }
        Text="";
        if ((i & FilterGray)==FilterGray)            Text=Text+(Text!=""?"+":"")+QCoreApplication::translate("wgt_QImageFilterTransform","Gray");
        if ((i & FilterEqualize)==FilterEqualize)    Text=Text+(Text!=""?"+":"")+QCoreApplication::translate("wgt_QImageFilterTransform","Equalize");
        if ((i & FilterDespeckle)==FilterDespeckle)  Text=Text+(Text!=""?"+":"")+QCoreApplication::translate("wgt_QImageFilterTransform","Despeckle");
        if (Text=="") Text=QCoreApplication::translate("wgt_QImageFilterTransform","No transformation");
        Table->setItem(i/4,i-(i/4)*4,new QTableWidgetItem(Text));
        i++;
    }
    ItemDelegate.ComboBox=this;
    setItemDelegate(&ItemDelegate);
    connect(Table,SIGNAL(itemSelectionChanged()),this,SLOT(s_ItemSelectionChanged()));
}

//========================================================================================================================

void cOnOffFilterComboBox::s_ItemSelectionChanged() {
    if (CurrentFilter) *CurrentFilter=((QTableWidget *)view())->currentRow()*4+((QTableWidget *)view())->currentColumn();
    QString Text="";
    if ((*CurrentFilter & FilterGray)==FilterGray)            Text=Text+(Text!=""?"+":"")+QCoreApplication::translate("wgt_QImageFilterTransform","Gray");
    if ((*CurrentFilter & FilterEqualize)==FilterEqualize)    Text=Text+(Text!=""?"+":"")+QCoreApplication::translate("wgt_QImageFilterTransform","Equalize");
    if ((*CurrentFilter & FilterDespeckle)==FilterDespeckle)  Text=Text+(Text!=""?"+":"")+QCoreApplication::translate("wgt_QImageFilterTransform","Despeckle");
    if (Text=="") Text=QCoreApplication::translate("wgt_QImageFilterTransform","No transformation");
    setItemText(*CurrentFilter/4,Text);
}

//========================================================================================================================

void cOnOffFilterComboBox::SetCurrentFilter(QImage *TheSourceImage,int *OnOffFilter) {
    if (TheSourceImage==NULL) return;
    CurrentFilter=OnOffFilter;
    SourceImage=TheSourceImage->scaledToWidth(this->width()/4);
    ((QTableWidget *)view())->setCurrentCell(*CurrentFilter/4,*CurrentFilter-(*CurrentFilter/4)*4);
    for (int i=0;i<4;i++)                  ((QTableWidget *)view())->setColumnWidth(i,SourceImage.width());
    for (int i=0;i<(MAXONOFFFILTER/4);i++) ((QTableWidget *)view())->setRowHeight(i,SourceImage.height());
    QString Text="";
    if ((*CurrentFilter & FilterGray)==FilterGray)            Text=Text+(Text!=""?"+":"")+QCoreApplication::translate("wgt_QImageFilterTransform","Gray");
    if ((*CurrentFilter & FilterEqualize)==FilterEqualize)    Text=Text+(Text!=""?"+":"")+QCoreApplication::translate("wgt_QImageFilterTransform","Equalize");
    if ((*CurrentFilter & FilterDespeckle)==FilterDespeckle)  Text=Text+(Text!=""?"+":"")+QCoreApplication::translate("wgt_QImageFilterTransform","Despeckle");
    if (Text=="") Text=QCoreApplication::translate("wgt_QImageFilterTransform","No transformation");
    setItemText(*OnOffFilter/4,Text);
    setCurrentIndex(*OnOffFilter/4);
}

//========================================================================================================================

int cOnOffFilterComboBox::GetCurrentFilter() {
    int CurrentRow=currentIndex();
    if (CurrentRow<0) return 0;
    int CurrentCol=((QTableWidget *)view())->currentColumn();
    if (CurrentCol<0) CurrentCol=0;
    *CurrentFilter=CurrentRow*4+CurrentCol;
    return *CurrentFilter;
}

//******************************************************************************************************************
// Custom Brush ComboBox ITEM
//******************************************************************************************************************

cBackgroundComboBoxItem::cBackgroundComboBoxItem(QObject *parent):QStyledItemDelegate(parent) {
}

//========================================================================================================================
void cBackgroundComboBoxItem::paint(QPainter *painter,const QStyleOptionViewItem &option,const QModelIndex &index) const {
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
    if (BackgroundList.List.count()>0) return QSize(BackgroundList.List[0].Icon.width(),64);
        else return QSize(114,64);
}

//******************************************************************************************************************
// Custom Brush ComboBox
//******************************************************************************************************************

cBackgroundComboBox::cBackgroundComboBox(QWidget *parent):QComboBox(parent) {
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
    connect(Table,SIGNAL(itemSelectionChanged()),this,SLOT(s_ItemSelectionChanged()));
}

//========================================================================================================================

void cBackgroundComboBox::SetCurrentBackground(QString BrushImage) {
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
    int i=currentIndex();
    MakeIcons();
    return BackgroundList.List[i].Name;
}

//========================================================================================================================

void cBackgroundComboBox::MakeIcons() {
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
    STOPMAJ=true;
    setCurrentIndex(((QTableWidget *)view())->currentRow());
    MakeIcons();
    emit currentIndexChanged(((QTableWidget *)view())->currentRow());
    STOPMAJ=false;
}
