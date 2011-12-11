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

#include "_StyleDefinitions.h"
#include "_SoundDefinitions.h"
#include "_ImagesDefinitions.h"
#include "_ImageFileWrapper.h"
#include "_VideoFileWrapper.h"
#include "_ApplicationDefinitions.h"


#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "wgt_QCustomThumbnails.h"
#include "cCustomTableWidget.h"

#include "DlgAbout.h"
#include "DlgBackgroundProperties.h"
#include "DlgMusicProperties.h"
#include "DlgSlideProperties.h"
#include "DlgTransitionProperties.h"
#include "DlgApplicationSettings.h"
#include "DlgRenderVideo.h"
#include "DlgCheckConfig.h"

#define DEBUGMODE

MainWindow  *GlobalMainWindow=NULL;

//====================================================================================================================

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::MainWindow";
    #endif

    ApplicationConfig       =new cApplicationConfig();
    CurrentThreadId         =this->thread()->currentThreadId();
    InternetBUILDVERSION    ="";
    GlobalMainWindow        =this;
    IsFirstInitDone         =false;                 // true when first show window was done
    FLAGSTOPITEMSELECTION   =false;        // Flag to stop Item Selection process for delete and move of object
    Clipboard_Object        =NULL;
    Clipboard_Block         =NULL;
    DragItemSource          =-1;
    DragItemDest            =-1;
    IsDragOn                =0;
    InPlayerUpdate          =false;
    CurrentRenderingDialog  =NULL;
    setAcceptDrops(true);
    ApplicationConfig->ParentWindow=this;
}

