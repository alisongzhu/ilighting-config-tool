#include "relaypropertytabwidget.h"
#include "ui_relaypropertytabwidget.h"

#include "global_res.h"
#include "logmanager.h"

#include <QModbusClient>
#include <QtDebug>
#include <QSqlError>
#include <QElapsedTimer>
#include <QTimer>


RelayPropertyTabWidget::RelayPropertyTabWidget(const QModelIndex &index,QSqlTableModel *model,QWidget *parent) :
    QTabWidget(parent),
    ui(new Ui::RelayPropertyTabWidget)
{
    ui->setupUi(this);

    dev_index = index;
    dbModel = model;

    m_address = dbModel->data(dbModel->index(dev_index.row(),0)).toInt();
    ui->addressEdit->setText(QString::number(m_address));

    int type = dbModel->data(dbModel->index(dev_index.row(),1)).toInt();
    ui->typeCombo->addItems(dev_typeList);
    ui->typeCombo->setCurrentIndex(type);

    relay_type = dbModel->data(dbModel->index(dev_index.row(),1)).toInt();

    on_hostDatetimeBtn_clicked();

    //创建场景定义列表
    int loop=0;
    if(relay_type==1)
    {
      loop=6;
    }
    else if(relay_type==2)
    {
      loop=12;
    }
    else if(relay_type==4)
    {
      loop=6;
      ui->globalGroup->setEnabled(false);
      ui->circuitSettingGroup->setEnabled(false);
      ui->clearEnergyBtn->setEnabled(false);
      ui->resetPowerStorageBtn->setEnabled(false);
      ui->calibrateBtn->setEnabled(false);
      ui->analogStatusBtn->setEnabled(false);
      ui->analogStatusTable->setEnabled(false);
    }else{
      loop=12;
      ui->globalGroup->setEnabled(false);
      ui->circuitSettingGroup->setEnabled(false);
      ui->clearEnergyBtn->setEnabled(false);
      ui->resetPowerStorageBtn->setEnabled(false);
      ui->calibrateBtn->setEnabled(false);
      ui->analogStatusBtn->setEnabled(false);
      ui->analogStatusTable->setEnabled(false);
    }


    for(int i = 0 ;i<loop;i++)
    {
        QListWidgetItem *item = new QListWidgetItem(tr("Circuit""%1").arg(i),ui->scenDefList);
        item->setCheckState(Qt::Unchecked);
    }

    //创建数字量状态监视列表
    for(int i = 0 ;i<loop;i++)
    {
        QListWidgetItem *item = new QListWidgetItem(tr("RelayState""%1").arg(i),ui->digitalStatusList);
        item->setCheckState(Qt::Unchecked);
    }

    //创建回路告警监视列表
    for(int i = 0 ;i<14;i++)
    {
        QListWidgetItem *item = new QListWidgetItem(tr("CircuitAlarm""%1").arg(i),ui->alarmList);
        item->setCheckState(Qt::Unchecked);
    }

    QListWidgetItem *fireAlarmItem = new QListWidgetItem(tr("FireAlarm"),ui->alarmList);
    fireAlarmItem->setCheckState(Qt::Unchecked);

    QListWidgetItem *totalAlarm = new QListWidgetItem(tr("TotalAlarm"),ui->alarmList);
    totalAlarm->setCheckState(Qt::Unchecked);

    //创建模拟量监视列表
    ui->analogStatusTable->setRowCount(6);
    ui->analogStatusTable->setColumnCount(1);

    QStringList analogVerticalHeader;
    analogVerticalHeader << tr("Voltage") << tr("Current");
    analogVerticalHeader << tr("Frequency") << tr("Factor");
    analogVerticalHeader << tr("Power") << tr("Energy");

    ui->analogStatusTable->setVerticalHeaderLabels(analogVerticalHeader);

    QStringList horizontalHeader;
    horizontalHeader << tr("Value");
    ui->analogStatusTable->setHorizontalHeaderLabels(horizontalHeader);

    for (int i = 0 ; i < 6; i++) {
        QTableWidgetItem *item = new QTableWidgetItem;
        ui->analogStatusTable->setItem(i,0,item);
    }

    //创建回路统计列表
    ui->statisticsTable->setRowCount(3);
    ui->statisticsTable->setColumnCount(1);
    QStringList statisticsVerticalHeader;
    statisticsVerticalHeader << tr("Execute Count");
    statisticsVerticalHeader << tr("Current On Time");
    statisticsVerticalHeader << tr("Alarm Value");

    ui->statisticsTable->setVerticalHeaderLabels(statisticsVerticalHeader);
    ui->statisticsTable->setHorizontalHeaderLabels(horizontalHeader);

    for (int i = 0 ; i < 3; i++) {
        QTableWidgetItem *item = new QTableWidgetItem;
        ui->statisticsTable->setItem(i,0,item);
    }


    //显示选项卡0
    setCurrentIndex(0);
}

RelayPropertyTabWidget::~RelayPropertyTabWidget()
{
    delete ui;
}


#if 0
void RelayPropertyTabWidget::setModbusClient(QModbusClient *client)
{
    modbusDevice = client;
}
#endif


void RelayPropertyTabWidget::Delay_MSec(unsigned int msec)
   {
       QEventLoop loop;//定义一个新的事件循环
       QTimer::singleShot(msec, &loop, SLOT(quit()));//创建单次定时器，槽函数为事件循环的退出函数
       loop.exec();//事件循环开始执行，程序会卡在这里，直到定时时间到，本循环被退出
   }

void RelayPropertyTabWidget::on_hostDatetimeBtn_clicked()
{
    QDateTime now = QDateTime::currentDateTime();
    ui->dateTimeEdit->setDateTime(now);
}

void RelayPropertyTabWidget::showConf(device_conf_s &conf)
{
    ui->baudCombo->setCurrentText(QString::number(conf.powerbus_baudrate));
    ui->periodSpin->setValue(conf.update_time);

    QDate date(conf.rtc.year,conf.rtc.month,conf.rtc.day);
    ui->dateTimeEdit->setDate(date);

    QTime time(conf.rtc.hour,conf.rtc.min,conf.rtc.sec);
    ui->dateTimeEdit->setTime(time);

    ui->addressEdit->setText(QString::number(conf.dev_addr));
}

