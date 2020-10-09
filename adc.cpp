#include "adc.h"
#include "ui_adc.h"

#include "touchpanel_conf.h"
#include "global_res.h"
#include "logmanager.h"
#include "touchpaneltabwidget.h"

#include <QModbusClient>
#include <QtDebug>
#include <QSqlError>
#include <QElapsedTimer>
#include <QTimer>

adc::adc(const QModelIndex &index,QSqlTableModel *model,int sensoraddress,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::adc)
{
    ui->setupUi(this);
    dev_index = index;
    dbModel = model;

    sensor_address = sensoraddress;
    m_address = dbModel->data(dbModel->index(dev_index.row(),0)).toInt();

    if(sensor_address==IIC2_START_ADDRESS)
    {
    ui->ChannelCombo->addItem(tr("1"));
    ui->ChannelCombo->addItem(tr("2"));
    }
    else if(sensor_address==ADC_START_ADDRESS)
    {
    ui->ChannelCombo->addItem(tr("1"));
    ui->ChannelCombo->addItem(tr("2"));
    }
    else if(sensor_address==SPI_START_ADDRESS)
    {
    ui->ChannelCombo->addItem(tr("1"));
    ui->ChannelCombo->addItem(tr("2"));
    }
    else if(sensor_address==RS485_START_ADDRESS)
    {
    ui->ChannelCombo->addItem(tr("1"));
    ui->ChannelCombo->addItem(tr("2"));
    }
}

adc::~adc()
{
    delete ui;
}

void adc::Delay_MSec(unsigned int msec)
   {
       QEventLoop loop;//定义一个新的事件循环
       QTimer::singleShot(msec, &loop, SLOT(quit()));//创建单次定时器，槽函数为事件循环的退出函数
       loop.exec();//事件循环开始执行，程序会卡在这里，直到定时时间到，本循环被退出
   }


void adc::getConf(TouchAdcConfigure *dev)
{
      dev->Adc1Mode=ui->ButtonCombo_1->currentText().toInt();
      dev->Adc2Mode=ui->ButtonCombo_2->currentText().toInt();
      dev->Adc3Mode=ui->ButtonCombo_3->currentText().toInt();
      dev->Adc4Mode=ui->ButtonCombo_1->currentText().toInt();

      if(ui->ButtonCombo_1->currentText().toInt()==1)
      {
      dev->Range1H=ui->Range1H->text().toInt();
      dev->Range1L=ui->Range1L->text().toInt();
      dev->Adc1Scene1=ui->Channe1scene1Edit->text().toInt();
      dev->Adc1Scene2=ui->Channe1scene2Edit->text().toInt();
      dev->Adc1DataNumber=0;
      }
      else if (ui->ButtonCombo_1->currentText().toInt()==2) {
      dev->Range1H=0;
      dev->Range1L=0;
      dev->Adc1Scene1=0;
      dev->Adc1Scene2=0;
      dev->Adc1DataNumber=ui->DataType1->text().toInt();
      }

      if(ui->ButtonCombo_2->currentText().toInt()==1)
      {
      dev->Range2H=ui->Range1H->text().toInt();
      dev->Range2L=ui->Range1L->text().toInt();
      dev->Adc2Scene1=ui->Channe2scene1Edit->text().toInt();
      dev->Adc2Scene2=ui->Channe2scene2Edit->text().toInt();
      dev->Adc2DataNumber=0;
      }
      else if (ui->ButtonCombo_2->currentText().toInt()==2) {
      dev->Range2H=0;
      dev->Range2L=0;
      dev->Adc2Scene1=0;
      dev->Adc2Scene2=0;
      dev->Adc2DataNumber=ui->DataType2->text().toInt();
      }

      if(ui->ButtonCombo_3->currentText().toInt()==1)
      {
      dev->Range3H=ui->Range1H->text().toInt();
      dev->Range3L=ui->Range1L->text().toInt();
      dev->Adc3Scene1=ui->Channe3scene1Edit->text().toInt();
      dev->Adc3Scene2=ui->Channe3scene2Edit->text().toInt();
      dev->Adc3DataNumber=0;
      }
      else if (ui->ButtonCombo_3->currentText().toInt()==2) {
      dev->Range3H=0;
      dev->Range3L=0;
      dev->Adc3Scene1=0;
      dev->Adc3Scene2=0;
      dev->Adc3DataNumber=ui->DataType3->text().toInt();
      }

      if(ui->ButtonCombo_4->currentText().toInt()==1)
      {
      dev->Range4H=ui->Range4H->text().toInt();
      dev->Range4L=ui->Range4L->text().toInt();
      dev->Adc4Scene1=ui->Channe4scene1Edit->text().toInt();
      dev->Adc4Scene2=ui->Channe4scene2Edit->text().toInt();
      dev->Adc4DataNumber=0;
      }
      else if (ui->ButtonCombo_4->currentText().toInt()==2) {
      dev->Range4H=0;
      dev->Range4L=0;
      dev->Adc4Scene1=0;
      dev->Adc4Scene2=0;
      dev->Adc4DataNumber=ui->DataType4->text().toInt();
      }

}