void MainWindow::InitWindow(QString ForceLanguage,QApplication *App) {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::InitWindow";
    #endif

    AddToSystemProperties(QString(STARTINGPATH_STR)+AdjustDirForOS(QDir::currentPath()));
    ApplicationConfig->InitConfigurationValues(ForceLanguage,App);

    QSplashScreen screen;
    screen.setPixmap(QPixmap("img/splash.png"));
    screen.show();

    ApplicationConfig->TranslatedRenderType.append(QApplication::translate("DlgRenderVideo","Advanced","Device database type"));           // EXPORTMODE_ADVANCED
    ApplicationConfig->TranslatedRenderType.append(QApplication::translate("DlgRenderVideo","Smartphone","Device database type"));         // EXPORTMODE_SMARTPHONE
    ApplicationConfig->TranslatedRenderType.append(QApplication::translate("DlgRenderVideo","Multimedia system","Device database type"));  // EXPORTMODE_MULTIMEDIASYS
    ApplicationConfig->TranslatedRenderType.append(QApplication::translate("DlgRenderVideo","For the WEB","Device database type"));        // EXPORTMODE_FORTHEWEB
    ApplicationConfig->TranslatedRenderSubtype[EXPORTMODE_SMARTPHONE].append(QApplication::translate("DlgRenderVideo","Smartphone","Device database type"));
    ApplicationConfig->TranslatedRenderSubtype[EXPORTMODE_SMARTPHONE].append(QApplication::translate("DlgRenderVideo","Portable Player","Device database type"));
    ApplicationConfig->TranslatedRenderSubtype[EXPORTMODE_SMARTPHONE].append(QApplication::translate("DlgRenderVideo","Netbook/NetPC","Device database type"));
    ApplicationConfig->TranslatedRenderSubtype[EXPORTMODE_SMARTPHONE].append(QApplication::translate("DlgRenderVideo","Handheld game console","Device database type"));
    ApplicationConfig->TranslatedRenderSubtype[EXPORTMODE_SMARTPHONE].append(QApplication::translate("DlgRenderVideo","Tablet computer","Device database type"));
    ApplicationConfig->TranslatedRenderSubtype[EXPORTMODE_MULTIMEDIASYS].append(QApplication::translate("DlgRenderVideo","Multimedia hard drive and gateway","Device database type"));
    ApplicationConfig->TranslatedRenderSubtype[EXPORTMODE_MULTIMEDIASYS].append(QApplication::translate("DlgRenderVideo","Player","Device database type"));
    ApplicationConfig->TranslatedRenderSubtype[EXPORTMODE_MULTIMEDIASYS].append(QApplication::translate("DlgRenderVideo","ADSL Box","Device database type"));
    ApplicationConfig->TranslatedRenderSubtype[EXPORTMODE_MULTIMEDIASYS].append(QApplication::translate("DlgRenderVideo","Game console","Device database type"));
    ApplicationConfig->TranslatedRenderSubtype[EXPORTMODE_FORTHEWEB].append(QApplication::translate("DlgRenderVideo","SWF Flash Player","Device database type"));
    ApplicationConfig->TranslatedRenderSubtype[EXPORTMODE_FORTHEWEB].append(QApplication::translate("DlgRenderVideo","Video-sharing and social WebSite","Device database type"));
    ApplicationConfig->TranslatedRenderSubtype[EXPORTMODE_FORTHEWEB].append(QApplication::translate("DlgRenderVideo","HTML 5","Device database type"));

    ui->setupUi(this);
    ui->preview->FLAGSTOPITEMSELECTION=&FLAGSTOPITEMSELECTION;
    ui->preview2->FLAGSTOPITEMSELECTION=&FLAGSTOPITEMSELECTION;
    ui->ToolBoxNormal->setCurrentIndex(0);

    AddToSystemProperties(QString(WORKINGPATH_STR)+AdjustDirForOS(QDir::currentPath()));
    AddToSystemProperties(QString(SYSTEMLOCAL_STR)+ApplicationConfig->CurrentLanguage);
    AddToSystemProperties(QString(LOADEDLOCAL_STR)+AdjustDirForOS(QDir().absoluteFilePath(QString("locale")+QDir::separator()+QString("locale_")+ApplicationConfig->CurrentLanguage+".qm")));
    AddSeparatorToSystemProperties();
    Transparent.setTextureImage(QImage("img/transparent.png"));  // Load transparent brush
    AddToSystemProperties(QString(VERSIONQT_STR)+QString(qVersion()));
    AddToSystemProperties(QString(FMTFILTERVERSION_STR)+"0.6.4-Licence=LGPL");

    // Now, we have application settings then we can init SDL
    screen.showMessage(QApplication::translate("MainWindow","Starting SDL..."),Qt::AlignHCenter|Qt::AlignBottom);
    SDLFirstInit(ApplicationConfig->PreviewFPS,ApplicationConfig->SDLAudioOldMode);
    AddToSystemProperties(QString(SDLVERSION_STR)+QString("%1").arg(SDL_MAJOR_VERSION)+"."+QString("%1").arg(SDL_MINOR_VERSION)+"."+QString("%1").arg(SDL_PATCHLEVEL)+"-Licence=GPL version 2.1 or later");

    // Register all formats and codecs for libavformat/libavcodec/etc ...
    screen.showMessage(QApplication::translate("MainWindow","Starting ffmpeg..."),Qt::AlignHCenter|Qt::AlignBottom);
    avcodec_init();
    av_register_all();
    //QString Conf;

    AddToSystemProperties(QString(LIBAVCODECVERSION_STR)+QString("%1").arg(LIBAVCODEC_VERSION_MAJOR)+"."+QString("%1").arg(LIBAVCODEC_VERSION_MINOR)+"."+QString("%1").arg(LIBAVCODEC_VERSION_MICRO)+"."+QString("%1").arg(avcodec_version())+"-Licence="+QString(avcodec_license()));
    //Conf=QString(avcodec_configuration());  Conf.replace(" --","\n  --"); AddToSystemProperties("  "+Conf+"\n");

    AddToSystemProperties(QString(LIBAVFORMATVERSION_STR)+QString("%1").arg(LIBAVFORMAT_VERSION_MAJOR)+"."+QString("%1").arg(LIBAVFORMAT_VERSION_MINOR)+"."+QString("%1").arg(LIBAVFORMAT_VERSION_MICRO)+"."+QString("%1").arg(avformat_version())+"-Licence="+QString(avformat_license()));
    //Conf=QString(avformat_configuration());  Conf.replace(" --","\n  --");  AddToSystemProperties("  "+Conf+"\n");

    AddToSystemProperties(QString(LIBSWSCALEVERSION_STR)+QString("%1").arg(LIBSWSCALE_VERSION_MAJOR)+"."+QString("%1").arg(LIBSWSCALE_VERSION_MINOR)+"."+QString("%1").arg(LIBSWSCALE_VERSION_MICRO)+"."+QString("%1").arg(swscale_version())+"-Licence="+QString(swscale_license()));
    //Conf=QString(swscale_configuration());  Conf.replace(" --","\n  --");   AddToSystemProperties("  "+Conf+"\n");

    // Check codec to know if they was finded
    AVCodec *p=NULL;
    while ((p=av_codec_next(p))) {
        if (p->type==AVMEDIA_TYPE_AUDIO) {
            for (int i=0;i<NBR_AUDIOCODECDEF;i++) if ((p->id==AUDIOCODECDEF[i].Codec_id)&&(p->encode!=NULL)&&(!AUDIOCODECDEF[i].IsFind)) {
                AUDIOCODECDEF[i].IsFind=true;
                strcpy(AUDIOCODECDEF[i].ShortName,p->name);
            }
            if (QString(p->name)==QString("libfaac")) strcpy(AUDIOCODECDEF[2].ShortName,p->name);
        }
        if (p->type==AVMEDIA_TYPE_VIDEO) for (int i=0;i<NBR_VIDEOCODECDEF;i++) if ((p->id==VIDEOCODECDEF[i].Codec_id)&&(p->encode!=NULL)&&(!VIDEOCODECDEF[i].IsFind)) {
            VIDEOCODECDEF[i].IsFind=true;
            strcpy(VIDEOCODECDEF[i].ShortName,p->name);
        }
        if (QString(p->name)==QString("libxvid")) strcpy(VIDEOCODECDEF[2].ShortName,p->name);
    }

    // Check format to know if they was finded
    AVOutputFormat *ofmt=NULL;
    while ((ofmt=av_oformat_next(ofmt))) {
        for (int i=0;i<NBR_FORMATDEF;i++) if (strcmp(ofmt->name,FORMATDEF[i].ShortName)==0) {
            QString     AllowedCodec=FORMATDEF[i].PossibleVideoCodec;
            QString     Codec="";
            int         Index=0;
            bool        IsFindVideoCodec=false;
            bool        IsFindAudioCodec=false;

            while (AllowedCodec.length()>0) {
                Index=AllowedCodec.indexOf("#");
                if (Index>0) {
                    Codec=AllowedCodec.left(Index);
                    AllowedCodec=AllowedCodec.right(AllowedCodec.length()-Index-1);
                } else {
                    Codec=AllowedCodec;
                    AllowedCodec="";
                }
                // Now find index of this codec in the VIDEOCODECDEF
                Index=0;
                while ((Index<NBR_VIDEOCODECDEF)&&(Codec!=QString(VIDEOCODECDEF[Index].FFD_VCODECST))) Index++;
                if ((Index<NBR_VIDEOCODECDEF)&&(VIDEOCODECDEF[Index].IsFind)) IsFindVideoCodec=true;
            }
            AllowedCodec=FORMATDEF[i].PossibleAudioCodec;
            Codec="";
            Index=0;
            while (AllowedCodec.length()>0) {
                Index=AllowedCodec.indexOf("#");
                if (Index>0) {
                    Codec=AllowedCodec.left(Index);
                    AllowedCodec=AllowedCodec.right(AllowedCodec.length()-Index-1);
                } else {
                    Codec=AllowedCodec;
                    AllowedCodec="";
                }
                // Now find index of this codec in the AUDIOCODECDEF
                Index=0;
                while ((Index<NBR_AUDIOCODECDEF)&&(Codec!=QString(AUDIOCODECDEF[Index].ShortName))) Index++;
                if ((Index<NBR_AUDIOCODECDEF)&&(AUDIOCODECDEF[Index].IsFind)) IsFindAudioCodec=true;
            }
            FORMATDEF[i].IsFind=IsFindAudioCodec && IsFindVideoCodec;
        }
    }

    // Display finding codecs & formats
    AddSeparatorToSystemProperties();   AddToSystemProperties(QApplication::translate("MainWindow","Registered video codecs for encoding :"));
    for (int i=0;i<NBR_VIDEOCODECDEF;i++) if (VIDEOCODECDEF[i].IsFind) AddToSystemProperties("  "+QString(VIDEOCODECDEF[i].LongName)+"-ffmpeg codec:"+QString(VIDEOCODECDEF[i].ShortName));
    AddSeparatorToSystemProperties();   AddToSystemProperties(QApplication::translate("MainWindow","Registered audio codecs for encoding :"));
    for (int i=0;i<NBR_AUDIOCODECDEF;i++) if (AUDIOCODECDEF[i].IsFind) AddToSystemProperties("  "+QString(AUDIOCODECDEF[i].LongName)+"-ffmpeg codec:"+QString(AUDIOCODECDEF[i].ShortName));
    AddSeparatorToSystemProperties();   AddToSystemProperties(QApplication::translate("MainWindow","Registered container formats for encoding :"));
    for (int i=0;i<NBR_FORMATDEF;i++)     if (FORMATDEF[i].IsFind) AddToSystemProperties("  "+QString(FORMATDEF[i].LongName));
    AddSeparatorToSystemProperties();   AddToSystemProperties(QString("%1").arg(ApplicationConfig->RenderDeviceModel.count())+QApplication::translate("MainWindow"," Device registered for rendering"));

    AddSeparatorToSystemProperties();   AddToSystemProperties(QApplication::translate("MainWindow","Library :"));
    QString Path;
    screen.showMessage(QApplication::translate("MainWindow","Loading background library..."),Qt::AlignHCenter|Qt::AlignBottom);
    Path="background";      BackgroundList.ScanDisk(Path,GEOMETRY_16_9); AddToSystemProperties(QString("  %1").arg(BackgroundList.List.count())+QApplication::translate("MainWindow"," images loaded into the background-library from ")+QDir(Path).absolutePath());
    screen.showMessage(QApplication::translate("MainWindow","Loading no-luma transitions..."),Qt::AlignHCenter|Qt::AlignBottom);
    for (int i=0;i<TRANSITIONMAXSUBTYPE_BASE;i++)       IconList.List.append(cIconObject(TRANSITIONFAMILLY_BASE,i));
    for (int i=0;i<TRANSITIONMAXSUBTYPE_ZOOMINOUT;i++)  IconList.List.append(cIconObject(TRANSITIONFAMILLY_ZOOMINOUT,i));
    for (int i=0;i<TRANSITIONMAXSUBTYPE_SLIDE;i++)      IconList.List.append(cIconObject(TRANSITIONFAMILLY_SLIDE,i));
    for (int i=0;i<TRANSITIONMAXSUBTYPE_PUSH;i++)       IconList.List.append(cIconObject(TRANSITIONFAMILLY_PUSH,i));
    AddToSystemProperties(QString("  %1").arg(IconList.List.count())+QApplication::translate("MainWindow"," no-luma transitions loaded into the transition-library"));
    screen.showMessage(QApplication::translate("MainWindow","Loading luma transitions..."),Qt::AlignHCenter|Qt::AlignBottom);
    Path="luma/Bar";        LumaList_Bar.ScanDisk(Path,TRANSITIONFAMILLY_LUMA_BAR);         AddToSystemProperties(QString("  %1").arg(LumaList_Bar.List.count())+QApplication::translate("MainWindow"," luma transitions loaded into the transition-library from ")+QDir(Path).absolutePath());
    Path="luma/Box";        LumaList_Box.ScanDisk(Path,TRANSITIONFAMILLY_LUMA_BOX);         AddToSystemProperties(QString("  %1").arg(LumaList_Box.List.count())+QApplication::translate("MainWindow"," luma transitions loaded into the transition-library from ")+QDir(Path).absolutePath());
    Path="luma/Center";     LumaList_Center.ScanDisk(Path,TRANSITIONFAMILLY_LUMA_CENTER);   AddToSystemProperties(QString("  %1").arg(LumaList_Center.List.count())+QApplication::translate("MainWindow"," luma transitions loaded into the transition-library from ")+QDir(Path).absolutePath());
    Path="luma/Checker";    LumaList_Checker.ScanDisk(Path,TRANSITIONFAMILLY_LUMA_CHECKER); AddToSystemProperties(QString("  %1").arg(LumaList_Checker.List.count())+QApplication::translate("MainWindow"," luma transitions loaded into the transition-library from ")+QDir(Path).absolutePath());
    Path="luma/Clock";      LumaList_Clock.ScanDisk(Path,TRANSITIONFAMILLY_LUMA_CLOCK);     AddToSystemProperties(QString("  %1").arg(LumaList_Clock.List.count())+QApplication::translate("MainWindow"," luma transitions loaded into the transition-library from ")+QDir(Path).absolutePath());
    Path="luma/Snake";      LumaList_Snake.ScanDisk(Path,TRANSITIONFAMILLY_LUMA_SNAKE);     AddToSystemProperties(QString("  %1").arg(LumaList_Snake.List.count())+QApplication::translate("MainWindow"," luma transitions loaded into the transition-library from ")+QDir(Path).absolutePath());
    AddToSystemProperties(QApplication::translate("MainWindow","  Total:")+QString("%1").arg(IconList.List.count())+QApplication::translate("MainWindow"," transitions loaded into the transition-library"));

    QFile file("BUILDVERSION.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        CurrentAppVersion=QString(file.readLine());
        if (CurrentAppVersion.endsWith("\n"))   CurrentAppVersion=CurrentAppVersion.left(CurrentAppVersion.length()-QString("\n").length());
        while (CurrentAppVersion.endsWith(" ")) CurrentAppVersion=CurrentAppVersion.left(CurrentAppVersion.length()-1);
        if (CurrentAppVersion.lastIndexOf(" ")) CurrentAppVersion=CurrentAppVersion.mid(CurrentAppVersion.lastIndexOf(" ")+1);
        file.close();
    }

    Diaporama=new cDiaporama(ApplicationConfig);
    Diaporama->Timeline=ui->timeline;
    ui->preview->InitDiaporamaPlay(Diaporama);
    ui->preview2->InitDiaporamaPlay(Diaporama);
    connect(ui->preview,SIGNAL(DoubleClick()),this,SLOT(s_ItemDoubleClicked()));
    connect(ui->preview2,SIGNAL(DoubleClick()),this,SLOT(s_ItemDoubleClicked()));

    ui->ZoomMinusBT->setEnabled(ApplicationConfig->TimelineHeight>TIMELINEMINHEIGH);
    ui->ZoomPlusBT->setEnabled(ApplicationConfig->TimelineHeight<TIMELINEMAXHEIGH);

    // We have finish with the SplashScreen
    screen.hide();

    connect(ui->ToolBoxNormal,SIGNAL(currentChanged(int)),this,SLOT(s_ToolbarChanged(int)));

    // Help menu
    connect(ui->Action_About_BT,SIGNAL(pressed()),this,SLOT(s_About()));                                connect(ui->Action_About_BT_2,SIGNAL(pressed()),this,SLOT(s_About()));
    connect(ui->ActionDocumentation_BT,SIGNAL(pressed()),this,SLOT(s_Documentation()));                 connect(ui->ActionDocumentation_BT_2,SIGNAL(pressed()),this,SLOT(s_Documentation()));
    connect(ui->ActionNewFunctions_BT,SIGNAL(pressed()),this,SLOT(s_NewFunctions()));                   connect(ui->ActionNewFunctions_BT_2,SIGNAL(pressed()),this,SLOT(s_NewFunctions()));

    // File menu
    connect(ui->Action_New_BT,SIGNAL(pressed()),this,SLOT(s_action_New()));                             connect(ui->Action_New_BT_2,SIGNAL(pressed()),this,SLOT(s_action_New()));
    connect(ui->Action_Open_BT,SIGNAL(pressed()),this,SLOT(s_action_Open()));                           connect(ui->Action_Open_BT_2,SIGNAL(pressed()),this,SLOT(s_action_Open()));
    connect(ui->Action_OpenRecent_BT,SIGNAL(pressed()),this,SLOT(s_action_OpenRecent()));               connect(ui->Action_OpenRecent_BT_2,SIGNAL(pressed()),this,SLOT(s_action_OpenRecent()));
    connect(ui->Action_Save_BT,SIGNAL(pressed()),this,SLOT(s_action_Save()));                           connect(ui->Action_Save_BT_2,SIGNAL(pressed()),this,SLOT(s_action_Save()));
    connect(ui->ActionSave_as_BT,SIGNAL(pressed()),this,SLOT(s_action_SaveAs()));                       connect(ui->ActionSave_as_BT_2,SIGNAL(pressed()),this,SLOT(s_action_SaveAs()));
    connect(ui->ActionConfiguration_BT,SIGNAL(pressed()),this,SLOT(s_ChangeApplicationSettings()));     connect(ui->ActionConfiguration_BT_2,SIGNAL(pressed()),this,SLOT(s_ChangeApplicationSettings()));

    connect(ui->Action_Exit_BT,SIGNAL(pressed()),this,SLOT(s_action_Exit()));                           connect(ui->Action_Exit_BT_2,SIGNAL(pressed()),this,SLOT(s_action_Exit()));

    // Project menu
    connect(ui->ActionAdd_BT,SIGNAL(pressed()),this,SLOT(s_action_AddFile()));                          connect(ui->ActionAdd_BT_2,SIGNAL(pressed()),this,SLOT(s_action_AddFile()));
    connect(ui->ActionAddtitle_BT,SIGNAL(pressed()),this,SLOT(s_action_AddTitle()));                    connect(ui->ActionAddtitle_BT_2,SIGNAL(pressed()),this,SLOT(s_action_AddTitle()));
    connect(ui->ActionAddProject_BT,SIGNAL(pressed()),this,SLOT(s_action_AddProject()));                connect(ui->ActionAddProject_BT_2,SIGNAL(pressed()),this,SLOT(s_action_AddProject()));
    connect(ui->ActionRemove_BT,SIGNAL(pressed()),this,SLOT(s_RemoveObject()));                         connect(ui->ActionRemove_BT_2,SIGNAL(pressed()),this,SLOT(s_RemoveObject()));
    connect(ui->ActionCut_BT,SIGNAL(pressed()),this,SLOT(s_CutToClipboard()));                          connect(ui->ActionCut_BT_2,SIGNAL(pressed()),this,SLOT(s_CutToClipboard()));
    connect(ui->ActionCopy_BT,SIGNAL(pressed()),this,SLOT(s_CopyToClipboard()));                        connect(ui->ActionCopy_BT_2,SIGNAL(pressed()),this,SLOT(s_CopyToClipboard()));
    connect(ui->ActionPaste_BT,SIGNAL(pressed()),this,SLOT(s_PasteFromClipboard()));                    connect(ui->ActionPaste_BT_2,SIGNAL(pressed()),this,SLOT(s_PasteFromClipboard()));
    connect(ui->ActionEdit_BT,SIGNAL(pressed()),this,SLOT(s_action_Edit()));                            connect(ui->ActionEdit_BT_2,SIGNAL(pressed()),this,SLOT(s_action_Edit()));

    connect(ui->actionEdit_background,SIGNAL(triggered()),this,SLOT(s_BackgroundDoubleClicked()));
    connect(ui->actionEdit_background_transition,SIGNAL(triggered()),this,SLOT(s_TransitionBackgroundDoubleClicked()));

    connect(ui->actionEdit_object,SIGNAL(triggered()),this,SLOT(s_ItemDoubleClicked()));

    connect(ui->actionEdit_object_in_transition,SIGNAL(triggered()),this,SLOT(s_TransitionItemDoubleClicked()));
    connect(ui->actionEdit_music,SIGNAL(triggered()),this,SLOT(s_MusicDoubleClicked()));

    // Render menu
    connect(ui->ActionRender_BT,SIGNAL(pressed()),this,SLOT(s_RenderVideo()));                          connect(ui->ActionRender_BT_2,SIGNAL(pressed()),this,SLOT(s_RenderVideo()));
    connect(ui->ActionSmartphone_BT,SIGNAL(pressed()),this,SLOT(s_RenderSmartphone()));                 connect(ui->ActionSmartphone_BT_2,SIGNAL(pressed()),this,SLOT(s_RenderSmartphone()));
    connect(ui->ActionMultimedia_BT,SIGNAL(pressed()),this,SLOT(s_RenderMultimedia()));                 connect(ui->ActionMultimedia_BT_2,SIGNAL(pressed()),this,SLOT(s_RenderMultimedia()));
    connect(ui->ActionForTheWEB_BT,SIGNAL(pressed()),this,SLOT(s_RenderForTheWEB()));                   connect(ui->ActionForTheWEB_BT_2,SIGNAL(pressed()),this,SLOT(s_RenderForTheWEB()));

    // Timeline
    connect(ui->ZoomPlusBT,SIGNAL(pressed()),this,SLOT(s_action_ZoomPlus()));
    connect(ui->ZoomMinusBT,SIGNAL(pressed()),this,SLOT(s_action_ZoomMinus()));
    connect(ui->timeline,SIGNAL(itemSelectionChanged()),this,SLOT(s_ItemSelectionChanged()));
    connect(ui->timeline,SIGNAL(DragMoveItem()),this,SLOT(s_DragMoveItem()));

    connect(ui->PartitionBT,SIGNAL(pressed()),this,SLOT(s_ChPartitionMode()));
    connect(ui->Partition2BT,SIGNAL(pressed()),this,SLOT(s_ChPartitionMode()));
    connect(ui->TABTooltip,SIGNAL(linkActivated(const QString)),this,SLOT(s_TABTooltipLink(const QString)));

    // Prepare title bar depending on running version
    TitleBar=QString(APPLICATION_NAME)+QString(" ")+QString(APPLICATION_VERSION);
    if ((TitleBar.indexOf("devel")!=-1)||(TitleBar.indexOf("beta")!=-1)) TitleBar=TitleBar+QString(" - ")+CurrentAppVersion;

    ApplicationConfig->MainWinWSP->ApplyToWindow(this);     // Restore window position
    SetTimelineHeight();                                    // setup initial size
    ui->StatusBar_SlideNumber->setText(QApplication::translate("MainWindow","Slide : ")+"0 / 0");
    s_ToolbarChanged(0);
    ToStatusBar("");
    SetModifyFlag(false);                                   // Setup title window and do first RefreshControls();

    if (ApplicationConfig->CheckConfigAtStartup) QTimer::singleShot(500,this,SLOT(s_DlgCheckConfig())); else {
        QString Status;
        if ((!CheckExiv2(Status))||(!Checkffmpeg(Status))) QTimer::singleShot(500,this,SLOT(s_DlgCheckConfig()));
    }
}

