#pragma once

#include "QvPlugin/Gui/QvGUIPluginInterface.hpp"
#include "core/Settings.hpp"
#include "ui_PluginSettingsWidget.h"

class SimplePluginSettingsWidget
    : public Qv2rayPlugin::Gui::PluginSettingsWidget
    , private Ui::PluginSettingsWidget
{
    Q_OBJECT

  public:
    explicit SimplePluginSettingsWidget(QWidget *parent = nullptr);

    // PluginSettingsWidget interface
  public:
    void Load() override
    {
        option.loadJson(settings);
    }
    void Store() override
    {
        settings = option.toJson();
    }

  protected:
    void changeEvent(QEvent *e) override;

  private:
    PluginOptions option;
};
