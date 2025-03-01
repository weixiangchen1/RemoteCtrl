#pragma once
#include "pch.h"
#include "framework.h"

class CServerSocket
{
public:
    static CServerSocket* GetInstance();
    static void ReleaseInstance();
    bool InitSockEnv();
    bool InitSocket();
    bool AcceptClient();
    int DealCommand();
    bool SendData(const char* pData, int nSize);

private:
    CServerSocket();
    ~CServerSocket();
    CServerSocket(const CServerSocket&) = delete;
    CServerSocket& operator=(const CServerSocket&) = delete;

    class CHelper {
    public:
        CHelper();
        ~CHelper();
    };
    SOCKET m_sockServer;
    SOCKET m_sockClient;

public:
    static CHelper m_pHelper;
    static CServerSocket* m_pInstance;
};