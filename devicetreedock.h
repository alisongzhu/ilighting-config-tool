#ifndef DEVICETREEDOCK_H
#define DEVICETREEDOCK_H

#include <QDockWidget>
#include <QSqlTableModel>

namespace Ui {
class DeviceTreeDock;
}

class DeviceTreeDock : public QDockWidget
{
    Q_OBJECT

public:
    explicit DeviceTreeDock(QSqlTableModel *model,QWidget *parent = nullptr);
    ~DeviceTreeDock();

//    void setSQLModel(QSqlTableModel *model);
//    void getSubmoduleMask(uint8_t* mask);

signals:
    void devListDoubleClicked(const QModelIndex &index);

private slots:
    void on_addButton_clicked();

    void on_deleteButton_clicked();

    void on_sortButton_clicked();

    void on_searchButton_clicked();

//    void on_treeView_doubleClicked(const QModelIndex &index);

private:
    Ui::DeviceTreeDock *ui;
    QSqlTableModel *devTreeModel;

};

#endif // DEVICETREEDOCK_H
