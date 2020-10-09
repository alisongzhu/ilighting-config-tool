#include "gatewaybasictabwidget.h"
#include "ui_gatewaybasictabwidget.h"

#include "global_res.h"
#include "logmanager.h"

#include <QHostAddress>
#include <QModbusClient>
#include <QSqlError>

#include <QtEndian>

//static QStringList weekdaylist;
//static QStringList monthlist = {"Jan","Fed","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};

GatewayBasicTabWidget::GatewayBasicTabWidget(const QModelIndex &index,QSqlTableModel *model,QWidget *parent) :
    QTabWidget(parent),
    ui(new Ui::GatewayBasicTabWidget)
{
    ui->setupUi(this);

    dev_index = index;
    dbModel = model;
    m_address = dbModel->data(dbModel->index(dev_index.row(),0)).toInt();
    ui->addressEdit->setText(QString::number(m_address));

    on_hostDatetimeBtn_clicked();

//    create item in schedule scene table
    ui->sceneTable->setRowCount(SCENE_COUNT_PER_SCHEDULE);
    ui->sceneTable->setColumnCount(1);
    QStringList header;
    header << tr("Scene Number");
    ui->sceneTable->setHorizontalHeaderLabels(header);

    QTableWidgetItem *item;
    for (int i = 0 ; i < SCENE_COUNT_PER_SCHEDULE; i++) {
        item = new QTableWidgetItem;
        ui->sceneTable->setItem(i,0,item);
    }


#if 0
    for(int i = 1; i < SUBMODULE_MAX_COUNT; i++)
    {
        QListWidgetItem *item = new QListWidgetItem(QString("module""%1").arg(i,3),ui->submoduleList);
        item->setCheckState(Qt::Unchecked);
    }
#endif

    QStringList weekdaylist;
    weekdaylist << tr("Monday ")
                << tr("Tuesday")
                << tr("Wednesday")
                << tr("Thursday")
                << tr("Friday")
                << tr("Saturday")
                << tr("Sunday");

    for(int i = 0 ; i< weekdaylist.size(); ++i)
    {
        QListWidgetItem *item = new QListWidgetItem(weekdaylist.at(i),ui->weekdayList);
        item->setCheckState(Qt::Checked);
    }

    QStringList monthlist;
    monthlist << tr("Jan") << tr("Feb") << tr("Mar") << tr("Apr");
    monthlist << tr("May") << tr("Jun") << tr("Jul") << tr("Aug");
    monthlist << tr("Sep") << tr("Oct") << tr("Nov") << tr("Dec");

    QStringListIterator iter(monthlist);
    while (iter.hasNext()) {
        QListWidgetItem *item = new QListWidgetItem(iter.next(),ui->monthList);
        item->setCheckState(Qt::Checked);
    }

//    QStringList::const_iterator constIterator;
    for (int i = 1;i <= 31; i++) {
        QListWidgetItem *item = new QListWidgetItem(tr("day""%1").arg(i,2),ui->dayList);
        item->setCheckState(Qt::Checked);
    }

//    network_param = {0};
//    setModuleNetwork(network_param);

    setCurrentIndex(0);
}

GatewayBasicTabWidget::~GatewayBasicTabWidget()
{
    delete ui;
}


#if 0
module_network GatewayBasicTabWidget::getModuleNetwork()
{
    QHostAddress ip(ui->ipEdit->text());

    bool ok = false;
    network_param.ip = ip.toIPv4Address(&ok);
//    if(!ok)

    ip.setAddress(ui->gatewayEdit->text());
    network_param.gw = ip.toIPv4Address();

    ip.setAddress(ui->maskEdit->text());
    network_param.mask = ip.toIPv4Address();

    return network_param;
}

module_mb_comm GatewayBasicTabWidget::getModuleMbComm()
{
    return comm_param;
}

