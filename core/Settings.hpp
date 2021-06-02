#pragma once

#include "QJsonStruct.hpp"

#include <QObject>

enum NetworkType
{
    NETWORK_UNSPECIFIED,
    NETWORK_TCP = 6,
    NETWORK_UDP = 17
};

class FilterRule : public QObject
{
    Q_OBJECT
    QJS_FUNCTION(FilterRule, F(portFrom, portTo, networkType, processName))
    QJS_PROP_D(int, portFrom, 0)
    QJS_PROP_D(int, portTo, 0)
    QJS_PROP_D(NetworkType, networkType, NETWORK_UNSPECIFIED)
    QJS_PROP_D(QString, processName, "")
    explicit FilterRule(int f, int t = 0, NetworkType n = NETWORK_UNSPECIFIED, QString proc = "")
    {
        portFrom = f;
        portTo = t;
        networkType = n;
        processName = proc;
    }
};

typedef QList<FilterRule> FilterRules;
static const inline FilterRules DefaultRules = QList<FilterRule>()                                      //
                                               << FilterRule{ 0, 0, NETWORK_UNSPECIFIED, "qv2ray.exe" } // Qv2ray
                                               << FilterRule{ 0, 0, NETWORK_UNSPECIFIED, "v2ray.exe" }  // V2Ray
                                               << FilterRule{ 67, 0, NETWORK_UDP }                      // DHCP
                                               << FilterRule{ 123, 0, NETWORK_UDP };                    // NTP

class PluginOptions : public QObject
{
    Q_OBJECT
  public:
    QJS_FUNCTION(PluginOptions, F(rules, autoStart))
    QJS_PROP_D(FilterRules, rules, DefaultRules, REQUIRED)
    QJS_PROP_D(bool, autoStart, false)
};
