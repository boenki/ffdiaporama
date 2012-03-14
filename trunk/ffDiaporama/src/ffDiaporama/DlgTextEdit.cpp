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

#include "DlgTextEdit.h"
#include "ui_DlgTextEdit.h"

#include <QTextCharFormat>
#include <QTextList>
#include <QAbstractTextDocumentLayout>

#define SIZERATIO   0.5

DlgTextEdit::DlgTextEdit(cCompositionObject *TheCurrentTextItem,QString HelpURL,cBaseApplicationConfig *ApplicationConfig,cSaveWindowPosition *DlgWSP,
                         cStyleCollection *TheStyleTextCollection,cStyleCollection *TheStyleTextBackgroundCollection,QWidget *parent):
    QCustomDialog(HelpURL,ApplicationConfig,DlgWSP,parent),ui(new Ui::DlgTextEdit) {

    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgTextEdit::DlgTextEdit");

    ui->setupUi(this);
    OkBt                            =ui->OKBT;
    CancelBt                        =ui->CancelBt;
    HelpBt                          =ui->HelpBT;
    CurrentTextItem                 =TheCurrentTextItem;
    StyleTextCollection             =TheStyleTextCollection;
    StyleTextBackgroundCollection   =TheStyleTextBackgroundCollection;
    StopMAJSpinbox                  =false;
    IsFirstInit                     =false;
}

//====================================================================================================================

DlgTextEdit::~DlgTextEdit() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgTextEdit::~DlgTextEdit");

    delete ui;
}

//====================================================================================================================
// Initialise dialog

void DlgTextEdit::DoInitDialog() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgTextEdit::DoInitDialog");

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
    ui->TextEdit->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    ui->TextEdit->setLineWrapMode(QTextEdit::WidgetWidth);
    ui->TextEdit->setAutoFillBackground(true);
    ui->TextEdit->setHtml(CurrentTextItem->Text);
    QTextCursor Cursor=ui->TextEdit->textCursor();
    Cursor.movePosition(QTextCursor::Start);
    ui->TextEdit->setTextCursor(Cursor);

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

    // Define handler
    connect(ui->TextEdit,SIGNAL(textChanged()),this,SLOT(s_TextEditChange()));
    connect(ui->TextEdit,SIGNAL(cursorPositionChanged()),this,SLOT(s_cursorPositionChanged()));

    connect(ui->FontColorCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChIndexFontColorCombo(int)));
    connect(ui->StyleShadowColorCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChIndexFontShadowColorCombo(int)));
    connect(ui->fontStyleCB,SIGNAL(currentFontChanged(QFont)),this,SLOT(s_ChangeFont(QFont)));
    connect(ui->fontSize,SIGNAL(currentIndexChanged(QString)),this,SLOT(s_ChangeSizeFont(QString)));

    connect(ui->bold,SIGNAL(released()),this,SLOT(s_SetBold()));
    connect(ui->Italic,SIGNAL(released()),this,SLOT(s_SetItalic()));
    connect(ui->Souligne,SIGNAL(released()),this,SLOT(s_SetUnderline()));
    connect(ui->TextSuperBt,SIGNAL(released()),this,SLOT(s_SetTextSuper()));
    connect(ui->TextSubBt,SIGNAL(released()),this,SLOT(s_SetTextSub()));

    connect(ui->TextStyleBT,SIGNAL(pressed()),this,SLOT(s_TextStyleBT()));

    connect(ui->textLeft,SIGNAL(pressed()),this,SLOT(s_SetTextLeft()));
    connect(ui->textCenter,SIGNAL(pressed()),this,SLOT(s_SetTextCenter()));
    connect(ui->textRight,SIGNAL(pressed()),this,SLOT(s_SetTextRight()));
    connect(ui->textJustif,SIGNAL(pressed()),this,SLOT(s_SetTextJustif()));

    connect(ui->Souligne,SIGNAL(released()),this,SLOT(s_SetUnderline()));
    connect(ui->Souligne,SIGNAL(released()),this,SLOT(s_SetUnderline()));

    connect(ui->IndentInBt,SIGNAL(pressed()),this,SLOT(s_IndentInBt()));
    connect(ui->IndentOutBt,SIGNAL(pressed()),this,SLOT(s_IndentOutBt()));

    connect(ui->ListBt,SIGNAL(pressed()),this,SLOT(s_ListBt()));
    connect(ui->ListNbrBt,SIGNAL(pressed()),this,SLOT(s_ListNbrBt()));

    // Block part
    connect(ui->textUp,SIGNAL(pressed()),this,SLOT(s_SetTextUp()));
    connect(ui->textVCenter,SIGNAL(pressed()),this,SLOT(s_SetTextVCenter()));
    connect(ui->textBottom,SIGNAL(pressed()),this,SLOT(s_SetTextBottom()));
    connect(ui->fontEffectCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChangeStyleFont(int)));

    // Brush part
    connect(ui->BrushTypeCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChangeBrushTypeCombo(int)));
    connect(ui->PatternBrushCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChIndexPatternBrushCombo(int)));
    connect(ui->OrientationCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChIndexGradientOrientationCombo(int)));
    connect(ui->FirstColorCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChIndexGradientFirstColorCombo(int)));
    connect(ui->FinalColorCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChIndexGradientFinalColorCombo(int)));
    connect(ui->IntermColorCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChIndexGradientIntermColorCombo(int)));
    connect(ui->BackgroundCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChIndexBackgroundCombo(int)));
    connect(ui->IntermPosSlider,SIGNAL(valueChanged(int)),this,SLOT(s_IntermPosED(int)));
    connect(ui->IntermPosED,SIGNAL(valueChanged(int)),this,SLOT(s_IntermPosED(int)));

    RefreshControls();
}

