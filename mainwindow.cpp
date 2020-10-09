#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "settingsdialog.h"
#include "devicetreedock.h"
#include "statusfield.h"

#include "gatewaybasictabwidget.h"
#include "relaypropertytabwidget.h"
#include "touchpaneltabwidget.h"


#include "logmanager.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QStatusBar>
#include <QMessageBox>
#include <QInputDialog>
#include <QLabel>
#include <QPalette>
#include <QDesktopServices>

#if 0
static constQString queryCreateDevTable("create table devTable (net_address int primary key, "
                            "dev_type int,"
                            "dev_name varchar(20), "
                            "discription varchar(40))");

#endif

static const
QString queryCreateDevTable("create table devTable ("
                            "net_address int UNIQUE ON CONFLICT ROLLBACK PRIMARY KEY ON CONFLICT ROLLBACK, "
                            "dev_type int,"
                            "dev_name varchar(20), "
                            "discription varchar(40),"
                            "basic_prop      BLOB,"
                            "adv_prop        BLOB,"
                            "property3       BLOB,"
                            "property4       BLOB,"
                            "property5       BLOB,"
                            "property6       BLOB,"
                            "property7       BLOB,"
                            "property8       BLOB)");



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

//    setWindowTitle(tr("iLighting config tool"));


    db = QSqlDatabase::addDatabase("QSQLITE");
    devListModel = new QSqlTableModel(this, db);
    m_settingsDialog = new SettingsDialog(this);

    m_devTreeDock = new DeviceTreeDock(devListModel,this);
    m_devTreeDock->setEnabled(false);
    addDockWidget(Qt::LeftDockWidgetArea,m_devTreeDock);


    m_statusField = new StatusField(this);
    m_statusField->setEnabled(false);
    addDockWidget(Qt::BottomDockWidgetArea,m_statusField);
//    addDockWidget(Qt::RightDockWidgetArea,m_statusField);
    if(!m_statusField->hasMessages())
    {
        m_statusField->close();
    }


//    initDb();

    initActions();

    lastWindow = ui->tabWidget;

    /******  状态栏显示   ********/
    ui->statusbar->setStyleSheet(QString("QStatusBar::item{border: 0px}")); //不显示边框
    ui->statusbar->showMessage(tr("欢迎使用iLighting-config_tool配置工具！"), 20000);
//    ui->statusBar->showMessage(tr("当前使用版本 V0.0"), 200000);
    // 创建标签，设置标签样式并显示信息，将其以永久部件的形式添加到状态栏
    QLabel *permanent = new QLabel(this);
    permanent->setFrameStyle(QFrame::Box | QFrame::Sunken);
    permanent->setText("版权所有：成都四为电子信息股份有限公司");
    ui->statusbar->addPermanentWidget(permanent);



//    QFile qssFile;
//    qssFile.setFileName(":/qss/my.qss");

//    qssFile.open(QFile::ReadOnly);
//    QString styleSheet = tr(qssFile.readAll());
//    qApp->setStyleSheet(styleSheet);
//    qssFile.close();
//    qDebug() << tr("换肤");

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initActions()
{
    ui->actionSave->setEnabled(false);
    ui->actionSave_as->setEnabled(false);

    connect(ui->actionNew_Project,&QAction::triggered,this,&MainWindow::newProject);
    connect(ui->actionOpen_Project,&QAction::triggered,this,&MainWindow::open);
    connect(ui->actionSave_as,&QAction::triggered,this,&MainWindow::saveAs);
    connect(ui->actionSave,&QAction::triggered,this,&MainWindow::save);

    connect(ui->actionSetting, &QAction::triggered, m_settingsDialog, &QDialog::show);
//    connect(m_settingsDialog, &SettingsDialog::createConnetionRequest, this, &MainWindow::onCreateConnectionRequest);

    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::about);
    connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::close);

//    connect(ui->connectMode, QOverload<int>::of(&QComboBox::currentIndexChanged),
//            this, &MainWindow::onConnectModeChanged);
//    connect(ui->connectType, QOverload<int>::of(&QComboBox::currentIndexChanged),
//            this, &MainWindow::onConnectTypeChanged);

//    connect(ui->connectButton, &QPushButton::clicked,
//            this, &MainWindow::onConnectButtonClicked);
//    connect(ui->devWindowApplyBtn,&QPushButton::clicked,this,&MainWindow::onDevWindowApplyBtnClicked);
}

void MainWindow::initDb()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(db_name);
    if(!db.open()){
        qDebug() << "Error: Failed to connect database." << db.lastError();
    }
    else
    {
        qDebug("Succeed to connect database.") ;     
    }

    QSqlQuery query;
    query.exec("select *  from sqlite_master where type = 'table' and name = 'person'");

    if(query.first())
    {
        qDebug("project table exists") ;
    }
    else {
        qDebug("project table dose not exist") ;
        query.exec("create table person (id int primary key, "
                   "firstname varchar(20), lastname varchar(20))");
    }
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About iLighting config tool"),
             tr("The <b>iLighting config tool</b> is used for configuring iLighting project.<br>"
                "<b>Author</b>: LuoH <br><b>E-mail</b> : sysware_alien#163.com"));
}



