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

#include "wgt_QCustomBrush.h"
#include "wgt_QCompositionWidget.h"
#include "ui_wgt_QCompositionWidget.h"

wgt_QCompositionWidget::wgt_QCompositionWidget(QWidget *parent):QWidget(parent),ui(new Ui::wgt_QCompositionWidget) {
    ui->setupUi(this);
    IsInit          = false;
    scene           = NULL;
    NextZValue      = 500;
    BackgroundImage = NULL;
    CompositionList = NULL;
    StopMAJSpinbox  = false;
    BLOCKCHSIZE     = false;

    ui->CustomBrushWidget->InitWidget(true,true);
    ui->tabWidget->setCurrentIndex(0);

    // Init check box
    ui->textLeft->setCheckable(true);
    ui->textCenter->setCheckable(true);
    ui->textJustif->setCheckable(true);
    ui->textRight->setCheckable(true);
    ui->textUp->setCheckable(true);
    ui->textVCenter->setCheckable(true);
    ui->textBottom->setCheckable(true);

    // Init font size
    QList<int>  sizes=QFontDatabase::standardSizes();
    QStringList Ssizes;
    for (int i=0;i<sizes.count();i++) Ssizes.append(QString("%1").arg(sizes[i]));
    ui->fontSize->insertItems(0,Ssizes);
    ui->fontSize->setCurrentIndex(6);

    // Init editor
    ui->plainTextEdit->setWordWrapMode(QTextOption::NoWrap);

    // Init combo box FontEffect
    ui->fontEffectCB->addItem(QCoreApplication::translate("wgt_QCompositionWidget","No effect"));
    ui->fontEffectCB->addItem(QCoreApplication::translate("wgt_QCompositionWidget","Outerline"));
    ui->fontEffectCB->addItem(QCoreApplication::translate("wgt_QCompositionWidget","Shadow upper left"));
    ui->fontEffectCB->addItem(QCoreApplication::translate("wgt_QCompositionWidget","Shadow upper right"));
    ui->fontEffectCB->addItem(QCoreApplication::translate("wgt_QCompositionWidget","Shadow bottom left"));
    ui->fontEffectCB->addItem(QCoreApplication::translate("wgt_QCompositionWidget","Shadow bottom right"));
    MakeTextStyleIcon(ui->fontEffectCB);

    // Init combo box Background form
    for (int i=0;i<13;i++) ui->BackgroundFormCB->addItem("");
    MakeFormIcon(ui->BackgroundFormCB);

    // Init combo box Background opacity
    ui->BackgroundTransparentCB->addItem("100%");
    ui->BackgroundTransparentCB->addItem(" 75%");
    ui->BackgroundTransparentCB->addItem(" 50%");
    ui->BackgroundTransparentCB->addItem(" 25%");
    ui->BackgroundTransparentCB->addItem("  0%");

    // Init Spinbox
    ui->PosXEd->setDecimals(2);
    ui->PosYEd->setDecimals(2);
    ui->WidthEd->setDecimals(2);
    ui->HeightEd->setDecimals(2);

    // Set handler
    connect(ui->CustomBrushWidget,SIGNAL(NeedRefreshControls()),this,SLOT(RefreshControls()));
    connect(ui->addText,SIGNAL(pressed()),this,SLOT(s_AddNewTextToImage()));
    connect(ui->RemoveText,SIGNAL(pressed()),this,SLOT(s_RemoveTextFromImage()));
    connect(ui->fontStyleCB,SIGNAL(currentFontChanged(QFont)),this,SLOT(s_ChangeFont(QFont)));
    connect(ui->fontSize,SIGNAL(currentIndexChanged(QString)),this,SLOT(s_ChangeSizeFont(QString)));
    connect(ui->bold,SIGNAL(released()),this,SLOT(s_SetBold()));
    connect(ui->Italic,SIGNAL(released()),this,SLOT(s_SetItalic()));
    connect(ui->Souligne,SIGNAL(released()),this,SLOT(s_SetUnderline()));
    connect(ui->textLeft,SIGNAL(pressed()),this,SLOT(s_SetTextLeft()));
    connect(ui->textCenter,SIGNAL(pressed()),this,SLOT(s_SetTextCenter()));
    connect(ui->textRight,SIGNAL(pressed()),this,SLOT(s_SetTextRight()));
    connect(ui->textJustif,SIGNAL(pressed()),this,SLOT(s_SetTextJustif()));
    connect(ui->textUp,SIGNAL(pressed()),this,SLOT(s_SetTextUp()));
    connect(ui->textVCenter,SIGNAL(pressed()),this,SLOT(s_SetTextVCenter()));
    connect(ui->textBottom,SIGNAL(pressed()),this,SLOT(s_SetTextBottom()));
    connect(ui->fontEffectCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChangeStyleFont(int)));
    connect(ui->PosXEd,SIGNAL(valueChanged(double)),this,SLOT(s_ChgPosXValue(double)));
    connect(ui->PosYEd,SIGNAL(valueChanged(double)),this,SLOT(s_ChgPosYValue(double)));
    connect(ui->WidthEd,SIGNAL(valueChanged(double)),this,SLOT(s_ChgWidthValue(double)));
    connect(ui->HeightEd,SIGNAL(valueChanged(double)),this,SLOT(s_ChgHeightValue(double)));
    connect(ui->BackgroundFormCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChangeBackgroundForm(int)));
    connect(ui->BackgroundTransparentCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChangeBackgroundTransparent(int)));
    connect(ui->PenSizeEd,SIGNAL(valueChanged(int)),this,SLOT(s_ChgPenSize(int)));
    connect(ui->plainTextEdit,SIGNAL(textChanged()),this,SLOT(s_plainTextEditChange()));

    connect(ui->FontColorCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChIndexFontColorCombo(int)));
    connect(ui->StyleShadowColorCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChIndexFontShadowColorCombo(int)));
    connect(ui->PenColorCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChIndexPenColorCombo(int)));
    connect(ui->fontColorB,SIGNAL(pressed()),this,SLOT(s_CustomFontColorBt()));
    connect(ui->StyleShadowColorBt,SIGNAL(pressed()),this,SLOT(s_CustomShadowColorBt()));
    connect(ui->PenColorBt,SIGNAL(pressed()),this,SLOT(s_CustomPenColorBt()));
}

