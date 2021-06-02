#include "SocksRedirector.hpp"

#include "NFEventHandler.hpp"

// Change this string after renaming and registering the driver under different name
#define NFDRIVER_NAME "netfilter2"

NetFilterCore::NetFilterCore()
{
    eh = new EventHandler(std::function{ [this](size_t c, PROTOCOL p, bool o, std::wstring l, std::wstring r, int pid, std::wstring path) {
        QString protocol;
        switch (p)
        {
            case PROTOCOL::TCP: protocol = "TCP"; break;
            case PROTOCOL::UDP: protocol = "UDP"; break;
            default: break;
        }
        emit OnLogMessageReady(c, protocol, o, QString::fromStdWString(l), QString::fromStdWString(r), pid, QString::fromStdWString(path));
    } });
    wsaData = new WSADATA;
    WSAStartup(MAKEWORD(2, 2), wsaData);
}

NetFilterCore::~NetFilterCore()
{
    WSACleanup();
    delete eh;
}

void NetFilterCore::Stop()
{
    eh->free();
    nfapi::nf_free();
}

bool NetFilterCore::Start(const FilterRules &rules, const QString &address, int port, const QString &username, const QString &password)
{
    const auto fullAddr = address + ":" + QString::number(port);

    if (!eh->init(fullAddr.toStdWString(), username.toStdString(), password.toStdString()))
    {
        printf("Failed to initialize the event handler");
        return false;
    }

    // Initialize the library and start filtering thread
    if (nf_init(NFDRIVER_NAME, eh) != NF_STATUS_SUCCESS)
    {
        printf("Failed to connect to driver");
        return false;
    }

#define ADD_PRIVATE_IPV4_ADDRESS(addr, mask)                                                                                                         \
    {                                                                                                                                                \
        nfapi::NF_RULE rule;                                                                                                                         \
        memset(&rule, 0, sizeof(rule));                                                                                                              \
        rule.filteringFlag = nfapi::NF_ALLOW;                                                                                                        \
        rule.ip_family = AF_INET;                                                                                                                    \
        inet_pton(AF_INET, addr, &rule.remoteIpAddress);                                                                                             \
        inet_pton(AF_INET, mask, &rule.remoteIpAddressMask);                                                                                         \
        nf_addRule(&rule, FALSE);                                                                                                                    \
    }

    ADD_PRIVATE_IPV4_ADDRESS("10.0.0.1", "255.0.0.0")
    ADD_PRIVATE_IPV4_ADDRESS("127.0.0.1", "255.0.0.0")
    ADD_PRIVATE_IPV4_ADDRESS("172.16.0.1", "255.240.0.0")
    ADD_PRIVATE_IPV4_ADDRESS("192.168.0.1", "255.255.0.0")
    ADD_PRIVATE_IPV4_ADDRESS("224.0.0.1", "240.0.0.0")

    {
        nfapi::NF_RULE rule;
        memset(&rule, 0, sizeof(rule));
        rule.filteringFlag = nfapi::NF_ALLOW;
        rule.ip_family = AF_INET6;
        rule.remoteIpAddress[15] = 1;
        nf_addRule(&rule, FALSE);
    }

    {
        nfapi::NF_RULE rule;
        memset(&rule, 0, sizeof(rule));
        rule.filteringFlag = nfapi::NF_ALLOW;
        rule.ip_family = AF_INET6;
        inet_pton(AF_INET, "::ffff:127.0.0.1", &rule.remoteIpAddress);
        nf_addRule(&rule, FALSE);
    }

    for (const auto &rule : rules)
    {
        nfapi::NF_RULE_EX ruleEx;
        memset(&ruleEx, 0, sizeof(ruleEx));
        ruleEx.filteringFlag = nfapi::NF_ALLOW;

        if (rule.portTo != 0)
        {
            ruleEx.remotePortRange.valueLow = rule.portFrom;
            ruleEx.remotePortRange.valueHigh = rule.portTo;
        }
        else if (rule.portFrom != 0)
        {
            ruleEx.remotePort = rule.portFrom;
        }

        if (rule.networkType != NETWORK_UNSPECIFIED)
            ruleEx.protocol = rule.networkType;

        if (!rule.processName->isEmpty())
            wcsncpy((wchar_t *) ruleEx.processName, rule.processName->toStdWString().c_str(), MAX_PATH);

        nf_addRuleEx(&ruleEx, TRUE);
    }

    // Proxy All
    {
        nfapi::NF_RULE_EX ruleEx;
        memset(&ruleEx, 0, sizeof(ruleEx));
        ruleEx.protocol = IPPROTO_TCP;
        ruleEx.filteringFlag = nfapi::NF_INDICATE_CONNECT_REQUESTS;
        nf_addRuleEx(&ruleEx, FALSE);

        // Filter UDP packets
        memset(&ruleEx, 0, sizeof(ruleEx));
        ruleEx.protocol = IPPROTO_UDP;
        ruleEx.filteringFlag = nfapi::NF_FILTER;
        nf_addRuleEx(&ruleEx, FALSE);
    }
    return true;
}
