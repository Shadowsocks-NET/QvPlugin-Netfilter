#include "NetfilterPlugin.hpp"

#include "core/EventHandler.hpp"
#include "ui/Interface.hpp"

bool NetfilterPlugin::InitializePlugin()
{
    options.loadJson(m_Settings);
    m_EventHandler = std::make_shared<NetfilterPluginEventHandler>();
    m_GUIInterface = new SimpleGUIInterface();
    return true;
}