//====================================================================================================================
// Initiale Undo

void DlgTextEdit::PrepareGlobalUndo() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgTextEdit::PrepareGlobalUndo");

    // Save object before modification for cancel button
    Undo=new QDomDocument(APPLICATION_NAME);
    QDomElement root=Undo->createElement("UNDO-DLG");       // Create xml document and root
    CurrentTextItem->SaveToXML(root,"UNDO-DLG-OBJECT","",NULL,false);  // Save object
    Undo->appendChild(root);                                // Add object to xml document
}

//====================================================================================================================
// Apply Undo : call when user click on Cancel button

void DlgTextEdit::DoGlobalUndo() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgTextEdit::DoGlobalUndo");

    // Restore element
    QDomElement root=Undo->documentElement();
    if (root.tagName()=="UNDO-DLG") CurrentTextItem->LoadFromXML(root,"UNDO-DLG-OBJECT","",NULL,NULL,false);
}

//====================================================================================================================

void DlgTextEdit::s_cursorPositionChanged() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgTextEdit::s_cursorPositionChanged");

    StopMAJSpinbox=true;

    QTextCursor      Cursor(ui->TextEdit->textCursor());
    QTextCharFormat  TCF;
    Qt::Alignment    Alignment;
    QTextBlockFormat TBF=Cursor.blockFormat();
    QTextList        *List=Cursor.currentList();

    if (Cursor.hasSelection())  TCF=Cursor.charFormat();                // Display option for current selection
        else                    TCF=ui->TextEdit->currentCharFormat();  // Display current default option

    Alignment=ui->TextEdit->alignment();
    CurrentTextItem->FontColor=TCF.foreground().color().name();

    ui->bold->    setChecked(TCF.fontWeight()==QFont::Bold);            ui->bold->    setDown(TCF.fontWeight()==QFont::Bold);
    ui->Italic->  setChecked(TCF.fontItalic());                         ui->Italic->  setDown(TCF.fontItalic());
    ui->Souligne->setChecked(TCF.fontUnderline());                      ui->Souligne->setDown(TCF.fontUnderline());

    ui->fontStyleCB->   setCurrentIndex(ui->fontStyleCB->findText(QString(TCF.fontFamily())));
    ui->fontSize->      setCurrentIndex(ui->fontSize->findText(QString("%1").arg(TCF.fontPointSize())));
    ui->FontColorCombo->SetCurrentColor(&CurrentTextItem->FontColor);

    ui->textLeft->  setChecked((Alignment & Qt::AlignLeft)!=0);         ui->textLeft->  setDown((Alignment & Qt::AlignLeft)!=0);
    ui->textCenter->setChecked((Alignment & Qt::AlignHCenter)!=0);      ui->textCenter->setDown((Alignment & Qt::AlignHCenter)!=0);
    ui->textJustif->setChecked((Alignment & Qt::AlignJustify)!=0);      ui->textJustif->setDown((Alignment & Qt::AlignJustify)!=0);
    ui->textRight-> setChecked((Alignment & Qt::AlignRight)!=0);        ui->textRight-> setDown((Alignment & Qt::AlignRight)!=0);

    ui->TextSuperBt->setChecked(TCF.verticalAlignment()==QTextCharFormat::AlignSuperScript);
    ui->TextSuperBt->setDown(TCF.verticalAlignment()==QTextCharFormat::AlignSuperScript);
    ui->TextSubBt->  setChecked(TCF.verticalAlignment()==QTextCharFormat::AlignSubScript);
    ui->TextSubBt->  setDown(TCF.verticalAlignment()==QTextCharFormat::AlignSubScript);

    int indent=0;
    if (List) indent=List->format().indent(); else indent=TBF.indent();
    ui->IndentInBt->setEnabled(indent<9);
    ui->IndentOutBt->setEnabled(indent>(List!=NULL?1:0));

    StopMAJSpinbox=false;
}

