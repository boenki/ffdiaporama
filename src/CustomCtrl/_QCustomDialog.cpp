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

#include "_QCustomDialog.h"
#include <QHeaderView>
#include <QInputDialog>
#include <QDialogButtonBox>

//====================================================================================================================

QString CustomInputDialog(QWidget *parent,const QString &title,const QString &label,int mode,const QString &text,bool *ok,Qt::InputMethodHints inputMethodHints) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:CustomInputDialog");

    Qt::WindowFlags Flags=(Qt::Dialog|Qt::CustomizeWindowHint|Qt::WindowSystemMenuHint|Qt::WindowMaximizeButtonHint)&(~Qt::WindowMinimizeButtonHint);

    QInputDialog dialog(parent,Flags);
    int x=QCursor::pos().x()-dialog.width();   if (x<0) x=0;
    int y=QCursor::pos().y()-dialog.height();  if (y<0) y=0;
    dialog.move(x,y);
    dialog.setWindowTitle(title);
    dialog.setLabelText(label);
    dialog.setTextValue(text);
    dialog.setTextEchoMode((QLineEdit::EchoMode)mode);
    dialog.setInputMethodHints(inputMethodHints);
    dialog.setWindowModality(Qt::ApplicationModal);

    int ret = dialog.exec();
    if (ok) *ok = !!ret;
    if (ret) return dialog.textValue();
        else return QString();
}

//====================================================================================================================

int CustomMessageBox(QWidget *parent,QMessageBox::Icon icon,const QString& title,const QString& text,QMessageBox::StandardButtons buttons,QMessageBox::StandardButton defaultButton) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:CustomMessageBox");

    Qt::WindowFlags Flags=(Qt::Dialog|Qt::CustomizeWindowHint|Qt::WindowSystemMenuHint|Qt::WindowMaximizeButtonHint)&(~Qt::WindowMinimizeButtonHint);

    QMessageBox         msgBox(icon,title,text,QMessageBox::NoButton,parent,Flags);
    QDialogButtonBox    *buttonBox = msgBox.findChild<QDialogButtonBox*>();

    uint mask = QMessageBox::FirstButton;
    while (mask <= QMessageBox::LastButton) {
        uint sb = buttons & mask;
        mask <<= 1;
        if (!sb) continue;
        QPushButton *button = msgBox.addButton((QMessageBox::StandardButton)sb);
        // Choose the first accept role as the default
        if (msgBox.defaultButton()) continue;
        if ((defaultButton == QMessageBox::NoButton && buttonBox->buttonRole(button) == QDialogButtonBox::AcceptRole)
            || (defaultButton != QMessageBox::NoButton && sb == uint(defaultButton)))
            msgBox.setDefaultButton(button);
    }
    msgBox.setWindowModality(Qt::ApplicationModal);
    if (msgBox.exec() == -1) return QMessageBox::Cancel;
    return msgBox.standardButton(msgBox.clickedButton());
}

//====================================================================================================================

QCustomDialog::QCustomDialog(QString HelpURL,cBaseApplicationConfig *BaseApplicationConfig,cSaveWindowPosition *DlgWSP,QWidget *parent):QDialog(parent) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomDialog::QCustomDialog");

    this->HelpURL               =HelpURL;
    this->BaseApplicationConfig =BaseApplicationConfig;
    this->DlgWSP                =DlgWSP;
    Undo                        =NULL;
    OkBt                        =NULL;
    CancelBt                    =NULL;
    HelpBt                      =NULL;
    UndoBt                      =NULL;

    setWindowFlags((windowFlags()|Qt::CustomizeWindowHint|Qt::WindowSystemMenuHint|Qt::WindowMaximizeButtonHint)&(~Qt::WindowMinimizeButtonHint));
}

//====================================================================================================================

QCustomDialog::~QCustomDialog() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomDialog::~QCustomDialog");

    if (Undo) {
        delete Undo;
        Undo=NULL;
    }
}

//====================================================================================================================
// Initialise dialog

void QCustomDialog::InitDialog() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomDialog::InitDialog");

    // Define handler for standard buttons
    if (OkBt)       connect(OkBt,SIGNAL(clicked()),this,SLOT(accept()));
    if (CancelBt)   connect(CancelBt,SIGNAL(clicked()),this,SLOT(reject()));
    if (UndoBt)     connect(UndoBt,SIGNAL(clicked()),this,SLOT(DoPartialUndo()));

    if (HelpBt) {
        if (HelpURL!="") connect(HelpBt,SIGNAL(clicked()),this,SLOT(doHelp()));
            else HelpBt->setVisible(false);
    }

    // Restore window size and position
    RestoreWindowState();

    // Prepare undo
    PrepareGlobalUndo();

    // Initialise dialog
    DoInitDialog();

    if (UndoBt) UndoBt->setEnabled(UndoDataList.count()>0);
}

//====================================================================================================================

void QCustomDialog::doHelp() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomDialog::Help");

    if (HelpURL!="") QDesktopServices::openUrl(QUrl(HelpURL.replace("<local>",BaseApplicationConfig->GetValideWEBLanguage(BaseApplicationConfig->CurrentLanguage))));
}

//====================================================================================================================

void QCustomDialog::accept() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomDialog::accept");

    SaveWindowState();  // Save Window size and position
    if (DoAccept())     // call overloaded function
        done(0);        // Close dialog
}

