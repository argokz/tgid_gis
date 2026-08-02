#include "stdafx.h"
#include <Winsock2.h>
//DllExport 
#if 0
void get_local_ips(vector<wstring>& ips)
{
    IP_ADAPTER_ADDRESSES*       adapters = NULL;
    IP_ADAPTER_ADDRESSES*       adapter = NULL;
    IP_ADAPTER_UNICAST_ADDRESS* adr = NULL;
    ULONG                       adapter_size = 0;
    ULONG                       err = 0;
    SOCKADDR_IN*                sockaddr = NULL;

    err = ::GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER | GAA_FLAG_SKIP_FRIENDLY_NAME, NULL, NULL, &adapter_size);
    adapters = (IP_ADAPTER_ADDRESSES*)malloc(adapter_size);
    err = ::GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER | GAA_FLAG_SKIP_FRIENDLY_NAME, NULL, adapters, &adapter_size);

    for (adapter = adapters; NULL != adapter; adapter = adapter->Next)
    {
        if (adapter->IfType == IF_TYPE_SOFTWARE_LOOPBACK) continue; // Skip Loopback
        if (adapter->OperStatus != IfOperStatusUp) continue;            // Live connection only  

        for (adr = adapter->FirstUnicastAddress; adr != NULL; adr = adr->Next)
        {
            sockaddr = (SOCKADDR_IN*)(adr->Address.lpSockaddr);
            char    ipstr[INET6_ADDRSTRLEN] = { 0 };
            wchar_t ipwstr[INET6_ADDRSTRLEN] = { 0 };
            inet_ntop(AF_INET, &(sockaddr->sin_addr), ipstr, INET_ADDRSTRLEN);
            mbstowcs(ipwstr, ipstr, INET6_ADDRSTRLEN);
            wstring wstr(ipwstr);
            if (wstr != "0.0.0.0") ips.push_back(wstr);
        }
    }

    free(adapters);
    adapters = NULL;
}
#endif