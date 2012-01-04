#ifndef DLGINFOFILE_H
#define DLGINFOFILE_H

// Basic inclusions (common to all files)
#include "../VariousClass/_GlobalDefines.h"
#include "_QCustomDialog.h"

// Include some additional standard class
#include <QString>

// Include some common various class
#include "../VariousClass/cBaseApplicationConfig.h"
#include "../VariousClass/cSaveWindowPosition.h"
#include "../VariousClass/cBaseMediaFile.h"

namespace Ui {
    class DlgInfoFile;
}

class DlgInfoFile : public QCustomDialog {
Q_OBJECT
public:
    cBaseMediaFile *MediaFile;

    explicit DlgInfoFile(cBaseMediaFile *MediaFile,QString HelpURL,cBaseApplicationConfig *ApplicationConfig,cSaveWindowPosition *DlgWSP,QWidget *parent = 0);
    ~DlgInfoFile();

    // function to be overloaded
    virtual void    DoInitDialog();                             // Initialise dialog
    virtual void    DoAccept()          {/*Nothing to do*/}     // Call when user click on Ok button
    virtual void    DoRejet()           {/*Nothing to do*/}     // Call when user click on Cancel button
    virtual void    PrepareGlobalUndo() {/*Nothing to do*/}     // Initiale Undo
    virtual void    DoGlobalUndo()      {/*Nothing to do*/}     // Apply Undo : call when user click on Cancel button

private:
    Ui::DlgInfoFile *ui;
};

#endif // DLGINFOFILE_H
