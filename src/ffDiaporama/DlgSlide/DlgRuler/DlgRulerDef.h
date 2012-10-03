#ifndef DLGRULERDEF_H
#define DLGRULERDEF_H

// Basic inclusions (common to all files)
#include "../../../engine/_GlobalDefines.h"
#include "../../../engine/_QCustomDialog.h"

#include <QDialog>

namespace Ui {
    class DlgRulerDef;
}

class DlgRulerDef : public QCustomDialog {
Q_OBJECT
public:
    int *RulerDef;
    int SavRuler;

    explicit DlgRulerDef(int *RulerDef,QString HelpURL,cBaseApplicationConfig *ApplicationConfig,cSaveWindowPosition *DlgWSP,QWidget *parent=0);
    ~DlgRulerDef();

    // function to be overloaded
    virtual void    DoInitDialog();                             // Initialise dialog
    virtual void    DoAccept()          {/*Nothing to do*/}     // Call when user click on Ok button
    virtual void    DoRejet()           {/*Nothing to do*/}     // Call when user click on Cancel button
    virtual void    PrepareGlobalUndo() {SavRuler=*RulerDef;}   // Initiale Undo
    virtual void    DoGlobalUndo()      {*RulerDef=SavRuler;}   // Apply Undo : call when user click on Cancel button

signals:
    void            RefreshDisplay();

private slots:
    void            CBStateChanged(int);

private:
    Ui::DlgRulerDef *ui;
};

#endif // DLGRULERDEF_H