//====================================================================================================================

MainWindow::~MainWindow() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::~MainWindow";
    #endif
    delete Diaporama;
    delete ApplicationConfig;
    SDLLastClose();
    if (Clipboard_Block) {
        delete Clipboard_Block;
        Clipboard_Block=NULL;
    }
    if (Clipboard_Object) {
        delete Clipboard_Object;
        Clipboard_Object=NULL;
    }
    delete ui;
}

//====================================================================================================================
// A REVOIR !!
void MainWindow::keyReleaseEvent(QKeyEvent *event) {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::keyReleaseEvent";
    #endif
    bool Find=false;

    if ((!ApplicationConfig->PartitionMode)&&(ui->ToolBoxNormal->currentIndex()!=0)) {
        Find=true;
        if (event->matches(QKeySequence::Quit))         s_action_Exit();
        else if (event->matches(QKeySequence::New))     s_action_New();
        else if (event->matches(QKeySequence::Open))    s_action_Open();
        else if (event->matches(QKeySequence::Save))    s_action_Save();
        else if (event->matches(QKeySequence::SaveAs))  s_action_SaveAs();
        else Find=false;
    }
    if ((!Find)&&(!ApplicationConfig->PartitionMode)&&(ui->ToolBoxNormal->currentIndex()!=1)) {
        Find=true;
        if (event->matches(QKeySequence::Copy))         s_CopyToClipboard();
        else if (event->matches(QKeySequence::Cut))     s_CutToClipboard();
        else if (event->matches(QKeySequence::Paste))   s_PasteFromClipboard();
        else if (event->matches(QKeySequence::Delete))  s_RemoveObject();
        //else if (event->matches(QKeySequence::ZoomIn))  s_action_ZoomPlus();
        //else if (event->matches(QKeySequence::ZoomOut)) s_action_ZoomMinus();
        else if (event->key()==Qt::Key_Insert)          s_action_AddFile();
        else Find=false;
    }
    if ((!Find)&&(!ApplicationConfig->PartitionMode)&&(ui->ToolBoxNormal->currentIndex()!=3)) {
        Find=true;
        if (event->key()==Qt::Key_F1)                   s_Documentation();
        else Find=false;
    }
    if (!Find) {
        Find=true;
        if (event->key()==Qt::Key_F5)                   s_BackgroundDoubleClicked();
        else if (event->key()==Qt::Key_F6)              s_ItemDoubleClicked();
        else if (event->key()==Qt::Key_F7)              s_MusicDoubleClicked();
        else if (event->key()==Qt::Key_F8)              s_TransitionItemDoubleClicked();
        else Find=false;
    }

    if (!Find) QMainWindow::keyReleaseEvent(event);
}

//====================================================================================================================

void MainWindow::ToStatusBar(QString Text) {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::ToStatusBar";
    #endif
    if (Text=="") {
        if (InternetBUILDVERSION!="") ui->StatusBar_General->setText(InternetBUILDVERSION); else ui->StatusBar_General->setText("");
    } else {
        ui->StatusBar_General->setText(Text);
    }
}

//====================================================================================================================

void MainWindow::SetTimelineHeight() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::SetTimelineHeight";
    #endif
    if (!ApplicationConfig->PartitionMode) {
        ui->scrollArea->setVisible(true);
        ui->ToolBoxPartition->setVisible(false);
        ui->ToolBoxNormal->setVisible(true);
        ui->preview->setVisible(true);
        ui->preview2->setVisible(false);
        ui->TABTooltip->setVisible(true);
        ui->TABToolimg->setVisible(true);
        ui->PartitionBT->setEnabled(false);
        ui->PartitionBT->setDown(true);
        ui->Partition2BT->setEnabled(true);
        QApplication::processEvents();          // Give time to Qt to redefine position of each control and preview height !
        ui->preview->setFixedWidth(Diaporama->GetWidthForHeight(ui->preview->height()-32));
    } else {
        ui->scrollArea->setVisible(false);
        ui->ToolBoxPartition->setVisible(true);
        ui->ToolBoxNormal->setVisible(false);
        ui->preview->setVisible(false);
        ui->preview2->setVisible(true);
        ui->preview2->setFixedWidth(Diaporama->GetWidthForHeight(ui->preview2->height()-32));
        ui->TABTooltip->setVisible(false);
        ui->TABToolimg->setVisible(false);
        ui->PartitionBT->setEnabled(true);
        ui->Partition2BT->setEnabled(false);
        ui->Partition2BT->setDown(true);
    }
    QApplication::processEvents();          // Give time to Qt to redefine position of each control and redraw timeline
    ui->timeline->SetTimelineHeight(ApplicationConfig->PartitionMode);
    QApplication::processEvents();          // Give time to Qt to redefine position of each control and redraw timeline
}

//====================================================================================================================

void MainWindow::closeEvent(QCloseEvent *Event) {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::closeEvent";
    #endif
    ui->preview->SetPlayerToPause(); // Ensure player is stop
    ui->preview2->SetPlayerToPause(); // Ensure player is stop
    if (Diaporama->IsModify) {
        int Bt=QMessageBox::question(this,QApplication::translate("MainWindow","Close application"),QApplication::translate("MainWindow","Want to save the project before closing?"),
        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes);
        if (Bt==QMessageBox::Yes) s_action_Save();
        if (Bt==QMessageBox::Cancel) {
            Event->setAccepted(false);
            return;
        }
    }
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    ApplicationConfig->MainWinWSP->SaveWindowState(this);
    ApplicationConfig->SaveConfigurationFile();
    QApplication::restoreOverrideCursor();
}

//====================================================================================================================

void MainWindow::resizeEvent(QResizeEvent *) {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::resizeEvent";
    #endif
    ui->preview->SetPlayerToPause(); // Ensure player is stop
    ui->preview2->SetPlayerToPause(); // Ensure player is stop
    SetTimelineHeight();
}

//====================================================================================================================

void MainWindow::showEvent(QShowEvent *) {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::showEvent";
    #endif
    if (!IsFirstInitDone) {
        IsFirstInitDone=true;                                   // do this only one time
        // Start a network process to give last ffdiaporama version from internet web site
        QNetworkAccessManager *mNetworkManager=new QNetworkAccessManager(this);
        connect(mNetworkManager,SIGNAL(finished(QNetworkReply*)),this,SLOT(onNetworkReply(QNetworkReply*)));
        QUrl            url(BUILDVERSION_WEBURL);
        QNetworkReply   *reply  = mNetworkManager->get(QNetworkRequest(url));
        reply->deleteLater();
    }
}

//====================================================================================================================
// Function use when reading BUILDVERSION from WEB Site
//====================================================================================================================

