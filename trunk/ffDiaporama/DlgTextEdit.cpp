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

#include "DlgTextEdit.h"
#include "ui_DlgTextEdit.h"

//#define DEBUGMODE

DlgTextEdit::DlgTextEdit(cCompositionObject *TheCurrentTextItem,QString HelpURL,cBaseApplicationConfig *ApplicationConfig,cSaveWindowPosition *DlgWSP,QWidget *parent):
    QCustomDialog(HelpURL,ApplicationConfig,DlgWSP,parent),ui(new Ui::DlgTextEdit) {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgTextEdit::DlgTextEdit";
    #endif
    ui->setupUi(this);
    OkBt            =ui->OKBT;
    CancelBt        =ui->CancelBt;
    HelpBt          =ui->HelpBT;
    CurrentTextItem =TheCurrentTextItem;
    StopMAJSpinbox  =false;
}

//====================================================================================================================

DlgTextEdit::~DlgTextEdit() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgTextEdit::~DlgTextEdit";
    #endif
    delete ui;
}

//====================================================================================================================
// Initialise dialog

void DlgTextEdit::DoInitDialog() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgTextEdit::DoInitDialog";
    #endif

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
    ui->fontEffectCB->addItem(QApplication::translate("DlgTextEdit","No effect"));
    ui->fontEffectCB->addItem(QApplication::translate("DlgTextEdit","Outerline"));
    ui->fontEffectCB->addItem(QApplication::translate("DlgTextEdit","Shadow upper left"));
    ui->fontEffectCB->addItem(QApplication::translate("DlgTextEdit","Shadow upper right"));
    ui->fontEffectCB->addItem(QApplication::translate("DlgTextEdit","Shadow bottom left"));
    ui->fontEffectCB->addItem(QApplication::translate("DlgTextEdit","Shadow bottom right"));
    ui->fontEffectCB->view()->setFixedWidth(160+18);
    ui->fontEffectCB->view()->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    MakeTextStyleIcon(ui->fontEffectCB);

    // Init combo box Background  type
    ui->BrushTypeCombo->addItem(QApplication::translate("DlgTextEdit","No brush"));              ui->BrushTypeCombo->setItemData(ui->BrushTypeCombo->count()-1,QVariant(int(BRUSHTYPE_NOBRUSH)));
    ui->BrushTypeCombo->addItem(QApplication::translate("DlgTextEdit","Solid brush"));           ui->BrushTypeCombo->setItemData(ui->BrushTypeCombo->count()-1,QVariant(int(BRUSHTYPE_SOLID)));
    ui->BrushTypeCombo->addItem(QApplication::translate("DlgTextEdit","Pattern brush"));         ui->BrushTypeCombo->setItemData(ui->BrushTypeCombo->count()-1,QVariant(int(BRUSHTYPE_PATTERN)));
    ui->BrushTypeCombo->addItem(QApplication::translate("DlgTextEdit","Gradient 2 colors"));     ui->BrushTypeCombo->setItemData(ui->BrushTypeCombo->count()-1,QVariant(int(BRUSHTYPE_GRADIENT2)));
    ui->BrushTypeCombo->addItem(QApplication::translate("DlgTextEdit","Gradient 3 colors"));     ui->BrushTypeCombo->setItemData(ui->BrushTypeCombo->count()-1,QVariant(int(BRUSHTYPE_GRADIENT3)));
    ui->BrushTypeCombo->addItem(QApplication::translate("DlgTextEdit","Image from library"));    ui->BrushTypeCombo->setItemData(ui->BrushTypeCombo->count()-1,QVariant(int(BRUSHTYPE_IMAGELIBRARY)));

    RefreshControls();

    // Define handler
    connect(ui->plainTextEdit,SIGNAL(textChanged()),this,SLOT(s_plainTextEditChange()));
    connect(ui->FontColorCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChIndexFontColorCombo(int)));
    connect(ui->StyleShadowColorCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChIndexFontShadowColorCombo(int)));
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

    connect(ui->TextStyleBT,SIGNAL(pressed()),this,SLOT(s_TextStyleBT()));
    connect(ui->BackgroundStyleBT,SIGNAL(pressed()),this,SLOT(s_BackgroundStyleBT()));

    // Brush part
    connect(ui->BrushTypeCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChangeBrushTypeCombo(int)));
    connect(ui->PatternBrushCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChIndexPatternBrushCombo(int)));
    connect(ui->OrientationCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChIndexGradientOrientationCombo(int)));
    connect(ui->FirstColorCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChIndexGradientFirstColorCombo(int)));
    connect(ui->FinalColorCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChIndexGradientFinalColorCombo(int)));
    connect(ui->IntermColorCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChIndexGradientIntermColorCombo(int)));
    connect(ui->BackgroundCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChIndexBackgroundCombo(int)));
    connect(ui->IntermPosSlider,SIGNAL(valueChanged(int)),this,SLOT(s_IntermPosSliderMoved(int)));
    connect(ui->IntermPosED,SIGNAL(valueChanged(int)),this,SLOT(s_IntermPosED(int)));

}