//====================================================================================================================

void DlgTextEdit::RefreshControls() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgTextEdit::RefreshControls");

    // Update text controls
    s_cursorPositionChanged();

    StopMAJSpinbox=true;

    ui->textUp->setChecked(CurrentTextItem->VAlign==0);                 ui->textUp->setDown(CurrentTextItem->VAlign==0);
    ui->textVCenter->setChecked(CurrentTextItem->VAlign==1);            ui->textVCenter->setDown(CurrentTextItem->VAlign==1);
    ui->textBottom->setChecked(CurrentTextItem->VAlign==2);             ui->textBottom->setDown(CurrentTextItem->VAlign==2);
    if (CurrentTextItem->StyleText!=ui->fontEffectCB->currentIndex())   ui->fontEffectCB->setCurrentIndex(CurrentTextItem->StyleText);
    ui->StyleShadowColorCombo->SetCurrentColor(&CurrentTextItem->FontShadowColor);
    ui->StyleShadowColorCombo->setEnabled(CurrentTextItem->StyleText!=0);
    ui->fontEffectCB->view()->setFixedWidth(250);
    //ui->TextStyleED->setText(StyleTextCollection->GetStyleName(CurrentTextItem->GetTextStyle()));

    // Brush TAB part
    bool Allow_Brush  =(CurrentTextItem->BackgroundBrush->BrushType!=BRUSHTYPE_IMAGEDISK);
    bool Allow_Color1 =(Allow_Brush)&&((CurrentTextItem->BackgroundBrush->BrushType==BRUSHTYPE_SOLID)||(CurrentTextItem->BackgroundBrush->BrushType==BRUSHTYPE_PATTERN)||(CurrentTextItem->BackgroundBrush->BrushType==BRUSHTYPE_GRADIENT2)||(CurrentTextItem->BackgroundBrush->BrushType==BRUSHTYPE_GRADIENT3));
    bool Allow_Color2 =(Allow_Brush)&&((CurrentTextItem->BackgroundBrush->BrushType==BRUSHTYPE_GRADIENT2)||(CurrentTextItem->BackgroundBrush->BrushType==BRUSHTYPE_GRADIENT3));
    bool Allow_Color3 =(Allow_Brush)&&(CurrentTextItem->BackgroundBrush->BrushType==BRUSHTYPE_GRADIENT3);
    bool Allow_Pattern=(Allow_Brush)&&(CurrentTextItem->BackgroundBrush->BrushType==BRUSHTYPE_PATTERN);
    bool Allow_Library=(Allow_Brush)&&(CurrentTextItem->BackgroundBrush->BrushType==BRUSHTYPE_IMAGELIBRARY);

    //ui->BackgroundLabel->setVisible(Allow_Brush);
    ui->BackgroundStyleBT->setVisible(Allow_Brush);
    ui->tabWidget->setTabEnabled(1,Allow_Brush);
    //ui->BackgroundStyleED->setVisible(Allow_Brush);
    //if (Allow_Brush) ui->BackgroundStyleED->setText(StyleTextBackgroundCollection->GetStyleName(CurrentTextItem->GetBackgroundStyle()));
    //ui->BrushTypeLabel->setVisible(Allow_Brush);
    ui->BrushTypeCombo->setVisible(Allow_Brush);
    ui->ColorLabel_1->setVisible(Allow_Color2);
    ui->ColorLabel_2->setVisible(Allow_Color3);
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

    // Apply background to ui->TextEdit
    QBrush   *Brush=NULL;
    if (CurrentTextItem->BackgroundBrush->BrushType==BRUSHTYPE_NOBRUSH) {
        Brush=new QBrush(Transparent);
    } else Brush=CurrentTextItem->BackgroundBrush->GetBrush(QRectF(0,0,ui->TextEdit->width(),ui->TextEdit->height()),true,0,0,NULL,1,NULL,false);
    QPalette Palette;
    Palette.setBrush(QPalette::Base,*Brush);
    ui->TextEdit->setPalette(Palette);
    delete Brush;

    StopMAJSpinbox=false;
    emit RefreshDisplay();
}