void adc::showConf(TouchAdcConfigure &conf)
{
      ui->ButtonCombo_1->setCurrentText(QString::number(conf.Adc1Mode));
      ui->ButtonCombo_2->setCurrentText(QString::number(conf.Adc2Mode));
      ui->ButtonCombo_3->setCurrentText(QString::number(conf.Adc3Mode));
      ui->ButtonCombo_4->setCurrentText(QString::number(conf.Adc4Mode));

      ui->Range1H->setText(QString::number(conf.Range1H));
      ui->Range1L->setText(QString::number(conf.Range1L));
      ui->Range2H->setText(QString::number(conf.Range2H));
      ui->Range2L->setText(QString::number(conf.Range2L));
      ui->Range3H->setText(QString::number(conf.Range3H));
      ui->Range3L->setText(QString::number(conf.Range3L));
      ui->Range4H->setText(QString::number(conf.Range4H));
      ui->Range4L->setText(QString::number(conf.Range4L));

      ui->Channe1scene1Edit->setText(QString::number(conf.Adc1Scene1));
      ui->Channe1scene2Edit->setText(QString::number(conf.Adc1Scene2));
      ui->Channe2scene1Edit->setText(QString::number(conf.Adc2Scene1));
      ui->Channe2scene2Edit->setText(QString::number(conf.Adc2Scene2));
      ui->Channe3scene1Edit->setText(QString::number(conf.Adc3Scene1));
      ui->Channe3scene2Edit->setText(QString::number(conf.Adc3Scene2));
      ui->Channe4scene1Edit->setText(QString::number(conf.Adc4Scene1));
      ui->Channe4scene2Edit->setText(QString::number(conf.Adc4Scene2));

      ui->DataType1->setText(QString::number(conf.Adc1DataNumber));
      ui->DataType2->setText(QString::number(conf.Adc2DataNumber));
      ui->DataType3->setText(QString::number(conf.Adc3DataNumber));
      ui->DataType4->setText(QString::number(conf.Adc4DataNumber));
}

void adc::on_FunctionDetermineButton_clicked()
{
    if(ui->ButtonCombo_1->currentText().toInt()==1)
    {
       ui->Range1H->setEnabled(1);
       ui->Range1L->setEnabled(1);
       ui->Channe1scene1Edit->setEnabled(1);
       ui->Channe1scene2Edit->setEnabled(1);
    }else{
       ui->Range1H->setEnabled(0);
       ui->Range1L->setEnabled(0);
       ui->Channe1scene1Edit->setEnabled(0);
       ui->Channe1scene2Edit->setEnabled(0);
    }

    if(ui->ButtonCombo_2->currentText().toInt()==1)
    {
       ui->Range2H->setEnabled(1);
       ui->Range2L->setEnabled(1);
       ui->Channe2scene1Edit->setEnabled(1);
       ui->Channe2scene2Edit->setEnabled(1);
    }else{
       ui->Range2H->setEnabled(0);
       ui->Range2L->setEnabled(0);
       ui->Channe2scene1Edit->setEnabled(0);
       ui->Channe2scene2Edit->setEnabled(0);
    }

    if(ui->ButtonCombo_3->currentText().toInt()==1)
    {
       ui->Range3H->setEnabled(1);
       ui->Range3L->setEnabled(1);
       ui->Channe3scene1Edit->setEnabled(1);
       ui->Channe3scene2Edit->setEnabled(1);
    }else{
       ui->Range3H->setEnabled(0);
       ui->Range3L->setEnabled(0);
       ui->Channe3scene1Edit->setEnabled(0);
       ui->Channe3scene2Edit->setEnabled(0);
    }

    if(ui->ButtonCombo_4->currentText().toInt()==1)
    {
       ui->Range4H->setEnabled(1);
       ui->Range4L->setEnabled(1);
       ui->Channe4scene1Edit->setEnabled(1);
       ui->Channe4scene2Edit->setEnabled(1);
    }else{
       ui->Range4H->setEnabled(0);
       ui->Range4L->setEnabled(0);
       ui->Channe4scene1Edit->setEnabled(0);
       ui->Channe4scene2Edit->setEnabled(0);
    }

    /*****判断****/
    if(ui->ButtonCombo_1->currentText().toInt()==2)
    {
       ui->DataType1->setEnabled(1);
    }else{
       ui->DataType1->setEnabled(0);
    }

    if(ui->ButtonCombo_2->currentText().toInt()==2)
    {
       ui->DataType2->setEnabled(1);
    }else{
       ui->DataType2->setEnabled(0);
    }

    if(ui->ButtonCombo_3->currentText().toInt()==2)
    {
       ui->DataType3->setEnabled(1);
    }else{
       ui->DataType3->setEnabled(0);
    }

    if(ui->ButtonCombo_4->currentText().toInt()==2)
    {
       ui->DataType4->setEnabled(1);
    }else{
       ui->DataType4->setEnabled(0);
    }
}