//====================================================================================================================

wgt_QCompositionWidget::~wgt_QCompositionWidget() {
    StopMAJSpinbox=true;
    delete ui;
    if (scene!=NULL) {
        delete scene;
        scene=NULL;
    }
    if (BackgroundImage!=NULL) {
        delete BackgroundImage;
        BackgroundImage=NULL;
    }
}

//====================================================================================================================

void wgt_QCompositionWidget::resizeEvent(QResizeEvent *) {
    if ((scene!=NULL)&&(BackgroundImage!=NULL)) {
        // Calc and adjust ui->SceneBox depending on geometry
        xmax=double(ui->SceneBox->width());
        ymax=double(ui->SceneBox->width())*(double(double(BackgroundImage->height())/BackgroundImage->width()));
        // Setup the scene
        scene->setSceneRect(QRectF(0,0,xmax,ymax));
        ui->SceneBox->fitInView(QRectF(0,0,xmax,ymax),Qt::KeepAspectRatio);

        // send a message to parent to refresh background image
        emit NeedRefreshBackgroundImage();
    }
}

//====================================================================================================================

void wgt_QCompositionWidget::showEvent(QShowEvent *) {
    // Make vertical dispay text for some label
    QPainter Painter;
    QFont   Font("Helvetica",10,QFont::Normal,true);
/*
    QPixmap Image(ui->BorderLabel->width(),ui->BorderLabel->height());
    Painter.begin(&Image);
    Painter.setFont(Font);
    Painter.setPen(Qt::black);
    Painter.fillRect(QRectF(0,0,ui->BorderLabel->width(),ui->BorderLabel->height()),QColor(Qt::white));
    Painter.rotate(-90);
    Painter.translate(-ui->BorderLabel->height(),-ui->BorderLabel->width()/2);
    Painter.drawText(QPointF(0,ui->BorderLabel->width()),QCoreApplication::translate("wgt_QCompositionWidget","Border"));
    Painter.end();
    ui->BorderLabel->setPixmap(Image);

    QPixmap Image2(ui->ColorLabel->width(),ui->ColorLabel->height());
    Painter.begin(&Image2);
    Painter.setFont(Font);
    Painter.setPen(Qt::black);
    Painter.fillRect(QRectF(0,0,ui->ColorLabel->width(),ui->ColorLabel->height()),QColor(Qt::white));
    Painter.rotate(-90);
    Painter.translate(-ui->ColorLabel->height(),-ui->ColorLabel->width()/2);
    Painter.drawText(QPointF(0,ui->ColorLabel->width()),QCoreApplication::translate("wgt_QCompositionWidget","Gradient colors"));
    Painter.end();
    ui->ColorLabel->setPixmap(Image2);
*/
    if (scene==NULL) {
        //--------------------------------------------------------------------
        // Prepare the scene
        //--------------------------------------------------------------------
        // Calc and adjust ui->SceneBox depending on geometry
        xmax=double(ui->SceneBox->width());
        ymax=double(ui->SceneBox->height());
        //ymax=double(ui->SceneBox->width())/(double(double(BackgroundImage->width())/BackgroundImage->height()));

        // Create the scene
        scene = new QGraphicsScene();
        connect(scene,SIGNAL(selectionChanged()),this,SLOT(s_SelectionChangeEvent()));
        scene->setSceneRect(QRectF(0,0,xmax,ymax));

        // Setup scene to control
        ui->SceneBox->setScene(scene);
        ui->SceneBox->setInteractive(true);
        ui->SceneBox->setDragMode(QGraphicsView::ScrollHandDrag);
        ui->SceneBox->fitInView(QRectF(0,0,xmax,ymax),Qt::KeepAspectRatio);

        // send a message to parent to refresh background image (because now we know size)
        emit NeedRefreshBackgroundImage();

    } else RefreshBackgroundImage();
}

