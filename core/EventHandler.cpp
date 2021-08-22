#include "EventHandler.hpp"

#include "NetfilterPlugin.hpp"

#include <QMessageBox>

NetfilterPluginEventHandler::NetfilterPluginEventHandler() : QObject(), Qv2rayPlugin::IEventHandler()
{
    connect(&core, &NetFilterCore::OnLogMessageReady, this, &NetfilterPluginEventHandler::OnLog);
}

void NetfilterPluginEventHandler::ProcessEvent(const Connectivity::EventObject &o)
{
    switch (o.Type)
    {
        case Connectivity::EventType::Connected:
        {
            int socksPort = [&] {
                for (auto it = o.InboundData.constKeyValueBegin(); it != o.InboundData.constKeyValueEnd(); it++)
                {
                    const auto &[protocol, address, port] = it->second;
                    if (protocol == "socks")
                        return port.from;
                }
                return 0;
            }();
            core.Start(NetfilterPlugin::PluginInstance->options.rules, "127.0.0.1", socksPort);
            break;
        }
        case Connectivity::EventType::Disconnecting:
        {
            core.Stop();
            break;
        }
        default: break;
    }
}

void NetfilterPluginEventHandler::OnLog(size_t cid, QString protocol, bool isOpen, QString local, QString remote, int pid, QString path)
{
    // plugin_instance->PluginLog("Connection: " + QString::number(cid) + ":" + protocol + ":" + (isOpen ? "opening" : "closing") + " " + local +
    //                           " -> " + remote + " :: " + path);
}