void MainWindow::onNetworkReply(QNetworkReply* reply) {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::onNetworkReply";
    #endif
    if (reply->error()==QNetworkReply::NoError) {
        int httpstatuscode=reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toUInt();
        if ((httpstatuscode>=200)&&(httpstatuscode<300)&&(reply->isReadable())) {
            InternetBUILDVERSION=QString::fromUtf8(reply->readAll().data());
            if (InternetBUILDVERSION.endsWith("\n"))   InternetBUILDVERSION=InternetBUILDVERSION.left(InternetBUILDVERSION.length()-QString("\n").length());
            while (InternetBUILDVERSION.endsWith(" ")) InternetBUILDVERSION=InternetBUILDVERSION.left(InternetBUILDVERSION.length()-1);
            if (InternetBUILDVERSION.lastIndexOf(" ")) InternetBUILDVERSION=InternetBUILDVERSION.mid(InternetBUILDVERSION.lastIndexOf(" ")+1);
            int CurrentVersion =CurrentAppVersion.toInt();
            int InternetVersion=InternetBUILDVERSION.toInt();
            if (InternetVersion>CurrentVersion) InternetBUILDVERSION=QApplication::translate("MainWindow","A new ffDiaporama release is available from WEB site. Please update from http://ffdiaporama.tuxfamily.org !");
                else InternetBUILDVERSION="";
        } else InternetBUILDVERSION="";
    } else InternetBUILDVERSION="";
    ToStatusBar(InternetBUILDVERSION);
}

//====================================================================================================================

void MainWindow::OpenHelp(QString HelpFile) {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::OpenHelp";
    #endif
    if (HelpFile.startsWith(("file://"))) {
        QDesktopServices::openUrl(QUrl(HelpFile));
    } else if (HelpFile.startsWith(("http://"))) {
        QString HelpPath;
        if ((ApplicationConfig->CurrentLanguage!="fr")&&(ApplicationConfig->CurrentLanguage!="es")&&(ApplicationConfig->CurrentLanguage!="it")&&(ApplicationConfig->CurrentLanguage!="en")) HelpPath=HelpFile.replace("<local>","en");
            else HelpPath=HelpFile.replace("<local>",ApplicationConfig->CurrentLanguage);
        QDesktopServices::openUrl(QUrl(HelpPath));
    } else {
        QString HelpPath;
        if ((ApplicationConfig->CurrentLanguage!="fr")&&(ApplicationConfig->CurrentLanguage!="es")&&(ApplicationConfig->CurrentLanguage!="it")&&(ApplicationConfig->CurrentLanguage!="en")) HelpPath=WIKI_CMS_PATH+HelpFile+"&lang=en";
            else HelpPath=WIKI_CMS_PATH+HelpFile+"&lang="+ApplicationConfig->CurrentLanguage;
        QDesktopServices::openUrl(QUrl(HelpPath));
    }
}

//====================================================================================================================

void MainWindow::RefreshControls() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::RefreshControls";
    #endif
    // Timeline actions
    ui->ActionRemove_BT->setEnabled(ui->timeline->NbrItem()>0);                                             ui->ActionRemove_BT_2->setEnabled(ui->timeline->NbrItem()>0);
    ui->ActionEdit_BT->setEnabled(ui->timeline->NbrItem()>0);                                               ui->ActionEdit_BT_2->setEnabled(ui->timeline->NbrItem()>0);
    ui->ZoomMinusBT->setEnabled((ui->timeline->NbrItem()>0)&&(ApplicationConfig->TimelineHeight>TIMELINEMINHEIGH));
    ui->ZoomPlusBT->setEnabled((ui->timeline->NbrItem()>0)&&(ApplicationConfig->TimelineHeight<TIMELINEMAXHEIGH));

    // File menu
    ui->Action_Save_BT->setEnabled(Diaporama->IsModify);                                                    ui->Action_Save_BT_2->setEnabled(Diaporama->IsModify);
    ui->ActionSave_as_BT->setEnabled(Diaporama->List.count()>0);                                            ui->ActionSave_as_BT_2->setEnabled(Diaporama->List.count()>0);
    ui->Action_OpenRecent_BT->setEnabled(ApplicationConfig->RecentFile.count()>0);                          ui->Action_OpenRecent_BT_2->setEnabled(ApplicationConfig->RecentFile.count()>0);

    // Project menu
    ui->actionEdit_background->setEnabled(ui->timeline->NbrItem()>0);
    ui->actionEdit_background_transition->setEnabled(ui->timeline->NbrItem()>0);
    ui->actionEdit_object->setEnabled(ui->timeline->NbrItem()>0);
    ui->actionEdit_object_in_transition->setEnabled(ui->timeline->NbrItem()>0);
    ui->actionEdit_music->setEnabled(ui->timeline->NbrItem()>0);

    // Clipboard_Object
    ui->ActionPaste_BT->setEnabled(Clipboard_Object!=NULL);                                                 ui->ActionPaste_BT_2->setEnabled(Clipboard_Object!=NULL);
    ui->ActionCopy_BT->setEnabled(ui->timeline->CurrentSelected()>=0);                                      ui->ActionCopy_BT_2->setEnabled(ui->timeline->CurrentSelected()>=0);
    ui->ActionCut_BT->setEnabled(ui->timeline->CurrentSelected()>=0);                                       ui->ActionCut_BT_2->setEnabled(ui->timeline->CurrentSelected()>=0);

    ui->ActionRender_BT->setEnabled(ui->timeline->NbrItem()>0);                                             ui->ActionRender_BT_2->setEnabled(ui->timeline->NbrItem()>0);
    ui->ActionSmartphone_BT->setEnabled(ui->timeline->NbrItem()>0);                                         ui->ActionSmartphone_BT_2->setEnabled(ui->timeline->NbrItem()>0);
    ui->ActionMultimedia_BT->setEnabled(ui->timeline->NbrItem()>0);                                         ui->ActionMultimedia_BT_2->setEnabled(ui->timeline->NbrItem()>0);
    ui->ActionForTheWEB_BT->setEnabled(ui->timeline->NbrItem()>0);                                          ui->ActionForTheWEB_BT_2->setEnabled(ui->timeline->NbrItem()>0);

    ui->StatusBar_SlideNumber->setText(QApplication::translate("MainWindow","Slide : ")+QString("%1").arg(Diaporama->CurrentCol+(Diaporama->List.count()>0?1:0))+" / "+QString("%1").arg(Diaporama->List.count()));
}

//====================================================================================================================

void MainWindow::SetModifyFlag(bool IsModify) {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::SetModifyFlag";
    #endif
    Diaporama->IsModify=IsModify;
    this->setWindowTitle(TitleBar+QString("-")+
                         (Diaporama->ProjectFileName!=""?Diaporama->ProjectFileName:QApplication::translate("MainWindow","<new project>","when project have no name define"))+
                         (Diaporama->IsModify?"*":""));
    RefreshControls();
}

//====================================================================================================================

void MainWindow::s_About() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_About";
    #endif
    ui->Action_About_BT->setDown(false);
    ui->Action_About_BT_2->setDown(false);
    DlgAbout(this).exec();
}

//====================================================================================================================

void MainWindow::s_DlgCheckConfig() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_DlgCheckConfig";
    #endif
    DlgCheckConfig(this).exec();
    QString Status;
    if ((!CheckExiv2(Status))||(!Checkffmpeg(Status))) {
        QMessageBox::critical(this,"ffDiaporama",QApplication::translate("MainWindow","Configuration not correct!"));
        close();
    }
}

//====================================================================================================================

void MainWindow::s_Documentation() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_Documentation";
    #endif
    ui->ActionDocumentation_BT->setDown(false);
    ui->ActionDocumentation_BT_2->setDown(false);
    OpenHelp(HELPFILE_SUPPORT);
}

//====================================================================================================================

void MainWindow::s_NewFunctions() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_NewFunctions";
    #endif
    ui->ActionNewFunctions_BT->setDown(false);
    ui->ActionNewFunctions_BT_2->setDown(false);
    OpenHelp(HELPFILE_NEWS);
}

//====================================================================================================================

void MainWindow::s_action_Exit() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_action_Exit";
    #endif
    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(500,this,SLOT(s_action_Exit()));
        return;
    }

    close();
}

//====================================================================================================================

void MainWindow::s_action_ZoomPlus() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_action_ZoomPlus";
    #endif
    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(500,this,SLOT(s_action_ZoomPlus()));
        return;
    }

    if ((ui->timeline->rowHeight(0)-ApplicationConfig->TimelineHeight/2-TIMELINESOUNDHEIGHT*2)<TIMELINEMAXHEIGH) {
        ApplicationConfig->TimelineHeight+=20;
        SetTimelineHeight();
    }
    RefreshControls();
}

//====================================================================================================================

void MainWindow::s_action_ZoomMinus() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_action_ZoomMinus";
    #endif
    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(500,this,SLOT(s_action_ZoomMinus()));
        return;
    }

    if ((ui->timeline->rowHeight(0)-ApplicationConfig->TimelineHeight/2-TIMELINESOUNDHEIGHT*2)>TIMELINEMINHEIGH) {
        ApplicationConfig->TimelineHeight-=20;
        SetTimelineHeight();
    }
    RefreshControls();
}

//====================================================================================================================

void MainWindow::s_ChPartitionMode() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_ChPartitionMode";
    #endif
    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(500,this,SLOT(s_ChPartitionMode()));
        return;
    }

    int Selected=ui->timeline->CurrentSelected(); // Save current seleted item
    ApplicationConfig->PartitionMode=!ApplicationConfig->PartitionMode;
    SetTimelineHeight();
    // Re-select previous current seleted item
    if ((Selected>=0)&&(Selected<ui->timeline->NbrItem())) ui->timeline->SetCurrentCell(Selected);
    (ApplicationConfig->PartitionMode?ui->preview2:ui->preview)->SeekPlayer(Diaporama->GetObjectStartPosition(Diaporama->CurrentCol)+Diaporama->GetTransitionDuration(Diaporama->CurrentCol));
    (ApplicationConfig->PartitionMode?ui->preview2:ui->preview)->Resize();
}

//====================================================================================================================
// Double click on widget in the object track
//====================================================================================================================

void MainWindow::s_ItemDoubleClicked() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_ItemDoubleClicked";
    #endif
    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(500,this,SLOT(s_ItemDoubleClicked()));
        return;
    }

    bool DoneAgain=true;
    while (DoneAgain) {
        DoneAgain=false;
        int Ret=DlgSlideProperties(&(Diaporama->List[Diaporama->CurrentCol]),this).exec();
        if (Ret!=1) {
            SetModifyFlag(true);
            if (Diaporama->List[Diaporama->CurrentCol].Thumbnail) {
                delete Diaporama->List[Diaporama->CurrentCol].Thumbnail;
                Diaporama->List[Diaporama->CurrentCol].Thumbnail=NULL;
            }
            (ApplicationConfig->PartitionMode?ui->preview2:ui->preview)->SeekPlayer(Diaporama->GetObjectStartPosition(Diaporama->CurrentCol)+Diaporama->GetTransitionDuration(Diaporama->CurrentCol));
            AdjustRuller();
        }
        if ((Ret==2)||(Ret==3)) {
            Diaporama->CurrentCol=Diaporama->CurrentCol+((Ret==2)?-1:1);
            Diaporama->Timeline->SetCurrentCell(Diaporama->CurrentCol);

            // Update slider mark
            if (Diaporama->List.count()>0)
                (ApplicationConfig->PartitionMode?ui->preview2:ui->preview)->SetStartEndPos(
                        Diaporama->GetObjectStartPosition(Diaporama->CurrentCol),                                                               // Current slide
                        Diaporama->List[Diaporama->CurrentCol].GetDuration(),
                        (Diaporama->CurrentCol>0)?Diaporama->GetObjectStartPosition(Diaporama->CurrentCol-1):((Diaporama->CurrentCol==0)?0:-1), // Previous slide
                        (Diaporama->CurrentCol>0)?Diaporama->List[Diaporama->CurrentCol-1].GetDuration():((Diaporama->CurrentCol==0)?Diaporama->GetTransitionDuration(Diaporama->CurrentCol):0),
                        Diaporama->CurrentCol<(Diaporama->List.count()-1)?Diaporama->GetObjectStartPosition(Diaporama->CurrentCol+1):-1,        // Next slide
                        Diaporama->CurrentCol<(Diaporama->List.count()-1)?Diaporama->List[Diaporama->CurrentCol+1].GetDuration():0);
            else (ApplicationConfig->PartitionMode?ui->preview2:ui->preview)->SetStartEndPos(0,0,-1,0,-1,0);
            // open dialog again
            DoneAgain=true;
        }
    }
}

