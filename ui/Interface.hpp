#include "QvGUIPluginInterface.hpp"
#include "ui/PluginMainWindowWidget.hpp"
#include "ui/PluginSettingsWidget.hpp"

using namespace Qv2rayPlugin;

class SimpleGUIInterface : public Qv2rayGUIInterface
{
  public:
    explicit SimpleGUIInterface() : Qv2rayGUIInterface(){};
    ~SimpleGUIInterface(){};
    QList<QV2RAY_PLUGIN_GUI_COMPONENT_TYPE> GetComponents() const override
    {
        return {
            GUI_COMPONENT_SETTINGS,           //
            GUI_COMPONENT_MAIN_WINDOW_ACTIONS //
        };
    }
    std::unique_ptr<PluginSettingsWidget> createSettingsWidgets() const override
    {
        return std::make_unique<SimplePluginSettingsWidget>();
    }
    QList<typed_plugin_editor> createInboundEditors() const override
    {
        return {};
    }
    QList<typed_plugin_editor> createOutboundEditors() const override
    {
        return {};
    }
    std::unique_ptr<PluginMainWindowWidget> createMainWindowWidget() const override
    {
        return std::make_unique<SimplePluginMainWindowWidget>();
    }
    QIcon Icon() const override
    {
        return QIcon(":/assets/qv2ray.png");
    }
};