schedule_item GatewayBasicTabWidget::getCurrentScheduleItem()
{
    item.enable = ui->scheduleEnableCheck->checkState();

    item.hour = ui->timeEdit->time().hour();
    item.minute = ui->timeEdit->time().minute();
    item.day_mask = getListMask(ui->dayList);
    item.month_mask = getListMask(ui->monthList);
    item.weekday_mask = getListMask(ui->monthList);

    return item;
}

void GatewayBasicTabWidget::setModuleNetwork(module_network &param)
{
    QHostAddress ip;

    ip.setAddress(param.ip);
    ui->ipEdit->setText(ip.toString());

    ip.setAddress(param.gw);
    ui->gatewayEdit->setText(ip.toString());

    ip.setAddress(param.mask);
    ui->maskEdit->setText(ip.toString());
}

#endif

void GatewayBasicTabWidget::showConf(device_conf_s &dev)
{
//    ui->baudCombo->setCurrentIndex(dev->powerbus_baudrate);
    ui->baudCombo->setCurrentText(QString::number(dev.powerbus_baudrate));
    ui->periodSpin->setValue(dev.update_time);

    QDate date(dev.rtc.year,dev.rtc.month,dev.rtc.day);
    ui->dateTimeEdit->setDate(date);

    QTime time(dev.rtc.hour,dev.rtc.min,dev.rtc.sec);
    ui->dateTimeEdit->setTime(time);
}

