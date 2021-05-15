#pragma once

#include "QvGUIPluginInterface.hpp"
#include "QvPluginInterface.hpp"
#include "core/Settings.hpp"

#include <QObject>
#include <QtPlugin>

using namespace Qv2rayPlugin;

class NetfilterPlugin
    : public QObject
    , Qv2rayInterface
{
    Q_INTERFACES(Qv2rayPlugin::Qv2rayInterface)
    Q_PLUGIN_METADATA(IID Qv2rayInterface_IID)
    Q_OBJECT
    friend class NetfilterPluginEventHandler;

  public:
    const QvPluginMetadata GetMetadata() const override
    {
        return { "Windows Transparent Proxy",                               //
                 "Community",                                               //
                 "qvplugin_winnetfilter",                                   //
                 "Transparent Proxy Plugin on Windows, using NetfilterSDK", //
                 "v0.1",                                                    //
                 "",                                                        //
                 {
                     COMPONENT_EVENT_HANDLER, //
                     COMPONENT_GUI            //
                 },
                 UPDATE_GITHUB_RELEASE };
    }
    ~NetfilterPlugin(){};
    bool InitializePlugin(const QString &, const QJsonObject &) override;
    void SettingsUpdated() override{};

  signals:
    void PluginLog(QString) const override;
    void PluginErrorMessageBox(QString, QString) const override;

  private:
    PluginOptions options;
};

inline NetfilterPlugin *plugin_instance;