void DlgTextEdit::MakeTextStyleIcon(QComboBox *UICB) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgTextEdit::MakeTextStyleIcon");

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
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgTextEdit::s_SetBold");
    if (StopMAJSpinbox) return;

    QTextCursor     Cursor(ui->TextEdit->textCursor());
    QTextCharFormat TCF;
    if (Cursor.hasSelection())  TCF=Cursor.charFormat();                // Modify current selection
        else                    TCF=ui->TextEdit->currentCharFormat();  // Modify default option
    TCF.setFontWeight(TCF.fontWeight()==QFont::Normal?QFont::Bold:QFont::Normal);
    if (Cursor.hasSelection())  Cursor.setCharFormat(TCF);
        else                    ui->TextEdit->setCurrentCharFormat(TCF);

    CurrentTextItem->IsBold=(TCF.fontWeight()==QFont::Bold);
    CurrentTextItem->Text=ui->TextEdit->toHtml();
    RefreshControls();
    ui->TextEdit->setFocus();
}

//========= Text style italic
void DlgTextEdit::s_SetItalic() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgTextEdit::s_SetItalic");
    if (StopMAJSpinbox) return;

    QTextCursor     Cursor(ui->TextEdit->textCursor());
    QTextCharFormat TCF;
    if (Cursor.hasSelection())  TCF=Cursor.charFormat();                // Modify current selection
        else                    TCF=ui->TextEdit->currentCharFormat();  // Modify default option
    TCF.setFontItalic(!TCF.fontItalic());
    if (Cursor.hasSelection())  Cursor.setCharFormat(TCF);
        else                    ui->TextEdit->setCurrentCharFormat(TCF);

    CurrentTextItem->IsItalic=TCF.fontItalic();
    CurrentTextItem->Text=ui->TextEdit->toHtml();
    RefreshControls();
    ui->TextEdit->setFocus();
}

//========= Text style underline
void DlgTextEdit::s_SetUnderline() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgTextEdit::s_SetUnderline");
    if (StopMAJSpinbox) return;

    QTextCursor     Cursor(ui->TextEdit->textCursor());
    QTextCharFormat TCF;
    if (Cursor.hasSelection())  TCF=Cursor.charFormat();                // Modify current selection
        else                    TCF=ui->TextEdit->currentCharFormat();  // Modify default option
    TCF.setFontUnderline(!TCF.fontUnderline());
    if (Cursor.hasSelection())  Cursor.setCharFormat(TCF);
        else                    ui->TextEdit->setCurrentCharFormat(TCF);

    CurrentTextItem->IsUnderline=TCF.fontUnderline();
    CurrentTextItem->Text=ui->TextEdit->toHtml();
    RefreshControls();
    ui->TextEdit->setFocus();
}