void RelayPropertyTabWidget::getConf(device_conf_s *dev)
{
    dev->dev_addr = m_address;
    dev->powerbus_baudrate = ui->baudCombo->currentText().toUShort();
    dev->update_time = ui->periodSpin->value();
//    QDateTimeEdit
    dev->rtc.year = ui->dateTimeEdit->date().year();
    dev->rtc.month = ui->dateTimeEdit->date().month();
    dev->rtc.day = ui->dateTimeEdit->date().day();
    dev->rtc.weekday = ui->dateTimeEdit->date().dayOfWeek();

    dev->rtc.hour = ui->dateTimeEdit->time().hour();
    dev->rtc.min = ui->dateTimeEdit->time().minute();
    dev->rtc.sec = ui->dateTimeEdit->time().second();
}

void RelayPropertyTabWidget::showConf(relay_global_conf_s &conf)
{
    ui->devGlobalAlarmEnabe->setCheckState(conf.alarm_enable?Qt::Checked:Qt::Unchecked);
    ui->badLampDetecEnable->setCheckState(conf.bad_lamp_detection_enable?Qt::Checked:Qt::Unchecked);

    ui->fireModeCombo->setCurrentIndex(conf.fire_protecttion_mode);
    ui->lampProtectionTime->setValue(conf.lamp_protection_time);
    ui->relaySwitchDelay->setValue(conf.lamp_switch_random_factor);
    ui->currentUpdateThreshold->setValue(conf.current_update_threshold);
    ui->voltageUpdateThreshold->setValue(conf.voltage_update_threshold);
    ui->currentDeadband->setValue(conf.current_deadzone);
    ui->relayPulseDelay->setValue(conf.relay_pulse_interval);
    ui->relaySwitchFixedDelay->setValue(conf.relay_control_interval);
    ui->relaySwitchRandomDelay->setValue(conf.relay_control_interval_random_factor);
}

void RelayPropertyTabWidget::getConf(relay_global_conf_s *conf)
{
    conf->alarm_enable = ui->devGlobalAlarmEnabe->checkState() == Qt::Checked?1:0;
    conf->bad_lamp_detection_enable = ui->badLampDetecEnable->checkState()== Qt::Checked?1:0;
    conf->fire_protecttion_mode = ui->fireModeCombo->currentIndex();
    conf->lamp_protection_time = ui->lampProtectionTime->value();
    conf->lamp_switch_random_factor = ui->relaySwitchDelay->value();
    conf->current_update_threshold = ui->currentUpdateThreshold->value();
    conf->voltage_update_threshold = ui->voltageUpdateThreshold->value();
    conf->current_deadzone = ui->currentDeadband->value();
    conf->relay_pulse_interval=ui->relayPulseDelay->value();
    conf->relay_control_interval=ui->relaySwitchFixedDelay->value();
    conf->relay_control_interval_random_factor=ui->relaySwitchRandomDelay->value();
}

void RelayPropertyTabWidget::showConf(relay_module_status_s &conf)
{
    ui->lastExcutedSceneEdit->setText(QString::number(conf.current_scene));
    ui->temperatureEdit->setText(QString::number(conf.temperature));
    ui->sceneCountEdit->setText(QString::number(conf.scene_count));
}

void RelayPropertyTabWidget::showConf(relay_circuit_conf_s &conf)
{
    ui->circuitAlarmEnbale->setCheckState(conf.alarm_enable == 0?Qt::Unchecked : Qt::Checked);
    ui->voltageAlarmSpin->setValue(conf.voltage_alarm_threshold);
    ui->currentAlarmSpin->setValue(conf.current_alarm_threshold);
    ui->dataUpdateSpin->setValue(conf.update_period);
}

void RelayPropertyTabWidget::getConf(relay_circuit_conf_s *conf)
{
    conf->alarm_enable = ui->circuitAlarmEnbale->checkState() == Qt::Unchecked ? 0:1;
    conf->voltage_alarm_threshold = ui->voltageAlarmSpin->value();
    conf->current_alarm_threshold = ui->currentAlarmSpin->value();
    conf->update_period = ui->dataUpdateSpin->value();
    conf->pad=0;
}

void RelayPropertyTabWidget::showConf(scene_def_s &conf)
{
    ui->sceneDefNumberEdit->setText(QString::number(conf.number));
    setListMask(ui->scenDefList,conf.definition);
}

void RelayPropertyTabWidget::getConf(scene_def_s *conf)
{
    conf->number = ui->sceneDefNumberEdit->text().toUShort();
    conf->definition = getListMask(ui->scenDefList);
}

void RelayPropertyTabWidget::showConf(relay_circuit_analog_s &conf)
{
    ui->analogStatusTable->item(0,0)->setData( Qt::DisplayRole,conf.vol);
    ui->analogStatusTable->item(1,0)->setData( Qt::DisplayRole,conf.cur);
    ui->analogStatusTable->item(2,0)->setData( Qt::DisplayRole,conf.freq);
    ui->analogStatusTable->item(3,0)->setData( Qt::DisplayRole,conf.factor);
    ui->analogStatusTable->item(4,0)->setData( Qt::DisplayRole,conf.power);
    ui->analogStatusTable->item(5,0)->setData( Qt::DisplayRole,conf.energy_reading);
}

void RelayPropertyTabWidget::showConf(relay_circuit_statistics_s &conf)
{
    ui->statisticsTable->item(0,0)->setData( Qt::DisplayRole,conf.exec_count);
    ui->statisticsTable->item(1,0)->setData( Qt::DisplayRole,conf.uptime);
    ui->statisticsTable->item(2,0)->setData( Qt::DisplayRole,conf.alarm);
}

void RelayPropertyTabWidget::execSingleCircuit(quint8 number, bool onOff)
{
    if(getModbusClient()->state()!= QModbusDevice::ConnectedState)
    {
        logWarn(tr("Device must be in connected state! connection state:%1").arg(getModbusClient()->state()));;
    }

    //prepare DataUnit
    QModbusDataUnit writeUnit(QModbusDataUnit::Coils,number,1);

    if(onOff)
    {
        writeUnit.setValue(0,1);
    }
    else writeUnit.setValue(0,0);

    if(auto *reply = getModbusClient()->sendWriteRequest(writeUnit,m_address))
    {
        if (!reply->isFinished())
        {
            connect(reply, &QModbusReply::finished, this, [reply,number]() {
                if (reply->error() == QModbusDevice::NoError) {
                    logInfo(tr(" Control Single Circuit %1 Successfully.").arg(number));

                }else {
                    logError( tr("Control Single Circuit %1 response error: %2").arg(number)
                             .arg(reply->errorString()));
                }
                reply->deleteLater();
            });
        }
        else{
            logInfo(tr("Broadcast msg Sent."));
            reply->deleteLater();
        }
    }
    else
    {
        logError(tr("Read relay basic conf error:%1 ").arg(getModbusClient()->errorString()));
    }
}

