#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "global_res.h"

#include "logmanager.h"

#include <QModbusClient>
#include <QModbusTcpClient>
#include <QModbusRtuSerialMaster>
#include <QUrl>

enum ModbusConnection {
    Serial,
    Tcp
};

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    ui->parityCombo->setCurrentIndex(0);
    ui->baudCombo->setCurrentText(QString::number(m_settings.baud));
    ui->dataBitsCombo->setCurrentText(QString::number(m_settings.dataBits));
    ui->stopBitsCombo->setCurrentText(QString::number(m_settings.stopBits));

    ui->timeoutSpinner->setValue(m_settings.responseTime);
    ui->retriesSpinner->setValue(m_settings.numberOfRetries);

    ui->connectionTypeCombo->setCurrentIndex(0);
    ui->portEdit->setText(QLatin1String("COM1"));
    onConnectionTypeChanged(0);

    //    connect(ui->treeView,SIGNAL(doubleClicked(QModelIndex)),model,SLOT(ReadItem()));

    connect(ui->connectionTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SettingsDialog::onConnectionTypeChanged);
    connect(ui->connectButton,&QPushButton::clicked,this,&SettingsDialog::onConnectButtonClicked);
    connect(ui->closeButton,&QPushButton::clicked,this,&SettingsDialog::hide);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

#if 0
SettingsDialog::Settings SettingsDialog::settings() const
{
    return m_settings;
}


QModbusClient* SettingsDialog::getModbusClient()
{
    return modbusDevice;
}
#endif

void SettingsDialog::onConnectButtonClicked()
{
    if (!getModbusClient())
        return;

    m_settings.parity = ui->parityCombo->currentIndex();
    if (m_settings.parity > 0)
        m_settings.parity++;

    m_settings.baud = ui->baudCombo->currentText().toInt();
    m_settings.dataBits = ui->dataBitsCombo->currentText().toInt();
    m_settings.stopBits = ui->stopBitsCombo->currentText().toInt();

    m_settings.responseTime = ui->timeoutSpinner->value();
    m_settings.numberOfRetries = ui->retriesSpinner->value();

//        statusBar()->clearMessage();
    if (getModbusClient()->state() != QModbusDevice::ConnectedState) {
        if (static_cast<ModbusConnection>(ui->connectionTypeCombo->currentIndex()) == Serial) {
            getModbusClient()->setConnectionParameter(QModbusDevice::SerialPortNameParameter,
                ui->portEdit->text());
#if QT_CONFIG(modbus_serialport)
            getModbusClient()->setConnectionParameter(QModbusDevice::SerialParityParameter,
                m_settings.parity);
            getModbusClient()->setConnectionParameter(QModbusDevice::SerialBaudRateParameter,
                m_settings.baud);
            getModbusClient()->setConnectionParameter(QModbusDevice::SerialDataBitsParameter,
                m_settings.dataBits);
            getModbusClient()->setConnectionParameter(QModbusDevice::SerialStopBitsParameter,
                m_settings.stopBits);
#endif
        } else {
            const QUrl url = QUrl::fromUserInput(ui->portEdit->text());
            getModbusClient()->setConnectionParameter(QModbusDevice::NetworkPortParameter, url.port());
            getModbusClient()->setConnectionParameter(QModbusDevice::NetworkAddressParameter, url.host());
        }
        getModbusClient()->setTimeout(m_settings.responseTime);
        getModbusClient()->setNumberOfRetries(m_settings.numberOfRetries);
        if (!getModbusClient()->connectDevice()) {
            logError(tr("Connect failed:%1").arg(getModbusClient()->errorString()));
        } else {
            logInfo(tr("Modbus Master is Successfully connected!"));
        }
    }
    else
    {
        getModbusClient()->disconnectDevice();
    }

}

void SettingsDialog::onConnectionTypeChanged(int index)
{
    if (getModbusClient()) {
        getModbusClient()->disconnectDevice();
        destroyModbusClient();
    }

    auto type = static_cast<ModbusConnection>(index);
    if (type == Serial)
    {
        ui->serialParaGroupBox->setEnabled(true);
        setModbusClient(new QModbusRtuSerialMaster(this));
        ui->portEdit->setText(QLatin1String("COM1"));
    }
    else if (type == Tcp)
    {
        ui->serialParaGroupBox->setEnabled(false);
        setModbusClient(new QModbusTcpClient(this));
//        if (ui->portEdit->text().isEmpty())
            ui->portEdit->setText(QLatin1String("192.168.1.12:502"));
    }

    connect(getModbusClient(), &QModbusClient::errorOccurred, [](QModbusDevice::Error) {
        logError(getModbusClient()->errorString());
    });

    if (!getModbusClient()) {
        ui->connectButton->setDisabled(true);
        if(type == Serial)
        {
            logError(tr("Could not create Modbus RTU master."));
        }
        else
        {
            logError(tr("Could not create Modbus TCP master."));
        }
    }
    else
    {
        logInfo(tr("Modbus Master is Successfully created!"));
        connect(getModbusClient(), &QModbusClient::stateChanged,
                this,&SettingsDialog::onModbusStateChanged);
    }
}

void SettingsDialog::onModbusStateChanged(int state)
{
//    bool connected = (state != QModbusDevice::UnconnectedState);
    if (state == QModbusDevice::UnconnectedState)
        ui->connectButton->setText(tr("Connect"));
    else if (state == QModbusDevice::ConnectedState)
        ui->connectButton->setText(tr("Disconnect"));
}
