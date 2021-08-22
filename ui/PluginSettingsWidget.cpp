#include "PluginSettingsWidget.hpp"

SimplePluginSettingsWidget::SimplePluginSettingsWidget(QWidget *parent) : Qv2rayPlugin::Gui::PluginSettingsWidget(parent)
{
    setupUi(this);
    option.autoStart.ReadWriteBind(autoStartCB, "checked", &QCheckBox::toggled);
}

void SimplePluginSettingsWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type())
    {
        case QEvent::LanguageChange: retranslateUi(this); break;
        default: break;
    }
}