void RelayPropertyTabWidget::execSpecialOperation(quint16 address, quint16 value)
{
    if(getModbusClient()->state()!= QModbusDevice::ConnectedState)
    {
        logWarn(tr("Device must be in connected state! connection state:%1").arg(getModbusClient()->state()));;
    }

    int modbusaddress=m_address;
    if(ui->ControlRadioButton->isChecked())
    {
        modbusaddress=0;
    }

    //prepare DataUnit
    QModbusDataUnit writeUnit(QModbusDataUnit::HoldingRegisters,address,1);
    writeUnit.setValue(0,value);

    if(auto *reply = getModbusClient()->sendWriteRequest(writeUnit,modbusaddress))
    {
        if (!reply->isFinished())
        {
            connect(reply, &QModbusReply::finished, this, [reply,address]() {
                if (reply->error() == QModbusDevice::NoError) {
                    logInfo(tr("Excute Special Operation address:%1 Successfully.").arg(address));

                }else {
                    logError( tr("Excute Special Operation address:%1 response error: %2").arg(address)
                             .arg(reply->errorString()));
                }
                reply->deleteLater();
            });
        }
        else{
            logInfo(tr("Broadcast msg Sent."));
            reply->deleteLater();
        }
    }
    else
    {
        logError(tr("Excute Special Operation error:%1 ").arg(getModbusClient()->errorString()));
    }
}

#if 0
QModbusDataUnit RelayPropertyTabWidget::readBasicRequest() const
{
    int startAddress = DEV_BASIC_CONF_MODBUS_START_ADDRESS;
    quint16 numberOfEntries = sizeof(device_conf_s)/2;

    return QModbusDataUnit(QModbusDataUnit::HoldingRegisters,startAddress,numberOfEntries);
}

QModbusDataUnit RelayPropertyTabWidget::writeBasicRequest() const
{
    int startAddress = DEV_BASIC_CONF_MODBUS_START_ADDRESS;
    quint16 numberOfEntries = sizeof(device_conf_s)/2;

    QModbusDataUnit writeUnit(QModbusDataUnit::HoldingRegisters,startAddress,numberOfEntries);

    return writeUnit;
}
#endif

void RelayPropertyTabWidget::on_basicUploadBtn_clicked()
{
    if(getModbusClient()->state()!= QModbusDevice::ConnectedState)
    {
        logWarn(tr("Device must be in connected state! connection state:%1").arg(getModbusClient()->state()));
        return ;
    }

    int startAddress = DEV_BASIC_CONF_MODBUS_START_ADDRESS;
    quint16 numberOfEntries = sizeof(device_conf_s)/2;

    QModbusDataUnit readUnit(QModbusDataUnit::HoldingRegisters,startAddress,numberOfEntries);

    if (auto *reply = getModbusClient()->sendReadRequest(readUnit, m_address)) {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, [this,reply](){
                if (reply->error() == QModbusDevice::NoError) {
                    logInfo(tr("Read relay baisc conf successfully!"));

                    const QModbusDataUnit unit = reply->result();

                    device_conf_s conf;
                    quint16 * ptr = (quint16 *)&conf;
                    for (int i = 0,count = unit.values().size(); i < count; i++) {
                        ptr[i] = unit.values().at(i);
                    }
                    this->showConf(conf);

                } else {
                    logWarn( tr("Read relay basic conf response error: %1 ").
                                                arg(reply->errorString()));
                }
                reply->deleteLater();
            });
        else
            delete reply; // broadcast replies return immediately
    }
    else
    {
        logError(tr("Read relay basic conf error:%1 ").arg(getModbusClient()->errorString()));
    }
}

void RelayPropertyTabWidget::on_basicDownloadBtn_clicked()
{
    if(getModbusClient()->state()!= QModbusDevice::ConnectedState)
    {
        logWarn(tr("Device must be in connected state! connection state:%1").arg(getModbusClient()->state()));
        return ;
    }

    //prepare DataUnit
    int startAddress = DEV_BASIC_CONF_MODBUS_START_ADDRESS;
    quint16 numberOfEntries = sizeof(device_conf_s)/2;

    device_conf_s conf;
    QModbusDataUnit writeUnit(QModbusDataUnit::HoldingRegisters,startAddress,numberOfEntries);;
    getConf(&conf);
    quint16 *val = (quint16* )&conf;


   int modbusaddress=m_address;
    if(ui->basicRadioButton->isChecked())
    {
       writeUnit.setStartAddress(startAddress+1);
       writeUnit.setValueCount(numberOfEntries-1);
       modbusaddress=0;
       for (uint i = 0 ,total = int(writeUnit.valueCount()) ;i < total;i++) {
            writeUnit.setValue(i,val[i+1]);
       }
    }
    else
    {
      for (uint i = 0 ,total = int(writeUnit.valueCount()) ;i < total;i++) {
          writeUnit.setValue(i,val[i]);
      }
    }

    if(auto *reply = getModbusClient()->sendWriteRequest(writeUnit,modbusaddress))
    {
        if (!reply->isFinished())
        {
            connect(reply, &QModbusReply::finished, this, [reply]() {
                if (reply->error() == QModbusDevice::NoError) {
                    logInfo(tr(" Write relay basic conf Successfully."));

                }else {
                    logError( tr("Write relay basic conf response error: %1")
                             .arg(reply->errorString()));
                }
                reply->deleteLater();
            });
        }
        else{
            logInfo(tr("Broadcast msg Sent."));
            reply->deleteLater();
        }
    }
    else
    {
        logError(tr("Read relay basic conf error:%1 ").arg(getModbusClient()->errorString()));
    }

}

void RelayPropertyTabWidget::on_basicGetFromDbBtn_clicked()
{
    QByteArray blob = dbModel->data(dbModel->index(dev_index.row(),BASIC_CONF_IN_DB_COLUMN)).toByteArray();
    if(!blob.size())
    {
        logWarn(tr("There is no reley basic data in db now!"));
        return;
    }
    device_conf_s conf;
    memcpy(&conf,blob.data(),sizeof(device_conf_s));
    showConf(conf);
}