//========= Text style to superscript
void DlgTextEdit::s_SetTextSuper() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgTextEdit::s_SetTextSuper");
    if (StopMAJSpinbox) return;

    QTextCursor     Cursor(ui->TextEdit->textCursor());
    QTextCharFormat TCF;
    if (Cursor.hasSelection())  TCF=Cursor.charFormat();                // Modify current selection
        else                    TCF=ui->TextEdit->currentCharFormat();  // Modify default option

    if (TCF.verticalAlignment()==QTextCharFormat::AlignSuperScript) TCF.setVerticalAlignment(QTextCharFormat::AlignNormal);
        else                                                        TCF.setVerticalAlignment(QTextCharFormat::AlignSuperScript);

    if (Cursor.hasSelection())  Cursor.setCharFormat(TCF);
        else                    ui->TextEdit->setCurrentCharFormat(TCF);

    CurrentTextItem->Text=ui->TextEdit->toHtml();
    RefreshControls();
    ui->TextEdit->setFocus();
}

//========= Text style to subscript
void DlgTextEdit::s_SetTextSub() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgTextEdit::s_SetTextSub");
    if (StopMAJSpinbox) return;

    QTextCursor     Cursor(ui->TextEdit->textCursor());
    QTextCharFormat TCF;
    if (Cursor.hasSelection())  TCF=Cursor.charFormat();                // Modify current selection
        else                    TCF=ui->TextEdit->currentCharFormat();  // Modify default option

    if (TCF.verticalAlignment()==QTextCharFormat::AlignSubScript)   TCF.setVerticalAlignment(QTextCharFormat::AlignNormal);
        else                                                        TCF.setVerticalAlignment(QTextCharFormat::AlignSubScript);

    if (Cursor.hasSelection())  Cursor.setCharFormat(TCF);
        else                    ui->TextEdit->setCurrentCharFormat(TCF);

    CurrentTextItem->IsUnderline=TCF.fontUnderline();
    CurrentTextItem->Text=ui->TextEdit->toHtml();
    RefreshControls();
    ui->TextEdit->setFocus();
}

//========= Horizontal alignment left
void DlgTextEdit::s_SetTextLeft() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgTextEdit::s_SetTextLeft");
    if (StopMAJSpinbox) return;
    ui->TextEdit->setAlignment((ui->TextEdit->alignment() & 0xf0)|Qt::AlignLeft);

    CurrentTextItem->HAlign=0;
    CurrentTextItem->Text=ui->TextEdit->toHtml();
    RefreshControls();
    ui->TextEdit->setFocus();
}

//========= Horizontal alignment center
void DlgTextEdit::s_SetTextCenter() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgTextEdit::s_SetTextCenter");
    if (StopMAJSpinbox) return;
    ui->TextEdit->setAlignment((ui->TextEdit->alignment() & 0xf0)|Qt::AlignHCenter);
    CurrentTextItem->HAlign=1;
    CurrentTextItem->Text=ui->TextEdit->toHtml();
    RefreshControls();
    ui->TextEdit->setFocus();
}

//========= Horizontal alignment right
void DlgTextEdit::s_SetTextRight() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgTextEdit::s_SetTextRight");
    if (StopMAJSpinbox) return;
    ui->TextEdit->setAlignment((ui->TextEdit->alignment() & 0xf0)|Qt::AlignRight);
    CurrentTextItem->HAlign=2;
    CurrentTextItem->Text=ui->TextEdit->toHtml();
    RefreshControls();
    ui->TextEdit->setFocus();
}

//========= Horizontal alignment justify
void DlgTextEdit::s_SetTextJustif() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgTextEdit::s_SetTextJustif");
    if (StopMAJSpinbox) return;
    ui->TextEdit->setAlignment((ui->TextEdit->alignment() & 0xf0)|Qt::AlignJustify);
    CurrentTextItem->HAlign=3;
    CurrentTextItem->Text=ui->TextEdit->toHtml();
    RefreshControls();
    ui->TextEdit->setFocus();
}

//========= Increase indent
void DlgTextEdit::s_IndentInBt() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgTextEdit::s_IndentInBt");
    if (StopMAJSpinbox) return;

    QTextCursor         Cursor(ui->TextEdit->textCursor());
    QTextBlockFormat    TBF  =Cursor.blockFormat();
    QTextList *List=Cursor.currentList();
    if (!List) {
        if (TBF.indent()<9) {
             QTextBlockFormat modifier;
             modifier.setIndent(TBF.indent()+1);
             Cursor.mergeBlockFormat(modifier);
        }
     } else {
        QTextListFormat format=List->format();
        if (format.indent()<9) {
            format.setIndent(format.indent()+1);
            if (List->itemNumber(Cursor.block())==1) List->setFormat(format);
                else Cursor.createList(format);
        }
     }

    CurrentTextItem->Text=ui->TextEdit->toHtml();
    RefreshControls();
    ui->TextEdit->setFocus();
}