//====================================================================================================================

QSize wgt_QCompositionWidget::GetSceneBoxSize() {
    return QSize(ui->SceneBox->width(),ui->SceneBox->height());
}

//====================================================================================================================

void wgt_QCompositionWidget::SetCompositionObject(cCompositionList *TheCompositionList,QPixmap *TheBackgroundImage) {
    if (scene==NULL) return;
    if (BackgroundImage!=NULL) {
        delete BackgroundImage;
        BackgroundImage=NULL;
    }
    CompositionList =TheCompositionList;
    BackgroundImage =TheBackgroundImage;
    if ((CompositionList!=NULL)&&(BackgroundImage!=NULL)) {
        xmax=double(ui->SceneBox->width());
        ymax=double(ui->SceneBox->width())*(double(double(BackgroundImage->height())/BackgroundImage->width()));
        scene->setSceneRect(QRectF(0,0,xmax,ymax));
        ui->SceneBox->fitInView(QRectF(0,0,xmax,ymax),Qt::KeepAspectRatio);

        if (!IsInit) {
            // Create cCustomGraphicsRectItem associate to existing cCompositionObject
            NextZValue=500;
            for (int i=0;i<CompositionList->List.count();i++) {
                // Create and add to scene a cCustomGraphicsRectItem
                new cCustomGraphicsRectItem(scene,CompositionList->List[i].ZValue,&CompositionList->List[i].x,&CompositionList->List[i].y,
                                            NULL,&CompositionList->List[i].w,&CompositionList->List[i].h,xmax,ymax,false,NULL,this,TYPE_wgt_QCompositionWidget);
                if (NextZValue<CompositionList->List[i].ZValue) NextZValue=CompositionList->List[i].ZValue;
            }
            NextZValue+=10;  // 10 by 10 step for ZValue

            IsInit=true;
        }

        RefreshControls();
    }
    // Resize et repos all item in the scene
    if (BackgroundImage!=NULL) {
        for (int i=0;i<scene->items().count();i++) if (scene->items()[i]->data(0).toString()=="CustomGraphicsRectItem") {
            cCustomGraphicsRectItem *RectItem=(cCustomGraphicsRectItem *)scene->items()[i];
            for (int j=0;j<CompositionList->List.count();j++) if (CompositionList->List[j].ZValue==RectItem->zValue()) {
                cCompositionObject  *CurrentTextItem=&CompositionList->List[j];
                RectItem->setPos(CurrentTextItem->x*xmax,CurrentTextItem->y*ymax);
                QRectF Rect=RectItem->mapRectFromScene(QRectF(CurrentTextItem->x*xmax,CurrentTextItem->y*ymax,xmax*CurrentTextItem->w,ymax*CurrentTextItem->h));
                RectItem->setRect(Rect);
                RectItem->RecalcEmbededResizeRectItem();
            }
        }
    } else emit NeedRefreshBackgroundImage();
}

//====================================================================================================================

void wgt_QCompositionWidget::StartRefreshControls() {
    RefreshControls();
}

