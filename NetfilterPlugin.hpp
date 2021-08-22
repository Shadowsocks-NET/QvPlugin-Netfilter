#pragma once

#include "QvPlugin/PluginInterface.hpp"
#include "core/Settings.hpp"

#include <QObject>
#include <QtPlugin>

using namespace Qv2rayPlugin;

class NetfilterPlugin
    : public QObject
    , public Qv2rayInterface<NetfilterPlugin>
{
    Q_OBJECT
    QV2RAY_PLUGIN(NetfilterPlugin)
    friend class NetfilterPluginEventHandler;

  public:
    const QvPluginMetadata GetMetadata() const override
    {
        return QvPluginMetadata{
            "NetFilter Transparent Proxy",
            "Community",
            PluginId{ "netfilter" },
            "A transparent proxy plugin for Qv2ray, uses NetfilterSDK",
            QUrl{},
            { Qv2rayPlugin::COMPONENT_EVENT_HANDLER, Qv2rayPlugin::COMPONENT_GUI },
        };
    }

    bool InitializePlugin() override;
    void SettingsUpdated() override{};

  private:
    PluginOptions options;
};