void adc::readmodbus(int address)
{
    if(getModbusClient()->state()!= QModbusDevice::ConnectedState)
    {
        logWarn(tr("Device must be in connected state! connection state:%1").arg(getModbusClient()->state()));
        return ;
    }

    QModbusDataUnit readUnit(QModbusDataUnit::HoldingRegisters,address,1);

    if (auto *reply = getModbusClient()->sendReadRequest(readUnit, m_address)) {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, [this,reply](){
                if (reply->error() == QModbusDevice::NoError) {
                    logInfo(tr("Read device baisc conf successfully!"));

                    const QModbusDataUnit unit = reply->result();

                    quint16 conf;
                    quint16 * ptr = (quint16 *)&conf;
                    for (int i = 0,count = unit.values().size(); i < count; i++) {
                        ptr[i] = unit.values().at(i);
                    }
                    ui->ChannelCombo->setCurrentText(QString::number(conf));

                } else {
                    logWarn( tr("Read device basic conf response error: %1 ").
                                                arg(reply->errorString()));
                }
                reply->deleteLater();
            });
        else
            delete reply; // broadcast replies return immediately
    }
    else
    {
        logError(tr("Read device basic conf error:%1 ").arg(getModbusClient()->errorString()));
    }
}

void adc::on_basicUploadBtn_clicked()
{
    if(getModbusClient()->state()!= QModbusDevice::ConnectedState)
    {
        logWarn(tr("Device must be in connected state! connection state:%1").arg(getModbusClient()->state()));
        return ;
    }


    if(sensor_address==IIC2_START_ADDRESS)
    {
     this->readmodbus(11);
    }
    else if(sensor_address==ADC_START_ADDRESS)
    {
     this->readmodbus(12);
    }
    else if(sensor_address==SPI_START_ADDRESS)
    {
     this->readmodbus(13);
    }
    else if(sensor_address==RS485_START_ADDRESS)
    {
     this->readmodbus(15);
    }


    Delay_MSec(200);//延时100ms

    quint16 numberOfEntries = sizeof(TouchAdcConfigure)/2;
    QModbusDataUnit readUnit(QModbusDataUnit::HoldingRegisters,sensor_address,numberOfEntries);

    if (auto *reply = getModbusClient()->sendReadRequest(readUnit, m_address)) {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, [this,reply](){
                if (reply->error() == QModbusDevice::NoError) {
                    logInfo(tr("Read device baisc conf successfully!"));

                    const QModbusDataUnit unit = reply->result();

                    TouchAdcConfigure conf;
                    quint16 * ptr = (quint16 *)&conf;
                    for (int i = 0,count = unit.values().size(); i < count; i++) {
                        ptr[i] = unit.values().at(i);
                    }
                    this->showConf(conf);

                } else {
                    logWarn( tr("Read device basic conf response error: %1 ").
                                                arg(reply->errorString()));
                }
                reply->deleteLater();
            });
        else
        {
            delete reply; // broadcast replies return immediately
        }
    }
    else
    {
        logError(tr("Read device basic conf error:%1 ").arg(getModbusClient()->errorString()));
    }
}