//========= Decrease indent
void DlgTextEdit::s_IndentOutBt() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgTextEdit::s_IndentOutBt");
    if (StopMAJSpinbox) return;

    QTextCursor         Cursor(ui->TextEdit->textCursor());
    QTextBlockFormat    TBF  =Cursor.blockFormat();

    QTextList *List=Cursor.currentList();
    if (!List) {
        if (TBF.indent()>0) {
            QTextBlockFormat modifier;
            modifier.setIndent(TBF.indent()-1);
            Cursor.mergeBlockFormat(modifier);
        }
    } else {
        QTextListFormat ListFmt=List->format();
        if (ListFmt.indent()>0) {
            ListFmt.setIndent(ListFmt.indent()-1);
            List->setFormat(ListFmt);
        }
    }

    CurrentTextItem->Text=ui->TextEdit->toHtml();
    RefreshControls();
    ui->TextEdit->setFocus();
}

//========= Activate/deactivate bullet list
void DlgTextEdit::s_ListBt() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgTextEdit::s_ListBt");
    if (StopMAJSpinbox) return;

    QTextCursor Cursor(ui->TextEdit->textCursor());
    if (Cursor.currentList()!=NULL) {
        Cursor.currentList()->remove(Cursor.block());
        QTextBlockFormat TBF=Cursor.blockFormat();
        TBF.setIndent(0);
        Cursor.setBlockFormat(TBF);
    } else {
        Cursor.beginEditBlock();
        QTextBlockFormat BlockFmt=Cursor.blockFormat();
        QTextListFormat  ListFmt;
        ListFmt.setStyle(QTextListFormat::ListSquare);
        ListFmt.setIndent(BlockFmt.indent()+1);
        BlockFmt.setIndent(0);
        Cursor.mergeBlockFormat(BlockFmt);
        Cursor.createList(ListFmt);
        Cursor.endEditBlock();
    }

    CurrentTextItem->Text=ui->TextEdit->toHtml();
    RefreshControls();
    ui->TextEdit->setFocus();
}

//========= Activate/deactivate numbered list
void DlgTextEdit::s_ListNbrBt() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgTextEdit::s_ListNbrBt");
    if (StopMAJSpinbox) return;

    QTextCursor Cursor(ui->TextEdit->textCursor());
    if (Cursor.currentList()!=NULL) {
        Cursor.currentList()->remove(Cursor.block());
        QTextBlockFormat TBF=Cursor.blockFormat();
        TBF.setIndent(0);
        Cursor.setBlockFormat(TBF);
    } else {
        Cursor.beginEditBlock();
        QTextBlockFormat BlockFmt=Cursor.blockFormat();
        QTextListFormat  ListFmt;
        ListFmt.setStyle(QTextListFormat::ListDecimal);
        ListFmt.setIndent(BlockFmt.indent()+1);
        BlockFmt.setIndent(0);
        Cursor.setBlockFormat(BlockFmt);
        Cursor.createList(ListFmt);
        Cursor.endEditBlock();
    }

    CurrentTextItem->Text=ui->TextEdit->toHtml();
    RefreshControls();
    ui->TextEdit->setFocus();
}

//========= Vertical alignment up
void DlgTextEdit::s_SetTextUp() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgTextEdit::s_SetTextUp");

    if (StopMAJSpinbox) return;
    CurrentTextItem->VAlign=0;
    ui->TextEdit->setAlignment((ui->TextEdit->alignment() & 0x0f)|Qt::AlignTop);
    CurrentTextItem->Text=ui->TextEdit->toHtml();
    RefreshControls();
    ui->TextEdit->setFocus();
}

//========= Vertical alignment center
void DlgTextEdit::s_SetTextVCenter() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgTextEdit::s_SetTextVCenter");

    if (StopMAJSpinbox) return;
    CurrentTextItem->VAlign=1;
    ui->TextEdit->setAlignment((ui->TextEdit->alignment() & 0x0f)|Qt::AlignVCenter);
    CurrentTextItem->Text=ui->TextEdit->toHtml();
    RefreshControls();
    ui->TextEdit->setFocus();
}

