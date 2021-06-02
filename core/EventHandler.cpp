#include "EventHandler.hpp"

#include "NetfilterPlugin.hpp"

#include <QMessageBox>

NetfilterPluginEventHandler::NetfilterPluginEventHandler() : QObject(), Qv2rayPlugin::handlers::event::PluginEventHandler()
{
    connect(&core, &NetFilterCore::OnLogMessageReady, this, &NetfilterPluginEventHandler::OnLog);
}

void NetfilterPluginEventHandler::ProcessEvent(const Connectivity::EventObject &o)
{
    switch (o.Type)
    {
        case Connectivity::EventType::Connected:
        {
            core.Start(((NetfilterPlugin *) PluginInstance)->options.filterOptions, "127.0.0.1", o.InboundPorts["socks"]);
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