void RelayPropertyTabWidget::on_basicSaveToDbBtn_clicked()
{
//    QByteArray blob = dbModel->data(dbModel->index(dev_index.row(),BASIC_CONF_IN_DB_COLUMN)).toByteArray();
//    if(!blob.size())
//    {
//        blob.resize(sizeof(device_conf_s));
//        blob.fill(0,blob.size());
//    }

//    device_conf_s *conf = reinterpret_cast <device_conf_s *>(blob.data());

//    on_hostDatetimeBtn_clicked();

//    getConf(conf);

//    dbModel->setData(dbModel->index(dev_index.row(),BASIC_CONF_IN_DB_COLUMN),blob);
//    dbModel->submitAll();

//    logInfo(tr("Save basic conf to DB :%1.").arg(dbModel->lastError().text()));
    device_conf_s *conf = nullptr;
    saveDataToDb(conf,BASIC_CONF_IN_DB_COLUMN);
    logInfo(tr("Save relay basic conf to DB :%1.").arg(dbModel->lastError().text()));
}

template<typename T>
void RelayPropertyTabWidget::saveDataToDb(T *conf, int index)
{
    QByteArray blob = dbModel->data(dbModel->index(dev_index.row(),index)).toByteArray();
    if(!blob.size())
    {
        blob.resize(sizeof(T));
        blob.fill(0,blob.size());
    }

    conf = reinterpret_cast <T *>(blob.data());

    on_hostDatetimeBtn_clicked();

    getConf(conf);

    dbModel->setData(dbModel->index(dev_index.row(),index),blob);
    dbModel->submitAll();

//    logInfo(tr("Save conf to DB :%1.").arg(dbModel->lastError().text()));
}

void RelayPropertyTabWidget::on_globalUploadBtn_clicked()
{
    if(getModbusClient()->state()!= QModbusDevice::ConnectedState)
    {
        logWarn(tr("Device must be in connected state! connection state:%1").arg(getModbusClient()->state()));
        return ;
    }

    int startAddress = RELAY_GLOBAL_CONF_MODBUS_START_ADDRESS;
    quint16 numberOfEntries = sizeof(relay_global_conf_s)/2;

    QModbusDataUnit readUnit(QModbusDataUnit::HoldingRegisters,startAddress,numberOfEntries);

    if (auto *reply = getModbusClient()->sendReadRequest(readUnit, m_address)) {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, [this,reply](){
                if (reply->error() == QModbusDevice::NoError) {
                    logInfo(tr("Read relay global conf successfully!"));

                    const QModbusDataUnit unit = reply->result();

                    relay_global_conf_s conf ;
                    quint16 * ptr = (quint16 *)&conf;
                    for (int i = 0,count = unit.values().size(); i < count; i++) {
                        ptr[i] = unit.values().at(i);
                    }
                    this->showConf(conf);
                }
                else {
                    logWarn( tr("Read relay global conf response error: %1").
                                                arg(reply->errorString()));
                }
                reply->deleteLater();
            });
        else
            delete reply; // broadcast replies return immediately
    }
    else
    {
        logError(tr("Read relay global conf error:%1 ").arg(getModbusClient()->errorString()));
    }
}

void RelayPropertyTabWidget::on_globalDownloadBtn_clicked()
{
    if(getModbusClient()->state()!= QModbusDevice::ConnectedState)
    {
        logWarn(tr("Device must be in connected state! connection state:%1").arg(getModbusClient()->state()));
        return ;
    }

    //prepare DataUnit
    int startAddress = RELAY_GLOBAL_CONF_MODBUS_START_ADDRESS;
    quint16 numberOfEntries = sizeof(relay_global_conf_s)/2;
    relay_global_conf_s conf;
    getConf(&conf);
    int modbusaddress=m_address;
    QModbusDataUnit writeUnit(QModbusDataUnit::HoldingRegisters,startAddress,numberOfEntries);
    quint16 *val = (quint16* )&conf;

    if(ui->basicRadioButton->isChecked())
    {
       modbusaddress=0;
    }

    for (uint i = 0 ,total = int(writeUnit.valueCount()) ;i < total;i++) {
        writeUnit.setValue(i,val[i]);
    }

    if(auto *reply = getModbusClient()->sendWriteRequest(writeUnit,modbusaddress))
    {
        if (!reply->isFinished())
        {
            connect(reply, &QModbusReply::finished, this, [reply]() {
                if (reply->error() == QModbusDevice::NoError) {
                    logInfo(tr(" Write relay global conf Successfully."));

                }else {
                    logError( tr("Write relay global conf response error: %1")
                             .arg(reply->errorString()));
                }
                reply->deleteLater();
            });
        }
        else{
            logInfo(tr("Broadcast msg Sent."));
            reply->deleteLater();
        }
    }
    else
    {
        logError(tr("Write relay global conf error:%1 ").arg(getModbusClient()->errorString()));
    }
}

void RelayPropertyTabWidget::on_globalGetFromDbBtn_clicked()
{
    QByteArray blob = dbModel->data(dbModel->index(dev_index.row(),RELAY_GLOBAL_CONF_IN_DB_COLUMN)).toByteArray();
    if(!blob.size())
    {
        logWarn(tr("There is no reley global data in db now!"));
        return;
    }
    relay_global_conf_s *conf = reinterpret_cast <relay_global_conf_s *>(blob.data());
    showConf(*conf);
}

void RelayPropertyTabWidget::on_globalSaveToDbBtn_clicked()
{
    QByteArray blob = dbModel->data(dbModel->index(dev_index.row(),RELAY_GLOBAL_CONF_IN_DB_COLUMN)).toByteArray();
    if(!blob.size())
    {
        blob.resize(sizeof(relay_global_conf_s));
        blob.fill(0,blob.size());
    }

    relay_global_conf_s *conf = reinterpret_cast <relay_global_conf_s *>(blob.data());
    getConf(conf);

    dbModel->setData(dbModel->index(dev_index.row(),RELAY_GLOBAL_CONF_IN_DB_COLUMN),blob);
    dbModel->submitAll();

    logInfo(tr("Save relay global conf to DB :%1.").arg(dbModel->lastError().text()));
}

