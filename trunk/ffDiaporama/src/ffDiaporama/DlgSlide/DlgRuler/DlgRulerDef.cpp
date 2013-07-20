#include "DlgRulerDef.h"
#include "ui_DlgRulerDef.h"

#define RULER_HORIZ_SCREENBORDER    0x0001
#define RULER_HORIZ_TVMARGIN        0x0002
#define RULER_HORIZ_SCREENCENTER    0x0004
#define RULER_HORIZ_UNSELECTED      0x0008
#define RULER_VERT_SCREENBORDER     0x0010
#define RULER_VERT_TVMARGIN         0x0020
#define RULER_VERT_SCREENCENTER     0x0040
#define RULER_VERT_UNSELECTED       0x0080

DlgRulerDef::DlgRulerDef(int *RulerDef,bool AllowTVMargins,cBaseApplicationConfig *ApplicationConfig,cSaveWindowPosition *DlgWSP,QWidget *parent):
    QCustomDialog(ApplicationConfig,DlgWSP,parent),ui(new Ui::DlgRulerDef) {

    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgRulerDef::DlgRulerDef");

    this->RulerDef=RulerDef;
    ui->setupUi(this);
    OkBt    =ui->OKBT;
    CancelBt=ui->CancelBt;
    HelpTT  =ui->HelpTT;

    this->AllowTVMargins=AllowTVMargins;
}

//====================================================================================================================

DlgRulerDef::~DlgRulerDef() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgRulerDef::~DlgRulerDef");
    delete ui;
}

//====================================================================================================================

void DlgRulerDef::DoInitDialog() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgRulerDef::DoInitDialog");
    ui->Horiz_ScreenBorderCB-> setChecked(((*RulerDef)&RULER_HORIZ_SCREENBORDER)!=0);   connect(ui->Horiz_ScreenBorderCB, SIGNAL(stateChanged(int)),this,SLOT(CBStateChanged(int)));
    ui->Horiz_TVMarginsCB->    setChecked(((*RulerDef)&RULER_HORIZ_TVMARGIN)!=0);       connect(ui->Horiz_TVMarginsCB,    SIGNAL(stateChanged(int)),this,SLOT(CBStateChanged(int)));
    ui->Horiz_ScreenCenterCB-> setChecked(((*RulerDef)&RULER_HORIZ_SCREENCENTER)!=0);   connect(ui->Horiz_ScreenCenterCB, SIGNAL(stateChanged(int)),this,SLOT(CBStateChanged(int)));
    ui->Horiz_UnselectedObjCB->setChecked(((*RulerDef)&RULER_HORIZ_UNSELECTED)!=0);     connect(ui->Horiz_UnselectedObjCB,SIGNAL(stateChanged(int)),this,SLOT(CBStateChanged(int)));
    ui->Vert_ScreenBorderCB->  setChecked(((*RulerDef)&RULER_VERT_SCREENBORDER)!=0);    connect(ui->Vert_ScreenBorderCB,  SIGNAL(stateChanged(int)),this,SLOT(CBStateChanged(int)));
    ui->Vert_TVMarginsCB->     setChecked(((*RulerDef)&RULER_VERT_TVMARGIN)!=0);        connect(ui->Vert_TVMarginsCB,     SIGNAL(stateChanged(int)),this,SLOT(CBStateChanged(int)));
    ui->Vert_ScreenCenterCB->  setChecked(((*RulerDef)&RULER_VERT_SCREENCENTER)!=0);    connect(ui->Vert_ScreenCenterCB,  SIGNAL(stateChanged(int)),this,SLOT(CBStateChanged(int)));
    ui->Vert_UnselectedObjCB-> setChecked(((*RulerDef)&RULER_VERT_UNSELECTED)!=0);      connect(ui->Vert_UnselectedObjCB, SIGNAL(stateChanged(int)),this,SLOT(CBStateChanged(int)));
    ui->Horiz_TVMarginsCB->setVisible(AllowTVMargins);
    ui->Vert_TVMarginsCB->setVisible(AllowTVMargins);
}

//====================================================================================================================

void DlgRulerDef::CBStateChanged(int) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgRulerDef::CBStateChanged");
    *RulerDef=0;
    if (ui->Horiz_ScreenBorderCB-> isChecked())  (*RulerDef)|=RULER_HORIZ_SCREENBORDER;
    if (ui->Horiz_TVMarginsCB->    isChecked())  (*RulerDef)|=RULER_HORIZ_TVMARGIN;
    if (ui->Horiz_ScreenCenterCB-> isChecked())  (*RulerDef)|=RULER_HORIZ_SCREENCENTER;
    if (ui->Horiz_UnselectedObjCB->isChecked())  (*RulerDef)|=RULER_HORIZ_UNSELECTED;
    if (ui->Vert_ScreenBorderCB->  isChecked())  (*RulerDef)|=RULER_VERT_SCREENBORDER;
    if (ui->Vert_TVMarginsCB->     isChecked())  (*RulerDef)|=RULER_VERT_TVMARGIN;
    if (ui->Vert_ScreenCenterCB->  isChecked())  (*RulerDef)|=RULER_VERT_SCREENCENTER;
    if (ui->Vert_UnselectedObjCB-> isChecked())  (*RulerDef)|=RULER_VERT_UNSELECTED;
    emit RefreshDisplay();
}