//====================================================================================================================
// Double click on transition part of widget in the object track
//====================================================================================================================

void MainWindow::s_TransitionItemDoubleClicked() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_TransitionItemDoubleClicked";
    #endif
    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(500,this,SLOT(s_TransitionItemDoubleClicked()));
        return;
    }

    if (DlgTransitionProperties(&(Diaporama->List[Diaporama->CurrentCol]),false,this).exec()==0) {
        SetModifyFlag(true);
        (ApplicationConfig->PartitionMode?ui->preview2:ui->preview)->SeekPlayer(Diaporama->GetObjectStartPosition(Diaporama->CurrentCol)+Diaporama->GetTransitionDuration(Diaporama->CurrentCol));
        AdjustRuller();
    }
}

//====================================================================================================================
// Double click on sound part of widget in the object track
//====================================================================================================================

void MainWindow::s_SoundItemDoubleClicked() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_SoundItemDoubleClicked";
    #endif
    s_ItemDoubleClicked();  // No separated process at this time !
}

//====================================================================================================================
// // Double click on widget in the background track
//====================================================================================================================

void MainWindow::s_BackgroundDoubleClicked() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_BackgroundDoubleClicked";
    #endif
    if (Diaporama->CurrentCol>=Diaporama->List.count()) return;
    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(500,this,SLOT(s_BackgroundDoubleClicked()));
        return;
    }

    if (DlgBackgroundProperties(&(Diaporama->List[Diaporama->CurrentCol]),this).exec()==0) {
        SetModifyFlag(true);
        (ApplicationConfig->PartitionMode?ui->preview2:ui->preview)->SeekPlayer(Diaporama->GetObjectStartPosition(Diaporama->CurrentCol)+Diaporama->GetTransitionDuration(Diaporama->CurrentCol));
        AdjustRuller();
    }
}

//====================================================================================================================
// Double click on transition part of widget in the background track
//====================================================================================================================

void MainWindow::s_TransitionBackgroundDoubleClicked() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_TransitionBackgroundDoubleClicked";
    #endif
    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(500,this,SLOT(s_TransitionBackgroundDoubleClicked()));
        return;
    }

//    QMessageBox::critical(this,QApplication::translate("MainWindow","Not implemented"),QApplication::translate("MainWindow","Sorry, not yet done !"));
/*
    if (DlgTransitionProperties(&(Diaporama->List[Diaporama->CurrentCol]),true,this).exec()==0) {
        SetModifyFlag(true);
        (ApplicationConfig->PartitionMode?ui->preview2:ui->preview)->SeekPlayer(Diaporama->GetObjectStartPosition(Diaporama->CurrentCol)+Diaporama->GetTransitionDuration(Diaporama->CurrentCol));
        AdjustRuller();
    }
*/
}

//====================================================================================================================
// // Double click on widget in the music track
//====================================================================================================================

void MainWindow::s_MusicDoubleClicked() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_MusicDoubleClicked";
    #endif
    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(500,this,SLOT(s_MusicDoubleClicked()));
        return;
    }

    if (DlgMusicProperties(&(Diaporama->List[Diaporama->CurrentCol]),this).exec()==0) {
        SetModifyFlag(true);
        (ApplicationConfig->PartitionMode?ui->preview2:ui->preview)->SeekPlayer(Diaporama->GetObjectStartPosition(Diaporama->CurrentCol)+Diaporama->GetTransitionDuration(Diaporama->CurrentCol));
        AdjustRuller();
    }
}

//====================================================================================================================

void MainWindow::s_DragMoveItem() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_DragMoveItem";
    #endif
    if (DragItemSource<DragItemDest) DragItemDest--;
    Diaporama->List.move(DragItemSource,DragItemDest);
    ui->timeline->setUpdatesEnabled(false);
    ui->timeline->SetCurrentCell(DragItemDest);
    ui->timeline->setUpdatesEnabled(true);  // Reset timeline painting
}

//====================================================================================================================
// Current diaporama object selection changed
//====================================================================================================================

void MainWindow::s_ItemSelectionChanged() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_ItemSelectionChanged";
    #endif
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(500,this,SLOT(s_ItemSelectionChanged()));
        return;
    }
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    if (!FLAGSTOPITEMSELECTION) {
        int Selected=ui->timeline->CurrentSelected();

        if (Selected>=Diaporama->List.count()) {
            Selected=Diaporama->List.count()-1;
            FLAGSTOPITEMSELECTION=true;
            ui->timeline->SetCurrentCell(Selected);
            FLAGSTOPITEMSELECTION=false;
        }
        if (Selected<0) {
            Selected=0;
            FLAGSTOPITEMSELECTION=true;
            ui->timeline->SetCurrentCell(Selected);
            FLAGSTOPITEMSELECTION=false;
        }
        if (Diaporama->CurrentCol<0) Diaporama->CurrentCol=0;

        if ((Diaporama->CurrentCol!=Selected)||((Diaporama->List.count()==1)&&(Diaporama->CurrentCol==0))) {
            //************************************************************
            // We are here only if user has click on the timeline
            //************************************************************

            if (Diaporama->List.count()>0) {
                ui->preview->SetPlayerToPause();    // Ensure player is stop
                ui->preview2->SetPlayerToPause();   // Ensure player is stop
                Diaporama->CurrentCol=Selected;
                Diaporama->CurrentPosition=Diaporama->GetObjectStartPosition(Diaporama->CurrentCol)+Diaporama->GetTransitionDuration(Diaporama->CurrentCol);
                if (Diaporama->List[Diaporama->CurrentCol].GetTransitDuration()>0) Diaporama->CurrentPosition--;
                AdjustRuller();
            } else {
                (ApplicationConfig->PartitionMode?ui->preview2:ui->preview)->SeekPlayer(0);
                (ApplicationConfig->PartitionMode?ui->preview2:ui->preview)->SetStartEndPos(0,0,-1,0,-1,0);
            }
        }
        Diaporama->FreeUnusedMemory(Diaporama->CurrentCol,Diaporama->ApplicationConfig->NbrSlideInCache);
        RefreshControls();
    }
    QApplication::restoreOverrideCursor();
}

//====================================================================================================================
// Update dock informations
//====================================================================================================================

void MainWindow::s_TABTooltipLink(const QString Link) {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_TABTooltipLink";
    #endif
    OpenHelp(Link);
}

void MainWindow::s_ToolbarChanged(int MenuIndex) {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_ToolbarChanged";
    #endif
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    QString Html;
    switch (MenuIndex) {
    case 0: Html=QApplication::translate("MainWindow","<html><body>Select a project to open or to create a new project<br>"\
                                         "To discover ffDiaporama:<br><a href=\"1567\">Consult the WIKI</a></body></html>");
            break;
    case 1: Html=QApplication::translate("MainWindow","<html><body>Add empty slides or slides based on photos or videos<br>"\
                                         "To discover how to build your slide show and to animate slides:<br><a href=\"1306\">Discover the principles of functioning of ffDiaporama</a></body></html>");
            break;
    case 2: Html=QApplication::translate("MainWindow","<html><body>Select the equipment type that you plan to use for your video<br>"\
                                         "To discover how to render videos:<br><a href=\"1322\">Consult the rendering videos WIKI page</a></body></html>");
            break;
    case 3: Html=QApplication::translate("MainWindow","<html><body>Visit the ffDiaporama Web site to use the forum,<br>"\
                "consult tutorials and learn the lastest news:<br><a href=\"http://ffdiaporama.tuxfamily.org\">http://ffdiaporama.tuxfamily.org</a></body></html>");
            break;
    }

    ui->TABTooltip->setText(Html);
    QApplication::restoreOverrideCursor();
}

//====================================================================================================================
// Render project
//====================================================================================================================

void MainWindow::s_RenderVideo() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_RenderVideo";
    #endif
    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(500,this,SLOT(s_RenderVideo()));
        return;
    }
    ui->ActionRender_BT->setDown(false);
    ui->ActionRender_BT_2->setDown(false);

    DlgRenderVideo(*Diaporama,EXPORTMODE_ADVANCED,this).exec();
    CurrentRenderingDialog=NULL;
    AdjustRuller();
}

void MainWindow::s_RenderSmartphone() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_RenderSmartphone";
    #endif
    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(500,this,SLOT(s_RenderSmartphone()));
        return;
    }
    ui->ActionSmartphone_BT->setDown(false);
    ui->ActionSmartphone_BT_2->setDown(false);

    DlgRenderVideo(*Diaporama,EXPORTMODE_SMARTPHONE,this).exec();
    CurrentRenderingDialog=NULL;
    AdjustRuller();
}

void MainWindow::s_RenderMultimedia() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_RenderMultimedia";
    #endif
    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(500,this,SLOT(s_RenderMultimedia()));
        return;
    }
    ui->ActionMultimedia_BT->setDown(false);
    ui->ActionMultimedia_BT_2->setDown(false);

    DlgRenderVideo(*Diaporama,EXPORTMODE_MULTIMEDIASYS,this).exec();
    CurrentRenderingDialog=NULL;
    AdjustRuller();
}

void MainWindow::s_RenderForTheWEB() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_RenderForTheWEB";
    #endif
    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(500,this,SLOT(s_RenderForTheWEB()));
        return;
    }
    ui->ActionForTheWEB_BT->setDown(false);
    ui->ActionForTheWEB_BT_2->setDown(false);

    DlgRenderVideo(*Diaporama,EXPORTMODE_FORTHEWEB,this).exec();
    CurrentRenderingDialog=NULL;
    AdjustRuller();
}

//====================================================================================================================
// Change application settings
//====================================================================================================================

void MainWindow::s_ChangeApplicationSettings() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_ChangeApplicationSettings";
    #endif
    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(500,this,SLOT(s_ChangeApplicationSettings()));
        return;
    }
    ui->ActionConfiguration_BT->setDown(false);
    ui->ActionConfiguration_BT_2->setDown(false);

    if (DlgApplicationSettings(*ApplicationConfig,this).exec()==0) {
        ToStatusBar(QApplication::translate("MainWindow","Saving configuration file and applying new configuration ..."));
        QTimer::singleShot(500,this,SLOT(s_DoChangeApplicationSettings()));
    }
}

void MainWindow::s_DoChangeApplicationSettings() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_DoChangeApplicationSettings";
    #endif
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    ui->preview->WantedFPS=ApplicationConfig->PreviewFPS;
    ui->preview2->WantedFPS=ApplicationConfig->PreviewFPS;
    SDLSetFPS(ApplicationConfig->PreviewFPS,ApplicationConfig->SDLAudioOldMode);  // Reinit SDL if Preview FPS has changed
    // Save configuration
    ApplicationConfig->MainWinWSP->SaveWindowState(this);
    ApplicationConfig->SaveConfigurationFile();
    QApplication::restoreOverrideCursor();
    ToStatusBar("");
}

