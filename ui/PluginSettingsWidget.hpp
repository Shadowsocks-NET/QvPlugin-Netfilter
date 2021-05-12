#pragma once

#include "QvGUIPluginInterface.hpp"
#include "core/Settings.hpp"
#include "ui_PluginSettingsWidget.h"

class SimplePluginSettingsWidget
    : public Qv2rayPlugin::QvPluginSettingsWidget
    , private Ui::PluginSettingsWidget
{
    Q_OBJECT

  public:
    explicit SimplePluginSettingsWidget(QWidget *parent = nullptr);
    void SetSettings(const QJsonObject &o) override
    {
        option.loadJson(o);
        ignoredPatternTxt->setPlainText(option.filterOptions->bypassProcesses->join("\n"));
    }
    QJsonObject GetSettings() override
    {
        return option.toJson();
    }

  protected:
    void changeEvent(QEvent *e) override;

  private slots:
    void on_ignoredPatternTxt_textChanged();

  private:
    QJS_BINDING_HELPERS
    PluginOptions option;
};
