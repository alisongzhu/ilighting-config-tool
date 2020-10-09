#ifndef IIC1_H
#define IIC1_H

#include <QWidget>
#include "touchpanel_conf.h"

#include <QTabWidget>
#include <QSqlTableModel>
#include <QModbusDataUnit>

class QModbusClient;
class QModbusReply;

namespace Ui {
class iic1;
}

class iic1 : public QWidget
{
    Q_OBJECT

public:
    explicit iic1(const QModelIndex &index,QSqlTableModel *model,int sensoraddress,QWidget *parent = nullptr);
    ~iic1();
    void Delay_MSec(unsigned int msec);

private slots:
    void on_ModuleDetermineButton_clicked();

    void on_FunctionDetermineButton_clicked();

    void on_basicUploadBtn_clicked();

    void on_basicDownloadBtn_clicked();

    void on_basicGetFromDbBtn_clicked();

    void on_basicSaveToDbBtn_clicked();

private:
    Ui::iic1 *ui;

    void readmodbus(int address);
    void wirtemodbus(int address);
    void getConf(TouchPanelConfigure *dev);
    void showConf(TouchPanelConfigure &conf);

    int ChannelAddress;
    QModelIndex dev_index;
    QSqlTableModel *dbModel = nullptr;

    int m_address;
    int sensor_address;
};

#endif // IIC1_H