void RelayPropertyTabWidget::on_circuitUploadBtn_clicked()
{
    if(getModbusClient()->state()!= QModbusDevice::ConnectedState)
    {
        logWarn(tr("Device must be in connected state! connection state:%1").arg(getModbusClient()->state()));
        return ;
    }

    //prepare DataUnit
    int startAddress = RELAY_CIRCUIT_CONF_MODBUS_START_ADDRESS + ui->circuitNumberSpin->value()*sizeof(relay_circuit_conf_s)/2;
    quint16 numberOfEntries = sizeof(relay_circuit_conf_s)/2;
    QModbusDataUnit readUnit(QModbusDataUnit::HoldingRegisters,startAddress,numberOfEntries);

    if(auto *reply = getModbusClient()->sendReadRequest(readUnit,m_address)){
        if (!reply->isFinished()){
            connect(reply, &QModbusReply::finished, this, [this,reply](){
                if (reply->error() == QModbusDevice::NoError) {
                    logInfo(tr("Read curcuit conf successfully!"));
                    const QModbusDataUnit unit = reply->result();

                    relay_circuit_conf_s conf ;
                    quint16 * ptr = (quint16 *)&conf;
                    for (int i = 0,count = unit.values().size(); i < count; i++) {
                        ptr[i] = unit.values().at(i);
                    }
                    this->showConf(conf);

                } else {
                    logWarn( tr("Read curcuit conf response error: %1").
                                                arg(reply->errorString()));
                }
                reply->deleteLater();
            });
        }
        else{
            // broadcast replies return immediately
            delete reply;
        }
    }
}

void RelayPropertyTabWidget::on_circuitDownloadBtn_clicked()
{
    if(getModbusClient()->state()!= QModbusDevice::ConnectedState)
    {
        logWarn(tr("Device must be in connected state! connection state:%1").arg(getModbusClient()->state()));
        return ;
    }
    //prepare DataUnit
    int startAddress = RELAY_CIRCUIT_CONF_MODBUS_START_ADDRESS + ui->circuitNumberSpin->value()*sizeof(relay_circuit_conf_s)/2;

    quint16 numberOfEntries =sizeof(relay_circuit_conf_s)/2;
    QModbusDataUnit writeUnit;
    QModbusDataUnit write1Unit;
    relay_circuit_conf_s conf;
    getConf(&conf);
    quint16 *val = (quint16* )&conf;

    int modbusaddress=m_address;
    QString str=ui->advConfigModeCombo->currentText();
    if(str=="单回路")
    {
      QModbusDataUnit tempUint(QModbusDataUnit::HoldingRegisters,startAddress,numberOfEntries);
      for (uint i = 0 ,total = int(tempUint.valueCount()) ;i < total;i++) {
          tempUint.setValue(i,val[i]);
      }
      writeUnit = tempUint;
    }
    else if(str=="全回路")
    {
      QModbusDataUnit tempUint(QModbusDataUnit::HoldingRegisters,RELAY_CIRCUIT_CONF_MODBUS_START_ADDRESS,24);
      uint8_t * p=(uint8_t*)malloc(48);
      for(uint j=0;j<6;j++)
      {
          memcpy(&p[8*j],&conf,sizeof(relay_circuit_conf_s));
      }
      quint16 *temp = (quint16*)p;

      for (uint i = 0 ,total = int(tempUint.valueCount()) ;i < total;i++) {
          tempUint.setValue(i,temp[i]);
      }
      writeUnit = tempUint;


      QModbusDataUnit temp1Uint(QModbusDataUnit::HoldingRegisters,RELAY_CIRCUIT_CONF_MODBUS_START_ADDRESS+24,24);
      uint8_t * q=(uint8_t*)malloc(48);
      q=p;
      quint16 *temp1 = (quint16*)q;

      for (uint i = 0 ,total = int(temp1Uint.valueCount()) ;i < total;i++) {
          temp1Uint.setValue(i,temp1[i]);
      }
      write1Unit = temp1Uint;
    }
    else if(str=="广播单回路")
    {
      modbusaddress=0;
      QModbusDataUnit tempUint(QModbusDataUnit::HoldingRegisters,startAddress,numberOfEntries);
      for (uint i = 0 ,total = int(tempUint.valueCount()) ;i < total;i++) {
          tempUint.setValue(i,val[i]);
      }
      writeUnit = tempUint;
    }
    else{
        modbusaddress=0;
        QModbusDataUnit tempUint(QModbusDataUnit::HoldingRegisters,RELAY_CIRCUIT_CONF_MODBUS_START_ADDRESS,24);
        uint8_t * p=(uint8_t*)malloc(48);
        for(uint j=0;j<6;j++)
        {
            memcpy(&p[8*j],&conf,sizeof(relay_circuit_conf_s));
        }
        quint16 *temp = (quint16*)p;

        for (uint i = 0 ,total = int(tempUint.valueCount()) ;i < total;i++) {
            tempUint.setValue(i,temp[i]);
        }
        writeUnit = tempUint;


        QModbusDataUnit temp1Uint(QModbusDataUnit::HoldingRegisters,RELAY_CIRCUIT_CONF_MODBUS_START_ADDRESS+24,24);
        uint8_t * q=(uint8_t*)malloc(48);
        q=p;
        quint16 *temp1 = (quint16*)q;

        for (uint i = 0 ,total = int(temp1Uint.valueCount()) ;i < total;i++) {
            temp1Uint.setValue(i,temp1[i]);
        }
        write1Unit = temp1Uint;
      }

    if(auto *reply = getModbusClient()->sendWriteRequest(writeUnit,modbusaddress)){
        if (!reply->isFinished()){
            connect(reply, &QModbusReply::finished, this, [reply](){
                if (reply->error() == QModbusDevice::NoError) {
                    logInfo(tr("Write circuit conf successfully!"));                    
                } else {
                    logWarn( tr("Write circuit conf response error: %1").
                                                arg(reply->errorString()));
                }
                reply->deleteLater();
            });        
        }
        else{
            // broadcast replies return immediately
            delete reply;
        }
    }

   Delay_MSec(2000);//延时1s

    if(str=="全回路"||str=="广播全回路")
    {
      if(auto *reply1 = getModbusClient()->sendWriteRequest(write1Unit,modbusaddress)){
          if (!reply1->isFinished()){
              connect(reply1, &QModbusReply::finished, this, [reply1](){
                  if (reply1->error() == QModbusDevice::NoError) {
                      logInfo(tr("Write circuit conf successfully!"));
                  } else {
                      logWarn( tr("Write circuit conf response error: %1").
                                                  arg(reply1->errorString()));
                  }
                  reply1->deleteLater();
              });
          }
          else{
              // broadcast replies return immediately
              delete reply1;
          }
      }
    }
}

void RelayPropertyTabWidget::on_circuitGetFromDbBtn_clicked()
{
    QByteArray blob = dbModel->data(dbModel->index(dev_index.row(),RELAY_CIRCUIT_CONF_IN_DB_COLUMN)).toByteArray();
    if(!blob.size())
    {
        logWarn(tr("There is no reley circuit data in db now!"));
        return;
    }
    relay_circuit_conf_s *conf = reinterpret_cast <relay_circuit_conf_s *>(blob.data());

    showConf(conf[ui->circuitNumberSpin->value()]);
}