//====================================================================================================================
// Initiale Undo

void DlgTextEdit::PrepareGlobalUndo() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgTextEdit::PrepareGlobalUndo";
    #endif
    // Save object before modification for cancel button
    Undo=new QDomDocument(APPLICATION_NAME);
    QDomElement root=Undo->createElement("UNDO-DLG");       // Create xml document and root
    CurrentTextItem->SaveToXML(root,"UNDO-DLG-OBJECT",GlobalMainWindow->Diaporama->ProjectFileName,true);  // Save object
    Undo->appendChild(root);                                // Add object to xml document
}

//====================================================================================================================
// Apply Undo : call when user click on Cancel button

void DlgTextEdit::DoGlobalUndo() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgTextEdit::DoGlobalUndo";
    #endif
    // Restore element
    QDomElement root=Undo->documentElement();
    if (root.tagName()=="UNDO-DLG") CurrentTextItem->LoadFromXML(root,"UNDO-DLG-OBJECT","",NULL,NULL);
}

//====================================================================================================================

void DlgTextEdit::RefreshControls() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgTextEdit::RefreshControls";
    #endif
    StopMAJSpinbox=true;
    // Update text controls
    ui->TextStyleED->setText(GlobalMainWindow->ApplicationConfig->StyleTextCollection.GetStyleName(CurrentTextItem->GetTextStyle()));
    if (CurrentTextItem->FontSize!=ui->fontSize->currentIndex())        ui->fontSize->setCurrentIndex(ui->fontSize->findText(QString("%1").arg(CurrentTextItem->FontSize)));
    if (CurrentTextItem->Text!=ui->plainTextEdit->toPlainText())        ui->plainTextEdit->setPlainText(CurrentTextItem->Text);
    if (CurrentTextItem->FontName!=ui->fontStyleCB->currentText())      ui->fontStyleCB->setCurrentIndex(ui->fontStyleCB->findText(QString(CurrentTextItem->FontName)));
    if (CurrentTextItem->StyleText!=ui->fontEffectCB->currentIndex())   ui->fontEffectCB->setCurrentIndex(CurrentTextItem->StyleText);
    ui->textLeft->setChecked(CurrentTextItem->HAlign==0);               ui->textLeft->setDown(CurrentTextItem->HAlign==0);
    ui->textCenter->setChecked(CurrentTextItem->HAlign==1);             ui->textCenter->setDown(CurrentTextItem->HAlign==1);
    ui->textJustif->setChecked(CurrentTextItem->HAlign==3);             ui->textJustif->setDown(CurrentTextItem->HAlign==3);
    ui->textRight->setChecked(CurrentTextItem->HAlign==2);              ui->textRight->setDown(CurrentTextItem->HAlign==2);
    ui->textUp->setChecked(CurrentTextItem->VAlign==0);                 ui->textUp->setDown(CurrentTextItem->VAlign==0);
    ui->textVCenter->setChecked(CurrentTextItem->VAlign==1);            ui->textVCenter->setDown(CurrentTextItem->VAlign==1);
    ui->textBottom->setChecked(CurrentTextItem->VAlign==2);             ui->textBottom->setDown(CurrentTextItem->VAlign==2);
    ui->bold->setChecked(CurrentTextItem->IsBold);                      ui->bold->setDown(CurrentTextItem->IsBold);
    ui->Italic->setChecked(CurrentTextItem->IsItalic);                  ui->Italic->setDown(CurrentTextItem->IsItalic);
    ui->Souligne->setChecked(CurrentTextItem->IsUnderline);             ui->Souligne->setDown(CurrentTextItem->IsUnderline);
    ui->FontColorCombo->SetCurrentColor(&CurrentTextItem->FontColor);
    ui->StyleShadowColorCombo->SetCurrentColor(&CurrentTextItem->FontShadowColor);
    ui->StyleShadowColorCombo->setEnabled(CurrentTextItem->StyleText!=0);
    ui->fontEffectCB->view()->setFixedWidth(250);

    // Brush TAB part
    bool Allow_Brush  =(CurrentTextItem->BackgroundBrush->BrushType!=BRUSHTYPE_IMAGEDISK);
    bool Allow_Color1 =(Allow_Brush)&&((CurrentTextItem->BackgroundBrush->BrushType==BRUSHTYPE_SOLID)||(CurrentTextItem->BackgroundBrush->BrushType==BRUSHTYPE_PATTERN)||(CurrentTextItem->BackgroundBrush->BrushType==BRUSHTYPE_GRADIENT2)||(CurrentTextItem->BackgroundBrush->BrushType==BRUSHTYPE_GRADIENT3));
    bool Allow_Color2 =(Allow_Brush)&&((CurrentTextItem->BackgroundBrush->BrushType==BRUSHTYPE_GRADIENT2)||(CurrentTextItem->BackgroundBrush->BrushType==BRUSHTYPE_GRADIENT3));
    bool Allow_Color3 =(Allow_Brush)&&(CurrentTextItem->BackgroundBrush->BrushType==BRUSHTYPE_GRADIENT3);
    bool Allow_Pattern=(Allow_Brush)&&(CurrentTextItem->BackgroundBrush->BrushType==BRUSHTYPE_PATTERN);
    bool Allow_Library=(Allow_Brush)&&(CurrentTextItem->BackgroundBrush->BrushType==BRUSHTYPE_IMAGELIBRARY);

    ui->BackgroundLabel->setVisible(Allow_Brush);
    ui->BackgroundStyleBT->setVisible(Allow_Brush);
    ui->BackgroundStyleED->setVisible(Allow_Brush);
    if (Allow_Brush) ui->BackgroundStyleED->setText(GlobalMainWindow->ApplicationConfig->StyleTextBackgroundCollection.GetStyleName(CurrentTextItem->GetBackgroundStyle()));
    ui->BrushTypeLabel->setVisible(Allow_Brush);
    ui->BrushTypeCombo->setVisible(Allow_Brush);
    ui->ColorLabel1->setVisible(Allow_Color1);
    ui->ColorLabel2->setVisible(Allow_Color1);
    ui->FirstColorCombo->setVisible(Allow_Color1);
    ui->FinalColorCombo->setVisible(Allow_Color2);
    ui->IntermColorCombo->setVisible(Allow_Color3);
    ui->OrientationSpacer->setVisible(Allow_Color2 & !Allow_Color3);
    ui->OrientationCombo->setVisible(Allow_Color2);
    ui->IntermPosLabel->setVisible(Allow_Color2);
    ui->IntermPosSlider->setVisible(Allow_Color3);
    ui->IntermPosED->setVisible(Allow_Color3);
    ui->PatternLabel->setVisible(Allow_Pattern);
    ui->PatternBrushCombo->setVisible(Allow_Pattern);
    ui->ImageLibraryLabel->setVisible(Allow_Library);
    ui->ImageLibraryLabel2->setVisible(Allow_Library);
    ui->BackgroundCombo->setVisible(Allow_Library);

    // Set brush type combo index
    for (int i=0;i<ui->BrushTypeCombo->count();i++) if (ui->BrushTypeCombo->itemData(i).toInt()==CurrentTextItem->BackgroundBrush->BrushType) ui->BrushTypeCombo->setCurrentIndex(i);
    ui->PatternBrushCombo->SetCurrentBrush(CurrentTextItem->BackgroundBrush);
    ui->FirstColorCombo->SetCurrentColor(&CurrentTextItem->BackgroundBrush->ColorD);
    ui->IntermColorCombo->SetCurrentColor(&CurrentTextItem->BackgroundBrush->ColorIntermed);
    ui->FinalColorCombo->SetCurrentColor(&CurrentTextItem->BackgroundBrush->ColorF);
    ui->OrientationCombo->SetCurrentBrush(CurrentTextItem->BackgroundBrush);
    ui->FirstColorCombo->SetCurrentColor(&CurrentTextItem->BackgroundBrush->ColorD);

    // Set controls depending on brush type
    switch (CurrentTextItem->BackgroundBrush->BrushType) {
        case BRUSHTYPE_NOBRUSH :
            break;
        case BRUSHTYPE_PATTERN :
        case BRUSHTYPE_SOLID :          break;
            break;
        case BRUSHTYPE_GRADIENT3 :
        case BRUSHTYPE_GRADIENT2 :
            ui->IntermPosSlider->setValue(CurrentTextItem->BackgroundBrush->Intermediate*100);
            ui->IntermPosED->setValue(CurrentTextItem->BackgroundBrush->Intermediate*100);
            break;
        case BRUSHTYPE_IMAGELIBRARY :
            // Ensure BrushImage is valide
            if ((BackgroundList.SearchImage(CurrentTextItem->BackgroundBrush->BrushImage)==-1)&&(BackgroundList.List.count()>0)) CurrentTextItem->BackgroundBrush->BrushImage=BackgroundList.List[0].Name;
            ui->BackgroundCombo->SetCurrentBackground(CurrentTextItem->BackgroundBrush->BrushImage);
            break;
    }

    StopMAJSpinbox=false;
    emit RefreshDisplay();
}

