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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// Basic inclusions (common to all files)
#include "../engine/_GlobalDefines.h"

// Include some common various class
#include "../engine/_SDL_Support.h"
#include "../engine/cDriveList.h"
#include "DlgWorkingTask/DlgWorkingTask.h"

#include <QNetworkReply>
#include <QTreeWidgetItem>

// Specific inclusions
#include "_Diaporama.h"

namespace Ui {
    class MainWindow;
}

class DlgRenderVideo;

class MainWindow : public QMainWindow {
Q_OBJECT
public:
    bool                    IsFirstInitDone;                // true when first show window was done
    bool                    FLAGSTOPITEMSELECTION;          // Flag to stop Item Selection process in the timeline
    cApplicationConfig      *ApplicationConfig;
    cDiaporama              *Diaporama;
    cDriveList              *DriveList;
    int                     LastCount;
    QString                 InternetBUILDVERSION;
    QString                 TitleBar;
    QTime                   LastLogMessageTime;
    QStringList             AliasList;

    // Variables use for actions
    QString                 FileForIO;
    int                     SavedCurIndex,CurIndex;
    QStringList             FileList;

    // Drag & drop operation
    int                     DragItemSource;
    int                     DragItemDest;
    int                     IsDragOn;                       // DragOff=0, DragInternal=0 or DragExternal=0
    Qt::HANDLE              CurrentThreadId;
    bool                    InPlayerUpdate;

    DlgWorkingTask          *DlgWorkingTaskDialog;
    bool                    CancelAction;
    int                     CurrentDriveCheck;

    // Loading operation
    QDomElement             CurrentLoadingProjectDocument;
    int                     CurrentLoadingProjectObject;
    int                     CurrentLoadingProjectNbrObject;

    // Appending a project operation
    QString                 CurrentAppendingProjectName;
    QDomDocument            CurrentAppendingProjectDocument;
    QDomElement             CurrentAppendingRoot;
    int                     CurrentAppendingProjectObject;
    int                     CurrentAppendingProjectNbrObject;


    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void    InitWindow(QString ForceLanguage,QApplication *App);

    void    OpenHelp(QString HelpFile);
    void    RefreshControls();
    void    SetModifyFlag(bool IsModify);
    void    AddObjectToTimeLine(int CurIndex);
    void    AdjustRuller();
    void    SetTimelineHeight();
    void    CheckVersion();
    void    ToStatusBar(QString Text);
    void    SetTimelineCurrentCell(int Cell);

    void    DoOpenFileParam();                              // Must be in public part because it was called by main

protected:
    virtual void    resizeEvent(QResizeEvent *);
    virtual void    closeEvent(QCloseEvent *);
    virtual void    showEvent(QShowEvent *);
    virtual void    keyReleaseEvent(QKeyEvent *event);
    virtual void    customEvent(QEvent *);                  // LOG messages event


private slots:
    void    s_CleanStatusBar();
    void    DoMaximized();                                  // Use on init to force window maximized
    void    s_Event_SetModifyFlag();
    void    s_Event_RefreshDisplay();

    // Event functions
    void    s_Event_ClipboardChanged();                     // Something have changed in the clipboard
    void    s_Event_ToolbarChanged(int MenuIndex);          // User change current toolbar TAB
    void    s_Event_NetworkReply(QNetworkReply*);           // Build version was downloaded from internet

    // Timeline & player
    void    s_Event_TimelineDragMoveItem();                 // Drag & drop operation in timeline
    void    s_Event_TimelineAddDragAndDropFile();           // Add files from drag & drop (event send from timeline)
    void    s_Event_TimelineSelectionChanged();             // Timeline current item changed
    void    s_Event_DoubleClickedOnObject();                // Double click on widget in the object track
    void    s_Event_DoubleClickedOnTransition();            // Double click on transition part of widget in the object track
    void    s_Event_DoubleClickedOnVideoSound();            // Double click on video sound part of widget in the object track
    void    s_Event_DoubleClickedOnBackground();            // Double click on widget in the background track
    void    s_Event_DoubleClickedOnMusic();                 // Double click on widget in the music track
    void    s_Event_ContextualMenu(QMouseEvent *);          // Right click
    void    s_Event_SaveImageEvent();                       // Click on preview save image button

