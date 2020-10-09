#include "devicetreedock.h"
#include "ui_devicetreedock.h"


#include <QMessageBox>
#include <QSqlRecord>

DeviceTreeDock::DeviceTreeDock(QSqlTableModel *model,QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::DeviceTreeDock)
{
    ui->setupUi(this);

    devTreeModel= model;
    devTreeModel->select();
    ui->treeView->setModel(devTreeModel);

    connect(ui->treeView,&QTreeView::doubleClicked,this,&DeviceTreeDock::devListDoubleClicked);
}

DeviceTreeDock::~DeviceTreeDock()
{
    delete ui;
}

#if 0
void DeviceTreeDock::setSQLModel(QSqlTableModel *model)
{
    devTreeModel= model;
    devTreeModel->select();
    ui->treeView->setModel(devTreeModel);
//    ui->treeView->show();
//    setEnabled(true);
}

void DeviceTreeDock::getSubmoduleMask(uint8_t *mask)
{
    int row_count = devTreeModel->rowCount();

    int address;

    for(int i = 0;i < row_count;i++)
    {
        address = devTreeModel->data(devTreeModel->index(i,0)).toInt();
//        address = devTreeModel->record(i).value("net_address").toInt();
        //TODO;
    }

}
#endif

void DeviceTreeDock::on_addButton_clicked()
{
    int address = ui->addressEdit->text().toInt();
    if(address < 1 || address > 250)
    {
        return;
    }

    int type = ui->typeCombo->currentIndex();
    int row_count = devTreeModel->rowCount();
    if(0 == type)
    {
        for (int i = 0; i < row_count; i++) {
            if(type == devTreeModel->data(devTreeModel->index(i,1)).toInt())
            {
                QMessageBox::warning(this,tr("Warning"),tr("There is only one Gateway in each project!"));
                return;
            }
        }
    }

    devTreeModel->insertRow(row_count);
    devTreeModel->setData(devTreeModel->index(row_count,0),address);
    devTreeModel->setData(devTreeModel->index(row_count,1),type);
    devTreeModel->submitAll();
    devTreeModel->select();
}

void DeviceTreeDock::on_deleteButton_clicked()
{
    int curRow = ui->treeView->currentIndex().row();

//    devTreeModel->database().transaction();
    devTreeModel->removeRow(curRow);
    devTreeModel->select();

#if 0
    int ok = QMessageBox::warning(this,tr("Delete"),tr("Confirm to delete?"),QMessageBox::Yes,QMessageBox::No);
    if(ok == QMessageBox::No)
    {
        devTreeModel->revertAll();
    }
    else
        devTreeModel->submitAll();
#endif

}

void DeviceTreeDock::on_sortButton_clicked()
{
    devTreeModel->setSort(0,Qt::AscendingOrder);
    devTreeModel->select();
}

void DeviceTreeDock::on_searchButton_clicked()
{
//    int address = ui->addressEdit->text().toInt();
//    ui->treeView.select

}

//void DeviceTreeDock::on_treeView_doubleClicked(const QModelIndex &index)
//{
//    emit devListDoubleClicked(index);
//}