void wgt_QCompositionWidget::RefreshControls() {
    if (StopMAJSpinbox) return;
    StopMAJSpinbox=true;

    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    ui->CustomBrushWidget->RefreshControls((CurrentTextItem!=NULL)?&CurrentTextItem->BackgroundBrush:NULL,
                                           (CurrentTextItem!=NULL)&&(CurrentTextItem->BackgroundForm!=0)&&(CurrentTextItem->BackgroundTransparent<4));

    if (CurrentTextItem!=NULL) {
        //***********************
        // Text TAB
        //***********************
        BLOCKCHSIZE=true;
        ui->fontSize->setDisabled(false);
        ui->fontSize->setCurrentIndex(ui->fontSize->findText(QString("%1").arg(CurrentTextItem->FontSize)));
        BLOCKCHSIZE=false;

        ui->RemoveText->setDisabled(false);
        ui->textLeft->setDisabled(false);       ui->textLeft->setChecked(CurrentTextItem->HAlign==0);       ui->textLeft->setDown(CurrentTextItem->HAlign==0);
        ui->textCenter->setDisabled(false);     ui->textCenter->setChecked(CurrentTextItem->HAlign==1);     ui->textCenter->setDown(CurrentTextItem->HAlign==1);
        ui->textJustif->setDisabled(false);     ui->textJustif->setChecked(CurrentTextItem->HAlign==3);     ui->textJustif->setDown(CurrentTextItem->HAlign==3);
        ui->textRight->setDisabled(false);      ui->textRight->setChecked(CurrentTextItem->HAlign==2);      ui->textRight->setDown(CurrentTextItem->HAlign==2);
        ui->textUp->setDisabled(false);         ui->textUp->setChecked(CurrentTextItem->VAlign==0);         ui->textUp->setDown(CurrentTextItem->VAlign==0);
        ui->textVCenter->setDisabled(false);    ui->textVCenter->setChecked(CurrentTextItem->VAlign==1);    ui->textVCenter->setDown(CurrentTextItem->VAlign==1);
        ui->textBottom->setDisabled(false);     ui->textBottom->setChecked(CurrentTextItem->VAlign==2);     ui->textBottom->setDown(CurrentTextItem->VAlign==2);
        ui->bold->setDisabled(false);           ui->bold->setChecked(CurrentTextItem->IsBold);              ui->bold->setDown(CurrentTextItem->IsBold);
        ui->Italic->setDisabled(false);         ui->Italic->setChecked(CurrentTextItem->IsItalic);          ui->Italic->setDown(CurrentTextItem->IsItalic);
        ui->Souligne->setDisabled(false);       ui->Souligne->setChecked(CurrentTextItem->IsUnderline);     ui->Souligne->setDown(CurrentTextItem->IsUnderline);

        ui->plainTextEdit->setDisabled(false);  if (ui->plainTextEdit->toPlainText()!=CurrentTextItem->Text) ui->plainTextEdit->setPlainText(CurrentTextItem->Text);
        ui->fontStyleCB->setDisabled(false);    ui->fontStyleCB->setCurrentIndex(ui->fontStyleCB->findText(QString(CurrentTextItem->FontName)));
        ui->fontEffectCB->setDisabled(false);   ui->fontEffectCB->setCurrentIndex(CurrentTextItem->StyleText);

        ui->FontColorCombo->setDisabled(false);
        ui->FontColorCombo->SetCurrentColor(CurrentTextItem->FontColor);
        ui->fontColorB->setVisible(ui->FontColorCombo->StandardColor==false);
        ui->fontColorB->setEnabled(ui->FontColorCombo->StandardColor==false);

        ui->StyleShadowColorCombo->setDisabled(CurrentTextItem->StyleText==0);
        ui->StyleShadowColorCombo->SetCurrentColor(CurrentTextItem->FontShadowColor);
        ui->StyleShadowColorBt->setVisible((ui->StyleShadowColorCombo->StandardColor==false)&&(CurrentTextItem->StyleText!=0));
        ui->StyleShadowColorBt->setEnabled((ui->StyleShadowColorCombo->StandardColor==false)&&(CurrentTextItem->StyleText!=0));

        //***********************
        // Shape TAB
        //***********************
        StopMAJSpinbox=true;    // Disable reintrence in this RefreshControls function
        ui->BackgroundTransparentCB->setCurrentIndex(CurrentTextItem->BackgroundTransparent);
        ui->PenColorCombo->SetCurrentColor(CurrentTextItem->PenColor);
        StopMAJSpinbox=false;

        ui->BackgroundFormCB->setDisabled(false);   ui->BackgroundFormCB->setCurrentIndex(CurrentTextItem->BackgroundForm);
        ui->BackgroundTransparentCB->setDisabled(CurrentTextItem->BackgroundForm==0);

        ui->PenSizeEd->setEnabled(CurrentTextItem->BackgroundForm!=0);
        ui->PenSizeEd->setValue(int(CurrentTextItem->PenSize)); ui->PenSizeEd->setMinimum(0); ui->PenSizeEd->setMaximum(10);
        ui->PenColorCombo->setDisabled((CurrentTextItem->BackgroundForm==0)||(CurrentTextItem->PenSize==0));
        ui->PenColorBt->setVisible((CurrentTextItem->BackgroundForm!=0)&&(CurrentTextItem->PenSize!=0)&&(!ui->PenColorCombo->StandardColor));
        ui->PenColorBt->setEnabled((CurrentTextItem->BackgroundForm!=0)&&(CurrentTextItem->PenSize!=0)&&(!ui->PenColorCombo->StandardColor));

        //***********************
        // Size & Position TAB
        //***********************
        StopMAJSpinbox=true;    // Disable reintrence in this RefreshControls function
        ui->PosXEd->setDisabled(false);     ui->PosXEd->setRange(0,99-CurrentTextItem->w*100);      ui->PosXEd->setValue(CurrentTextItem->x*100);   ui->PosXEd->setSingleStep(int(100/20));
        ui->PosYEd->setDisabled(false);     ui->PosYEd->setRange(0,99-CurrentTextItem->h*100);      ui->PosYEd->setValue(CurrentTextItem->y*100);   ui->PosYEd->setSingleStep(int(100/20));
        ui->WidthEd->setDisabled(false);    ui->WidthEd->setRange(3,99-CurrentTextItem->x*100);     ui->WidthEd->setValue(CurrentTextItem->w*100);  ui->WidthEd->setSingleStep(int(100/20));
        ui->HeightEd->setDisabled(false);   ui->HeightEd->setRange(3,99-CurrentTextItem->y*100);    ui->HeightEd->setValue(CurrentTextItem->h*100); ui->HeightEd->setSingleStep(int(100/20));
        StopMAJSpinbox=false;
    } else {
        //***********************
        // Text TAB
        //***********************
        ui->fontSize->setDisabled(true);
        ui->RemoveText->setDisabled(true);
        ui->textLeft->setDisabled(true);
        ui->textCenter->setDisabled(true);
        ui->textJustif->setDisabled(true);
        ui->textRight->setDisabled(true);
        ui->textUp->setDisabled(true);
        ui->textVCenter->setDisabled(true);
        ui->textBottom->setDisabled(true);
        ui->bold->setDisabled(true);
        ui->Italic->setDisabled(true);
        ui->Souligne->setDisabled(true);
        ui->plainTextEdit->setPlainText("");
        ui->fontStyleCB->setDisabled(true);
        ui->fontEffectCB->setDisabled(true);
        ui->FontColorCombo->setDisabled(true);
        ui->fontColorB->setDisabled(true);
        ui->StyleShadowColorCombo->setDisabled(true);
        ui->StyleShadowColorBt->setDisabled(true);

        //***********************
        // Shape TAB
        //***********************
        ui->BackgroundFormCB->setDisabled(true);
        ui->BackgroundTransparentCB->setDisabled(true);
        ui->PenSizeEd->setDisabled(true);
        ui->PenColorCombo->setDisabled(true);
        ui->PenColorBt->setDisabled(true);

        //***********************
        // Size & Position TAB
        //***********************
        ui->PosXEd->setDisabled(true);      ui->PosXEd->setValue(0);
        ui->PosYEd->setDisabled(true);      ui->PosYEd->setValue(0);
        ui->WidthEd->setDisabled(true);     ui->WidthEd->setValue(0);
        ui->HeightEd->setDisabled(true);    ui->HeightEd->setValue(0);
    }

    StopMAJSpinbox=false;
    RefreshBackgroundImage();
}

