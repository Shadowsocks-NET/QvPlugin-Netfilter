#pragma once
#include "QvPlugin/Handlers/EventHandler.hpp"
#include "netfilter/SocksRedirector.hpp"

using namespace Qv2rayPlugin::Event;

class NetfilterPluginEventHandler
    : public QObject
    , public Qv2rayPlugin::Event::IEventHandler
{
    Q_OBJECT
  public:
    NetfilterPluginEventHandler();
    void ProcessEvent(const Connectivity::EventObject &) override;

  private slots:
    void OnLog(size_t cid, QString protocol, bool isOpen, QString local, QString remote, int pid, QString path);

  private:
    NetFilterCore core;
};
