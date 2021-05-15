#pragma once

#include "QvGUIPluginInterface.hpp"
#include "ui_PluginMainWindowWidget.h"

class SimplePluginMainWindowWidget
    : public Qv2rayPlugin::PluginMainWindowWidget
    , private Ui::PluginMainWindowWidget
{
    Q_OBJECT

  public:
    explicit SimplePluginMainWindowWidget(QWidget *parent = nullptr);

  protected:
    void changeEvent(QEvent *e) override;
};
