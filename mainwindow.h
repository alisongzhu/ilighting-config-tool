#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QModbusDataUnit>
#include <QtSql>
#include <QSqlTableModel>

QT_BEGIN_NAMESPACE

//class QModbusClient;
//class QModbusReply;

namespace Ui {
 class MainWindow;
 class SettingsDialog;
 class DeviceTreeDock;
 class GatewayBasicTabWidget;
 class StatusField;
}

QT_END_NAMESPACE

class SettingsDialog;
class DeviceTreeDock;
class GatewayBasicTabWidget;
class StatusField;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void initActions();
    void initDb();
    QModbusDataUnit readRequest() const;
    QModbusDataUnit writeRequest() const;

    bool createConnection(QSqlDatabase &db,QString &name);

private slots:
    void about();
    void newProject();
    void saveAs();
    void open();
    void save();

    void process_devList_doubleClicked(const QModelIndex &index);

    void on_actionHelp_triggered();

private:
    Ui::MainWindow *ui;

//    QModbusReply *lastRequest = nullptr;
//    QModbusClient *modbusDevice = nullptr;

    SettingsDialog *m_settingsDialog  = nullptr;
    DeviceTreeDock *m_devTreeDock = nullptr;

    StatusField *m_statusField = nullptr;

    QSqlDatabase db;
    QString db_name;

    QSqlTableModel *devListModel = nullptr;
//    GatewayBasicTabWidget *gateway = nullptr;

    QWidget *currentWindow = nullptr;
    QWidget *lastWindow= nullptr;
};
#endif // MAINWINDOW_H
