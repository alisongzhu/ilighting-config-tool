#ifndef STATUSFIELD_H
#define STATUSFIELD_H

#include <QDockWidget>
#include <QAbstractAnimation>

namespace Ui {
class StatusField;
}

class StatusField : public QDockWidget
{
    Q_OBJECT

public:
    explicit StatusField(QWidget *parent = nullptr);
    ~StatusField();

    bool hasMessages() const;

private:
    void addEntry(const QString &type, const QString& text);

//    void flashItems(const QList<QTableWidgetItem*>& items);
    void setupMenu();
    void readRecentMessages();

private slots:
    void info(const QString& text);
    void warn(const QString& text);
    void error(const QString& text);
//    void reset();

private:
    Ui::StatusField *ui;

//    QList<QAbstractAnimation*> itemAnimations;
    bool noFlashing = false;

    static const int timeStampColumnWidth = 70;
    static const int itemCountLimit = 30;
    static constexpr const char* timeStampFormat = "hh:mm:ss";

};

#endif // STATUSFIELD_H
