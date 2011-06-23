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

#include "mainwindow.h"
#include "DlgTextEdit.h"
#include "ui_DlgTextEdit.h"
#include "DlgSlideProperties.h"

DlgTextEdit::DlgTextEdit(cCompositionObject *TheCurrentTextItem,QWidget *parent):QDialog(parent),ui(new Ui::DlgTextEdit) {
    ui->setupUi(this);
    CurrentTextItem =TheCurrentTextItem;
    ParentWindow    =parent;

    // Save object before modification for cancel button
    Undo=new QDomDocument(APPLICATION_NAME);
    QDomElement root=Undo->createElement("UNDO-DLG");       // Create xml document and root
    CurrentTextItem->SaveToXML(root,"UNDO-DLG-OBJECT",QFileInfo(GlobalMainWindow->Diaporama->ProjectFileName).absolutePath(),true);  // Save object
    Undo->appendChild(root);                                // Add object to xml document

    StopMAJSpinbox =false;
    IsFirstInitDone=false;

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
    ui->fontEffectCB->addItem(QCoreApplication::translate("DlgTextEdit","No effect"));
    ui->fontEffectCB->addItem(QCoreApplication::translate("DlgTextEdit","Outerline"));
    ui->fontEffectCB->addItem(QCoreApplication::translate("DlgTextEdit","Shadow upper left"));
    ui->fontEffectCB->addItem(QCoreApplication::translate("DlgTextEdit","Shadow upper right"));
    ui->fontEffectCB->addItem(QCoreApplication::translate("DlgTextEdit","Shadow bottom left"));
    ui->fontEffectCB->addItem(QCoreApplication::translate("DlgTextEdit","Shadow bottom right"));
    ui->fontEffectCB->view()->setFixedWidth(160+18);
    ui->fontEffectCB->view()->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    MakeTextStyleIcon(ui->fontEffectCB);

    RefreshControls();

    // Define handler
    connect(ui->CloseBT,SIGNAL(clicked()),this,SLOT(reject()));
    connect(ui->OKBT,SIGNAL(clicked()),this,SLOT(accept()));
    connect(ui->HelpBT,SIGNAL(clicked()),this,SLOT(Help()));
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
}

//====================================================================================================================

DlgTextEdit::~DlgTextEdit() {
    delete ui;
    delete Undo;
}

//====================================================================================================================

void DlgTextEdit::reject() {
    // Save Window size and position
    GlobalMainWindow->ApplicationConfig->DlgTextEditWSP->SaveWindowState(this);
    QDomElement root=Undo->documentElement();
    if (root.tagName()=="UNDO-DLG") CurrentTextItem->LoadFromXML(root,"UNDO-DLG-OBJECT","",NULL);
    done(1);
}

//====================================================================================================================

void DlgTextEdit::accept() {
    // Save Window size and position
    GlobalMainWindow->ApplicationConfig->DlgTextEditWSP->SaveWindowState(this);
    // Close the box
    done(0);
}

//====================================================================================================================

void DlgTextEdit::Help() {
    GlobalMainWindow->OpenHelp(HELPFILE_DlgTextEdit);
}

//====================================================================================================================

void DlgTextEdit::resizeEvent(QResizeEvent *) {
}

//====================================================================================================================

void DlgTextEdit::SetSavedWindowGeometry() {
    GlobalMainWindow->ApplicationConfig->DlgTextEditWSP->ApplyToWindow(this);
}

//====================================================================================================================

void DlgTextEdit::showEvent(QShowEvent *ev) {
    QDialog::showEvent(ev);
    if (IsFirstInitDone) return;
    QTimer::singleShot(0,this,SLOT(SetSavedWindowGeometry()));
    IsFirstInitDone=true;                                   // Set this flag to true to indicate that now we can prepeare display
}

//====================================================================================================================

void DlgTextEdit::RefreshControls() {
    StopMAJSpinbox=true;
    // Update text controls
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
    StopMAJSpinbox=false;
    ((DlgSlideProperties *)ParentWindow)->RefreshSceneImage();
}