void GatewayBasicTabWidget::getConf(device_conf_s *dev)
{
//    dev->powerbus_baudrate = ui->baudCombo->currentIndex();
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

void GatewayBasicTabWidget::showConf(module_network_s &ethernet)
{
    QHostAddress ip;

    ip.setAddress(qToBigEndian(ethernet.ip));
    ui->ipEdit->setText(ip.toString());

    ip.setAddress(qToBigEndian(ethernet.gw));
    ui->gatewayEdit->setText(ip.toString());

    ip.setAddress(qToBigEndian(ethernet.mask));
    ui->maskEdit->setText(ip.toString());

    ui->portEdit->setText(QString::number(ethernet.port));
}

void GatewayBasicTabWidget::getConf(module_network_s *ethernet)
{
    QHostAddress ip(ui->ipEdit->text());

    bool ok = false;
    ethernet->ip = qToBigEndian(ip.toIPv4Address(&ok));
//    ethernet->ip = ip.toIPv4Address(&ok);
//    if(!ok)

    ip.setAddress(ui->gatewayEdit->text());
    ethernet->gw = qToBigEndian(ip.toIPv4Address());

    ip.setAddress(ui->maskEdit->text());
    ethernet->mask = qToBigEndian(ip.toIPv4Address());

    ethernet->port = ui->portEdit->text().toInt();
}

void GatewayBasicTabWidget::showConf(module_mb_comm_s &comm)
{
    ui->rs485BaudCombo->setCurrentText(QString::number(comm.baudrate));
    ui->rs485ParityCombo->setCurrentIndex(comm.parity);
    ui->rs485DatabitsCombo->setCurrentText(QString::number(comm.databits));
    ui->rs485StopbitsCombo->setCurrentText(QString::number(comm.stopbits));
}

void GatewayBasicTabWidget::getConf(module_mb_comm_s *comm)
{
    comm->baudrate = ui->rs485BaudCombo->currentText().toUInt();
    comm->parity = ui->rs485ParityCombo->currentIndex();
    comm->databits = ui->rs485DatabitsCombo->currentText().toUInt();
    comm->stopbits = ui->rs485StopbitsCombo->currentText().toUInt();
}

void GatewayBasicTabWidget::showConf(module_holding_s &holding)
{
    showConf(holding.dev);
    showConf(holding.net);
    showConf(holding.comm);
}

void GatewayBasicTabWidget::getConf(module_holding_s *holding)
{
    getConf(&holding->dev);
    getConf(&holding->net);
    getConf(&holding->comm);
}


#if 0
uint8_t GatewayBasicTabWidget::getWeekdayMask()
{
    uint8_t rval = 0;
    int rowCount = ui->weekdayList->count();

    for(int i = 0; i< rowCount;i++)
    {
       if(Qt::Checked == ui->weekdayList->takeItem(i)->checkState())
       {
           rval = (1<< i)|rval;
       }

    }
    return rval;
}


uint32_t GatewayBasicTabWidget::getListMask(QListWidget *list)
{
    uint32_t rval = 0;
    int rowCount = list->count();

    for(int i = 0; i< rowCount;i++)
    {
       if(Qt::Checked == list->item(i)->checkState())
       {
           rval = (1<< i)|rval;
       }
    }
    return rval;
}

void GatewayBasicTabWidget::setListMask(QListWidget *list,uint32_t mask)
{
    int rowCount = list->count();
    for(int i = 0; i<rowCount;i++)
    {
        if((1<<i)&mask)
        {
            list->item(i)->setCheckState(Qt::Checked);
        }
        else
            list->item(i)->setCheckState(Qt::Unchecked);
    }
}
#endif

void GatewayBasicTabWidget::showConf(schedule_item_s &schedule)
{
    ui->scheduleEnableCheck->setCheckState(schedule.enable?Qt::Checked:Qt::Unchecked);

    QTime time(schedule.hour,schedule.minute);
    ui->timeEdit->setTime(time);

//    QTableWidgetItem *item = new QTableWidgetItem;
    for (int i=0;i < ui->sceneTable->rowCount();i++) {
//        item = ui->sceneTable->item(i,0);
        ui->sceneTable->item(i,0)->setData(Qt::DisplayRole,schedule.scene_list[i]);
//        item->setData(Qt::DisplayRole,schedule->scene_list[i]);
//        ui->sceneTable->setItem(i,0,item);
    }

    setListMask(ui->weekdayList,schedule.weekday_mask);
    setListMask(ui->monthList,schedule.month_mask);
    setListMask(ui->dayList,schedule.day_mask);
}

void GatewayBasicTabWidget::getConf(schedule_item_s *schedule)
{
    schedule->enable = ui->scheduleEnableCheck->checkState() == Qt::Checked ?1:0;
    schedule->hour = ui->timeEdit->time().hour();
    schedule->minute = ui->timeEdit->time().minute();

    QTableWidgetItem *item;
//    int val = 0;
    for (int i =0 ;i < SCENE_COUNT_PER_SCHEDULE;i++) {
        item = ui->sceneTable->item(i,0);
#if 0
        if(item)
        {
            val = item->data(Qt::DisplayRole).toInt();
        }
        else{
            val = 0;
        }
        qDebug() << tr("scene %1 : %2").arg(i).arg(val);
#endif
        schedule->scene_list[i] = item?item->data(Qt::DisplayRole).toInt():0;
    }

    schedule->weekday_mask = getListMask(ui->weekdayList);
    schedule->month_mask = getListMask(ui->monthList);
    schedule->day_mask = getListMask(ui->dayList);
}

void GatewayBasicTabWidget::getSubmoduleMask(uint8_t *buf)
{
    int address;
    for (int i = 0;i<32;i++)
    {
        buf[i] = 0;
    }

    for (int i = 0,row_count = dbModel->rowCount();i<row_count;i++)
    {
        address =  dbModel->data(dbModel->index(i,ADDRESS_CONF_IN_DB_COLUMN)).toInt();
        if( m_address != address)
        {
            setMaskBits(buf,address-1,1,1);
        }
    }
}

#if 0
int GatewayBasicTabWidget::getAddress()
{
    return m_address;
}

void GatewayBasicTabWidget::setAddress(int address)
{
    m_address = address;
}


void GatewayBasicTabWidget::setModbusClient(QModbusClient *client)
{
    modbusDevice = client;
}

#endif

void GatewayBasicTabWidget::on_hostDatetimeBtn_clicked()
{
    QDateTime now = QDateTime::currentDateTime();
    ui->dateTimeEdit->setDateTime(now);
}

void GatewayBasicTabWidget::on_basicSaveToDbBtn_clicked()
{
#if 1
    QByteArray blob = dbModel->data(dbModel->index(dev_index.row(),BASIC_CONF_IN_DB_COLUMN)).toByteArray();
//    qDebug("blob size = %d",blob.size());
    if(!blob.size())
    {
        blob.resize(sizeof(module_holding_s));
        blob.fill(0,blob.size());
    }
//    qDebug("blob size = %d",blob.size());
//    qDebug("struct holding size = %d",sizeof(module_holding_s));

    module_holding_s *holding = reinterpret_cast <module_holding_s *>(blob.data());

    on_hostDatetimeBtn_clicked();

    getConf(holding);

    dbModel->setData(dbModel->index(dev_index.row(),BASIC_CONF_IN_DB_COLUMN),blob);
    dbModel->submitAll();

    logInfo(tr("Save basic conf to DB :%1.").arg(dbModel->lastError().text()));
#endif
//    module_holding_s *conf = nullptr;
//    saveDataToDb(conf,BASIC_CONF_IN_DB_COLUMN);
}

void GatewayBasicTabWidget::on_basicGetFromDbBtn_clicked()
{
    QByteArray blob = dbModel->data(dbModel->index(dev_index.row(),BASIC_CONF_IN_DB_COLUMN)).toByteArray();
    if(!blob.size())
    {
//        qDebug("There is no data in db now!");
        logWarn(tr("There is no data in db now!"));
        return;
    }
    module_holding_s holding;
    memcpy(&holding,blob.data(),sizeof(module_holding_s));

    showConf(holding);
}



void GatewayBasicTabWidget::on_basicDownloadBtn_clicked()
{
//    if(QModbusDevice::ConnectedState != modbusDevice->state())
    if(getModbusClient()->state()!= QModbusDevice::ConnectedState)
    {
//        qDebug() << tr("Device must be in connected state! connection state:%1").arg(getModbusClient()->state());
        logWarn(tr("Device must be in connected state! connection state:%1").arg(getModbusClient()->state()));
        return ;
    }

    module_holding_s holding;
    getConf(&holding);
    getSubmoduleMask(holding.submodules);
    holding.dev.dev_addr = m_address;

    //prepare DataUnit
    int startAddress = DEV_BASIC_CONF_MODBUS_START_ADDRESS;
    quint16 numberOfEntries = sizeof(module_holding_s)/2;
    QModbusDataUnit writeUnit(QModbusDataUnit::HoldingRegisters,startAddress,numberOfEntries);

//    quint16 *val = (quint16* )&dev;
    quint16 *val = (quint16* )&holding;
    for (uint i = 0 ,total = int(writeUnit.valueCount()) ;i < total;i++) {
        writeUnit.setValue(i,val[i]);
    }

    if(auto *reply = getModbusClient()->sendWriteRequest(writeUnit,m_address))
    {
        if (!reply->isFinished())
        {
            connect(reply, &QModbusReply::finished, this, [reply]() {
                if (reply->error() == QModbusDevice::NoError) {
                    logInfo(tr(" Download basic conf Successfully."));
                }
                else
                    logWarn(tr("Download device basic conf response error: %1 (code: 0x%2)").
                             arg(reply->errorString()).arg(reply->error(), -1, 16));
            });
        }
        else{
            logError(tr("Broadcast basic conf msg."));
            reply->deleteLater();
        }

    }

}

void GatewayBasicTabWidget::on_basicUploadBtn_clicked()
{
//    logInfo(tr("Modbus client state : %1").arg(getModbusClient()->state()));
//    logInfo(tr("Modbus client error : %1").arg(getModbusClient()->error()));
    if(getModbusClient()->state()!= QModbusDevice::ConnectedState)
    {
        logWarn(tr("Device must be in connected state! connection state:%1").arg(getModbusClient()->state()));
        return ;
    }

    //prepare DataUnit
    int startAddress = DEV_BASIC_CONF_MODBUS_START_ADDRESS;
    quint16 numberOfEntries = sizeof(module_holding_s)/2;
    QModbusDataUnit readUnit(QModbusDataUnit::HoldingRegisters,startAddress,numberOfEntries);


    if(auto *reply = getModbusClient()->sendReadRequest(readUnit,m_address)){
        if (!reply->isFinished()){
            connect(reply, &QModbusReply::finished, this, [this,reply](){
                if (reply->error() == QModbusDevice::NoError) {
                    logInfo(tr("Read baisc conf successfully!"));
                    const QModbusDataUnit unit = reply->result();

                    module_holding_s conf ;
                    quint16 * ptr = (quint16 *)&conf;
                    for (int i = 0,count = unit.values().size(); i < count; i++) {
                        ptr[i] = unit.values().at(i);
                    }
                    this->showConf(conf);

                } else if (reply->error() == QModbusDevice::ProtocolError) {
                    logWarn( tr("Read device basic conf response error: %1 (Mobus exception: 0x%2)").
                                                arg(reply->errorString()).
                                                arg(reply->rawResult().exceptionCode(), -1, 16));
                } else {
                    logWarn(tr("Read device basic conf response error: %1 (code: 0x%2)").
                                                arg(reply->errorString()).
                                                arg(reply->error(), -1, 16));
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


void GatewayBasicTabWidget::on_advUploadBtn_clicked()
{
    if(getModbusClient()->state()!= QModbusDevice::ConnectedState)
    {
        logWarn(tr("Device must be in connected state! connection state:%1").arg(getModbusClient()->state()));
        return ;
    }

    //prepare DataUnit
    int startAddress = GATEWAY_ADVANCED_CONF_MODBUS_START_ADDRESS + ui->scheduleOrderSpin->value()*sizeof(schedule_item_s)/2;
    quint16 numberOfEntries = sizeof(schedule_item_s)/2;
    QModbusDataUnit readUnit(QModbusDataUnit::HoldingRegisters,startAddress,numberOfEntries);

    if(auto *reply = getModbusClient()->sendReadRequest(readUnit,m_address)){
        if (!reply->isFinished()){
            connect(reply, &QModbusReply::finished, this, [this,reply](){
                if (reply->error() == QModbusDevice::NoError) {
                    logInfo(tr("Read single schedule successfully!"));
                    const QModbusDataUnit unit = reply->result();

                    schedule_item_s conf ;
                    quint16 * ptr = (quint16 *)&conf;
                    for (int i = 0,count = unit.values().size(); i < count; i++) {
                        ptr[i] = unit.values().at(i);
                    }
                    this->showConf(conf);
                } else if (reply->error() == QModbusDevice::ProtocolError) {
                    logWarn( tr("Read device single schedule response error: %1 (Mobus exception: 0x%2)").
                                                arg(reply->errorString()).
                                                arg(reply->rawResult().exceptionCode(), -1, 16));
                } else {
                    logWarn( tr("Read device single schedule response error: %1 (code: 0x%2)").
                                                arg(reply->errorString()).
                                                arg(reply->error(), -1, 16));
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

void GatewayBasicTabWidget::on_advDownloadBtn_clicked()
{
    if(getModbusClient()->state()!= QModbusDevice::ConnectedState)
    {
        logWarn(tr("Device must be in connected state! connection state:%1").arg(getModbusClient()->state()));
        return ;
    }

    schedule_item_s item;
    getConf(&item);

    //prepare DataUnit
    int startAddress = GATEWAY_ADVANCED_CONF_MODBUS_START_ADDRESS + ui->scheduleOrderSpin->value()*sizeof(schedule_item_s)/2;
    quint16 numberOfEntries = sizeof(schedule_item_s)/2;
    QModbusDataUnit writeUnit(QModbusDataUnit::HoldingRegisters,startAddress,numberOfEntries);

    quint16 *val = (quint16* )&item;
    for (uint i = 0;i < writeUnit.valueCount();i++) {
        writeUnit.setValue(i,*val++);
    }

    if(auto *reply = getModbusClient()->sendWriteRequest(writeUnit,m_address))
    {
        if (!reply->isFinished())
        {
            connect(reply, &QModbusReply::finished, this, [reply]() {
                if(reply->error() == QModbusDevice::NoError){
                    logInfo(tr("Read single schedule successfully!"));
                }else if (reply->error() == QModbusDevice::ProtocolError) {
                    logWarn(tr("Write device basic conf response error: %1 (Mobus exception: 0x%2)")
                               .arg(reply->errorString()).arg(reply->rawResult().exceptionCode(), -1, 16));
                }else if (reply->error() != QModbusDevice::NoError) {
                    logWarn(tr("Write device basic conf response error: %1 (code: 0x%2)").
                        arg(reply->errorString()).arg(reply->error(), -1, 16));
                }
                reply->deleteLater();
            });
        }
        else{
            // broadcast replies return immediately
            reply->deleteLater();
        }

    }
}

void GatewayBasicTabWidget::on_advGetFromDbBtn_clicked()
{
    QByteArray blob = dbModel->data(dbModel->index(dev_index.row(),ADVANCED_CONF_IN_DB_COLUMN)).toByteArray();
    if(!blob.size())
    {
        logWarn(tr("There is no schedule in DB now!"));
        return;
    }
    schedule_item_s *conf = reinterpret_cast <schedule_item_s *>(blob.data());

    showConf(conf[ui->scheduleOrderSpin->value()]);
}

void GatewayBasicTabWidget::on_advSaveToDbBtn_clicked()
{
    QByteArray blob = dbModel->data(dbModel->index(dev_index.row(),ADVANCED_CONF_IN_DB_COLUMN)).toByteArray();
//    qDebug("blob size = %d",blob.size());
    if(!blob.size())
    {
        blob.resize(sizeof(schedule_item_s)*SCHEDULE_LIST_COUNT);
        blob.fill(0,blob.size());
    }
//    qDebug("blob size = %d",blob.size());
//    qDebug("struct module_scheduleList_s size = %d",sizeof(module_scheduleList_s));

    schedule_item_s *conf = reinterpret_cast <schedule_item_s *>(blob.data());

    getConf(&conf[ui->scheduleOrderSpin->value()]);

    dbModel->setData(dbModel->index(dev_index.row(),ADVANCED_CONF_IN_DB_COLUMN),blob);
    dbModel->submitAll();

}

void GatewayBasicTabWidget::on_getScheduleCountInDbBtn_clicked()
{
    QByteArray blob = dbModel->data(dbModel->index(dev_index.row(),ADVANCED_CONF_IN_DB_COLUMN)).toByteArray();
    if(!blob.size())
    {
        logWarn(tr("There is zero schedule in DB now!"));
        ui->scheduleCountInDbEdit->setText(QString::number(0));
        return;
    }

    schedule_item_s *scheduleList = reinterpret_cast <schedule_item_s *>(blob.data());
    int count = 0;
    for (int i = 0 ; i < SCHEDULE_LIST_COUNT;i++) {
        if(scheduleList[i].enable)
        {
            count++;
        }
    }

    ui->scheduleCountInDbEdit->setText(QString::number(count));
}

void GatewayBasicTabWidget::execSpecialOperation(quint16 address, quint16 value)
{
    if(getModbusClient()->state()!= QModbusDevice::ConnectedState)
    {
        logWarn(tr("Device must be in connected state! connection state:%1").arg(getModbusClient()->state()));;
    }

    //prepare DataUnit
    QModbusDataUnit writeUnit(QModbusDataUnit::HoldingRegisters,address,1);
    writeUnit.setValue(0,value);

    if(auto *reply = getModbusClient()->sendWriteRequest(writeUnit,m_address))
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

void GatewayBasicTabWidget::on_ResetButton_clicked()
{
    execSpecialOperation(2000,0x5555);
}