void adc::wirtemodbus(int address)
{
    QModbusDataUnit writeUnit(QModbusDataUnit::HoldingRegisters,address,1);

    quint16 conf=ui->ChannelCombo->currentText().toInt();

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
                    logInfo(tr(" Write device basic conf Successfully."));

                }else {
                    logError( tr("Write device basic conf response error: %1")
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
        logError(tr("Read device basic conf error:%1 ").arg(getModbusClient()->errorString()));
    }

}

void adc::on_basicDownloadBtn_clicked()
{
    if(getModbusClient()->state()!= QModbusDevice::ConnectedState)
    {
        logWarn(tr("Device must be in connected state! connection state:%1").arg(getModbusClient()->state()));
        return ;
    }

    //prepare DataUnit

    quint16 numberOfEntries = sizeof(TouchAdcConfigure)/2;

    QModbusDataUnit writeUnit(QModbusDataUnit::HoldingRegisters,sensor_address,numberOfEntries);

    if(sensor_address==IIC2_START_ADDRESS)
    {
     this->wirtemodbus(11);
    }
    else if(sensor_address==ADC_START_ADDRESS)
    {
     this->wirtemodbus(12);
    }
    else if(sensor_address==SPI_START_ADDRESS)
    {
     this->wirtemodbus(13);
    }
    else if(sensor_address==RS485_START_ADDRESS)
    {
     this->wirtemodbus(15);
    }

    Delay_MSec(200);//延时100ms

    TouchAdcConfigure conf;
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
                    logInfo(tr(" Write device basic conf Successfully."));

                }else {
                    logError( tr("Write device basic conf response error: %1")
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
        logError(tr("Read device basic conf error:%1 ").arg(getModbusClient()->errorString()));
    }
}

void adc::on_basicSaveToDbBtn_clicked()
{
    int TempDBColumn=20;
    if(sensor_address==IIC2_START_ADDRESS)
    {
     TempDBColumn=IIC2_CONF_IN_DB_COLUMN;
    }
    else if(sensor_address==ADC_START_ADDRESS)
    {
     TempDBColumn=ADC_CONF_IN_DB_COLUMN;
    }
    else if(sensor_address==SPI_START_ADDRESS)
    {
     TempDBColumn=SPI_CONF_IN_DB_COLUMN;
    }
    else if(sensor_address==RS485_START_ADDRESS)
    {
     TempDBColumn=RS485_CONF_IN_DB_COLUMN;
    }

    QByteArray blob = dbModel->data(dbModel->index(dev_index.row(),TempDBColumn)).toByteArray();
    if(!blob.size())
    {
        blob.resize(sizeof(TouchAdcConfigure));
        blob.fill(0,blob.size());
    }

    TouchAdcConfigure *holding = reinterpret_cast <TouchAdcConfigure *>(blob.data());

    getConf(holding);

    dbModel->setData(dbModel->index(dev_index.row(),TempDBColumn),blob);
    dbModel->submitAll();

    logInfo(tr("Save basic conf to DB :%1.").arg(dbModel->lastError().text()));
}

void adc::on_basicGetFromDbBtn_clicked()
{
    int TempDBColumn=20;
    if(sensor_address==IIC2_START_ADDRESS)
    {
     TempDBColumn=IIC2_CONF_IN_DB_COLUMN;
    }
    else if(sensor_address==ADC_START_ADDRESS)
    {
     TempDBColumn=ADC_CONF_IN_DB_COLUMN;
    }
    else if(sensor_address==SPI_START_ADDRESS)
    {
     TempDBColumn=SPI_CONF_IN_DB_COLUMN;
    }
    else if(sensor_address==RS485_START_ADDRESS)
    {
     TempDBColumn=RS485_CONF_IN_DB_COLUMN;
    }


    QByteArray blob = dbModel->data(dbModel->index(dev_index.row(),TempDBColumn)).toByteArray();
    if(!blob.size())
    {
        logWarn(tr("There is no data in db now!"));
        return;
    }
    TouchAdcConfigure holding;
    memcpy(&holding,blob.data(),sizeof(TouchAdcConfigure));

    showConf(holding);
}
