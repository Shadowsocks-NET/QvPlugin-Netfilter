#pragma once

#include "QJsonStruct.hpp"

#include <QObject>

class NetFilterConfig : public QObject
{
    Q_OBJECT
    QJS_FUNCTION(NetFilterConfig, F(bypassProcesses))
    QJS_PROP_D(QStringList, bypassProcesses,
               QStringList{} << "qv2ray.exe"
                             << "v2ray.exe",
               REQUIRED)
};

class PluginOptions : public QObject
{
    Q_OBJECT
  public:
    QJS_FUNCTION(PluginOptions, F(filterOptions, autoStart))
    QJS_PROP(NetFilterConfig, filterOptions)
    QJS_PROP_D(bool, autoStart, false)
};