//====================================================================================================================
// New project
//====================================================================================================================

void MainWindow::s_action_New() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_action_New";
    #endif
    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(500,this,SLOT(s_action_New()));
        return;
    }
    ui->Action_New_BT->setDown(false);
    ui->Action_New_BT_2->setDown(false);

    if ((Diaporama->IsModify)&&(QMessageBox::question(this,QApplication::translate("MainWindow","New project"),QApplication::translate("MainWindow","Current project has been modified.\nDo you want to save-it ?"),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes)) s_action_Save();

    ImagesCache.List.clear();
    cDiaporama *NewDiaporama=new cDiaporama(ApplicationConfig);

    // Clean actual timeline and diaporama
    FLAGSTOPITEMSELECTION=true;
    ui->timeline->setUpdatesEnabled(false);
    ui->timeline->CleanAll();
    delete Diaporama;
    Diaporama=NULL;
    ui->timeline->setUpdatesEnabled(true);
    FLAGSTOPITEMSELECTION=false;

    // Create new diaporama
    Diaporama=NewDiaporama;
    BackgroundList.ScanDisk("background",Diaporama->ImageGeometry);
    Diaporama->Timeline=ui->timeline;
    ui->preview->InitDiaporamaPlay(Diaporama);
    ui->preview->SetActualDuration(Diaporama->GetDuration());
    ui->preview->SetStartEndPos(0,0,-1,0,-1,0);
    ui->preview2->InitDiaporamaPlay(Diaporama);
    ui->preview2->SetActualDuration(Diaporama->GetDuration());
    ui->preview2->SetStartEndPos(0,0,-1,0,-1,0);
    SetTimelineHeight();
    RefreshControls();
    SetModifyFlag(false);
}

//====================================================================================================================
// Open an existing project
//====================================================================================================================

void MainWindow::s_action_OpenRecent() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_action_OpenRecent";
    #endif
    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(500,this,SLOT(s_action_OpenRecent()));
        return;
    }
    QMenu *ContextMenu=new QMenu(this);
    for (int i=ApplicationConfig->RecentFile.count()-1;i>=0;i--) ContextMenu->addAction(AdjustDirForOS(ApplicationConfig->RecentFile.at(i)));
    QAction *Action=ContextMenu->exec(QCursor::pos());
    QString Selected="";
    if (Action) Selected=Action->iconText();
    delete ContextMenu;
    ui->Action_OpenRecent_BT->setDown(false);
    ui->Action_OpenRecent_BT_2->setDown(false);

    if (Selected!="") {
        ToStatusBar(QApplication::translate("MainWindow","Open file :")+QFileInfo(Selected).fileName());
        FileForIO=Selected;
        QTimer::singleShot(500,this,SLOT(s_DoOpenFile()));
    }
}

void MainWindow::s_action_Open() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_action_Open";
    #endif
    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(500,this,SLOT(s_action_Open()));
        return;
    }
    ui->Action_Open_BT->setDown(false);
    ui->Action_Open_BT_2->setDown(false);

    if ((Diaporama->IsModify)&&(QMessageBox::question(this,QApplication::translate("MainWindow","Open project"),QApplication::translate("MainWindow","Current project has been modified.\nDo you want to save-it ?"),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes)) s_action_Save();

    QString ProjectFileName=QFileDialog::getOpenFileName(this,QApplication::translate("MainWindow","Open project"),ApplicationConfig->LastProjectPath,QString("ffDiaporama (*.ffd)"));
    if (ProjectFileName!="") {
        ToStatusBar(QApplication::translate("MainWindow","Open file :")+QFileInfo(ProjectFileName).fileName());
        FileForIO=ProjectFileName;
        QTimer::singleShot(500,this,SLOT(s_DoOpenFile()));
    }
}

void MainWindow::s_DoOpenFileParam() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_DoOpenFileParam";
    #endif
    ToStatusBar(QApplication::translate("MainWindow","Open file :")+QFileInfo(FileForIO).fileName());
    QTimer::singleShot(500,this,SLOT(s_DoOpenFile()));
}

void MainWindow::s_DoOpenFile() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_DoOpenFile";
    #endif
    QString ProjectFileName=FileForIO;
    ProjectFileName=AdjustDirForOS(ProjectFileName);
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    ImagesCache.List.clear();

    // Manage Recent files list
    for (int i=0;i<ApplicationConfig->RecentFile.count();i++) if (AdjustDirForOS(ApplicationConfig->RecentFile.at(i))==ProjectFileName) {
        ApplicationConfig->RecentFile.removeAt(i);
        break;
    }

    ApplicationConfig->LastProjectPath=QFileInfo(ProjectFileName).dir().absolutePath();
    // Clean actual timeline and diaporama
    FLAGSTOPITEMSELECTION=true;
    ui->timeline->setUpdatesEnabled(false);
    ui->timeline->CleanAll();
    delete Diaporama;
    Diaporama=NULL;
    ui->timeline->setUpdatesEnabled(true);
    FLAGSTOPITEMSELECTION=false;

    // Create new diaporama
    Diaporama=new cDiaporama(ApplicationConfig);
    Diaporama->Timeline=ui->timeline;

    // Init GUI for this project
    ui->preview->InitDiaporamaPlay(Diaporama);
    ui->preview2->InitDiaporamaPlay(Diaporama);
    SetTimelineHeight();

    // Load file
    SetModifyFlag(false);
    Diaporama->LoadFile(this,ProjectFileName);
    for (int i=0;i<Diaporama->List.count();i++) AddObjectToTimeLine(i);
    AdjustRuller();
    SetModifyFlag(Diaporama->IsModify);
    if (Diaporama->List.count()>0) (ApplicationConfig->PartitionMode?ui->preview2:ui->preview)->SeekPlayer(Diaporama->GetTransitionDuration(0));
        else (ApplicationConfig->PartitionMode?ui->preview2:ui->preview)->SeekPlayer(0);

    ApplicationConfig->RecentFile.append(ProjectFileName);
    while (ApplicationConfig->RecentFile.count()>10) ApplicationConfig->RecentFile.takeFirst();
    ui->timeline->SetCurrentCell(0);
    RefreshControls();
    QApplication::restoreOverrideCursor();
    ToStatusBar("");
}

//====================================================================================================================
// Save current project
//====================================================================================================================

void MainWindow::s_action_Save() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_action_Save";
    #endif
    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(500,this,SLOT(s_action_Save()));
        return;
    }
    ui->Action_Save_BT->setDown(false);
    ui->Action_Save_BT_2->setDown(false);

    if (Diaporama->ProjectFileName=="") s_action_SaveAs(); else {
        ToStatusBar(QApplication::translate("MainWindow","Saving project file ...")+QFileInfo(Diaporama->ProjectFileName).fileName());
        QTimer::singleShot(500,this,SLOT(s_DoSaveFile()));
    }
}

//====================================================================================================================
void MainWindow::s_DoSaveFile() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_DoSaveFile";
    #endif
    if (Diaporama->SaveFile(this)) SetModifyFlag(false);
    ToStatusBar("");
}

//====================================================================================================================
// Save current project as
//====================================================================================================================

void MainWindow::s_action_SaveAs() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_action_SaveAs";
    #endif
    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(500,this,SLOT(s_action_SaveAs()));
        return;
    }
    ui->ActionSave_as_BT->setDown(false);
    ui->ActionSave_as_BT_2->setDown(false);

    Diaporama->ProjectFileName=QFileDialog::getSaveFileName(this,QApplication::translate("MainWindow","Save project as"),ApplicationConfig->LastProjectPath,QString("ffDiaporama (*.ffd)"));
    if (Diaporama->ProjectFileName!="") {
        if (QFileInfo(Diaporama->ProjectFileName).suffix()!="ffd") Diaporama->ProjectFileName=Diaporama->ProjectFileName+".ffd";
        ApplicationConfig->LastProjectPath=QFileInfo(Diaporama->ProjectFileName).dir().absolutePath();
        // Manage Recent files list
        for (int i=0;i<ApplicationConfig->RecentFile.count();i++) if (ApplicationConfig->RecentFile.at(i)==Diaporama->ProjectFileName) {
            ApplicationConfig->RecentFile.removeAt(i);
            break;
        }
        ApplicationConfig->RecentFile.append(Diaporama->ProjectFileName);
        while (ApplicationConfig->RecentFile.count()>10) ApplicationConfig->RecentFile.takeFirst();
        ToStatusBar(QApplication::translate("MainWindow","Saving project file ...")+QFileInfo(Diaporama->ProjectFileName).fileName());
        QTimer::singleShot(500,this,SLOT(s_DoSaveFile()));
    }
}

//====================================================================================================================
// Add a title object
//====================================================================================================================

void MainWindow::s_action_AddTitle() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_action_AddTitle";
    #endif
    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(500,this,SLOT(s_action_AddTitle()));
        return;
    }
    ui->ActionAddtitle_BT->setDown(false);
    ui->ActionAddtitle_BT_2->setDown(false);

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    int SavedCurIndex=ApplicationConfig->AppendObject?Diaporama->List.count():Diaporama->CurrentCol;
    int CurIndex=Diaporama->List.count()!=0?SavedCurIndex+1:0;
    if (SavedCurIndex==Diaporama->List.count()) SavedCurIndex--;

    Diaporama->List.insert(CurIndex,cDiaporamaObject(Diaporama));
    cDiaporamaObject *DiaporamaObject=&Diaporama->List[CurIndex];
    DiaporamaObject->List[0].Parent        =DiaporamaObject;
    DiaporamaObject->List[0].StaticDuration=GlobalMainWindow->ApplicationConfig->NoShotDuration;
    DiaporamaObject->Parent                =Diaporama;
    DiaporamaObject->TypeObject            =DIAPORAMAOBJECTTYPE_EMPTY;

    if (Diaporama->ApplicationConfig->RandomTransition) {
        qsrand(QTime(0,0,0,0).msecsTo(QTime::currentTime()));
        int Random=qrand();
        Random=int(double(IconList.List.count())*(double(Random)/double(RAND_MAX)));
        if (Random<IconList.List.count()) {
            Diaporama->List[CurIndex].TransitionFamilly=IconList.List[Random].TransitionFamilly;
            Diaporama->List[CurIndex].TransitionSubType=IconList.List[Random].TransitionSubType;
        }
    } else {
        Diaporama->List[CurIndex].TransitionFamilly=Diaporama->ApplicationConfig->DefaultTransitionFamilly;
        Diaporama->List[CurIndex].TransitionSubType=Diaporama->ApplicationConfig->DefaultTransitionSubType;
    }
    Diaporama->List[CurIndex].TransitionDuration=Diaporama->ApplicationConfig->DefaultTransitionDuration;
    AddObjectToTimeLine(CurIndex);
    ui->timeline->SetCurrentCell(SavedCurIndex+1);
    SetModifyFlag(true);
    AdjustRuller();
    QApplication::restoreOverrideCursor();
}

//====================================================================================================================
// Add an object (image or video)
//====================================================================================================================