void DlgTextEdit::MakeTextStyleIcon(QComboBox *UICB) {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgTextEdit::MakeTextStyleIcon";
    #endif
    for (int i=0;i<UICB->count();i++) {
        cCompositionObject Object(COMPOSITIONTYPE_BACKGROUND,0,BaseApplicationConfig);
        Object.Text="T";
        Object.x=0.15;
        Object.y=0.15;
        Object.w=0.7;
        Object.h=0.7;
        Object.HAlign           =1;                 // Center
        Object.VAlign           =1;                 // Center
        Object.FontColor        ="#00ff00";
        Object.FontShadowColor  ="#ff0000";
        Object.StyleText        =i;
        Object.FontSize         =200;
        Object.IsBold           =true;
        Object.PenSize          =0;
        Object.BackgroundForm   =1;
        Object.Opacity=0;
        QPixmap  Image(32,32);
        QPainter Painter;
        Painter.begin(&Image);
        Painter.fillRect(QRect(0,0,32,32),"#ffffff");
        Object.DrawCompositionObject(&Painter,1,0,0,32,32,true,0,0,NULL,1,NULL,false);
        Painter.end();
        UICB->setItemIcon(i,QIcon(Image));
    }
}

//====================================================================================================================
// Handler for text controls
//====================================================================================================================

