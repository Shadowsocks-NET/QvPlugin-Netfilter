#include "NFEventHandler.hpp"

#include "UdpProxy.h"

EventHandler::EventHandler(logfunc func)
{
    doLog = func;
    // m_tcpProxy = new TcpProxy::TCPProxy;
    m_udpProxy = new UdpProxy::UDPProxy;
}
EventHandler::~EventHandler()
{
    delete m_udpProxy;
}

void EventHandler::tcpCanReceive(nfapi::ENDPOINT_ID){};
void EventHandler::tcpCanSend(nfapi::ENDPOINT_ID){};
void EventHandler::udpConnectRequest(nfapi::ENDPOINT_ID, nfapi::PNF_UDP_CONN_REQUEST){};
void EventHandler::udpCanReceive(nfapi::ENDPOINT_ID){};
void EventHandler::udpCanSend(nfapi::ENDPOINT_ID){};

bool EventHandler::init(std::wstring address, std::string username, std::string password)
{
    memset(g_proxyAddress, 0, NF_MAX_ADDRESS_LENGTH);

    auto addrLen = NF_MAX_ADDRESS_LENGTH;
    auto err = WSAStringToAddress(address.data(), AF_INET, NULL, (LPSOCKADDR) &g_proxyAddress, &addrLen);
    if (err < 0)
    {
        addrLen = sizeof(g_proxyAddress);
        err = WSAStringToAddress(address.data(), AF_INET6, NULL, (LPSOCKADDR) &g_proxyAddress, &addrLen);
        if (err < 0)
        {
            printf("WSAStringToAddress failed, err=%d", WSAGetLastError());
            return false;
        }
    }

    const int size = (((sockaddr *) g_proxyAddress)->sa_family == AF_INET6) ? sizeof(sockaddr_in6) : sizeof(sockaddr_in);
    if (!m_udpProxy->init(this, (char *) g_proxyAddress, size, username, password))
    {
        printf("Unable to start UDP proxy");
        free();
        return false;
    }
    return true;
}

void EventHandler::free()
{
    m_udpProxy->free();
    std::lock_guard<std::mutex> guard(lock);
    for (auto &&[k, v] : m_udpCtxMap)
        delete v;
    m_udpCtxMap.clear();
}

void EventHandler::onUdpReceiveComplete(unsigned long long id, char *buf, int len, char *remoteAddress, int remoteAddressLen)
{
    (void) remoteAddressLen;
    std::lock_guard<std::mutex> guard(lock);

    auto it = m_udpCtxMap.find(id);
    if (it == m_udpCtxMap.end())
        return;

    nf_udpPostReceive(id, (const unsigned char *) remoteAddress, buf, len, it->second->m_options);
}

void EventHandler::tcpConnectRequest(nfapi::ENDPOINT_ID id, nfapi::PNF_TCP_CONN_INFO pConnInfo)
{

    ORIGINAL_CONN_INFO oci;
    memcpy(oci.remoteAddress, pConnInfo->remoteAddress, sizeof(oci.remoteAddress));
    // Save the original remote address
    m_connInfoMap[id] = oci;

    sockaddr *pAddr = (sockaddr *) pConnInfo->remoteAddress;
    int addrLen = (pAddr->sa_family == AF_INET6) ? sizeof(sockaddr_in6) : sizeof(sockaddr_in);

    //    if (pAddr->sa_family == AF_INET)
    //    {
    // Redirect the connection if it is not already redirected
    if (memcmp(pAddr, &g_proxyAddress, addrLen) != 0)
    {
        // Change the remote address
        memcpy(pConnInfo->remoteAddress, &g_proxyAddress, sizeof(pConnInfo->remoteAddress));
        pConnInfo->filteringFlag |= nfapi::NF_FILTER;
    }
    //    }

    //    if (pAddr->sa_family == AF_INET6)
    //    {
    //        sockaddr_in6 addrV6;
    //        memset(&addrV6, 0, sizeof(addrV6));
    //        addrV6.sin6_family = AF_INET6;
    //        inet_pton(AF_INET6, "::1", &addrV6.sin6_addr);
    //        addrV6.sin6_port = htons(socksPort);
    //        // Redirect the connection if it is not already redirected
    //        if (memcmp(pAddr, &addrV6, addrLen) != 0)
    //        {
    //            // Change the remote address
    //            memcpy(pConnInfo->remoteAddress, &addrV6, sizeof(pConnInfo->remoteAddress));
    //            pConnInfo->filteringFlag |= nfapi::NF_FILTER;
    //        }
    //    }
}

void EventHandler::tcpConnected(nfapi::ENDPOINT_ID id, nfapi::PNF_TCP_CONN_INFO pConnInfo)
{
    LogTCP(true, id, pConnInfo);
    fflush(stdout);
}

void EventHandler::tcpClosed(nfapi::ENDPOINT_ID id, nfapi::PNF_TCP_CONN_INFO pConnInfo)
{
    LogTCP(false, id, pConnInfo);
    m_connInfoMap.erase(id);
    fflush(stdout);
}

void EventHandler::tcpReceive(nfapi::ENDPOINT_ID id, const char *buf, int len)
{
    auto it = m_connInfoMap.find(id);
    if (it != m_connInfoMap.end())
    {
        if (!it->second.pendedSends.empty())
        {
            nfapi::nf_tcpPostSend(id, &it->second.pendedSends[0], (int) it->second.pendedSends.size());
        }

        m_connInfoMap.erase(id);
        return;
    }

    // Send the packet to application
    nfapi::nf_tcpPostReceive(id, buf, len);

    // Don't filter the subsequent packets (optimization)
    nfapi::nf_tcpDisableFiltering(id);
}

