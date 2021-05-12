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
    Stop();
    eh->free();
    WSACleanup();
    delete eh;
}

void NetFilterCore::Stop()
{
    nfapi::nf_free();
}

bool NetFilterCore::Start(const NetFilterConfig &conf, const QString &address, int port, const QString &username, const QString &password)
{
    const auto fullAddr = address + ":" + QString::number(port);

    unsigned char g_proxyAddress[NF_MAX_ADDRESS_LENGTH];
    memset(g_proxyAddress, 0, NF_MAX_ADDRESS_LENGTH);

    auto addrLen = NF_MAX_ADDRESS_LENGTH;
    auto err = WSAStringToAddress(fullAddr.toStdWString().data(), AF_INET, NULL, (LPSOCKADDR) &g_proxyAddress, &addrLen);
    if (err < 0)
    {
        addrLen = sizeof(g_proxyAddress);
        err = WSAStringToAddress(fullAddr.toStdWString().data(), AF_INET6, NULL, (LPSOCKADDR) &g_proxyAddress, &addrLen);
        if (err < 0)
        {
            printf("WSAStringToAddress failed, err=%d", WSAGetLastError());
            return false;
        }
    }

    if (!eh->init(g_proxyAddress))
    {
        printf("Failed to initialize the event handler");
        return false;
    }
    eh->username = username.toStdString();
    eh->password = password.toStdString();

    // Initialize the library and start filtering thread
    if (nf_init(NFDRIVER_NAME, eh) != NF_STATUS_SUCCESS)
    {
        printf("Failed to connect to driver");
        return false;
    }

    {
        nfapi::NF_RULE rule;
        memset(&rule, 0, sizeof(rule));
        rule.filteringFlag = nfapi::NF_ALLOW;
        rule.ip_family = AF_INET;
        inet_pton(AF_INET, "127.0.0.1", &rule.remoteIpAddress);
        inet_pton(AF_INET, "255.0.0.0", &rule.remoteIpAddressMask);
        nf_addRule(&rule, FALSE);

        memset(&rule, 0, sizeof(rule));
        rule.filteringFlag = nfapi::NF_ALLOW;
        rule.ip_family = AF_INET6;
        rule.remoteIpAddress[15] = 1;
        nf_addRule(&rule, FALSE);
    }

    for (const auto &process : qAsConst(*conf.bypassProcesses))
    {
        nfapi::NF_RULE_EX ruleEx;
        memset(&ruleEx, 0, sizeof(ruleEx));
        ruleEx.filteringFlag = nfapi::NF_ALLOW;
        wcsncpy((wchar_t *) ruleEx.processName, process.toStdWString().c_str(), MAX_PATH);
        nf_addRuleEx(&ruleEx, TRUE);
    }

    // Proxy All
    {
        nfapi::NF_RULE_EX ruleEx;
        memset(&ruleEx, 0, sizeof(ruleEx));
        ruleEx.protocol = IPPROTO_TCP;
        ruleEx.direction = nfapi::NF_D_OUT;
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