//========= Text style bold
void DlgTextEdit::s_SetBold() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgTextEdit::s_SetBold";
    #endif
    if (StopMAJSpinbox) return;
    if (CurrentTextItem->IsBold==true) CurrentTextItem->IsBold=false; else CurrentTextItem->IsBold=true;
    RefreshControls();
}

//========= Text style italic
void DlgTextEdit::s_SetItalic() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgTextEdit::s_SetItalic";
    #endif
    if (StopMAJSpinbox) return;
    if (CurrentTextItem->IsItalic==true) CurrentTextItem->IsItalic=false; else CurrentTextItem->IsItalic=true;
    RefreshControls();
}

//========= Text style underline
void DlgTextEdit::s_SetUnderline() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgTextEdit::s_SetUnderline";
    #endif
    if (StopMAJSpinbox) return;
    if (CurrentTextItem->IsUnderline==true) CurrentTextItem->IsUnderline=false; else CurrentTextItem->IsUnderline=true;
    RefreshControls();
}

//========= Horizontal alignment left
void DlgTextEdit::s_SetTextLeft() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgTextEdit::s_SetTextLeft";
    #endif
    if (StopMAJSpinbox) return;
    CurrentTextItem->HAlign=0;
    RefreshControls();
}

//========= Horizontal alignment center
void DlgTextEdit::s_SetTextCenter() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgTextEdit::s_SetTextCenter";
    #endif
    if (StopMAJSpinbox) return;
    CurrentTextItem->HAlign=1;
    RefreshControls();
}

