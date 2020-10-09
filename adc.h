#ifndef ADC_H
#define ADC_H

#include <QWidget>
#include "touchpanel_conf.h"

#include <QTabWidget>
#include <QSqlTableModel>
#include <QModbusDataUnit>

namespace Ui {
class adc;
}

class adc : public QWidget
{
    Q_OBJECT

public:
    explicit adc(const QModelIndex &index,QSqlTableModel *model,int sensoraddress,QWidget *parent = nullptr);
    ~adc();
    void Delay_MSec(unsigned int msec);

private slots:
    void on_FunctionDetermineButton_clicked();

    void on_basicUploadBtn_clicked();

    void on_basicDownloadBtn_clicked();

    void on_basicSaveToDbBtn_clicked();

    void on_basicGetFromDbBtn_clicked();

private:
    Ui::adc *ui;

    void readmodbus(int address);
    void wirtemodbus(int address);
    void getConf(TouchAdcConfigure *dev);
    void showConf(TouchAdcConfigure &conf);

    int ChannelAddress;
    QModelIndex dev_index;
    QSqlTableModel *dbModel = nullptr;

    int m_address;
    int sensor_address;
};

#endif // ADC_H
