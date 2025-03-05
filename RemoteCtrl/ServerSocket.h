#pragma once
#include "pch.h"
#include "framework.h"

class CPacket {
public:
    CPacket();
    CPacket(const BYTE* pData, size_t& nSize);
    CPacket(const CPacket& packet);
    CPacket& operator=(const CPacket& packet);
    ~CPacket();

public:
    WORD sHead;             // 包头 (0xFEFF)
    DWORD nLength;          // 包长度
    WORD sCommand;          // 控制命令
    std::string strData;    // 包数据
    WORD sChecksum;         // 校验值 
};

class CServerSocket {
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
    CPacket m_packet;

public:
    static CHelper m_pHelper;
    static CServerSocket* m_pInstance;
};