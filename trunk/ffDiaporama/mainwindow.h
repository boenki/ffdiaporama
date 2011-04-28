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
    bool                FLAGSTOPITEMSELECTION;  // Flag to stop Item Selection process in the timeline
    cApplicationConfig  *ApplicationConfig;
    cDiaporama          *Diaporama;
    QDomDocument        *Clipboard;

    explicit MainWindow(cApplicationConfig *TheCurrentApplicationConfig,QWidget *parent = 0);
    ~MainWindow();

    void    RefreshControls();
    void    SetModifyFlag(bool IsModify);
    void    AddObjectToTimeLine(int CurIndex);
    void    UpdateDockInfo();
    void    AdjustRuller();
    void    SetTimelineHeight();

protected:
    virtual void resizeEvent(QResizeEvent *);
    virtual void closeEvent(QCloseEvent *);
    virtual void showEvent(QShowEvent *);

private slots:
    // Timeline
    void    s_ItemSelectionChanged();
    void    s_ItemDoubleClicked();                      // Double click on widget in the object track
    void    s_TransitionItemDoubleClicked();            // Double click on transition part of widget in the object track
    void    s_SoundItemDoubleClicked();                 // Double click on sound part of widget in the object track
    void    s_BackgroundDoubleClicked();                // Double click on widget in the background track
    void    s_TransitionBackgroundDoubleClicked();      // Double click on transition part of widget in the background track
    void    s_MusicDoubleClicked();                     // Double click on widget in the music track

    void    s_action_ZoomPlus();
    void    s_action_ZoomMinus();

    // File menu
    void    s_action_New();
    void    s_action_Open();
    void    s_action_Save();
    void    s_action_SaveAs();
    void    s_action_Exit();

    // Project menu
    void    s_action_AddFile();
    void    s_action_AddTitle();
    void    s_action_AddProject();
    void    s_RemoveObject();
    void    s_LeftObject();
    void    s_RightObject();
    void    s_CutToClipboard();
    void    s_CopyToClipboard();
    void    s_PasteFromClipboard();
    void    s_ChangeProjectSettings();

    // Tools menu
    void    s_RenderVideo();
    void    s_ChangeApplicationSettings();

    // Help menu
    void    s_About();
    void    s_Documentation();
    void    s_NewFunctions();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
