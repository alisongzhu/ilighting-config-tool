#ifndef TOUCHPANELTABWIDGET_H
#define TOUCHPANELTABWIDGET_H

#include <QSqlTableModel>
#include <QModbusDataUnit>

#include <QTabWidget>
#include "touchpanel_conf.h"


namespace Ui {
class TouchPaneltabwidget;
}

class TouchPaneltabwidget : public QTabWidget
{
    Q_OBJECT

public:
    explicit TouchPaneltabwidget(const QModelIndex &index,QSqlTableModel *model,QWidget *parent = nullptr);
    ~TouchPaneltabwidget();   

private slots:

    void on_SensorEnter_clicked();

    void on_hostDatetimeBtn_clicked();

    void on_basicUploadBtn_clicked();

    void on_basicDownloadBtn_clicked();

    void on_basicGetFromDbBtn_clicked();

    void on_basicSaveToDbBtn_clicked();

private:
    Ui::TouchPaneltabwidget *ui;

    void getConf(device_conf_s *dev);
    void showConf(device_conf_s &conf);
    QModelIndex dev_index;
    QSqlTableModel *dbModel = nullptr;

    QWidget *currentWindow = nullptr;
    QWidget *lastWindow= nullptr;

    int m_address;
};

#endif // TOUCHPANELTABWIDGET_H
