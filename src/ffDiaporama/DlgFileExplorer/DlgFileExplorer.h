#ifndef DLGFILEEXPLORER_H
#define DLGFILEEXPLORER_H

// Basic inclusions (common to all files)
#include "../../CustomCtrl/_QCustomDialog.h"
#include "../../engine/cDriveList.h"
#include "../DlgWorkingTask/DlgWorkingTask.h"

namespace Ui {
class DlgFileExplorer;
}

class DlgFileExplorer : public QCustomDialog {
Q_OBJECT
public:
    QString                 CurrentPath;
    QString                 BoxTitle;
    DlgWorkingTask          *DlgWorkingTaskDialog;
    bool                    CancelAction;
    int                     CurrentDriveCheck;

    explicit                DlgFileExplorer(int AllowedFilter,int CurrentFilter,bool AllowMultipleSelection,bool AllowDragDrop,QString StartupPath,QString BoxTitle,
                                            int HelpURL,cBaseApplicationConfig *ApplicationConfig,cSaveWindowPosition *DlgWSP,QWidget *parent=0);
    virtual                 ~DlgFileExplorer();
    
    // function to be overloaded
    virtual void            DoInitDialog();                             // Initialise dialog
    virtual bool            DoAccept();                                 // Call when user click on Ok button
    virtual void            DoRejet()           {/*Nothing to do*/}     // Call when user click on Cancel button
    virtual void            PrepareGlobalUndo();                        // Initiale Undo
    virtual void            DoGlobalUndo();                             // Apply Undo : call when user click on Cancel button
    virtual void            SaveWindowState();
    virtual void            RestoreWindowState();

    QStringList             GetCurrentSelectedFiles();

private slots:
    void                    s_Browser_FolderTreeItemChanged(QTreeWidgetItem *current,QTreeWidgetItem *);
    void                    s_Browser_RefreshAll();
    void                    s_Browser_RefreshDriveList();
    void                    s_Browser_RefreshDrive();
    void                    s_Browser_RefreshHere();
    void                    s_Browser_SetToPrevious();
    void                    s_Browser_SetToUpper();
    void                    s_Browser_DoubleClicked(QMouseEvent *);
    void                    s_Browser_OpenFolder();
    void                    s_Browser_RightClicked(QMouseEvent *);
    void                    s_Browser_Properties();
    void                    s_Browser_Favorite();
    void                    s_Browser_AddToFavorite();
    void                    s_Browser_ManageFavorite();
    void                    s_Browser_ChangeDisplayMode();
    void                    DoBrowserRefreshFolderInfo();
    void                    DoBrowserRefreshSelectedFileInfo();

private:
    QAction *CreateMenuAction(QImage *Icon,QString Text,int Data,bool Checkable,bool IsCheck);
    QAction *CreateMenuAction(QIcon Icon,QString Text,int Data,bool Checkable,bool IsCheck);

    Ui::DlgFileExplorer *ui;
};

#endif // DLGFILEEXPLORER_H
