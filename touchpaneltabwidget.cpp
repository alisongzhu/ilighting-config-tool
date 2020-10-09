#include "touchpaneltabwidget.h"
#include "ui_touchpaneltabwidget.h"

#include "global_res.h"
#include "logmanager.h"
#include "touchpanel_conf.h"
#include "iic1.h"
#include "adc.h"

#include <QModbusClient>
#include <QtDebug>
#include <QSqlError>

TouchPaneltabwidget::TouchPaneltabwidget(const QModelIndex &index,QSqlTableModel *model,QWidget *parent) :
    QTabWidget(parent),
    ui(new Ui::TouchPaneltabwidget)
{
    ui->setupUi(this);

    dev_index = index;
    dbModel = model;

    m_address = dbModel->data(dbModel->index(dev_index.row(),0)).toInt();
    ui->addressEdit->setText(QString::number(m_address));

    int type = dbModel->data(dbModel->index(dev_index.row(),1)).toInt();
    ui->typeCombo->addItems(dev_typeList);
    ui->typeCombo->setCurrentIndex(type);
}

TouchPaneltabwidget::~TouchPaneltabwidget()
{
    delete ui;
}

void TouchPaneltabwidget::on_SensorEnter_clicked()
{   
   if(currentWindow){
     lastWindow = currentWindow;
   }

   QString str=ui->SensorCombo->currentText();

   if(str=="IIC1"){
   currentWindow = new iic1(dev_index,dbModel,IIC1_START_ADDRESS);
   }else if(str=="IIC2"){
   currentWindow = new adc(dev_index,dbModel,IIC2_START_ADDRESS);
   }
   else if(str=="ADC"){
   currentWindow = new adc(dev_index,dbModel,ADC_START_ADDRESS);
   }
   else if(str=="SPI"){
   currentWindow = new adc(dev_index,dbModel,SPI_START_ADDRESS);
   }
   else if(str=="DI"){
   currentWindow = new iic1(dev_index,dbModel,DI_START_ADDRESS);
   }
   else if(str=="RS485"){
   currentWindow = new adc(dev_index,dbModel,RS485_START_ADDRESS);
   }

   ui->SensorgridLayout->addWidget(currentWindow);
//   ui->SensorgridLayout->replaceWidget(currentWindow,lastWindow);
   lastWindow->deleteLater();
}

void TouchPaneltabwidget::showConf(device_conf_s &conf)
{
    ui->baudCombo->setCurrentText(QString::number(conf.powerbus_baudrate));
    ui->periodSpin->setValue(conf.update_time);

    QDate date(conf.rtc.year,conf.rtc.month,conf.rtc.day);
    ui->dateTimeEdit->setDate(date);

    QTime time(conf.rtc.hour,conf.rtc.min,conf.rtc.sec);
    ui->dateTimeEdit->setTime(time);

    ui->addressEdit->setText(QString::number(conf.dev_addr));
}

void TouchPaneltabwidget::getConf(device_conf_s *dev)
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

void TouchPaneltabwidget::on_hostDatetimeBtn_clicked()
{
    QDateTime now = QDateTime::currentDateTime();
    ui->dateTimeEdit->setDateTime(now);
}

void TouchPaneltabwidget::on_basicUploadBtn_clicked()
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

void TouchPaneltabwidget::on_basicDownloadBtn_clicked()
{
    if(getModbusClient()->state()!= QModbusDevice::ConnectedState)
    {
        logWarn(tr("Device must be in connected state! connection state:%1").arg(getModbusClient()->state()));
        return ;
    }

    //prepare DataUnit
    int startAddress = DEV_BASIC_CONF_MODBUS_START_ADDRESS;
    quint16 numberOfEntries = sizeof(device_conf_s)/2;

    QModbusDataUnit writeUnit(QModbusDataUnit::HoldingRegisters,startAddress,numberOfEntries);

    device_conf_s conf;
    getConf(&conf);

    quint16 *val = (quint16* )&conf;
    for (uint i = 0 ,total = int(writeUnit.valueCount()) ;i < total;i++) {
        writeUnit.setValue(i,val[i]);
    }

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

}

void TouchPaneltabwidget::on_basicGetFromDbBtn_clicked()
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

void TouchPaneltabwidget::on_basicSaveToDbBtn_clicked()
{
    QByteArray blob = dbModel->data(dbModel->index(dev_index.row(),BASIC_CONF_IN_DB_COLUMN)).toByteArray();
    if(!blob.size())
    {
        blob.resize(sizeof(device_conf_s));
        blob.fill(0,blob.size());
    }
    device_conf_s *holding = reinterpret_cast <device_conf_s *>(blob.data());

    on_hostDatetimeBtn_clicked();

    getConf(holding);

    dbModel->setData(dbModel->index(dev_index.row(),BASIC_CONF_IN_DB_COLUMN),blob);
    dbModel->submitAll();

    logInfo(tr("Save basic conf to DB :%1.").arg(dbModel->lastError().text()));
}
