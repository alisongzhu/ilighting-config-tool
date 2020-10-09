#ifndef LOGMANAGER_H
#define LOGMANAGER_H

#include "global_res.h"

#include <QObject>

class LogManager : public QObject
{
    Q_OBJECT
    DECLARE_SINGLETON(LogManager)
public:
    explicit LogManager(QObject *parent = nullptr);

    QList<QString> getRecentErrors() const;
    QList<QString> getRecentWarnings() const;
    QList<QString> getRecentInfos() const;

signals:
    void logError(const QString& msg);
    void logWarning(const QString& msg);
    void logInfo(const QString& msg);

public slots:
    void error(const QString& msg);
    void warn(const QString& msg);
    void info(const QString& msg);

private:
    void addToRecentList(QStringList& list, const QString& message);

    static const constexpr int maxRecentMessages = 10;

    QStringList recentErrors;
    QStringList recentWarnings;
    QStringList recentInfos;
};

#define LOG_MANAGER LogManager::getInstance()

void logError(const QString& msg);
void logWarn(const QString& msg);
void logInfo(const QString& msg);

#endif // LOGMANGER_H
