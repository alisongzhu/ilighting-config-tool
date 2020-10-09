#ifndef RELAYPROPERTYTABWIDGET_H
#define RELAYPROPERTYTABWIDGET_H


#include "relay_module_conf.h"

#include <QTabWidget>
#include <QSqlTableModel>
#include <QModbusDataUnit>

//class QModbusClient;
//class QModbusReply;


namespace Ui {
class RelayPropertyTabWidget;
}

class RelayPropertyTabWidget : public QTabWidget
{
    Q_OBJECT

public:
    explicit RelayPropertyTabWidget(const QModelIndex &index,QSqlTableModel *model,QWidget *parent = nullptr);
    ~RelayPropertyTabWidget();

//    void setModbusClient(QModbusClient *client);

private:
    void showConf(device_conf_s& conf);
    void getConf(device_conf_s* conf);

    void showConf(relay_global_conf_s& conf);
    void getConf(relay_global_conf_s* conf);

    void showConf(relay_module_status_s &conf);

    void showConf(relay_circuit_conf_s& conf);
    void getConf(relay_circuit_conf_s* conf);

    void showConf(scene_def_s& conf);
    void getConf(scene_def_s* conf);

    void showConf(relay_circuit_analog_s &conf);
    void showConf(relay_circuit_statistics_s &conf);
    void Delay_MSec(unsigned int msec);

    template<typename T> void saveDataToDb(T *conf,int index);

#if 0
    QModbusDataUnit readBasicRequest() const;
    QModbusDataUnit writeBasicRequest() const;
#endif

    void execSingleCircuit(quint8 number,bool onOff);

    void execSpecialOperation(quint16 address, quint16 value);

private slots:
    void on_hostDatetimeBtn_clicked();

    void on_basicUploadBtn_clicked();

    void on_basicDownloadBtn_clicked();

    void on_basicGetFromDbBtn_clicked();

    void on_basicSaveToDbBtn_clicked();

    void on_globalUploadBtn_clicked();

    void on_globalDownloadBtn_clicked();

    void on_globalGetFromDbBtn_clicked();

    void on_globalSaveToDbBtn_clicked();

    void on_circuitUploadBtn_clicked();

    void on_circuitDownloadBtn_clicked();

    void on_circuitGetFromDbBtn_clicked();

    void on_circuitSaveToDbBtn_clicked();

    void on_sceneUploadBtn_clicked();

    void on_sceneDownloadBtn_clicked();

    void on_sceneGetFromDbBtn_clicked();

    void on_sceneSaveToDbBtn_clicked();

    void on_digitalStatusBtn_clicked();

    void on_analogStatusBtn_clicked();

    void on_statisticsBtn_clicked();

    void on_getAlarmBtn_clicked();

    void on_relayOnBtn_clicked();

    void on_relayOffBtn_clicked();


    void on_resetControlBtn_clicked();

    void on_saveControlBtn_clicked();

    void on_clearEnergyBtn_clicked();

    void on_clearCountBtn_clicked();

    void on_resetPowerStorageBtn_clicked();

    void on_calibrateBtn_clicked();

    void on_getDceneCountDEV_clicked();

    void on_allOnBtn_clicked();

    void on_allOffBtn_clicked();

    void on_getSceneCountDB_clicked();

    void on_statusReadBtn_clicked();

    void on_SceneButton_clicked();

private:
    Ui::RelayPropertyTabWidget *ui;

    QModelIndex dev_index;
    QSqlTableModel *dbModel = nullptr;

    int relay_type;
    int m_address;
};

#endif // RELAYPROPERTYTABWIDGET_H
