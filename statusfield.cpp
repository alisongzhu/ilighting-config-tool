#include "statusfield.h"
#include "ui_statusfield.h"

#include "logmanager.h"

#include <QDateTime>

StatusField::StatusField(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::StatusField)
{
    ui->setupUi(this);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    LogManager* log = LogManager::getInstance();
    connect(log, SIGNAL(logInfo(QString)), this, SLOT(info(QString)));
    connect(log, SIGNAL(logError(QString)), this, SLOT(error(QString)));
    connect(log, SIGNAL(logWarning(QString)), this, SLOT(warn(QString)));
}

StatusField::~StatusField()
{
    delete ui;
}

bool StatusField::hasMessages() const
{
    return ui->tableWidget->rowCount() > 0;
}

void StatusField::addEntry(const QString &type, const QString &text)
{
    int row = ui->tableWidget->rowCount();
    ui->tableWidget->setRowCount(row+1);

    if (row > itemCountLimit)
    {
        ui->tableWidget->removeRow(0);
        row--;
    }

//    QList<QTableWidgetItem*> itemsCreated;
    QTableWidgetItem* item = nullptr;

    item = new QTableWidgetItem(type);
    ui->tableWidget->setItem(row, 0, item);
//    itemsCreated << item;


    QString timeStr = "[" + QDateTime::currentDateTime().toString(timeStampFormat) + "]";
    item = new QTableWidgetItem(timeStr);
    ui->tableWidget->setItem(row, 1, item);
//    itemsCreated << item;

    item = new QTableWidgetItem();
    ui->tableWidget->setItem(row, 2, item);
    item->setText(text);
//    itemsCreated << item;
    setVisible(true);

    ui->tableWidget->scrollToBottom();
//    if (isVisible() && !noFlashing)
//        flashItems(itemsCreated, color);
}

void StatusField::info(const QString &text)
{
    addEntry(tr("info"),text);
}

void StatusField::warn(const QString &text)
{
    addEntry(tr("warn"),text);
}

void StatusField::error(const QString &text)
{
    addEntry(tr("error"),text);
}

#if 0
void StatusField::reset()
{
    for (QAbstractAnimation* anim : itemAnimations)
        anim->stop();

    itemAnimations.clear();
    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(0);
}
#endif
