#include "DlgInfoFile.h"
#include "ui_DlgInfoFile.h"

//#define DEBUGMODE

//====================================================================================================================

DlgInfoFile::DlgInfoFile(cBaseMediaFile *MediaFile,QString HelpURL,cBaseApplicationConfig *ApplicationConfig,cSaveWindowPosition *DlgWSP,QWidget *parent)
    :QCustomDialog(HelpURL,ApplicationConfig,DlgWSP,parent),ui(new Ui::DlgInfoFile) {

    #ifdef DEBUGMODE
    qDebug() << "IN:DlgInfoFile::DlgInfoFile";
    #endif

    this->MediaFile=MediaFile;

    ui->setupUi(this);
    //CancelBt=ui->CancelBt;
    OkBt    =ui->OkBt;
    HelpBt  =ui->HelpBT;
}

//====================================================================================================================

DlgInfoFile::~DlgInfoFile() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgInfoFile::~DlgInfoFile";
    #endif
    delete ui;
}

//====================================================================================================================
// Initialise dialog

void DlgInfoFile::DoInitDialog() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgInfoFile::DoInitDialog";
    #endif

    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    ui->tableWidget->horizontalHeader()->hide();
    ui->tableWidget->horizontalHeader()->setStretchLastSection(false);
    ui->tableWidget->horizontalHeader()->setSortIndicatorShown(false);
    ui->tableWidget->horizontalHeader()->setCascadingSectionResizes(false);
    ui->tableWidget->horizontalHeader()->setClickable(false);
    ui->tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->tableWidget->horizontalHeader()->setMovable(false);
    ui->tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Fixed);          //Fixed because ResizeToContents will be done after table filling
    ui->tableWidget->verticalHeader()->hide();
    ui->tableWidget->verticalHeader()->setStretchLastSection(false);
    ui->tableWidget->verticalHeader()->setSortIndicatorShown(false);
    ui->tableWidget->verticalHeader()->setResizeMode(QHeaderView::Fixed);            // Fixed because ResizeToContents will be done after table filling
    ui->tableWidget->setShowGrid(true);                  // Ensure grid display
    ui->tableWidget->setWordWrap(false);                 // Ensure no word wrap
    ui->tableWidget->setTextElideMode(Qt::ElideNone);    // Ensure no line ellipsis (...)
    ui->tableWidget->setColumnCount(2);
    ui->tableWidget->setHorizontalHeaderLabels(QString("Propertie;Value").split(";"));

    if (MediaFile) {
        /*
        ui->tableWidget->insertRow(ui->tableWidget->rowCount());
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,0,new QTableWidgetItem(QApplication::translate("DlgInfoFile","Object type")));
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,1,new QTableWidgetItem(CurrentTextItem->BackgroundBrush->BrushType!=BRUSHTYPE_IMAGEDISK?QApplication::translate("DlgInfoFile","Title"):
                                                                                  CurrentTextItem->BackgroundBrush->Image!=NULL?QApplication::translate("DlgInfoFile","Image"):
                                                                                  QApplication::translate("DlgInfoFile","Video")));
        */
        ui->tableWidget->setUpdatesEnabled(false);               // To allow and force a general update
        for (int i=0;i<MediaFile->InformationList.count();i++) {
            ui->tableWidget->insertRow(ui->tableWidget->rowCount());
            QStringList Value=MediaFile->InformationList[i].split("##");
            ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,0,new QTableWidgetItem(Value[0]));
            ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,1,new QTableWidgetItem(Value[1]));
        }
        ui->tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
        ui->tableWidget->setVisible(false);                      // To ensure all items of all columns are used to compute size
        ui->tableWidget->resizeColumnsToContents();              // Resize column widht
        ui->tableWidget->resizeRowsToContents();                 // Resize row height
        ui->tableWidget->setVisible(true);                       // To allow display
        ui->tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
        ui->tableWidget->setUpdatesEnabled(true);                // To allow and force a general update
    }
}
