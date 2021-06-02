#pragma once

#include "core/Settings.hpp"

#include <QObject>

class EventHandler;
struct WSAData;

class NetFilterCore : public QObject
{
    Q_OBJECT
  public:
    NetFilterCore();
    bool Start(const FilterRules &rules, const QString &address, int port, const QString &username = "", const QString &password = "");
    void Stop();
    virtual ~NetFilterCore();

  signals:
    void OnLogMessageReady(size_t cid, QString protocol, bool isOpen, QString local, QString remote, int pid, QString path);

  private:
    EventHandler *eh;
    WSAData *wsaData;
};