//====================================================================================================================

void wgt_QCompositionWidget::RefreshBackgroundImage() {
    if ((scene==NULL)||(BackgroundImage==NULL)) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    // Remove old image if exist
    int i=0;
    while ((i<scene->items().count())&&(scene->items().at(i)->data(0).toString()!="image")) i++;
    if ((i<scene->items().count())&&(scene->items().at(i)->data(0).toString()=="image")) {
        QGraphicsPixmapItem *im=(QGraphicsPixmapItem *)scene->items().at(i);
        scene->removeItem(im);
        delete im;
    }

    QPixmap *NewImage=new QPixmap(BackgroundImage->scaled(xmax,ymax));
    QPainter P;
    P.begin(NewImage);

    for (int i=0;i<CompositionList->List.count();i++) {
        // Draw composition
        CompositionList->List[i].DrawCompositionObject(P,0,0,xmax,ymax);
        // Draw border
        if (GetSelectedCompositionObject()==&CompositionList->List[i]) {
            // draw rect out of the rectangle
            P.setCompositionMode(QPainter::RasterOp_SourceXorDestination);
            QPen pen=QPen(Qt::red);
            pen.setWidth(2);
            pen.setStyle(Qt::DotLine);
            P.setPen(pen);
            P.setBrush(Qt::NoBrush);
            P.drawRect(QRectF(CompositionList->List[i].x*xmax,CompositionList->List[i].y*ymax,CompositionList->List[i].w*xmax,CompositionList->List[i].h*ymax));
            P.setCompositionMode(QPainter::CompositionMode_SourceOver);
        } else {
            // draw rect out of the rectangle
            P.setCompositionMode(QPainter::RasterOp_SourceXorDestination);
            QPen pen=QPen(Qt::lightGray);
            pen.setWidth(2);
            pen.setStyle(Qt::DotLine);
            P.setPen(pen);
            P.setBrush(Qt::NoBrush);
            P.drawRect(QRectF(CompositionList->List[i].x*xmax,CompositionList->List[i].y*ymax,CompositionList->List[i].w*xmax,CompositionList->List[i].h*ymax));
            P.setCompositionMode(QPainter::CompositionMode_SourceOver);
        }
    }
    P.end();

    // Add image to the background of the scene
    QGraphicsPixmapItem *im=scene->addPixmap(*NewImage);    // Image will be delete by scene
    im->setData(0,QVariant(QString("image")));
    im->setZValue(200);
    im->setPos(0,0);
//    delete NewImage;

    QApplication::restoreOverrideCursor();
    emit BackgroundImageUpdated();
}

//====================================================================================================================

