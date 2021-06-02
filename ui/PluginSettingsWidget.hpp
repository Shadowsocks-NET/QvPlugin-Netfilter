#pragma once

#include "QvGUIPluginInterface.hpp"
#include "core/Settings.hpp"
#include "ui_PluginSettingsWidget.h"

class SimplePluginSettingsWidget
    : public Qv2rayPlugin::PluginSettingsWidget
    , private Ui::PluginSettingsWidget
{
    Q_OBJECT

  public:
    explicit SimplePluginSettingsWidget(QWidget *parent = nullptr);
    void SetSettings(const QJsonObject &o) override
    {
        option.loadJson(o);
    }

    QJsonObject GetSettings() override
    {
        return option.toJson();
    }

  protected:
    void changeEvent(QEvent *e) override;

  private:
    PluginOptions option;
};