void RelayPropertyTabWidget::on_circuitSaveToDbBtn_clicked()
{
    QByteArray blob = dbModel->data(dbModel->index(dev_index.row(),RELAY_CIRCUIT_CONF_IN_DB_COLUMN)).toByteArray();
    if(!blob.size())
    {
        blob.resize(sizeof(relay_circuit_conf_s)*12);
        blob.fill(0,blob.size());
    }
    relay_circuit_conf_s *conf = reinterpret_cast <relay_circuit_conf_s *>(blob.data());
    getConf(&conf[ui->circuitNumberSpin->value()]);

    QString str=ui->advConfigModeCombo->currentText();
    if(str=="广播全回路"||str=="全回路")
    {
      for(int i=0;i<12;i++)
      {
       memcpy(&blob.data()[i*8],&conf[ui->circuitNumberSpin->value()],sizeof(relay_circuit_conf_s));
      }
    }
    dbModel->setData(dbModel->index(dev_index.row(),RELAY_CIRCUIT_CONF_IN_DB_COLUMN),blob);
    dbModel->submitAll();
}

void RelayPropertyTabWidget::on_sceneUploadBtn_clicked()
{
    if(getModbusClient()->state()!= QModbusDevice::ConnectedState)
    {
        logWarn(tr("Device must be in connected state! connection state:%1").arg(getModbusClient()->state()));
        return ;
    }

    //prepare DataUnit
    int startAddress = RELAY_SCENE_DEF_MODBUS_START_ADDRESS + ui->sceneOrderSpin->value()*sizeof(scene_def_s)/2;
    quint16 numberOfEntries = sizeof(scene_def_s)/2;
    QModbusDataUnit readUnit(QModbusDataUnit::HoldingRegisters,startAddress,numberOfEntries);

    if(auto *reply = getModbusClient()->sendReadRequest(readUnit,m_address)){
        if (!reply->isFinished()){
            connect(reply, &QModbusReply::finished, this, [this,reply](){
                if (reply->error() == QModbusDevice::NoError) {
                    logInfo(tr("Read scene definition successfully!"));
                    const QModbusDataUnit unit = reply->result();
                    scene_def_s conf ;
                    quint16 * ptr = (quint16 *)&conf;
                    for (int i = 0,count = unit.values().size(); i < count; i++) {
                        ptr[i] = unit.values().at(i);
                    }
                    this->showConf(conf);

                } else {
                    logWarn( tr("Read scene definition response error: %1").
                                                arg(reply->errorString()));
                }
                reply->deleteLater();
            });
        }
        else{
            // broadcast replies return immediately
            delete reply;
        }
    }
}

void RelayPropertyTabWidget::on_sceneDownloadBtn_clicked()
{
    if(getModbusClient()->state()!= QModbusDevice::ConnectedState)
    {
        logWarn(tr("Device must be in connected state! connection state:%1").arg(getModbusClient()->state()));
        return ;
    }

    //prepare DataUnit
    int startAddress = RELAY_SCENE_DEF_MODBUS_START_ADDRESS + ui->sceneOrderSpin->value()*sizeof(scene_def_s)/2;

    quint16 numberOfEntries =sizeof(scene_def_s)/2;
    QModbusDataUnit writeUnit(QModbusDataUnit::HoldingRegisters,startAddress,numberOfEntries);
    scene_def_s conf;
    getConf(&conf);
    quint16 *val = (quint16* )&conf;

    int modbusaddress=m_address;
    QString str=ui->advConfigModeCombo->currentText();
   if(str=="广播单回路")
    {
      modbusaddress=0;
    }

    for (uint i = 0 ,total = int(writeUnit.valueCount()) ;i < total;i++) {
       writeUnit.setValue(i,val[i]);
    }

    if(auto *reply = getModbusClient()->sendWriteRequest(writeUnit,modbusaddress)){
        if (!reply->isFinished()){
            connect(reply, &QModbusReply::finished, this, [reply](){
                if (reply->error() == QModbusDevice::NoError) {
                    logInfo(tr("Write scene definition successfully!"));
                } else {
                    logWarn( tr("Write scene definition response error: %1").
                                                arg(reply->errorString()));
                }
                reply->deleteLater();
            });
        }
        else{
            // broadcast replies return immediately
            delete reply;
        }
    }
}

void RelayPropertyTabWidget::on_sceneGetFromDbBtn_clicked()
{
    QByteArray blob = dbModel->data(dbModel->index(dev_index.row(),RELAY_SCENE_CONF_IN_DB_COLUMN)).toByteArray();
    if(!blob.size())
    {
        logWarn(tr("There is no scene definition data in db now!"));
        return;
    }
    scene_def_s *conf = reinterpret_cast <scene_def_s *>(blob.data());

    showConf(conf[ui->sceneOrderSpin->value()]);    
}

void RelayPropertyTabWidget::on_sceneSaveToDbBtn_clicked()
{
    QByteArray blob = dbModel->data(dbModel->index(dev_index.row(),RELAY_SCENE_CONF_IN_DB_COLUMN)).toByteArray();
    if(!blob.size())
    {
        blob.resize(sizeof(scene_def_s)*MAX_SCENE_DEFINITION_COUNT);
        blob.fill(0,blob.size());
    }
    scene_def_s *conf = reinterpret_cast <scene_def_s *>(blob.data());
    getConf(&conf[ui->sceneOrderSpin->value()]);
    dbModel->setData(dbModel->index(dev_index.row(),RELAY_SCENE_CONF_IN_DB_COLUMN),blob);
    dbModel->submitAll();
}

void RelayPropertyTabWidget::on_digitalStatusBtn_clicked()
{
    if(getModbusClient()->state()!= QModbusDevice::ConnectedState)
    {
        logWarn(tr("Device must be in connected state! connection state:%1").arg(getModbusClient()->state()));
        return ;
    }

    //prepare DataUnit
    int startAddress = 0;
    quint16 numberOfEntries = 16;
    QModbusDataUnit readUnit(QModbusDataUnit::DiscreteInputs,startAddress,numberOfEntries);

    if(auto *reply = getModbusClient()->sendReadRequest(readUnit,m_address)){
        if (!reply->isFinished()){
            connect(reply, &QModbusReply::finished, this, [this,reply](){
                if (reply->error() == QModbusDevice::NoError) {
                    logInfo(tr("Read switch state successfully!"));
                    const QModbusDataUnit unit = reply->result();

                    quint16 val;
                    int count = 0;
                    if(relay_type==1||relay_type==4)
                    {
                      count=6;
                    }
                    else if(relay_type==2||relay_type==5)
                    {
                      count=12;
                    }


                    for (int i = 0;i < count; i++) {
                        val = unit.values().at(i);
                        if(0 == val)
                        {
                            this->ui->digitalStatusList->item(i)->setCheckState(Qt::Unchecked);
                        }
                        else
                            this->ui->digitalStatusList->item(i)->setCheckState(Qt::Checked);
                    }
                } else {
                    logWarn( tr("Read switch state response error: %1").
                                                arg(reply->errorString()));
                }
                reply->deleteLater();
            });
        }
        else{
            // broadcast replies return immediately
            delete reply;
        }
    }
}

