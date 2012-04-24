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
#include "../sharedfiles/_GlobalDefines.h"

// Include some common various class
#include "../sharedfiles/_SDL_Support.h"

#include <QNetworkReply>

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
    int                     LastCount;
    QString                 InternetBUILDVERSION;
    QString                 TitleBar;

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

    DlgRenderVideo          *CurrentRenderingDialog;

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

    void    DoAddDragAndDropFile();                         // Must be in public part because it was called by cCustomTableWidget
    void    DoOpenFileParam();                              // Must be in public part because it was called by main

protected:
    virtual void resizeEvent(QResizeEvent *);
    virtual void closeEvent(QCloseEvent *);
    virtual void showEvent(QShowEvent *);
    virtual void keyReleaseEvent(QKeyEvent *event);


private slots:
    void    DoMaximized();                                  // Use on init to force window maximized
    void    s_Event_SetModifyFlag();
    void    s_Event_RefreshDisplay();

    // Event functions
    void    s_Event_ClipboardChanged();                     // Something have changed in the clipboard
    void    s_Event_ToolbarChanged(int MenuIndex);          // User change current toolbar TAB
    void    s_Event_NetworkReply(QNetworkReply*);           // Build version was downloaded from internet

    // Timeline & player
    void    s_Event_TimelineDragMoveItem();                 // Drag & drop operation in timeline
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
    void    s_Action_ChPartitionMode();

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
    void    DoSaveFile();

private:

    Ui::MainWindow *ui;
};

extern MainWindow  *GlobalMainWindow;

#endif // MAINWINDOW_H
