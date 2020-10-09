#include "iic1.h"
#include "ui_iic1.h"

#include "touchpanel_conf.h"
#include "global_res.h"
#include "logmanager.h"
#include "touchpaneltabwidget.h"

#include <QModbusClient>
#include <QtDebug>
#include <QSqlError>
#include <QtEndian>
#include <QElapsedTimer>
#include <QTimer>

iic1::iic1(const QModelIndex &index,QSqlTableModel *model,int sensoraddress,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::iic1)
{
    ui->setupUi(this);
    dev_index = index;
    dbModel = model;

    sensor_address = sensoraddress;
    m_address = dbModel->data(dbModel->index(dev_index.row(),0)).toInt();

    if(sensor_address==IIC1_START_ADDRESS)
    {
    ui->typeCombo->addItem(tr("1"));
    ui->typeCombo->addItem(tr("2"));
    ui->typeCombo->addItem(tr("3"));
    }
    else if(sensor_address==DI_START_ADDRESS)
    {
    ui->typeCombo->addItem(tr("1"));
    }

}

iic1::~iic1()
{
    delete ui;
}

void iic1::Delay_MSec(unsigned int msec)
   {
       QEventLoop loop;//定义一个新的事件循环
       QTimer::singleShot(msec, &loop, SLOT(quit()));//创建单次定时器，槽函数为事件循环的退出函数
       loop.exec();//事件循环开始执行，程序会卡在这里，直到定时时间到，本循环被退出
   }

void iic1::getConf(TouchPanelConfigure *dev)
{
    dev->TouchMode1=ui->ButtonCombo_1->currentText().toInt();
    dev->TouchMode2=ui->ButtonCombo_2->currentText().toInt();
    dev->TouchMode3=ui->ButtonCombo_3->currentText().toInt();
    dev->TouchMode4=ui->ButtonCombo_4->currentText().toInt();

    if(ui->ButtonCombo_1->currentText().toInt()==1)
    {
      dev->TouchButton1Scene1=ui->Button1scene1Edit->text().toInt();
      dev->TouchButton1Scene2=ui->Button1scene2Edit->text().toInt();
      dev->TouchDataNumber1=0;
    }
    else if (ui->ButtonCombo_1->currentText().toInt()==2) {
      dev->TouchDataNumber1=ui->DataType1->text().toInt();
      dev->TouchButton1Scene1=0;
      dev->TouchButton1Scene2=0;
    }

    if(ui->ButtonCombo_2->currentText().toInt()==1)
    {
      dev->TouchButton2Scene1=ui->Button2scene1Edit->text().toInt();
      dev->TouchButton2Scene2=ui->Button2scene2Edit->text().toInt();
      dev->TouchDataNumber2=0;
    }
    else if (ui->ButtonCombo_2->currentText().toInt()==2) {
      dev->TouchDataNumber2=ui->DataType2->text().toInt();
      dev->TouchButton2Scene1=0;
      dev->TouchButton2Scene2=0;
    }

    if(ui->ButtonCombo_3->currentText().toInt()==1)
    {
      dev->TouchButton3Scene1=ui->Button3scene1Edit->text().toInt();
      dev->TouchButton3Scene2=ui->Button3scene2Edit->text().toInt();
      dev->TouchDataNumber3=0;
    }
    else if (ui->ButtonCombo_3->currentText().toInt()==2) {
      dev->TouchDataNumber3=ui->DataType3->text().toInt();
      dev->TouchButton3Scene1=0;
      dev->TouchButton3Scene2=0;
    }

    if(ui->ButtonCombo_4->currentText().toInt()==1)
    {
      dev->TouchButton4Scene1=ui->Button4scene1Edit->text().toInt();
      dev->TouchButton4Scene2=ui->Button4scene2Edit->text().toInt();
      dev->TouchDataNumber4=0;
    }
    else if (ui->ButtonCombo_4->currentText().toInt()==2) {
      dev->TouchDataNumber4=ui->DataType4->text().toInt();
      dev->TouchButton4Scene1=0;
      dev->TouchButton4Scene2=0;
    }
}