void RelayPropertyTabWidget::on_analogStatusBtn_clicked()
{
    if(getModbusClient()->state()!= QModbusDevice::ConnectedState)
    {
        logWarn(tr("Device must be in connected state! connection state:%1").arg(getModbusClient()->state()));
        return ;
    }

    //prepare DataUnit
    int startAddress = RELAY_CIRCUIT_ANALOG_MODBUS_START_ADDRESS + ui->analogCircuitSpin->value() * sizeof(relay_circuit_analog_s)/2;
    quint16 numberOfEntries = sizeof(relay_circuit_analog_s)/2;
    QModbusDataUnit readUnit(QModbusDataUnit::InputRegisters,startAddress,numberOfEntries);

    if(auto *reply = getModbusClient()->sendReadRequest(readUnit,m_address)){
        if (!reply->isFinished()){
            connect(reply, &QModbusReply::finished, this, [this,reply](){
                if (reply->error() == QModbusDevice::NoError) {
                    logInfo(tr("Read circuit status successfully!"));
                    const QModbusDataUnit unit = reply->result();
                    relay_circuit_analog_s conf;
                    quint16 * ptr = (quint16 *)&conf;
                    for (int i = 0,count = unit.values().size(); i < count; i++) {
                        ptr[i] = unit.values().at(i);
                    }
                    this->showConf(conf);

                } else {
                    logWarn( tr("Read circuit status response error: %1").
                                                arg(reply->errorString()));
                }
                reply->deleteLater();
            });
        }
        else{
            // broadcast replies return immediately
            delete reply;
        }
    }
}

void RelayPropertyTabWidget::on_statisticsBtn_clicked()
{
    if(getModbusClient()->state()!= QModbusDevice::ConnectedState)
    {
        logWarn(tr("Device must be in connected state! connection state:%1").arg(getModbusClient()->state()));
        return ;
    }

    //prepare DataUnit
    int startAddress = RELAY_CIRCUIT_STATISTICS_MODBUS_START_ADDRESS + ui->analogCircuitSpin->value() * sizeof(relay_circuit_statistics_s)/2;
    quint16 numberOfEntries = sizeof(relay_circuit_statistics_s)/2;
    QModbusDataUnit readUnit(QModbusDataUnit::InputRegisters,startAddress,numberOfEntries);

    if(auto *reply = getModbusClient()->sendReadRequest(readUnit,m_address)){
        if (!reply->isFinished()){
            connect(reply, &QModbusReply::finished, this, [this,reply](){
                if (reply->error() == QModbusDevice::NoError) {
                    logInfo(tr("Read circuit statistics successfully!"));
                    const QModbusDataUnit unit = reply->result();
                    relay_circuit_statistics_s conf;
                    quint16 * ptr = (quint16 *)&conf;
                    for (int i = 0,count = unit.values().size(); i < count; i++) {
                        ptr[i] = unit.values().at(i);
                    }
                    this->showConf(conf);

                } else {
                    logWarn( tr("Read circuit statistics response error: %1").
                                                arg(reply->errorString()));
                }
                reply->deleteLater();
            });
        }
        else{
            // broadcast replies return immediately
            delete reply;
        }
    }
}

void RelayPropertyTabWidget::on_getAlarmBtn_clicked()
{
    if(getModbusClient()->state()!= QModbusDevice::ConnectedState)
    {
        logWarn(tr("Device must be in connected state! connection state:%1").arg(getModbusClient()->state()));
        return ;
    }

    //prepare DataUnit
    int startAddress = 16;
    quint16 numberOfEntries = 16;
    QModbusDataUnit readUnit(QModbusDataUnit::DiscreteInputs,startAddress,numberOfEntries);

    if(auto *reply = getModbusClient()->sendReadRequest(readUnit,m_address)){
        if (!reply->isFinished()){
            connect(reply, &QModbusReply::finished, this, [this,reply](){
                if (reply->error() == QModbusDevice::NoError) {
                    logInfo(tr("Read circuit alarm successfully!"));
                    const QModbusDataUnit unit = reply->result();
                    quint16 val;
                    for (int i = 0,count = unit.valueCount();i < count; i++) {
                        val = unit.values().at(i);
                        if(0 == val)
                        {
                            this->ui->alarmList->item(i)->setCheckState(Qt::Unchecked);
                        }
                        else
                            this->ui->alarmList->item(i)->setCheckState(Qt::Checked);
                    }

                } else {
                    logWarn( tr("Read circuit alarm response error: %1").
                                                arg(reply->errorString()));
                }
                reply->deleteLater();
            });
        }
        else{
            // broadcast replies return immediately
            delete reply;
        }
    }
}

void RelayPropertyTabWidget::on_relayOnBtn_clicked()
{
#if 0
    if(getModbusClient()->state()!= QModbusDevice::ConnectedState)
    {
        logWarn(tr("Device must be in connected state! connection state:%1").arg(getModbusClient()->state()));
        return ;
    }

    //prepare DataUnit
    int startAddress = ui->analogCircuitSpin->value();
    quint16 numberOfEntries = 1;
    QModbusDataUnit writeUnit(QModbusDataUnit::Coils,startAddress,numberOfEntries);

    writeUnit.setValue(0,1);

    if(auto *reply = getModbusClient()->sendWriteRequest(writeUnit,m_address))
    {
        if (!reply->isFinished())
        {
            connect(reply, &QModbusReply::finished, this, [reply]() {
                if (reply->error() == QModbusDevice::NoError) {
                    logInfo(tr(" Write relay basic conf Successfully."));

                }else {
                    logError( tr("Write relay basic conf response error: %1")
                             .arg(reply->errorString()));
                }
                reply->deleteLater();
            });
        }
        else{
            logInfo(tr("Broadcast msg Sent."));
            reply->deleteLater();
        }
    }
    else
    {
        logError(tr("Read relay basic conf error:%1 ").arg(getModbusClient()->errorString()));
    }
#endif
    execSingleCircuit(ui->analogCircuitSpin->value(),true);
}