cCustomGraphicsRectItem *wgt_QCompositionWidget::GetSelectItem() {
    if (scene==NULL) return NULL;
    cCustomGraphicsRectItem *CurrentTextItem=NULL;
    for (int i=0;i<scene->selectedItems().count();i++) {
        QGraphicsItem   *Item=scene->selectedItems()[i];
        QString         data =Item->data(0).toString();
        if (data=="CustomGraphicsRectItem")      CurrentTextItem=(cCustomGraphicsRectItem *)Item;
        else if (data=="ResizeGraphicsRectItem") CurrentTextItem=((cResizeGraphicsRectItem *)Item)->RectItem;
    }
    return CurrentTextItem;
}

//====================================================================================================================

cCompositionObject *wgt_QCompositionWidget::GetSelectedCompositionObject() {
    cCustomGraphicsRectItem *CurrentCustomGraphicsRectItem=GetSelectItem();
    if (CurrentCustomGraphicsRectItem!=NULL) {
        for (int i=0;i<CompositionList->List.count();i++) if (CompositionList->List[i].ZValue==CurrentCustomGraphicsRectItem->zValue()) return &CompositionList->List[i];
    }
    return NULL;
}

//====================================================================================================================

void wgt_QCompositionWidget::s_SelectionChangeEvent() {
    RefreshControls();
}

//====================================================================================================================

void wgt_QCompositionWidget::s_AddNewTextToImage() {
    CompositionList->List.append(cCompositionObject());
    cCompositionObject *CompositionObject=&CompositionList->List[CompositionList->List.count()-1];
    CompositionObject->ZValue=NextZValue;
    CompositionObject->Text=QCoreApplication::translate("wgt_QCompositionWidget","Text","Default text value");

    // Unselect all item
    while (scene->selectedItems().count()>0) scene->selectedItems()[0]->setSelected(false);

    // Create and add to scene a cCustomGraphicsRectItem
    cCustomGraphicsRectItem *GraphicsRectItem=new cCustomGraphicsRectItem(scene,NextZValue,&CompositionObject->x,&CompositionObject->y,
                                                                          NULL,&CompositionObject->w,&CompositionObject->h,xmax,ymax,false,NULL,this,TYPE_wgt_QCompositionWidget);

    // select new item
    GraphicsRectItem->setSelected(true);

    // 10 by 10 step for ZValue
    NextZValue+=10;
    RefreshControls();
}

//====================================================================================================================

void wgt_QCompositionWidget::s_RemoveTextFromImage() {
    cCompositionObject      *CurrentComposeItem=GetSelectedCompositionObject();
    cCustomGraphicsRectItem *CurrentRectItem   =GetSelectItem();
    if ((CurrentComposeItem==NULL)||(CurrentRectItem==NULL)) return;
    int i=0;
    while ((i<CompositionList->List.count())&&(&CompositionList->List[i]!=CurrentComposeItem)) i++;
    if ((i<CompositionList->List.count())&&(&CompositionList->List[i]==CurrentComposeItem)) {
        scene->removeItem(CurrentRectItem);
        CompositionList->List.removeAt(i);
    }
    RefreshControls();
}

//====================================================================================================================

void wgt_QCompositionWidget::s_SetBold() {
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if (CurrentTextItem==NULL) return;
    if (CurrentTextItem->IsBold==true) CurrentTextItem->IsBold=false; else CurrentTextItem->IsBold=true;
    RefreshControls();
}

//====================================================================================================================

void wgt_QCompositionWidget::s_SetItalic() {
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if (CurrentTextItem==NULL) return;
    if (CurrentTextItem->IsItalic==true) CurrentTextItem->IsItalic=false; else CurrentTextItem->IsItalic=true;
    RefreshControls();
}

//====================================================================================================================

void wgt_QCompositionWidget::s_SetUnderline() {
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if (CurrentTextItem==NULL) return;
    if (CurrentTextItem->IsUnderline==true) CurrentTextItem->IsUnderline=false; else CurrentTextItem->IsUnderline=true;
    RefreshControls();
}

//====================================================================================================================

void wgt_QCompositionWidget::s_SetTextLeft() {
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if (CurrentTextItem==NULL) return;
    CurrentTextItem->HAlign=0;
    RefreshControls();
}

//====================================================================================================================

void wgt_QCompositionWidget::s_SetTextCenter() {
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if (CurrentTextItem==NULL) return;
    CurrentTextItem->HAlign=1;
    RefreshControls();
}

//====================================================================================================================

void wgt_QCompositionWidget::s_SetTextRight() {
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if (CurrentTextItem==NULL) return;
    CurrentTextItem->HAlign=2;
    RefreshControls();
}

//====================================================================================================================

void wgt_QCompositionWidget::s_SetTextJustif() {
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if (CurrentTextItem==NULL) return;
    CurrentTextItem->HAlign=3;
    RefreshControls();
}

//====================================================================================================================

void wgt_QCompositionWidget::s_SetTextUp() {
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if (CurrentTextItem==NULL) return;
    CurrentTextItem->VAlign=0;
    RefreshControls();
}

