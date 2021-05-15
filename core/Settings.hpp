#pragma once

#include "3rdparty/QJsonStruct/QJsonStruct.hpp"

#include <QObject>

class NetFilterConfig : public QObject
{
    Q_OBJECT
    QJS_PROP_D(QStringList, bypassProcesses,
               QStringList{} << "qv2ray.exe"
                             << "v2ray.exe",
               REQUIRED)
    QJS_FUNCTION(NetFilterConfig, F(bypassProcesses))
};

class PluginOptions : public QObject
{
    Q_OBJECT
  public:
    QJS_PROP(NetFilterConfig, filterOptions)
    QJS_PROP_D(bool, autoStart, false)
    QJS_FUNCTION(PluginOptions, F(filterOptions, autoStart))
};