void RelayPropertyTabWidget::on_relayOffBtn_clicked()
{
#if 0
    if(getModbusClient()->state()!= QModbusDevice::ConnectedState)
    {
        logWarn(tr("Device must be in connected state! connection state:%1").arg(getModbusClient()->state()));
        return ;
    }

    //prepare DataUnit
    int startAddress = ui->analogCircuitSpin->value();
    quint16 numberOfEntries = 1;
    QModbusDataUnit writeUnit(QModbusDataUnit::Coils,startAddress,numberOfEntries);

    writeUnit.setValue(0,0);

    if(auto *reply = getModbusClient()->sendWriteRequest(writeUnit,m_address))
    {
        if (!reply->isFinished())
        {
            connect(reply, &QModbusReply::finished, this, [reply]() {
                if (reply->error() == QModbusDevice::NoError) {
                    logInfo(tr(" Write relay basic conf Successfully."));

                }else {
                    logError( tr("Write relay basic conf response error: %1")
                             .arg(reply->errorString()));
                }
                reply->deleteLater();
            });
        }
        else{
            logInfo(tr("Broadcast msg Sent."));
            reply->deleteLater();
        }
    }
    else
    {
        logError(tr("Read relay basic conf error:%1 ").arg(getModbusClient()->errorString()));
    }
#endif
    execSingleCircuit(ui->analogCircuitSpin->value(),false);
}



void RelayPropertyTabWidget::on_resetControlBtn_clicked()
{
    execSpecialOperation(2000,0x5555);
}

void RelayPropertyTabWidget::on_saveControlBtn_clicked()
{
    execSpecialOperation(2001,0x5555);
}

void RelayPropertyTabWidget::on_clearEnergyBtn_clicked()
{
    execSpecialOperation(2003,0xffff);
}

void RelayPropertyTabWidget::on_clearCountBtn_clicked()
{
    execSpecialOperation(2004,0xffff);
}

void RelayPropertyTabWidget::on_resetPowerStorageBtn_clicked()
{
    execSpecialOperation(2005,0xffff);
}

void RelayPropertyTabWidget::on_calibrateBtn_clicked()
{
    execSpecialOperation(2006,0xffff);
}

void RelayPropertyTabWidget::on_allOnBtn_clicked()
{
    execSpecialOperation(2002,0x01);
}

void RelayPropertyTabWidget::on_allOffBtn_clicked()
{
    execSpecialOperation(2002,0x02);
}

void RelayPropertyTabWidget::on_getDceneCountDEV_clicked()
{
    if(getModbusClient()->state()!= QModbusDevice::ConnectedState)
    {
        logWarn(tr("Device must be in connected state! connection state:%1").arg(getModbusClient()->state()));
        return ;
    }

    int startAddress = 0;
    quint16 numberOfEntries = sizeof(relay_module_status_s)/2;

    QModbusDataUnit readUnit(QModbusDataUnit::InputRegisters,startAddress,numberOfEntries);

    if (auto *reply = getModbusClient()->sendReadRequest(readUnit, m_address)) {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, [this,reply](){
                if (reply->error() == QModbusDevice::NoError) {
                    logInfo(tr("Read scence count successfully!"));

                    const QModbusDataUnit unit = reply->result();

                    relay_module_status_s conf;
                    quint16 * ptr = (quint16 *)&conf;
                    for (int i = 0,count = unit.values().size(); i < count; i++) {
                        ptr[i] = unit.values().at(i);
                    }
                    ui->lineEdit_SceneDEV->setText(QString::number(conf.scene_count));

                } else {
                    logWarn( tr("Read scence count response error: %1 ").
                                                arg(reply->errorString()));
                }
                reply->deleteLater();
            });
        else
            delete reply; // broadcast replies return immediately
    }
    else
    {
        logError(tr("Read scence count error:%1 ").arg(getModbusClient()->errorString()));
    }
}

void RelayPropertyTabWidget::on_getSceneCountDB_clicked()
{
    QByteArray blob = dbModel->data(dbModel->index(dev_index.row(),RELAY_SCENE_CONF_IN_DB_COLUMN)).toByteArray();
    if(!blob.size())
    {
        logWarn(tr("There is zero schedule in DB now!"));
        ui->lineEdit_SceneDB->setText(QString::number(0));
        return;
    }
    int count=0;
    for (int i = 0 ; i < MAX_SCENE_DEFINITION_COUNT;i+=3) {
        if(((unsigned char)blob[i]!=0) && ((unsigned char)blob[1+1]!=0))
        {
            count++;
        }
    }
    ui->lineEdit_SceneDB->setText(QString::number(count));
}

void RelayPropertyTabWidget::on_statusReadBtn_clicked()
{
    if(getModbusClient()->state()!= QModbusDevice::ConnectedState)
    {
        logWarn(tr("Device must be in connected state! connection state:%1").arg(getModbusClient()->state()));
        return ;
    }

    int startAddress = 0;
    quint16 numberOfEntries = sizeof(relay_module_status_s)/2;

    QModbusDataUnit readUnit(QModbusDataUnit::InputRegisters,startAddress,numberOfEntries);

    if (auto *reply = getModbusClient()->sendReadRequest(readUnit, m_address)) {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, [this,reply](){
                if (reply->error() == QModbusDevice::NoError) {
                    logInfo(tr("Read scence count successfully!"));

                    const QModbusDataUnit unit = reply->result();

                    relay_module_status_s conf;
                    quint16 * ptr = (quint16 *)&conf;
                    for (int i = 0,count = unit.values().size(); i < count; i++) {
                        ptr[i] = unit.values().at(i);
                    }
                    ui->lastExcutedSceneEdit->setText(QString::number(conf.current_scene));
                    ui->sceneCountEdit->setText(QString::number(conf.scene_count));
                    ui->temperatureEdit->setText(QString::number((float)(conf.temperature)/100));
                } else {
                    logWarn( tr("Read scence count response error: %1 ").
                                                arg(reply->errorString()));
                }
                reply->deleteLater();
            });
        else
            delete reply; // broadcast replies return immediately
    }
    else
    {
        logError(tr("Read scence count error:%1 ").arg(getModbusClient()->errorString()));
    }
}


void RelayPropertyTabWidget::on_SceneButton_clicked()
{
    execSpecialOperation(2002,ui->SceneEdit->text().toInt());
}