void DlgTextEdit::MakeTextStyleIcon(QComboBox *UICB) {
    for (int i=0;i<UICB->count();i++) {
        cCompositionObject Object(COMPOSITIONTYPE_BACKGROUND,0);
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
        Object.DrawCompositionObject(&Painter,0,0,32,32,true,0,0,NULL,1,NULL,false);
        Painter.end();
        UICB->setItemIcon(i,QIcon(Image));
    }
}

//====================================================================================================================
// Handler for text controls
//====================================================================================================================

//========= Text style bold
void DlgTextEdit::s_SetBold() {
    if (StopMAJSpinbox) return;
    if (CurrentTextItem->IsBold==true) CurrentTextItem->IsBold=false; else CurrentTextItem->IsBold=true;
    RefreshControls();
}

//========= Text style italic
void DlgTextEdit::s_SetItalic() {
    if (StopMAJSpinbox) return;
    if (CurrentTextItem->IsItalic==true) CurrentTextItem->IsItalic=false; else CurrentTextItem->IsItalic=true;
    RefreshControls();
}

//========= Text style underline
void DlgTextEdit::s_SetUnderline() {
    if (StopMAJSpinbox) return;
    if (CurrentTextItem->IsUnderline==true) CurrentTextItem->IsUnderline=false; else CurrentTextItem->IsUnderline=true;
    RefreshControls();
}

//========= Horizontal alignment left
void DlgTextEdit::s_SetTextLeft() {
    if (StopMAJSpinbox) return;
    CurrentTextItem->HAlign=0;
    RefreshControls();
}

//========= Horizontal alignment center
void DlgTextEdit::s_SetTextCenter() {
    if (StopMAJSpinbox) return;
    CurrentTextItem->HAlign=1;
    RefreshControls();
}

//========= Horizontal alignment right
void DlgTextEdit::s_SetTextRight() {
    if (StopMAJSpinbox) return;
    CurrentTextItem->HAlign=2;
    RefreshControls();
}

//========= Horizontal alignment justify
void DlgTextEdit::s_SetTextJustif() {
    if (StopMAJSpinbox) return;
    CurrentTextItem->HAlign=3;
    RefreshControls();
}

//========= Vertical alignment up
void DlgTextEdit::s_SetTextUp() {
    if (StopMAJSpinbox) return;
    CurrentTextItem->VAlign=0;
    RefreshControls();
}

//========= Vertical alignment center
void DlgTextEdit::s_SetTextVCenter() {
    if (StopMAJSpinbox) return;
    CurrentTextItem->VAlign=1;
    RefreshControls();
}

//========= Vertical alignment bottom
void DlgTextEdit::s_SetTextBottom() {
    if (StopMAJSpinbox) return;
    CurrentTextItem->VAlign=2;
    RefreshControls();
}

//========= Font family
void DlgTextEdit::s_ChangeFont(QFont font) {
    if (StopMAJSpinbox) return;
    if (font.family()!="") CurrentTextItem->FontName=font.family();
    RefreshControls();
}

//========= Size font
void DlgTextEdit::s_ChangeSizeFont(QString size) {
    if (StopMAJSpinbox) return;
    if (size!="") CurrentTextItem->FontSize=size.toInt();
    RefreshControls();
}

//========= Style font
void DlgTextEdit::s_ChangeStyleFont(int Style) {
    if (StopMAJSpinbox) return;
    CurrentTextItem->StyleText=Style;
    RefreshControls();
}

//========= Plain text edit
void DlgTextEdit::s_plainTextEditChange() {
    if (StopMAJSpinbox) return;
    CurrentTextItem->Text=ui->plainTextEdit->toPlainText();
    RefreshControls();
}

//========= Font color
void DlgTextEdit::s_ChIndexFontColorCombo(int) {
    if (StopMAJSpinbox) return;
    CurrentTextItem->FontColor=ui->FontColorCombo->GetCurrentColor();
    RefreshControls();
}

//========= Text shadow color
void DlgTextEdit::s_ChIndexFontShadowColorCombo(int) {
    if (StopMAJSpinbox) return;
    CurrentTextItem->FontShadowColor=ui->StyleShadowColorCombo->GetCurrentColor();
    RefreshControls();
}