void iic1::showConf(TouchPanelConfigure &conf)
{
      ui->ButtonCombo_1->setCurrentText(QString::number(conf.TouchMode1));
      ui->ButtonCombo_2->setCurrentText(QString::number(conf.TouchMode2));
      ui->ButtonCombo_3->setCurrentText(QString::number(conf.TouchMode3));
      ui->ButtonCombo_4->setCurrentText(QString::number(conf.TouchMode4));

      ui->Button1scene1Edit->setText(QString::number(conf.TouchButton1Scene1));
      ui->Button1scene2Edit->setText(QString::number(conf.TouchButton1Scene2));
      ui->Button2scene1Edit->setText(QString::number(conf.TouchButton2Scene1));
      ui->Button2scene2Edit->setText(QString::number(conf.TouchButton2Scene2));
      ui->Button3scene1Edit->setText(QString::number(conf.TouchButton3Scene1));
      ui->Button3scene2Edit->setText(QString::number(conf.TouchButton3Scene2));
      ui->Button4scene1Edit->setText(QString::number(conf.TouchButton4Scene1));
      ui->Button4scene2Edit->setText(QString::number(conf.TouchButton4Scene2));

      ui->DataType1->setText(QString::number(conf.TouchDataNumber1));
      ui->DataType2->setText(QString::number(conf.TouchDataNumber2));
      ui->DataType3->setText(QString::number(conf.TouchDataNumber3));
      ui->DataType4->setText(QString::number(conf.TouchDataNumber4));
}

void iic1::on_ModuleDetermineButton_clicked()
{
  if(sensor_address==IIC1_START_ADDRESS)
  {
   int Type=ui->typeCombo->currentText().toInt();
   switch(Type)
   {
       case 1:
           ui->ButtonCombo_1->setEnabled(1);
           ui->ButtonCombo_2->setEnabled(0);
           ui->ButtonCombo_3->setEnabled(0);
           ui->ButtonCombo_4->setEnabled(0);
           break;
       case 2:
           ui->ButtonCombo_1->setEnabled(1);
           ui->ButtonCombo_2->setEnabled(1);
           ui->ButtonCombo_3->setEnabled(0);
           ui->ButtonCombo_4->setEnabled(0);
           break;
       case 3:
           ui->ButtonCombo_1->setEnabled(1);
           ui->ButtonCombo_2->setEnabled(1);
           ui->ButtonCombo_3->setEnabled(1);
           ui->ButtonCombo_4->setEnabled(1);
           break;

       default:
          break;
   }
  }
  else if(sensor_address==DI_START_ADDRESS)
  {
   ui->ButtonCombo_1->setEnabled(1);
   ui->ButtonCombo_2->setEnabled(1);
   ui->ButtonCombo_3->setEnabled(1);
   ui->ButtonCombo_4->setEnabled(1);
  }
}