//====================================================================================================================

void QCustomDialog::reject() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomDialog::accept");

    SaveWindowState();  // Save Window size and position
    DoGlobalUndo();     // Undo change
    DoRejet();          // call overloaded function
    done(1);            // Close dialog
}

//====================================================================================================================

void QCustomDialog::SaveWindowState() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomDialog::SaveWindowState");

    // Save Window size and position
    if (DlgWSP) DlgWSP->SaveWindowState(this);
}

//====================================================================================================================

void QCustomDialog::RestoreWindowState() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomDialog::RestoreWindowState");

    // Restore window size and position
    if (DlgWSP) DlgWSP->ApplyToWindow(this);
}

//====================================================================================================================
// utility function to init a table widget

void QCustomDialog::DoInitTableWidget(QTableWidget *Table,QString TableColumns) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomDialog::DoInitTableWidget");

    Table->setSelectionBehavior(QAbstractItemView::SelectRows);
    Table->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    Table->horizontalHeader()->show();
    Table->horizontalHeader()->setStretchLastSection(false);
    Table->horizontalHeader()->setSortIndicatorShown(false);
    Table->horizontalHeader()->setCascadingSectionResizes(false);
    Table->horizontalHeader()->setClickable(false);
    Table->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    Table->horizontalHeader()->setMovable(false);
    Table->horizontalHeader()->setResizeMode(QHeaderView::Fixed);          //Fixed because ResizeToContents will be done after table filling
    Table->verticalHeader()->hide();
    Table->verticalHeader()->setStretchLastSection(false);
    Table->verticalHeader()->setSortIndicatorShown(false);
    Table->verticalHeader()->setResizeMode(QHeaderView::Fixed);            // Fixed because ResizeToContents will be done after table filling
    Table->setShowGrid(true);                  // Ensure grid display
    Table->setWordWrap(false);                 // Ensure no word wrap
    Table->setTextElideMode(Qt::ElideNone);    // Ensure no line ellipsis (...)
    Table->setColumnCount(TableColumns.split(";").count());
    Table->setHorizontalHeaderLabels(TableColumns.split(";"));
}

//====================================================================================================================
// utility function to create a QTableWidgetItem

QTableWidgetItem *QCustomDialog::CreateItem(QString ItemText,int Alignment,QBrush Background) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomDialog::CreateItem");

    QTableWidgetItem *Item=new QTableWidgetItem(ItemText);
    Item->setTextAlignment(Alignment);
    Item->setBackground(Background);
    return Item;
}

//====================================================================================================================
// utility function to resize columns in a table widget

void QCustomDialog::DoResizeColumnsTableWidget(QTableWidget *Table) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomDialog::DoResizeColumnsTableWidget");

    Table->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
    Table->setVisible(false);                      // To ensure all items of all columns are used to compute size
    Table->resizeColumnsToContents();              // Resize column widht
    Table->resizeRowsToContents();                 // Resize row height
    Table->setVisible(true);                       // To allow display
    Table->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
}

//====================================================================================================================
// utility function to add a partial undo to the list

void QCustomDialog::AppendPartialUndo(int ActionType,QWidget *FocusWindow,bool ForceAdd) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomDialog::AppendPartialUndo");
    if ((ForceAdd)||(UndoDataList.count()==0)||(ActionType!=UndoDataList.last().ActionType)) {

        QDomDocument DomDocument("UNDO");
        QDomElement  root=DomDocument.createElement("UNDO-DATA");
        PreparePartialUndo(ActionType,root);
        DomDocument.appendChild(root);

        sUndoData   Data;
        Data.ActionType =ActionType;
        Data.Data       =DomDocument.toString(1);
        Data.FocusWindow=FocusWindow;
        UndoDataList.append(Data);
        if (UndoBt) UndoBt->setEnabled(UndoDataList.count()>0);
    }
}

//====================================================================================================================
// utility function to undo latest undo insertion

void QCustomDialog::RemoveLastPartialUndo() {
    UndoDataList.removeLast();
    if (UndoBt) UndoBt->setEnabled(UndoDataList.count()>0);
}

//====================================================================================================================
// utility function to undo latest action

void QCustomDialog::DoPartialUndo() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomDialog::DoPartialUndo");
    if (UndoDataList.count()==0) {
        if (Undo!=NULL) DoGlobalUndo();
    } else {
        sUndoData Data=UndoDataList.last();
        UndoDataList.removeLast();
        QDomDocument DomDocument("UNDO");
        if (DomDocument.setContent(Data.Data,true)) {
            QDomElement root=DomDocument.documentElement();
            if (root.tagName()=="UNDO-DATA") ApplyPartialUndo(Data.ActionType,root);
        }
        if (Data.FocusWindow!=NULL) Data.FocusWindow->setFocus();
    }
    if (UndoBt) UndoBt->setEnabled(UndoDataList.count()>0);
}

//====================================================================================================================
// Utility function use to set current index in a combobox base on an itemdata

void QCustomDialog::SetCBIndex(QComboBox *CB,int ItemData) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomDialog::SetCBIndex");
    for (int i=0;i<CB->count();i++) if (CB->itemData(i).toInt()==ItemData) {
        CB->setCurrentIndex(i);
        break;
    }
}
