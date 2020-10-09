#include "logmanager.h"

#include <QDebug>

DEFINE_SINGLETON(LogManager)

LogManager::LogManager(QObject *parent) : QObject(parent)
{

}

QList<QString> LogManager::getRecentErrors() const
{
    return recentErrors;
}

QList<QString> LogManager::getRecentWarnings() const
{
    return recentWarnings;
}

QList<QString> LogManager::getRecentInfos() const
{
    return recentInfos;
}

void LogManager::error(const QString &msg)
{
    addToRecentList(recentErrors, msg);
    emit logError(msg);
}

void LogManager::warn(const QString &msg)
{
    addToRecentList(recentWarnings, msg);
    emit logWarning(msg);
}

void LogManager::info(const QString &msg)
{
    addToRecentList(recentInfos, msg);
    emit logInfo(msg);
}

void LogManager::addToRecentList(QStringList &list, const QString &message)
{
    list << message;
    if (list.size() <= maxRecentMessages)
        return;

    list = list.mid(list.length() - maxRecentMessages);
}

void logError(const QString &msg)
{
    qDebug() << "Error from log manager:" << msg;
    LogManager::getInstance()->error(msg);
}

void logWarn(const QString &msg)
{
    qDebug() << "Warning from log manager:" << msg;
    LogManager::getInstance()->warn(msg);
}

void logInfo(const QString &msg)
{
    LogManager::getInstance()->info(msg);
}
