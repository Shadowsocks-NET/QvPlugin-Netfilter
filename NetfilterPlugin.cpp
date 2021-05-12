#include "NetfilterPlugin.hpp"

#include "core/EventHandler.hpp"
#include "ui/Interface.hpp"

bool NetfilterPlugin::InitializePlugin(const QString &, const QJsonObject &_settings)
{
    plugin_instance = this;
    emit PluginLog("Initialize plugin.");
    this->settings = _settings;
    options.loadJson(settings);
    eventHandler = std::make_shared<NetfilterPluginEventHandler>();
    guiInterface = new SimpleGUIInterface();
    return true;
}