//========= Vertical alignment bottom
void DlgTextEdit::s_SetTextBottom() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgTextEdit::s_SetTextBottom");

    if (StopMAJSpinbox) return;
    CurrentTextItem->VAlign=2;
    ui->TextEdit->setAlignment((ui->TextEdit->alignment() & 0x0f)|Qt::AlignBottom);
    CurrentTextItem->Text=ui->TextEdit->toHtml();
    RefreshControls();
    ui->TextEdit->setFocus();
}

//========= Font family
void DlgTextEdit::s_ChangeFont(QFont font) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgTextEdit::s_ChangeFont");
    if (StopMAJSpinbox) return;

    QTextCursor     Cursor(ui->TextEdit->textCursor());
    QTextCharFormat TCF;
    if (Cursor.hasSelection())  TCF=Cursor.charFormat();                // Modify current selection
        else                    TCF=ui->TextEdit->currentCharFormat();  // Modify default option
    TCF.setFontFamily(font.family());
    if (Cursor.hasSelection())  Cursor.setCharFormat(TCF);
        else                    ui->TextEdit->setCurrentCharFormat(TCF);

    CurrentTextItem->FontName=font.family();
    CurrentTextItem->Text=ui->TextEdit->toHtml();
    RefreshControls();
    ui->TextEdit->setFocus();
}

//========= Size font
void DlgTextEdit::s_ChangeSizeFont(QString size) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgTextEdit::s_ChangeSizeFont");
    if (StopMAJSpinbox) return;

    QTextCursor     Cursor(ui->TextEdit->textCursor());
    QTextCharFormat TCF;
    if (Cursor.hasSelection())  TCF=Cursor.charFormat();                // Modify current selection
        else                    TCF=ui->TextEdit->currentCharFormat();  // Modify default option
    TCF.setFontPointSize(size.toInt());
    if (Cursor.hasSelection())  Cursor.setCharFormat(TCF);
        else                    ui->TextEdit->setCurrentCharFormat(TCF);

    CurrentTextItem->FontSize=size.toInt();
    CurrentTextItem->Text=ui->TextEdit->toHtml();
    RefreshControls();
    ui->TextEdit->setFocus();
}

//========= Style font
void DlgTextEdit::s_ChangeStyleFont(int Style) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgTextEdit::s_ChangeStyleFont");

    if (StopMAJSpinbox) return;
    CurrentTextItem->StyleText=Style;
    RefreshControls();
}

//========= Plain text edit
void DlgTextEdit::s_TextEditChange() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgTextEdit::s_TextEditChange");

    if (StopMAJSpinbox) return;
    CurrentTextItem->Text=ui->TextEdit->toHtml();
    RefreshControls();
}

//========= Font color
void DlgTextEdit::s_ChIndexFontColorCombo(int) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgTextEdit::s_ChIndexFontColorCombo");
    if (StopMAJSpinbox) return;

    CurrentTextItem->FontColor=ui->FontColorCombo->GetCurrentColor();

    QTextCursor     Cursor(ui->TextEdit->textCursor());
    QTextCharFormat TCF;
    if (Cursor.hasSelection())  TCF=Cursor.charFormat();                // Modify current selection
        else                    TCF=ui->TextEdit->currentCharFormat();  // Modify default option

    TCF.setForeground(QBrush(QColor(CurrentTextItem->FontColor)));

    if (Cursor.hasSelection())  Cursor.setCharFormat(TCF);
        else                    ui->TextEdit->setCurrentCharFormat(TCF);

    CurrentTextItem->Text=ui->TextEdit->toHtml();
    RefreshControls();
    ui->TextEdit->setFocus();
}

//========= Text shadow color
void DlgTextEdit::s_ChIndexFontShadowColorCombo(int) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgTextEdit::s_ChIndexFontShadowColorCombo");

    if (StopMAJSpinbox) return;
    CurrentTextItem->FontShadowColor=ui->StyleShadowColorCombo->GetCurrentColor();
    RefreshControls();
}

//====================================================================================================================