//====================================================================================================================

void wgt_QCompositionWidget::s_SetTextVCenter() {
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if (CurrentTextItem==NULL) return;
    CurrentTextItem->VAlign=1;
    RefreshControls();
}

//====================================================================================================================

void wgt_QCompositionWidget::s_SetTextBottom() {
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if (CurrentTextItem==NULL) return;
    CurrentTextItem->VAlign=2;
    RefreshControls();
}

//====================================================================================================================

void wgt_QCompositionWidget::s_ChangeFont(QFont font) {
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if (CurrentTextItem==NULL) return;
    if (font.family()!="") CurrentTextItem->FontName=font.family();
    RefreshControls();
}

//====================================================================================================================

void wgt_QCompositionWidget::s_ChangeSizeFont(QString size) {
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if (CurrentTextItem==NULL) return;
    if ((size!="")&&(BLOCKCHSIZE==false)) CurrentTextItem->FontSize=size.toInt();
    RefreshControls();
}

//====================================================================================================================

void wgt_QCompositionWidget::s_ChangeStyleFont(int Style) {
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if (CurrentTextItem==NULL) return;
    CurrentTextItem->StyleText=Style;
    RefreshControls();
}

//====================================================================================================================

void wgt_QCompositionWidget::s_ChgPosXValue(double Value) {
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if (CurrentTextItem==NULL) return;
    if (StopMAJSpinbox) return;
    CurrentTextItem->x=Value/100;
    cCustomGraphicsRectItem *RectItem=GetSelectItem();
    RectItem->setPos(CurrentTextItem->x*xmax,CurrentTextItem->y*ymax);
    QRectF Rect=RectItem->mapRectFromScene(QRectF(CurrentTextItem->x*xmax,CurrentTextItem->y*ymax,xmax*CurrentTextItem->w,ymax*CurrentTextItem->h));
    RectItem->setRect(Rect);
    RectItem->RecalcEmbededResizeRectItem();
    RefreshControls();
}

//====================================================================================================================

void wgt_QCompositionWidget::s_ChgPosYValue(double Value) {
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if (CurrentTextItem==NULL) return;
    if (StopMAJSpinbox) return;
    CurrentTextItem->y=Value/100;
    cCustomGraphicsRectItem *RectItem=GetSelectItem();
    RectItem->setPos(CurrentTextItem->x*xmax,CurrentTextItem->y*ymax);
    QRectF Rect=RectItem->mapRectFromScene(QRectF(CurrentTextItem->x*xmax,CurrentTextItem->y*ymax,xmax*CurrentTextItem->w,ymax*CurrentTextItem->h));
    RectItem->setRect(Rect);
    RectItem->RecalcEmbededResizeRectItem();
    RefreshControls();
}

//====================================================================================================================

void wgt_QCompositionWidget::s_ChgWidthValue(double Value) {
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if (CurrentTextItem==NULL) return;
    if (StopMAJSpinbox) return;
    CurrentTextItem->w=Value/100;
    cCustomGraphicsRectItem *RectItem=GetSelectItem();
    RectItem->setPos(CurrentTextItem->x*xmax,CurrentTextItem->y*ymax);
    QRectF Rect=RectItem->mapRectFromScene(QRectF(CurrentTextItem->x*xmax,CurrentTextItem->y*ymax,xmax*CurrentTextItem->w,ymax*CurrentTextItem->h));
    RectItem->setRect(Rect);
    RectItem->RecalcEmbededResizeRectItem();
    RefreshControls();
}

//====================================================================================================================

void wgt_QCompositionWidget::s_ChgHeightValue(double Value) {
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if (CurrentTextItem==NULL) return;
    if (StopMAJSpinbox) return;
    CurrentTextItem->h=Value/100;
    cCustomGraphicsRectItem *RectItem=GetSelectItem();
    RectItem->setPos(CurrentTextItem->x*xmax,CurrentTextItem->y*ymax);
    QRectF Rect=RectItem->mapRectFromScene(QRectF(CurrentTextItem->x*xmax,CurrentTextItem->y*ymax,xmax*CurrentTextItem->w,ymax*CurrentTextItem->h));
    RectItem->setRect(Rect);
    RectItem->RecalcEmbededResizeRectItem();
    RefreshControls();
}

//====================================================================================================================

void wgt_QCompositionWidget::s_ChangeBackgroundForm(int Style) {
    if (StopMAJSpinbox) return;
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if (CurrentTextItem==NULL) return;
    CurrentTextItem->BackgroundForm=Style;
    RefreshControls();
}

//====================================================================================================================

void wgt_QCompositionWidget::s_ChangeBackgroundTransparent(int Style) {
    if (StopMAJSpinbox) return;
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if (CurrentTextItem==NULL) return;
    CurrentTextItem->BackgroundTransparent=Style;
    RefreshControls();
}

//====================================================================================================================