    // Actions small bottom right toolbar menu
    void    s_Action_ZoomPlus();
    void    s_Action_ZoomMinus();
    void    s_Action_ChWindowDisplayMode_ToPlayerMode();
    void    s_Action_ChWindowDisplayMode_ToPartitionMode();
    void    s_Action_ChWindowDisplayMode_ToBrowserMode();
    void    s_Action_ChWindowDisplayMode(int Mode);

    // Actions File menu
    void    s_Action_New();
    void    s_Action_Open();
    void    s_Action_OpenRecent();
    void    s_Action_Save();
    void    s_Action_SaveAs();
    void    s_Action_ProjectProperties();
    void    s_Action_ChangeApplicationSettings();
    void    s_Action_DlgCheckConfig();
    void    s_Action_Exit();

    // Actions Project menu
    void    s_Action_AddFile();
    void    s_Action_AddTitle();
    void    s_Action_AddProject();
    void    s_Action_DoAddFile();
    void    s_Action_DoAppendFile();
    void    s_Action_DoUseAsPlayList(QStringList &MusicFileList,int Index);
    void    s_Action_RemoveObject();
    void    s_Action_EditObject();
    void    s_Action_CutToClipboard();
    void    s_Action_CopyToClipboard();
    void    s_Action_PasteFromClipboard();

    // Actions Render menu
    void    s_Action_RenderVideo();
    void    s_Action_RenderSmartphone();
    void    s_Action_RenderMultimedia();
    void    s_Action_RenderForTheWEB();
    void    s_Action_RenderLossLess();

    // Actions Help menu
    void    s_Action_About();
    void    s_Action_Documentation();
    void    s_Action_NewFunctions();
    void    s_Action_OpenTABHelpLink(const QString);

    // Do function (call by action or event function)
    void    DoChangeApplicationSettings();
    void    DoOpenFile();
    void    DoOpenFileObject();
    void    DoSaveFile();

    // Browser event
    void    s_Browser_FloderTreeItemChanged(QTreeWidgetItem *current,QTreeWidgetItem *);
    void    s_Browser_RefreshAll();
    void    s_Browser_RefreshDriveList();
    void    s_Browser_RefreshDrive();
    void    s_Browser_RefreshHere();
    void    s_Browser_ChangeDisplayMode();
    void    s_Browser_DoubleClicked(QMouseEvent *);
    void    s_Browser_OpenFile();
    void    s_Browser_RightClicked(QMouseEvent *);
    void    DoBrowserRefreshFolderInfo();
    void    DoBrowserRefreshSelectedFileInfo();
    void    s_Browser_SetToPrevious();
    void    s_Browser_SetToUpper();
    void    s_Browser_Favorite();
    void    s_Browser_AddToFavorite();
    void    s_Browser_ManageFavorite();
    void    s_Browser_RemoveFolder();
    void    s_Browser_RenameFile();
    void    s_Action_DoRemoveFile();
    void    s_Browser_RenameFolder();
    void    s_Browser_Properties();
    void    s_Browser_RemoveFile();
    void    s_Browser_UseAsPlaylist();
    void    s_Browser_AddFiles();

private:

    // Utility functions
    void    SortFileList();
    QAction *CreateMenuAction(QImage *Icon,QString Text,int Data,bool Checkable,bool IsCheck);
    QAction *CreateMenuAction(QIcon Icon,QString Text,int Data,bool Checkable,bool IsCheck);
    bool    s_Browser_InRemoveFolder(QString FolderPath);

    Ui::MainWindow *ui;
};

extern MainWindow  *GlobalMainWindow;

#endif // MAINWINDOW_H
