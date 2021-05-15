#include "PluginSettingsWidget.hpp"

SimplePluginSettingsWidget::SimplePluginSettingsWidget(QWidget *parent) : Qv2rayPlugin::PluginSettingsWidget(parent)
{
    setupUi(this);
    QJS_RWBINDING(option.autoStart, autoStartCB, "checked", &QCheckBox::toggled);
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

void SimplePluginSettingsWidget::on_ignoredPatternTxt_textChanged()
{
    option.filterOptions->bypassProcesses = ignoredPatternTxt->toPlainText().split("\n");
}
