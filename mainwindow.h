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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// Basic inclusions (common to all files)
#include "_GlobalDefines.h"

// Specific inclusions
#include "_Diaporama.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
Q_OBJECT
public:
    bool                    IsFirstInitDone;                // true when first show window was done
    bool                    FLAGSTOPITEMSELECTION;          // Flag to stop Item Selection process in the timeline
    cApplicationConfig      *ApplicationConfig;
    cDiaporama              *Diaporama;
    QDomDocument            *Clipboard_Object;
    QDomDocument            *Clipboard_Block;
    QStringList             StatusBarList;
    QTimer                  Timer;
    int                     LastCount;
    QString                 InternetBUILDVERSION;
    // Drag & drop operation
    int                     DragItemSource;
    int                     DragItemDest;
    bool                    IsDragOn;

    explicit MainWindow(cApplicationConfig *TheCurrentApplicationConfig,QWidget *parent = 0);
    ~MainWindow();

    void    OpenHelp(QString HelpFile);

    void    RefreshControls();
    void    SetModifyFlag(bool IsModify);
    void    AddObjectToTimeLine(int CurIndex);
    void    AdjustRuller();
    void    SetTimelineHeight();
    void    OpenFile(QString ProjectFileName);
    void    CheckVersion();

protected:
    virtual void resizeEvent(QResizeEvent *);
    virtual void closeEvent(QCloseEvent *);
    virtual void showEvent(QShowEvent *);

private slots:
    void    s_WebViewOpen(QUrl);
    void    s_ToolbarChanged(int MenuIndex);
    void    s_TimerEvent();
    void    onNetworkReply(QNetworkReply*);

    // Timeline
    void    s_DragMoveItem();                           // Drag & drop operation
    void    s_ItemSelectionChanged();
    void    s_ItemDoubleClicked();                      // Double click on widget in the object track
    void    s_TransitionItemDoubleClicked();            // Double click on transition part of widget in the object track
    void    s_SoundItemDoubleClicked();                 // Double click on sound part of widget in the object track
    void    s_BackgroundDoubleClicked();                // Double click on widget in the background track
    void    s_TransitionBackgroundDoubleClicked();      // Double click on transition part of widget in the background track
    void    s_MusicDoubleClicked();                     // Double click on widget in the music track

    void    s_action_ZoomPlus();
    void    s_action_ZoomMinus();
    void    s_ChPartitionMode();

    // File menu
    void    s_action_New();
    void    s_action_Open();
    void    s_action_OpenRecent();
    void    s_action_Save();
    void    s_action_SaveAs();
    void    s_action_Exit();
    void    s_ChangeApplicationSettings();

    // Project menu
    void    s_action_AddFile();
    void    s_action_AddTitle();
    void    s_action_AddProject();
    void    s_RemoveObject();
    void    s_CutToClipboard();
    void    s_CopyToClipboard();
    void    s_PasteFromClipboard();
    void    s_action_Edit();

    // Render menu
    void    s_RenderVideo();
    void    s_RenderSmartphone();
    void    s_RenderMultimedia();
    void    s_RenderForTheWEB();

    // Help menu
    void    s_About();
    void    s_Documentation();
    void    s_NewFunctions();

private:
    Ui::MainWindow *ui;
};

extern MainWindow  *GlobalMainWindow;

#endif // MAINWINDOW_H