//========= Horizontal alignment right
void DlgTextEdit::s_SetTextRight() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgTextEdit::s_SetTextRight";
    #endif
    if (StopMAJSpinbox) return;
    CurrentTextItem->HAlign=2;
    RefreshControls();
}

//========= Horizontal alignment justify
void DlgTextEdit::s_SetTextJustif() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgTextEdit::s_SetTextJustif";
    #endif
    if (StopMAJSpinbox) return;
    CurrentTextItem->HAlign=3;
    RefreshControls();
}

//========= Vertical alignment up
void DlgTextEdit::s_SetTextUp() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgTextEdit::s_SetTextUp";
    #endif
    if (StopMAJSpinbox) return;
    CurrentTextItem->VAlign=0;
    RefreshControls();
}

//========= Vertical alignment center
void DlgTextEdit::s_SetTextVCenter() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgTextEdit::s_SetTextVCenter";
    #endif
    if (StopMAJSpinbox) return;
    CurrentTextItem->VAlign=1;
    RefreshControls();
}

//========= Vertical alignment bottom
void DlgTextEdit::s_SetTextBottom() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgTextEdit::s_SetTextBottom";
    #endif
    if (StopMAJSpinbox) return;
    CurrentTextItem->VAlign=2;
    RefreshControls();
}

//========= Font family
void DlgTextEdit::s_ChangeFont(QFont font) {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgTextEdit::s_ChangeFont";
    #endif
    if (StopMAJSpinbox) return;
    if (font.family()!="") CurrentTextItem->FontName=font.family();
    RefreshControls();
}

//========= Size font
void DlgTextEdit::s_ChangeSizeFont(QString size) {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgTextEdit::s_ChangeSizeFont";
    #endif
    if (StopMAJSpinbox) return;
    if (size!="") CurrentTextItem->FontSize=size.toInt();
    RefreshControls();
}

//========= Style font
void DlgTextEdit::s_ChangeStyleFont(int Style) {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgTextEdit::s_ChangeStyleFont";
    #endif
    if (StopMAJSpinbox) return;
    CurrentTextItem->StyleText=Style;
    RefreshControls();
}

//========= Plain text edit
void DlgTextEdit::s_plainTextEditChange() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgTextEdit::s_plainTextEditChange";
    #endif
    if (StopMAJSpinbox) return;
    CurrentTextItem->Text=ui->plainTextEdit->toPlainText();
    RefreshControls();
}

//========= Font color
void DlgTextEdit::s_ChIndexFontColorCombo(int) {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgTextEdit::s_ChIndexFontColorCombo";
    #endif
    if (StopMAJSpinbox) return;
    CurrentTextItem->FontColor=ui->FontColorCombo->GetCurrentColor();
    RefreshControls();
}

//========= Text shadow color
void DlgTextEdit::s_ChIndexFontShadowColorCombo(int) {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgTextEdit::s_ChIndexFontShadowColorCombo";
    #endif
    if (StopMAJSpinbox) return;
    CurrentTextItem->FontShadowColor=ui->StyleShadowColorCombo->GetCurrentColor();
    RefreshControls();
}

//====================================================================================================================

void DlgTextEdit::s_ChangeBrushTypeCombo(int Value) {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgTextEdit::s_ChangeBrushTypeCombo";
    #endif
    if (StopMAJSpinbox) return;
    CurrentTextItem->BackgroundBrush->BrushType=ui->BrushTypeCombo->itemData(Value).toInt();
    RefreshControls();
}

//====================================================================================================================

void DlgTextEdit::s_IntermPosSliderMoved(int Value) {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgTextEdit::s_IntermPosSliderMoved";
    #endif
    if (StopMAJSpinbox) return;
    CurrentTextItem->BackgroundBrush->Intermediate=double(Value)/100;
    RefreshControls();
}