void iic1::on_FunctionDetermineButton_clicked()
{
    if(ui->ButtonCombo_1->currentText().toInt()==1)
    {
       ui->Button1scene1Edit->setEnabled(1);
       ui->Button1scene2Edit->setEnabled(1);
    }else{
       ui->Button1scene1Edit->setEnabled(0);
       ui->Button1scene2Edit->setEnabled(0);
    }

    if(ui->ButtonCombo_2->currentText().toInt()==1)
    {
       ui->Button2scene1Edit->setEnabled(1);
       ui->Button2scene2Edit->setEnabled(1);
    }else{
       ui->Button2scene1Edit->setEnabled(0);
       ui->Button2scene2Edit->setEnabled(0);
    }

    if(ui->ButtonCombo_3->currentText().toInt()==1)
    {
       ui->Button3scene1Edit->setEnabled(1);
       ui->Button3scene2Edit->setEnabled(1);
    }else{
       ui->Button3scene1Edit->setEnabled(0);
       ui->Button3scene2Edit->setEnabled(0);
    }

    if(ui->ButtonCombo_4->currentText().toInt()==1)
    {
       ui->Button4scene1Edit->setEnabled(1);
       ui->Button4scene2Edit->setEnabled(1);
    }else{
       ui->Button4scene1Edit->setEnabled(0);
       ui->Button4scene2Edit->setEnabled(0);
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


void iic1::readmodbus(int address)
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
                    ui->typeCombo->setCurrentText(QString::number(conf));

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
void iic1::on_basicUploadBtn_clicked()
{
    if(getModbusClient()->state()!= QModbusDevice::ConnectedState)
    {
        logWarn(tr("Device must be in connected state! connection state:%1").arg(getModbusClient()->state()));
        return ;
    }

    if(sensor_address==IIC1_START_ADDRESS)
    {
     this->readmodbus(10);
    }
    else if(sensor_address==DI_START_ADDRESS)
    {
     this->readmodbus(14);
    }

    Delay_MSec(200);//延时100ms


    quint16 numberOfEntries = sizeof(TouchPanelConfigure)/2;

    QModbusDataUnit readUnit(QModbusDataUnit::HoldingRegisters,sensor_address,numberOfEntries);

    if (auto *reply = getModbusClient()->sendReadRequest(readUnit, m_address)) {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, [this,reply](){
                if (reply->error() == QModbusDevice::NoError) {
                    logInfo(tr("Read device baisc conf successfully!"));

                    const QModbusDataUnit unit = reply->result();

                    TouchPanelConfigure conf;
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


void iic1::wirtemodbus(int address)
{
    QModbusDataUnit writeUnit(QModbusDataUnit::HoldingRegisters,address,1);

    quint16 conf=ui->typeCombo->currentText().toInt();

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
void iic1::on_basicDownloadBtn_clicked()
{
    if(getModbusClient()->state()!= QModbusDevice::ConnectedState)
    {
        logWarn(tr("Device must be in connected state! connection state:%1").arg(getModbusClient()->state()));
        return ;
    }

    //prepare DataUnit
    if(sensor_address==IIC1_START_ADDRESS)
    {
     this->wirtemodbus(10);
    }
    else if(sensor_address==DI_START_ADDRESS)
    {
     this->wirtemodbus(14);
    }

    Delay_MSec(200);//延时100ms

    quint16 numberOfEntries = sizeof(TouchPanelConfigure)/2;

    QModbusDataUnit writeUnit(QModbusDataUnit::HoldingRegisters,sensor_address,numberOfEntries);

    TouchPanelConfigure conf;
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
                    logError( tr("Write iic1 basic conf response error: %1")
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

void iic1::on_basicGetFromDbBtn_clicked()
{
    int TempDBColumn=20;
    if(sensor_address==IIC1_START_ADDRESS)
    {
    TempDBColumn=IIC1_CONF_IN_DB_COLUMN;
    }
    else if(sensor_address==DI_START_ADDRESS)
    {
    TempDBColumn=DI_CONF_IN_DB_COLUMN;
    }
    QByteArray blob = dbModel->data(dbModel->index(dev_index.row(),TempDBColumn)).toByteArray();
    if(!blob.size())
    {
        logWarn(tr("There is no data in db now!"));
        return;
    }
    TouchPanelConfigure holding;
    memcpy(&holding,blob.data(),sizeof(TouchPanelConfigure));

    showConf(holding);
}

void iic1::on_basicSaveToDbBtn_clicked()
{
    int TempDBColumn=20;
    if(sensor_address==IIC1_START_ADDRESS)
    {
    TempDBColumn=IIC1_CONF_IN_DB_COLUMN;
    }
    else if(sensor_address==DI_START_ADDRESS)
    {
    TempDBColumn=DI_CONF_IN_DB_COLUMN;
    }

    QByteArray blob = dbModel->data(dbModel->index(dev_index.row(),TempDBColumn)).toByteArray();
    if(!blob.size())
    {
        blob.resize(sizeof(TouchPanelConfigure));
        blob.fill(0,blob.size());
    }

    TouchPanelConfigure *holding = reinterpret_cast <TouchPanelConfigure *>(blob.data());

    getConf(holding);

    dbModel->setData(dbModel->index(dev_index.row(),IIC1_CONF_IN_DB_COLUMN),blob);
    dbModel->submitAll();

    logInfo(tr("Save basic conf to DB :%1.").arg(dbModel->lastError().text()));
}
