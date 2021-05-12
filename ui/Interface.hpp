#include "QvGUIPluginInterface.hpp"
#include "ui/PluginMainWindowWidget.hpp"
#include "ui/PluginSettingsWidget.hpp"

using namespace Qv2rayPlugin;

class SimpleGUIInterface : public PluginGUIInterface
{
  public:
    explicit SimpleGUIInterface(){};
    ~SimpleGUIInterface(){};
    QList<PluginGuiComponentType> GetComponents() const override
    {
        return {
            GUI_COMPONENT_SETTINGS,         //
            GUI_COMPONENT_MAINWINDOW_WIDGET //
        };
    }
    std::unique_ptr<QvPluginSettingsWidget> createSettingsWidgets() const override
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
    std::unique_ptr<QvPluginMainWindowWidget> createMainWindowWidget() const override
    {
        return std::make_unique<SimplePluginMainWindowWidget>();
    }
    QIcon Icon() const override
    {
        return QIcon(":/assets/qv2ray.png");
    }
};
