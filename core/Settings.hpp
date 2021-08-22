#pragma once

#include "QvPlugin/Utils/BindableProps.hpp"
#include "QvPlugin/Utils/JsonConversion.hpp"

#include <QObject>

enum NetworkType
{
    NETWORK_UNSPECIFIED,
    NETWORK_TCP = 6,
    NETWORK_UDP = 17
};

struct FilterRule
{
    QJS_JSON(F(portFrom, portTo, networkType, processName))
    Bindable<int> portFrom{ 0 };
    Bindable<int> portTo{ 0 };
    Bindable<NetworkType> networkType{ NETWORK_UNSPECIFIED };
    Bindable<QString> processName{ "" };
    explicit FilterRule(int f = 0, int t = 0, NetworkType n = NETWORK_UNSPECIFIED, QString proc = "")
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

struct PluginOptions
{
    QJS_JSON(F(rules, autoStart))
    Bindable<FilterRules> rules{ DefaultRules };
    Bindable<bool> autoStart{ false };
};