void EventHandler::tcpSend(nfapi::ENDPOINT_ID id, const char *buf, int len)
{
    auto it = m_connInfoMap.find(id);

    if (it != m_connInfoMap.end())
    {
        SOCKS4_REQUEST request;
        sockaddr_in *pAddr;
        auto it = m_connInfoMap.find(id);
        if (it == m_connInfoMap.end())
            return;

        pAddr = (sockaddr_in *) &it->second.remoteAddress;
        if (pAddr->sin_family == AF_INET6)
        {
            return;
        }

        request.version = SOCKS_4;
        request.command = S4C_CONNECT;
        request.ip = pAddr->sin_addr.S_un.S_addr;
        request.port = pAddr->sin_port;
        request.userid[0] = '\0';

        // Send the request first
        nfapi::nf_tcpPostSend(id, (char *) &request, (int) sizeof(request));

        it->second.pendedSends.insert(it->second.pendedSends.end(), buf, buf + len);

        return;
    }

    // Send the packet to server
    nfapi::nf_tcpPostSend(id, buf, len);
}

void EventHandler::udpCreated(nfapi::ENDPOINT_ID id, nfapi::PNF_UDP_CONN_INFO pConnInfo)
{
    std::lock_guard<std::mutex> guard(lock);
    LogUDP(true, id, pConnInfo);
}

void EventHandler::udpClosed(nfapi::ENDPOINT_ID id, nfapi::PNF_UDP_CONN_INFO pConnInfo)
{
    LogUDP(false, id, pConnInfo);

    m_udpProxy->deleteProxyConnection(id);

    std::lock_guard<std::mutex> guard(lock);
    if (m_udpCtxMap.find(id) != m_udpCtxMap.end())
    {
        delete m_udpCtxMap[id];
        m_udpCtxMap.erase(id);
    }
}

void EventHandler::udpReceive(nfapi::ENDPOINT_ID id, const unsigned char *remoteAddress, const char *buf, int len, nfapi::PNF_UDP_OPTIONS options)
{
    // Send the packet to application
    nfapi::nf_udpPostReceive(id, remoteAddress, buf, len, options);
}

void EventHandler::udpSend(nfapi::ENDPOINT_ID id, const unsigned char *remoteAddress, const char *buf, int len, nfapi::PNF_UDP_OPTIONS options)
{
    {
        std::lock_guard<std::mutex> guard(lock);
        if (m_udpCtxMap.find(id) == m_udpCtxMap.end())
        {
            if (!m_udpProxy->createProxyConnection(id))
                return;
            m_udpCtxMap[id] = new UDPContext(options);
        }
    }

    int addrLen = (((sockaddr *) remoteAddress)->sa_family == AF_INET) ? sizeof(sockaddr_in) : sizeof(sockaddr_in6);
    if (!m_udpProxy->udpSend(id, (char *) buf, len, (char *) remoteAddress, addrLen))
    {
        nf_udpPostSend(id, remoteAddress, buf, len, options);
    }
}

void EventHandler::LogTCP(bool connected, nfapi::ENDPOINT_ID id, nfapi::PNF_TCP_CONN_INFO pConnInfo)
{
    TCHAR localAddr[MAX_PATH] = L"";
    TCHAR remoteAddr[MAX_PATH] = L"";
    DWORD dwLen;
    sockaddr *pAddr;
    TCHAR processName[MAX_PATH] = L"";

    pAddr = (sockaddr *) pConnInfo->localAddress;
    dwLen = sizeof(localAddr);

    WSAAddressToString((LPSOCKADDR) pAddr, (pAddr->sa_family == AF_INET6) ? sizeof(sockaddr_in6) : sizeof(sockaddr_in), NULL, localAddr, &dwLen);

    pAddr = (sockaddr *) pConnInfo->remoteAddress;
    dwLen = sizeof(remoteAddr);

    WSAAddressToString((LPSOCKADDR) pAddr, (pAddr->sa_family == AF_INET6) ? sizeof(sockaddr_in6) : sizeof(sockaddr_in), NULL, remoteAddr, &dwLen);

    if (!nfapi::nf_getProcessNameFromKernel(pConnInfo->processId, processName, sizeof(processName) / sizeof(processName[0])))
        processName[0] = '\0';

    doLog(id, PROTOCOL::TCP, connected, localAddr, remoteAddr, pConnInfo->processId, processName);
}

void EventHandler::LogUDP(bool created, nfapi::ENDPOINT_ID id, nfapi::PNF_UDP_CONN_INFO pConnInfo)
{
    TCHAR localAddr[MAX_PATH] = L"";
    sockaddr *pAddr;
    DWORD dwLen;
    TCHAR processName[MAX_PATH] = L"";

    pAddr = (sockaddr *) pConnInfo->localAddress;
    dwLen = sizeof(localAddr);

    WSAAddressToString((LPSOCKADDR) pAddr, (pAddr->sa_family == AF_INET6) ? sizeof(sockaddr_in6) : sizeof(sockaddr_in), NULL, localAddr, &dwLen);

    if (nfapi::nf_getProcessNameFromKernel(pConnInfo->processId, processName, sizeof(processName) / sizeof(processName[0])))
        processName[0] = '\0';
    doLog(id, PROTOCOL::UDP, created, localAddr, L"", pConnInfo->processId, processName);
}
