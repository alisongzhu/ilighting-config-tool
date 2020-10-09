#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

#include <QtSerialBus/qtserialbusglobal.h>
#include <QSerialPort>


//class QModbusClient;

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();

    struct Settings {
        int parity = QSerialPort::NoParity;
        int baud = QSerialPort::Baud115200;
        int dataBits = QSerialPort::Data8;
        int stopBits = QSerialPort::OneStop;

        int responseTime = 3000;
        int numberOfRetries = 0;
    };

#if 0
    Settings settings() const;
#endif

//    QModbusClient* getModbusClient();

private:
    Ui::SettingsDialog *ui;

    Settings m_settings;

//    QModbusClient *modbusDevice = nullptr;

private slots:
    void onConnectButtonClicked();
    void onConnectionTypeChanged(int);
    void onModbusStateChanged(int state);
};

#endif // SETTINGSDIALOG_H
