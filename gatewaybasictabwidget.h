#ifndef GATEWAYBASICTABWIDGET_H
#define GATEWAYBASICTABWIDGET_H


#include "gateway_module_conf.h"

#include <QTabWidget>
#include <QListWidget>
#include <QSqlTableModel>

//class QModbusClient;
//class QModbusReply;

namespace Ui {
class GatewayBasicTabWidget;
}

class GatewayBasicTabWidget : public QTabWidget
{
    Q_OBJECT

public:
    explicit GatewayBasicTabWidget(const QModelIndex &index,QSqlTableModel *model,QWidget *parent = nullptr);
    ~GatewayBasicTabWidget();

#if 0
    module_network getModuleNetwork();
    module_mb_comm getModuleMbComm();
    schedule_item getCurrentScheduleItem();

    void setModuleNetwork(module_network &param);
    void setModuleMbComm(module_mb_comm &param);
    void setScheduleItem(schedule_item &item);

    void getSettings(module_network &param);
    void getSettings(module_mb_comm &param);
    void getSettings(schedule_item &param);

    void setSettings(module_network &param);
    void setSettings(module_mb_comm &param);
    void setSettings(schedule_item &param);

    int getAddress();
    void setAddress(int address);

    void setModbusClient(QModbusClient *client);

#endif

private slots:

    void on_hostDatetimeBtn_clicked();

    void on_basicSaveToDbBtn_clicked();

    void on_basicGetFromDbBtn_clicked();

    void on_basicDownloadBtn_clicked();

    void on_basicUploadBtn_clicked();

    void on_advUploadBtn_clicked();

    void on_advDownloadBtn_clicked();

    void on_advGetFromDbBtn_clicked();

    void on_advSaveToDbBtn_clicked();

    void on_getScheduleCountInDbBtn_clicked();

    void on_ResetButton_clicked();

private:

#if 0
    uint8_t getWeekdayMask();
    uint16_t getMonthMask();
    uint32_t getDayMask();

    uint32_t getListMask(QListWidget *list);
    void setListMask(QListWidget *list,uint32_t mask);
#endif

    void showConf(device_conf_s& dev);
    void getConf(device_conf_s* dev);

    void showConf(module_network_s& ethernet);
    void getConf(module_network_s* ethernet);

    void showConf(module_mb_comm_s &comm);
    void getConf(module_mb_comm_s *comm);

    void showConf(module_holding_s &holding);
    void getConf(module_holding_s *holding);

    void showConf(schedule_item_s &schedule);
    void getConf(schedule_item_s *schedule);

    void getSubmoduleMask(uint8_t *buf);
    void execSpecialOperation(quint16 address, quint16 value);

    template<typename T> void saveDataToDb(T *conf,int index);

    Ui::GatewayBasicTabWidget *ui;

    QModelIndex dev_index;
    QSqlTableModel *dbModel = nullptr;

    int m_address;

};

#endif // GATEWAYBASICTABWIDGET_H