void MainWindow::s_action_AddFile() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_action_AddFile";
    #endif
    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(500,this,SLOT(s_action_AddFile()));
        return;
    }
    ui->ActionAdd_BT->setDown(false);
    ui->ActionAdd_BT_2->setDown(false);

    FileList=QFileDialog::getOpenFileNames(this,QApplication::translate("MainWindow","Add files"),
                                                       ApplicationConfig->RememberLastDirectories?ApplicationConfig->LastMediaPath:"",
                                                       ApplicationConfig->GetFilterForMediaFile(ALLFILE));
    if (FileList.count()>0) {

        // Calc position of new object depending on ApplicationConfig->AppendObject

        if (ApplicationConfig->AppendObject) {
            SavedCurIndex   =Diaporama->List.count();
            CurIndex        =Diaporama->List.count();
        } else {
            SavedCurIndex=Diaporama->CurrentCol;
            CurIndex=Diaporama->List.count()!=0?SavedCurIndex+1:0;
            if (SavedCurIndex==Diaporama->List.count()) SavedCurIndex--;
        }

        // Sort files in the fileList
        if (Diaporama->ApplicationConfig->SortFile) {
            // Sort by last number
            for (int i=0;i<FileList.count();i++) for (int j=0;j<FileList.count()-1;j++) {
                QString NameA=QFileInfo(FileList[j]).completeBaseName();
                int NumA=NameA.length()-1;
                while ((NumA>0)&&(NameA[NumA]>='0')&&(NameA[NumA]<='9')) NumA--;
                if (NumA>=0) NumA=NameA.mid(NumA+1).toInt();

                QString NameB=QFileInfo(FileList[j+1]).completeBaseName();
                int NumB=NameB.length()-1;
                while ((NumB>0)&&(NameB[NumB]>='0')&&(NameB[NumB]<='9')) NumB--;
                if (NumB>=0) NumB=NameB.mid(NumB+1).toInt();

                if (NumA>NumB) FileList.swap(j,j+1);
            }
        } else {
            // Sort by alphabetical order
            for (int i=0;i<FileList.count();i++) for (int j=0;j<FileList.count()-1;j++) {
                if (QFileInfo(FileList[j]).completeBaseName()>QFileInfo(FileList[j+1]).completeBaseName()) FileList.swap(j,j+1);
            }
        }

        ToStatusBar(QApplication::translate("MainWindow","Add file to project :")+QFileInfo(FileList[0]).fileName());
        QTimer::singleShot(500,this,SLOT(s_action_DoAddFile()));
    }
}

void MainWindow::s_action_DoAddDragAndDropFile() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_action_DoAddDragAndDropFile";
    #endif
    QTimer::singleShot(500,this,SLOT(s_action_DoAddFile()));
}

void MainWindow::s_action_DoAddFile() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_action_DoAddFile";
    #endif
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    // Add files
    QString NewFile=FileList.takeFirst();
    if (ApplicationConfig->RememberLastDirectories) ApplicationConfig->LastMediaPath=QFileInfo(NewFile).absolutePath();     // Keep folder for next use

    Diaporama->List.insert(CurIndex,cDiaporamaObject(Diaporama));
    cDiaporamaObject *DiaporamaObject=&Diaporama->List[CurIndex];
    DiaporamaObject->List[0].Parent        =DiaporamaObject;
    DiaporamaObject->List[0].StaticDuration=GlobalMainWindow->ApplicationConfig->NoShotDuration;
    DiaporamaObject->Parent                =Diaporama;
    DiaporamaObject->TypeObject            =DIAPORAMAOBJECTTYPE_EMPTY;

    // Create and append a composition block to the object list
    DiaporamaObject->ObjectComposition.List.append(cCompositionObject(COMPOSITIONTYPE_OBJECT,DiaporamaObject->NextIndexKey));
    cCompositionObject *CompositionObject=&DiaporamaObject->ObjectComposition.List[DiaporamaObject->ObjectComposition.List.count()-1];
    cBrushDefinition   *CurrentBrush=&CompositionObject->BackgroundBrush;

    // Set CompositionObject to full screen
    CompositionObject->x=0;
    CompositionObject->y=0;
    CompositionObject->w=1;
    CompositionObject->h=1;

    // Set other values
    CompositionObject->Text     ="";
    CompositionObject->PenSize  =0;
    CurrentBrush->BrushType     =BRUSHTYPE_IMAGEDISK;
    DiaporamaObject->SlideName  =QFileInfo(NewFile).fileName();

    QString BrushFileName =QFileInfo(NewFile).absoluteFilePath();

    bool    IsValide =false;
    QString Extension=QFileInfo(BrushFileName).suffix().toLower();

    // Search if file is an image
    QStringList AliasList;
    for (int i=0;i<GlobalMainWindow->ApplicationConfig->AllowImageExtension.count();i++) if (GlobalMainWindow->ApplicationConfig->AllowImageExtension[i]==Extension) {
        // Create an image wrapper
        CurrentBrush->Image=new cimagefilewrapper();
        IsValide=CurrentBrush->Image->GetInformationFromFile(BrushFileName,AliasList);
        if (!IsValide) {
            delete CurrentBrush->Image;
            CurrentBrush->Image=NULL;
        }
        break;
    }
    // If it's not an image : search if file is a video
    if (CurrentBrush->Image==NULL) for (int i=0;i<GlobalMainWindow->ApplicationConfig->AllowVideoExtension.count();i++) if (GlobalMainWindow->ApplicationConfig->AllowVideoExtension[i]==Extension) {
        // Create a video wrapper
        CurrentBrush->Video=new cvideofilewrapper();
        IsValide=CurrentBrush->Video->GetInformationFromFile(BrushFileName,false,AliasList);
        if (!IsValide) {
            delete CurrentBrush->Video;
            CurrentBrush->Video=NULL;
        } else {
            CurrentBrush->Video->EndPos=CurrentBrush->Video->Duration;
            DiaporamaObject->List[0].StaticDuration=1000;
        }
        break;
    }
    if (IsValide) {
        QImage *Image=(CurrentBrush->Image?CurrentBrush->Image->ImageAt(true,true,&CurrentBrush->Image->BrushFileTransform,GlobalMainWindow->ApplicationConfig->Smoothing):
                       CurrentBrush->Video?CurrentBrush->Video->ImageAt(true,0,0,true,NULL,1,false,&CurrentBrush->Video->BrushFileTransform):
                       NULL);
        if (Image) {

            // Apply Styles
            CompositionObject->ApplyTextStyle(ApplicationConfig->StyleTextCollection.GetStyleDef(ApplicationConfig->StyleTextCollection.DecodeString(ApplicationConfig->DefaultBlockSL_IMG_TextST)));
            CompositionObject->ApplyBlockShapeStyle(ApplicationConfig->StyleBlockShapeCollection.GetStyleDef(ApplicationConfig->StyleBlockShapeCollection.DecodeString(ApplicationConfig->DefaultBlockSL_IMG_ShapeST)));
            // Force filtering for CoordinateStyle
            ApplicationConfig->StyleCoordinateCollection.SetImageGeometryFilter(Diaporama->ImageGeometry,CurrentBrush->Image?CurrentBrush->Image->ObjectGeometry:CurrentBrush->Video->ObjectGeometry);
            CompositionObject->ApplyCoordinateStyle(ApplicationConfig->StyleCoordinateCollection.GetStyleDef(ApplicationConfig->StyleCoordinateCollection.DecodeString(
                ApplicationConfig->DefaultBlockSL_IMG_CoordST[CurrentBrush->Image?CurrentBrush->Image->ObjectGeometry:CurrentBrush->Video->ObjectGeometry][Diaporama->ImageGeometry])));

            // Special case for nonstandard image => force to image geometry constraint and adapt frame coordinates
            if ((CurrentBrush->Image?CurrentBrush->Image->ObjectGeometry:CurrentBrush->Video->ObjectGeometry)==IMAGE_GEOMETRY_UNKNOWN) {
                double ImageGeometry=1;
                double ProjectGeometry=1;
                double NewW,NewH;

                switch (GlobalMainWindow->Diaporama->ImageGeometry) {
                    case GEOMETRY_4_3   : ProjectGeometry=double(1440)/double(1920);  break;
                    case GEOMETRY_16_9  : ProjectGeometry=double(1080)/double(1920);  break;
                    case GEOMETRY_40_17 : ProjectGeometry=double(816)/double(1920);   break;

                }
                ProjectGeometry=QString("%1").arg(ProjectGeometry,0,'e').toDouble();  // Rounded to same number as style managment
                switch (DiaporamaObject->Parent->ApplicationConfig->DefaultBlockSL_CLIPARTLOCK[DiaporamaObject->Parent->ImageGeometry]) {
                    case 0 :    // Adjust to Full in lock to image geometry mode
                        ImageGeometry=1;
                        if (CurrentBrush->Image)            ImageGeometry=double(CurrentBrush->Image->ImageHeight)/double(CurrentBrush->Image->ImageWidth);
                            else if (CurrentBrush->Video)   ImageGeometry=double(CurrentBrush->Video->ImageHeight)/double(CurrentBrush->Video->ImageWidth);
                        CurrentBrush->InitDefaultFramingStyle(true,ImageGeometry);
                        CurrentBrush->ApplyStyle(true,CurrentBrush->DefaultFramingF);
                        NewW=CompositionObject->w*GlobalMainWindow->Diaporama->InternalWidth;
                        NewH=NewW*CurrentBrush->BrushFileCorrect.AspectRatio;
                        NewW=NewW/GlobalMainWindow->Diaporama->InternalWidth;
                        NewH=NewH/GlobalMainWindow->Diaporama->InternalHeight;
                        if (NewH>1) {
                            NewH=CompositionObject->h*GlobalMainWindow->Diaporama->InternalHeight;
                            NewW=NewH/CurrentBrush->BrushFileCorrect.AspectRatio;
                            NewW=NewW/GlobalMainWindow->Diaporama->InternalWidth;
                            NewH=NewH/GlobalMainWindow->Diaporama->InternalHeight;
                        }
                        CompositionObject->w=NewW;
                        CompositionObject->h=NewH;
                        break;
                    case 1 :    // Lock to project geometry - To full
                        ImageGeometry=ProjectGeometry;
                        CurrentBrush->InitDefaultFramingStyle(true,ImageGeometry);
                        CurrentBrush->ApplyStyle(true,CurrentBrush->DefaultFramingF);
                        break;
                    case 2 :    // Lock to project geometry - To width
                        ImageGeometry=ProjectGeometry;
                        CurrentBrush->InitDefaultFramingStyle(true,ImageGeometry);
                        CurrentBrush->ApplyStyle(true,CurrentBrush->DefaultFramingW);
                        break;
                    case 3 :    // Lock to project geometry - To height
                        ImageGeometry=ProjectGeometry;
                        CurrentBrush->InitDefaultFramingStyle(true,ImageGeometry);
                        CurrentBrush->ApplyStyle(true,CurrentBrush->DefaultFramingH);
                        break;
                }
            }
            delete Image;
        }

        // Now create and append a shot composition block to all shot
        for (int i=0;i<DiaporamaObject->List.count();i++) {
            DiaporamaObject->List[i].ShotComposition.List.append(cCompositionObject(COMPOSITIONTYPE_SHOT,CompositionObject->IndexKey));
            DiaporamaObject->List[i].ShotComposition.List[DiaporamaObject->List[i].ShotComposition.List.count()-1].CopyFromCompositionObject(CompositionObject);
        }

        // Inc NextIndexKey
        DiaporamaObject->NextIndexKey++;

        if (Diaporama->ApplicationConfig->RandomTransition) {
            qsrand(QTime(0,0,0,0).msecsTo(QTime::currentTime()));
            int Random=qrand();
            Random=int(double(IconList.List.count())*(double(Random)/double(RAND_MAX)));
            if (Random<IconList.List.count()) {
                Diaporama->List[CurIndex].TransitionFamilly=IconList.List[Random].TransitionFamilly;
                Diaporama->List[CurIndex].TransitionSubType=IconList.List[Random].TransitionSubType;
            }
        } else {
            Diaporama->List[CurIndex].TransitionFamilly=Diaporama->ApplicationConfig->DefaultTransitionFamilly;
            Diaporama->List[CurIndex].TransitionSubType=Diaporama->ApplicationConfig->DefaultTransitionSubType;
        }
        Diaporama->List[CurIndex].TransitionDuration=Diaporama->ApplicationConfig->DefaultTransitionDuration;
        AddObjectToTimeLine(CurIndex);
        CurIndex++;
        SetModifyFlag(true);

    } else {
        QMessageBox::critical(NULL,QApplication::translate("MainWindow","Error","Error message"),NewFile+"\n\n"+QApplication::translate("MainWindow","Format not supported","Error message"),QMessageBox::Close);
        Diaporama->List.removeAt(CurIndex);
    }

    // Set current selection to first new object
    ui->timeline->SetCurrentCell(SavedCurIndex+1);

    // Set title flag
    AdjustRuller();
    QApplication::restoreOverrideCursor();

    if (FileList.count()>0) {
        ToStatusBar(QApplication::translate("MainWindow","Add file to project :")+QFileInfo(FileList[0]).fileName());
        QTimer::singleShot(500,this,SLOT(s_action_DoAddFile()));
    } else ToStatusBar("");
}

