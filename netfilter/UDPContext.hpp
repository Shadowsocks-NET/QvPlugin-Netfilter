#pragma once
#include "base.h"

struct UDPContext
{
    UDPContext(nfapi::PNF_UDP_OPTIONS options)
    {
        if (options)
        {
            m_options = (nfapi::PNF_UDP_OPTIONS) new char[sizeof(nfapi::NF_UDP_OPTIONS) + options->optionsLength];
            memcpy(m_options, options, sizeof(nfapi::NF_UDP_OPTIONS) + options->optionsLength - 1);
        }
        else
        {
            m_options = NULL;
        }
    }
    ~UDPContext()
    {
        if (m_options)
        {
            delete[] m_options;
            m_options = NULL;
        }
    }

    nfapi::PNF_UDP_OPTIONS m_options;
};

class UDPProxyHandler
{
  public:
    virtual void onUdpReceiveComplete(unsigned __int64 id, char *buf, int len, char *remoteAddress, int remoteAddressLen) = 0;
};