//====================================================================================================================

void DlgTextEdit::s_IntermPosED(int Value) {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgTextEdit::s_IntermPosED";
    #endif
    if (StopMAJSpinbox) return;
    CurrentTextItem->BackgroundBrush->Intermediate=double(Value)/100;
    RefreshControls();
}

//====================================================================================================================
// Handler for custom color/brush/pattern/gradient combo box index change
//====================================================================================================================

//========= Pattern shape combo
void DlgTextEdit::s_ChIndexPatternBrushCombo(int) {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgTextEdit::s_ChIndexPatternBrushCombo";
    #endif
    if (StopMAJSpinbox) return;
    CurrentTextItem->BackgroundBrush->PatternType=ui->PatternBrushCombo->GetCurrentBrush()->PatternType;
    RefreshControls();
}

//========= Gradient shape orientation
void DlgTextEdit::s_ChIndexGradientOrientationCombo(int) {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgTextEdit::s_ChIndexGradientOrientationCombo";
    #endif
    if (StopMAJSpinbox) return;
    CurrentTextItem->BackgroundBrush->GradientOrientation=ui->OrientationCombo->GetCurrentBrush()->GradientOrientation;
    RefreshControls();
}

//========= Shape/Gradient shape first color
void DlgTextEdit::s_ChIndexGradientFirstColorCombo(int) {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgTextEdit::s_ChIndexGradientFirstColorCombo";
    #endif
    if (StopMAJSpinbox) return;
    CurrentTextItem->BackgroundBrush->ColorD=ui->FirstColorCombo->GetCurrentColor();
    RefreshControls();
}

//========= Gradient shape last color
void DlgTextEdit::s_ChIndexGradientFinalColorCombo(int) {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgTextEdit::s_ChIndexGradientFinalColorCombo";
    #endif
    if (StopMAJSpinbox) return;
    CurrentTextItem->BackgroundBrush->ColorF=ui->FinalColorCombo->GetCurrentColor();
    RefreshControls();
}

//========= Gradient shape intermediate color
void DlgTextEdit::s_ChIndexGradientIntermColorCombo(int) {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgTextEdit::s_ChIndexGradientIntermColorCombo";
    #endif
    if (StopMAJSpinbox) return;
    CurrentTextItem->BackgroundBrush->ColorIntermed=ui->IntermColorCombo->GetCurrentColor();
    RefreshControls();
}

//========= Background image
void DlgTextEdit::s_ChIndexBackgroundCombo(int) {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgTextEdit::s_ChIndexBackgroundCombo";
    #endif
    if (StopMAJSpinbox) return;
    CurrentTextItem->BackgroundBrush->BrushImage=ui->BackgroundCombo->GetCurrentBackground();
    RefreshControls();
}

//====================================================================================================================
// Handler for style sheet management
//====================================================================================================================

void DlgTextEdit::s_TextStyleBT() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgTextEdit::s_TextStyleBT";
    #endif
    QString ActualStyle=CurrentTextItem->GetTextStyle();
    QString Item=GlobalMainWindow->ApplicationConfig->StyleTextCollection.PopupCollectionMenu(this,ActualStyle);
    ui->TextStyleBT->setDown(false);
    if (Item!="") CurrentTextItem->ApplyTextStyle(GlobalMainWindow->ApplicationConfig->StyleTextCollection.GetStyleDef(Item));
    RefreshControls();
}

void DlgTextEdit::s_BackgroundStyleBT() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgTextEdit::s_BackgroundStyleBT";
    #endif
    QString ActualStyle=CurrentTextItem->GetBackgroundStyle();
    QString Item=GlobalMainWindow->ApplicationConfig->StyleTextBackgroundCollection.PopupCollectionMenu(this,ActualStyle);
    ui->BackgroundStyleBT->setDown(false);
    if (Item!="") CurrentTextItem->ApplyBackgroundStyle(GlobalMainWindow->ApplicationConfig->StyleTextBackgroundCollection.GetStyleDef(Item));
    RefreshControls();
}
