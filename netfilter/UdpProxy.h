#pragma once

#include "SOCKS.h"
#include "UDPContext.hpp"
#include "iocp.h"
#include "nfapi.h"
#include "sync.h"

#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>

#define PACKET_SIZE 65536

namespace UdpProxy
{

    enum OV_TYPE
    {
        OVT_CONNECT,
        OVT_TCP_SEND,
        OVT_TCP_RECEIVE,
        OVT_UDP_SEND,
        OVT_UDP_RECEIVE
    };

    struct OV_DATA
    {
        OV_DATA()
        {
            memset(&ol, 0, sizeof(ol));
        }

        OVERLAPPED ol;
        unsigned __int64 id;
        OV_TYPE type;
        char remoteAddress[NF_MAX_ADDRESS_LENGTH];
        int remoteAddressLen;
        std::vector<char> buffer;
    };

    enum PROXY_STATE
    {
        PS_NONE,
        PS_AUTH,
        PS_AUTH_NEGOTIATION,
        PS_UDP_ASSOC,
        PS_CONNECTED,
        PS_CLOSED
    };

    struct UDP_PACKET
    {
        char remoteAddress[NF_MAX_ADDRESS_LENGTH];
        int remoteAddressLen;
        std::vector<char> buffer;
    };

    typedef std::list<UDP_PACKET *> tPacketList;

    struct PROXY_DATA
    {
        PROXY_DATA()
        {
            tcpSocket = INVALID_SOCKET;
            udpSocket = INVALID_SOCKET;
            proxyState = PS_NONE;
            memset(remoteAddress, 0, sizeof(remoteAddress));
            remoteAddressLen = 0;
            udpRecvStarted = false;
        }
        ~PROXY_DATA()
        {
            if (tcpSocket != INVALID_SOCKET)
            {
                shutdown(tcpSocket, SD_BOTH);
                closesocket(tcpSocket);
            }
            if (udpSocket != INVALID_SOCKET)
            {
                closesocket(udpSocket);
            }
            while (!udpSendPackets.empty())
            {
                tPacketList::iterator it = udpSendPackets.begin();
                delete (*it);
                udpSendPackets.erase(it);
            }
        }

        SOCKET tcpSocket;
        SOCKET udpSocket;

        PROXY_STATE proxyState;

        char remoteAddress[NF_MAX_ADDRESS_LENGTH];
        int remoteAddressLen;

        std::string userName;
        std::string userPassword;

        tPacketList udpSendPackets;

        bool udpRecvStarted;
    };

    class UDPProxy : public IOCPHandler
    {
      public:
        UDPProxy() = default;
        virtual ~UDPProxy() = default;

        void *getExtensionFunction(SOCKET s, const GUID *which_fn);
        bool initExtensions();

        bool init(UDPProxyHandler *hander, const char *addr, int addrlen, const std::string &user, const std::string &pass);
        void free();

        bool createProxyConnection(unsigned __int64 id);
        void deleteProxyConnection(unsigned __int64 id);

        bool udpSend(unsigned __int64 id, char *buf, int len, char *remoteAddress, int remoteAddressLen);

        OV_DATA *newOV_DATA();
        void deleteOV_DATA(OV_DATA *pov);

        bool startConnect(SOCKET socket, sockaddr *pAddr, int addrLen, unsigned __int64 id);
        bool startUdpReceive(SOCKET socket, unsigned __int64 id, OV_DATA *pov);
        bool startTcpReceive(SOCKET socket, unsigned __int64 id, OV_DATA *pov);
        bool startTcpSend(SOCKET socket, char *buf, int len, unsigned __int64 id);

        void onUdpSendComplete(SOCKET socket, DWORD dwTransferred, OV_DATA *pov, int error);
        void onUdpReceiveComplete(SOCKET socket, DWORD dwTransferred, OV_DATA *pov, int error);

        void onConnectComplete(SOCKET socket, DWORD dwTransferred, OV_DATA *pov, int error);
        void onTcpSendComplete(SOCKET socket, DWORD dwTransferred, OV_DATA *pov, int error);
        void onTcpReceiveComplete(SOCKET socket, DWORD dwTransferred, OV_DATA *pov, int error);

        virtual void onComplete(SOCKET socket, DWORD dwTransferred, OVERLAPPED *pOverlapped, int error);

      private:
        IOCPService m_service;
        UDPProxyHandler *m_pProxyHandler = nullptr;

        std::set<OV_DATA *> m_ovDataSet;
        std::map<uint64_t, PROXY_DATA *> m_socketMap;

        LPFN_CONNECTEX m_pConnectEx;

        char m_proxyAddress[NF_MAX_ADDRESS_LENGTH];
        int m_proxyAddressLen;

        std::string m_userName;
        std::string m_userPassword;

        CriticalSection m_cs;
    };
} // namespace UdpProxy