//====================================================================================================================
// Add an (sub) project
//====================================================================================================================

void MainWindow::s_action_AddProject() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_action_AddProject";
    #endif
    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(500,this,SLOT(s_action_AddProject()));
        return;
    }
    ui->ActionAddProject_BT->setDown(false);
    ui->ActionAddProject_BT_2->setDown(false);

    QString ProjectFileName=QFileDialog::getOpenFileName(this,QApplication::translate("MainWindow","Add a sub project"),ApplicationConfig->LastProjectPath,QString("ffDiaporama (*.ffd)"));
    if (ProjectFileName!="") {
        ToStatusBar(QApplication::translate("MainWindow","Add project file :")+QFileInfo(ProjectFileName).fileName());
        FileForIO=ProjectFileName;
        QTimer::singleShot(500,this,SLOT(s_action_DoAddProject()));
    }
}

void MainWindow::s_action_DoAddProject() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_action_DoAddProject";
    #endif
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    ApplicationConfig->LastProjectPath=QFileInfo(FileForIO).dir().absolutePath();
    Diaporama->AppendFile(this,FileForIO);  // Append file
    SetModifyFlag(true);
    AdjustRuller();
    QApplication::restoreOverrideCursor();
}

//====================================================================================================================

void MainWindow::s_action_Edit() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_action_Edit";
    #endif
    QMenu *ContextMenu=new QMenu(this);
    ContextMenu->addAction(ui->actionEdit_background);
    ContextMenu->addAction(ui->actionEdit_object);
    ContextMenu->addAction(ui->actionEdit_music);
    ContextMenu->addAction(ui->actionEdit_object_in_transition);
    ContextMenu->addAction(ui->actionEdit_background_transition);
    ContextMenu->exec(QCursor::pos());
    delete ContextMenu;
    ui->ActionEdit_BT->setDown(false);
    ui->ActionEdit_BT_2->setDown(false);
}

//====================================================================================================================

void MainWindow::s_RemoveObject() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_RemoveObject";
    #endif
    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(500,this,SLOT(s_RemoveObject()));
        return;
    }
    ui->ActionRemove_BT->setDown(false);
    ui->ActionRemove_BT_2->setDown(false);

    int Current=ui->timeline->CurrentSelected();
    if ((Current<0)||(Current>=Diaporama->List.count())) return;

    if ((ApplicationConfig->AskUserToRemove)&&(QMessageBox::question(this,QApplication::translate("MainWindow","Remove slide"),QApplication::translate("MainWindow","Are you sure to want to delete this slide?"),
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)==QMessageBox::No)) return;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    ui->timeline->setUpdatesEnabled(false);
    FLAGSTOPITEMSELECTION=true;
    Diaporama->List.removeAt(Current);
    if (Current>=Diaporama->List.count()) Current=Diaporama->List.count()-1;
    ui->timeline->ResetDisplay(Current);    // FLAGSTOPITEMSELECTION is set to false by ResetDisplay
    (ApplicationConfig->PartitionMode?ui->preview2:ui->preview)->SeekPlayer(Diaporama->GetObjectStartPosition(Current)+Diaporama->GetTransitionDuration(Current));
    SetModifyFlag(true);
    AdjustRuller();
    ui->timeline->setUpdatesEnabled(true);
    QApplication::restoreOverrideCursor();
}

//====================================================================================================================

void MainWindow::s_CutToClipboard() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_CutToClipboard";
    #endif
    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(500,this,SLOT(s_CutToClipboard()));
        return;
    }
    ui->ActionCut_BT->setDown(false);
    ui->ActionCut_BT_2->setDown(false);

    int Current=ui->timeline->CurrentSelected();
    if ((Current<0)||(Current>Diaporama->List.count()-1)) return;

    QDomElement root;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    if (Clipboard_Object!=NULL) delete Clipboard_Object;
    Clipboard_Object=new QDomDocument(APPLICATION_NAME);

    // Create xml document and root
    root=Clipboard_Object->createElement("CLIPBOARD");
    Clipboard_Object->appendChild(root);
    Diaporama->List[Current].SaveToXML(root,"CLIPBOARD-OBJECT",QFileInfo(Diaporama->ProjectFileName).absolutePath(),true);
    s_RemoveObject();
    QApplication::restoreOverrideCursor();
}

//====================================================================================================================

void MainWindow::s_CopyToClipboard() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_CopyToClipboard";
    #endif
    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(500,this,SLOT(s_CopyToClipboard()));
        return;
    }
    ui->ActionCopy_BT->setDown(false);
    ui->ActionCopy_BT_2->setDown(false);

    int Current=ui->timeline->CurrentSelected();
    if ((Current<0)||(Current>Diaporama->List.count()-1)) return;

    QDomElement root;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    if (Clipboard_Object!=NULL) delete Clipboard_Object;
    Clipboard_Object=new QDomDocument(APPLICATION_NAME);

    // Create xml document and root
    root=Clipboard_Object->createElement("CLIPBOARD");
    Clipboard_Object->appendChild(root);
    Diaporama->List[Current].SaveToXML(root,"CLIPBOARD-OBJECT",QFileInfo(Diaporama->ProjectFileName).absolutePath(),true);

    RefreshControls();
    QApplication::restoreOverrideCursor();
}

//====================================================================================================================

void MainWindow::s_PasteFromClipboard() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_PasteFromClipboard";
    #endif
    if (Clipboard_Object==NULL) return;

    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(500,this,SLOT(s_PasteFromClipboard()));
        return;
    }
    ui->ActionPaste_BT->setDown(false);
    ui->ActionPaste_BT_2->setDown(false);

    // Calc position of new object depending on ApplicationConfig->AppendObject
    int SavedCurIndex=ApplicationConfig->AppendObject?Diaporama->List.count():Diaporama->CurrentCol;
    int CurIndex=Diaporama->List.count()!=0?SavedCurIndex+1:0;
    if (SavedCurIndex==Diaporama->List.count()) SavedCurIndex--;

    Diaporama->List.insert(CurIndex,cDiaporamaObject(Diaporama));
    //Diaporama->List[CurIndex].List[0].Parent=&Diaporama->List[CurIndex];

    QDomElement root=Clipboard_Object->documentElement();
    if (root.tagName()=="CLIPBOARD") {
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        ui->timeline->setUpdatesEnabled(false);
        QStringList AliasList;
        Diaporama->List[CurIndex].LoadFromXML(root,"CLIPBOARD-OBJECT","",AliasList);
        AddObjectToTimeLine(CurIndex);
        SetModifyFlag(true);
        // Set current selection to first new object
        ui->timeline->SetCurrentCell(SavedCurIndex+1);
        AdjustRuller();
        ui->timeline->setUpdatesEnabled(true);
        QApplication::restoreOverrideCursor();
    }
}

//====================================================================================================================

void MainWindow::AddObjectToTimeLine(int CurIndex) {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::AddObjectToTimeLine";
    #endif
    ui->timeline->AddObjectToTimeLine(CurIndex);
}

//====================================================================================================================
// Adjust preview ruller depending on current Disporama Currentcol
//====================================================================================================================

void MainWindow::AdjustRuller() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::AdjustRuller";
    #endif
    ui->preview->SetActualDuration(Diaporama->GetDuration());
    ui->preview2->SetActualDuration(Diaporama->GetDuration());
    if (Diaporama->List.count()>0)  {
        ui->preview->SetStartEndPos(
                Diaporama->GetObjectStartPosition(Diaporama->CurrentCol),                                                           // Current slide
                Diaporama->List[Diaporama->CurrentCol].GetDuration(),
                (Diaporama->CurrentCol>0)?Diaporama->GetObjectStartPosition(Diaporama->CurrentCol-1):((Diaporama->CurrentCol==0)?0:-1),                            // Previous slide
                (Diaporama->CurrentCol>0)?Diaporama->List[Diaporama->CurrentCol-1].GetDuration():((Diaporama->CurrentCol==0)?Diaporama->GetTransitionDuration(Diaporama->CurrentCol):0),
                Diaporama->CurrentCol<(Diaporama->List.count()-1)?Diaporama->GetObjectStartPosition(Diaporama->CurrentCol+1):-1,    // Next slide
                Diaporama->CurrentCol<(Diaporama->List.count()-1)?Diaporama->List[Diaporama->CurrentCol+1].GetDuration():0);
        ui->preview2->SetStartEndPos(
                Diaporama->GetObjectStartPosition(Diaporama->CurrentCol),                                                           // Current slide
                Diaporama->List[Diaporama->CurrentCol].GetDuration(),
                (Diaporama->CurrentCol>0)?Diaporama->GetObjectStartPosition(Diaporama->CurrentCol-1):((Diaporama->CurrentCol==0)?0:-1),                            // Previous slide
                (Diaporama->CurrentCol>0)?Diaporama->List[Diaporama->CurrentCol-1].GetDuration():((Diaporama->CurrentCol==0)?Diaporama->GetTransitionDuration(Diaporama->CurrentCol):0),
                Diaporama->CurrentCol<(Diaporama->List.count()-1)?Diaporama->GetObjectStartPosition(Diaporama->CurrentCol+1):-1,    // Next slide
                Diaporama->CurrentCol<(Diaporama->List.count()-1)?Diaporama->List[Diaporama->CurrentCol+1].GetDuration():0);
    } else {
        ui->preview->SetStartEndPos(0,0,-1,0,-1,0);
        ui->preview2->SetStartEndPos(0,0,-1,0,-1,0);
    }
    ui->timeline->repaint();
    (ApplicationConfig->PartitionMode?ui->preview2:ui->preview)->SeekPlayer(Diaporama->CurrentPosition);
    RefreshControls();
}