void MainWindow::newProject()
{
    db_name = QInputDialog::getText(this,tr("新建工程"),tr("请输入工程名"));//get project db name
    if(db_name.isEmpty())
        return;

    if(!createConnection(db,db_name))
        return;

    QSqlQuery query;
    query.exec(queryCreateDevTable);
}

void MainWindow::saveAs()
{

}

void MainWindow::open()
{
    db_name = QFileDialog::getOpenFileName(this, tr("Open Project File"),
                                                  QDir::currentPath());
    if (db_name.isEmpty())
        return;

    if(!createConnection(db,db_name))
        return;

//    devListModel = new QSqlTableModel(this, db);
    devListModel->setTable("devTable");
    devListModel->select();

    devListModel->setHeaderData(0, Qt::Orientation::Horizontal, "设备地址");
    devListModel->setHeaderData(1, Qt::Orientation::Horizontal, "设备类型");
    devListModel->setHeaderData(2, Qt::Orientation::Horizontal, "设备名称");
    devListModel->setHeaderData(3, Qt::Orientation::Horizontal, "描述");
    devListModel->setHeaderData(4, Qt::Orientation::Horizontal, "基本属性");
    devListModel->setHeaderData(5, Qt::Orientation::Horizontal, "高级属性");
    devListModel->setHeaderData(6, Qt::Orientation::Horizontal, "保留");
    devListModel->setHeaderData(7, Qt::Orientation::Horizontal, "保留");
    devListModel->setHeaderData(8, Qt::Orientation::Horizontal, "保留");
    devListModel->setHeaderData(9, Qt::Orientation::Horizontal, "保留");
    devListModel->setHeaderData(10, Qt::Orientation::Horizontal, "保留");
    devListModel->setHeaderData(11, Qt::Orientation::Horizontal, "保留");


    connect(m_devTreeDock,&DeviceTreeDock::devListDoubleClicked,this,&MainWindow::process_devList_doubleClicked);
//    m_devTreeDock->setSQLModel(devListModel);
    m_devTreeDock->setEnabled(true);

//    qDebug("rowcount: %d",devListModel->rowCount());


//    ui->treeView->setModel(devListModel);
//    ui->treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
//    for (int i = 1; i < devListModel->columnCount();i ++) {
//        ui->treeView->hideColumn(i);
//    }

//    ui->treeView->show();

//    QFile file(fileName);
//    if (!file.open(QFile::ReadOnly | QFile::Text)) {
//        QMessageBox::warning(this, tr("Project File"),
//                             tr("Cannot read file %1:\n%2.")
//                             .arg(QDir::toNativeSeparators(fileName),
//                                  file.errorString()));
//        return;
//    }

//    XbelReader reader(treeWidget);
//    if (!reader.read(&file)) {
//        QMessageBox::warning(this, tr("QXmlStream Bookmarks"),
//                             tr("Parse error in file %1:\n\n%2")
//                             .arg(QDir::toNativeSeparators(fileName),
//                                  reader.errorString()));
//    } else {
//        statusBar()->showMessage(tr("File loaded"), 2000);
//    }

}

void MainWindow::save()
{

}


bool MainWindow::createConnection(QSqlDatabase &db, QString &name)
{
    if(db.isOpen())
    {
        db.close();
    }

    db.setDatabaseName(name);

    if(!db.open()){
//        qDebug() << "Error: Failed to connect database." << db.lastError();
        logError(tr("Failed to connect database.:%1").arg(db.lastError().text()) );
        return false;
    }
    else
    {
//        qDebug("Succeed to connect database.") ;
        logInfo(tr("Succeed to connect database."));
    }

    return true;
}


#if 0
void MainWindow::onCreateConnectionRequest()
{
    if (!modbusDevice)
        return;

}
#endif

void MainWindow::process_devList_doubleClicked(const QModelIndex &index)
{
    int dev_type = devListModel->data(devListModel->index(index.row(),1)).toInt();

    if(currentWindow){
        lastWindow = currentWindow;
    }

    switch(dev_type)
    {
     case 0:
        currentWindow = new GatewayBasicTabWidget(index,devListModel,ui->centralwidget);
        break;
     case 1:
        currentWindow = new RelayPropertyTabWidget(index,devListModel,ui->centralwidget);
        break;
     case 2:
        currentWindow = new RelayPropertyTabWidget(index,devListModel,ui->centralwidget);
        break;
     case 3:
        currentWindow = new TouchPaneltabwidget(index,devListModel,ui->centralwidget);
        break;
     case 4:
        currentWindow = new RelayPropertyTabWidget(index,devListModel,ui->centralwidget);
        break;
     case 5:
        currentWindow = new RelayPropertyTabWidget(index,devListModel,ui->centralwidget);
        break;

     default:
        break;
    }
//    if(dev_type == 0)
//    {
//        currentWindow = new GatewayBasicTabWidget(index,devListModel,ui->centralwidget);
//    }
//    else{
//        currentWindow = new RelayPropertyTabWidget(index,devListModel,ui->centralwidget);
//    }
    ui->gridLayout_2->replaceWidget(lastWindow,currentWindow);
    lastWindow->deleteLater();
}


void MainWindow::on_actionHelp_triggered()
{
    QProcess *helpProcess = new QProcess(this);
//    QStringList argument("C:/Users/Administrator.LA22NR22AFOORAV/Desktop/ilighting-config-tool/help.pdf");
    QStringList argument("./help.pdf");
    helpProcess->start("hh.exe", argument);
}
