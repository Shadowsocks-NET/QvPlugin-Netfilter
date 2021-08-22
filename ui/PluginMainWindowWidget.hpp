#pragma once

#include "QvPlugin/Gui/QvGUIPluginInterface.hpp"
#include "ui_PluginMainWindowWidget.h"

class SimplePluginMainWindowWidget
    : public Qv2rayPlugin::Gui::PluginMainWindowWidget
    , private Ui::PluginMainWindowWidget
{
    Q_OBJECT

  public:
    explicit SimplePluginMainWindowWidget(QWidget *parent = nullptr);

  protected:
    void changeEvent(QEvent *e) override;
};