void wgt_QCompositionWidget::s_ChgPenSize(int Value) {
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if (CurrentTextItem==NULL) return;
    if (StopMAJSpinbox) return;
    CurrentTextItem->PenSize=Value;
    RefreshControls();
}

//====================================================================================================================

void wgt_QCompositionWidget::s_plainTextEditChange() {
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if (CurrentTextItem==NULL) return;
    CurrentTextItem->Text=ui->plainTextEdit->toPlainText();
    RefreshControls();
}

//====================================================================================================================

void wgt_QCompositionWidget::MakeFormIcon(QComboBox *UICB) {
    for (int i=0;i<UICB->count();i++) {
        cCompositionObject Object;
        Object.Text                     ="";
        Object.x                        =0;
        Object.y                        =0;
        Object.w                        =1;
        Object.h                        =1;
        Object.PenSize                  =1;
        Object.PenColor                 ="#000000";
        Object.BackgroundForm           =i;
        Object.BackgroundTransparent    =4;
        QPixmap  Image(32,32);
        QPainter Painter;
        Painter.begin(&Image);
        Painter.fillRect(QRectF(0,0,32,32),Qt::white);
        Object.DrawCompositionObject(Painter,0,0,32,32);
        Painter.end();
        UICB->setItemIcon(i,QIcon(Image));
    }
}

void wgt_QCompositionWidget::MakeTextStyleIcon(QComboBox *UICB) {
    for (int i=0;i<UICB->count();i++) {
        cCompositionObject Object;
        Object.Text="T";
        Object.x=0.15;
        Object.y=0.15;
        Object.w=0.7;
        Object.h=0.7;
        Object.HAlign           =1;                 // Center
        Object.VAlign           =1;                 // Center
        Object.FontColor        ="#ffffff";         // White
        Object.FontShadowColor  ="#000000";         // Black
        Object.StyleText        =i;
        Object.FontSize         =200;
        Object.IsBold           =true;
        Object.PenSize          =0;
        Object.BackgroundForm   =0;
        Object.BackgroundTransparent=0;
        QPixmap  Image(32,32);
        QPainter Painter;
        Painter.begin(&Image);
        Painter.fillRect(QRectF(0,0,32,32),Qt::lightGray);
        Object.DrawCompositionObject(Painter,0,0,32,32);
        Painter.end();
        UICB->setItemIcon(i,QIcon(Image));
    }
}

//====================================================================================================================
// Functions call by custom button of each custom color combo box
//====================================================================================================================

//========= Font color
void wgt_QCompositionWidget::s_CustomFontColorBt() {
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if (CurrentTextItem==NULL) return;
    QColor color=QColorDialog::getColor(QColor(CurrentTextItem->FontColor));
    if (color.isValid()) {
        CurrentTextItem->FontColor=color.name();
        ui->FontColorCombo->SetCurrentColor(CurrentTextItem->FontColor);
        RefreshControls();
    }
}

//========= Text shadow color
void wgt_QCompositionWidget::s_CustomShadowColorBt() {
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if (CurrentTextItem==NULL) return;
    QColor color=QColorDialog::getColor(QColor(CurrentTextItem->FontShadowColor));
    if (color.isValid()) {
        CurrentTextItem->FontShadowColor=color.name();
        ui->StyleShadowColorCombo->SetCurrentColor(CurrentTextItem->FontShadowColor);
        RefreshControls();
    }
}

//========= Shape border color
void wgt_QCompositionWidget::s_CustomPenColorBt() {
    if (StopMAJSpinbox) return;
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if (CurrentTextItem==NULL) return;
    QColor color=QColorDialog::getColor(CurrentTextItem->PenColor);
    if (color.isValid()) {
        CurrentTextItem->PenColor=color.name();
        ui->PenColorCombo->SetCurrentColor(CurrentTextItem->PenColor);
        RefreshControls();
    }
}

//====================================================================================================================
// Handler for custom color/brush/pattern/gradient combo box index change
//====================================================================================================================

//========= Font color
void wgt_QCompositionWidget::s_ChIndexFontColorCombo(int) {
    if (StopMAJSpinbox) return;
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if (CurrentTextItem==NULL) return;
    CurrentTextItem->FontColor=ui->FontColorCombo->GetCurrentColor();
    RefreshControls();
}

//========= Text shadow color
void wgt_QCompositionWidget::s_ChIndexFontShadowColorCombo(int) {
    if (StopMAJSpinbox) return;
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if (CurrentTextItem==NULL) return;
    CurrentTextItem->FontShadowColor=ui->StyleShadowColorCombo->GetCurrentColor();
    RefreshControls();
}

//========= Shape border color
void wgt_QCompositionWidget::s_ChIndexPenColorCombo(int) {
    if (StopMAJSpinbox) return;
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if (CurrentTextItem==NULL) return;
    CurrentTextItem->PenColor=ui->PenColorCombo->GetCurrentColor();
    RefreshControls();
}