void DlgTextEdit::s_ChangeBrushTypeCombo(int Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgTextEdit::s_ChangeBrushTypeCombo");

    if (StopMAJSpinbox) return;
    CurrentTextItem->BackgroundBrush->BrushType=ui->BrushTypeCombo->itemData(Value).toInt();
    RefreshControls();
}

//====================================================================================================================

void DlgTextEdit::s_IntermPosED(int Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgTextEdit::s_IntermPosED");

    if (StopMAJSpinbox) return;
    CurrentTextItem->BackgroundBrush->Intermediate=double(Value)/100;
    RefreshControls();
}

//====================================================================================================================
// Handler for custom color/brush/pattern/gradient combo box index change
//====================================================================================================================

//========= Pattern shape combo
void DlgTextEdit::s_ChIndexPatternBrushCombo(int) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgTextEdit::s_ChIndexPatternBrushCombo");

    if (StopMAJSpinbox) return;
    CurrentTextItem->BackgroundBrush->PatternType=ui->PatternBrushCombo->GetCurrentBrush()->PatternType;
    RefreshControls();
}

//========= Gradient shape orientation
void DlgTextEdit::s_ChIndexGradientOrientationCombo(int) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgTextEdit::s_ChIndexGradientOrientationCombo");

    if (StopMAJSpinbox) return;
    CurrentTextItem->BackgroundBrush->GradientOrientation=ui->OrientationCombo->GetCurrentBrush()->GradientOrientation;
    RefreshControls();
}

//========= Shape/Gradient shape first color
void DlgTextEdit::s_ChIndexGradientFirstColorCombo(int) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgTextEdit::s_ChIndexGradientFirstColorCombo");

    if (StopMAJSpinbox) return;
    CurrentTextItem->BackgroundBrush->ColorD=ui->FirstColorCombo->GetCurrentColor();
    RefreshControls();
}

//========= Gradient shape last color
void DlgTextEdit::s_ChIndexGradientFinalColorCombo(int) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgTextEdit::s_ChIndexGradientFinalColorCombo");

    if (StopMAJSpinbox) return;
    CurrentTextItem->BackgroundBrush->ColorF=ui->FinalColorCombo->GetCurrentColor();
    RefreshControls();
}

//========= Gradient shape intermediate color
void DlgTextEdit::s_ChIndexGradientIntermColorCombo(int) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgTextEdit::s_ChIndexGradientIntermColorCombo");

    if (StopMAJSpinbox) return;
    CurrentTextItem->BackgroundBrush->ColorIntermed=ui->IntermColorCombo->GetCurrentColor();
    RefreshControls();
}

//========= Background image
void DlgTextEdit::s_ChIndexBackgroundCombo(int) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgTextEdit::s_ChIndexBackgroundCombo");

    if (StopMAJSpinbox) return;
    CurrentTextItem->BackgroundBrush->BrushImage=ui->BackgroundCombo->GetCurrentBackground();
    RefreshControls();
}

//====================================================================================================================
// Handler for style sheet management
//====================================================================================================================

void DlgTextEdit::s_TextStyleBT() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgTextEdit::s_TextStyleBT");

    QString ActualStyle=CurrentTextItem->GetTextStyle();
    QString Item=StyleTextCollection->PopupCollectionMenu(this,ActualStyle);
    ui->TextStyleBT->setDown(false);
    if (Item!="") {
        CurrentTextItem->ApplyTextStyle(StyleTextCollection->GetStyleDef(Item));
        ui->TextEdit->setHtml(CurrentTextItem->Text);
        QTextCursor Cursor=ui->TextEdit->textCursor();
        Cursor.movePosition(QTextCursor::Start);
        ui->TextEdit->setTextCursor(Cursor);
    }
    RefreshControls();
}

void DlgTextEdit::s_BackgroundStyleBT() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgTextEdit::s_BackgroundStyleBT");

    QString ActualStyle=CurrentTextItem->GetBackgroundStyle();
    QString Item=StyleTextBackgroundCollection->PopupCollectionMenu(this,ActualStyle);
    ui->BackgroundStyleBT->setDown(false);
    if (Item!="") CurrentTextItem->ApplyBackgroundStyle(StyleTextBackgroundCollection->GetStyleDef(Item));
    RefreshControls();
}
