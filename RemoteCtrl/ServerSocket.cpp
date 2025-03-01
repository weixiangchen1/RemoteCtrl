#include "pch.h"
#include "ServerSocket.h"

CServerSocket* CServerSocket::m_pInstance = nullptr;
CServerSocket::CHelper CServerSocket::m_pHelper;

CServerSocket* CServerSocket::GetInstance() {
    if (m_pInstance == nullptr) {
        m_pInstance = new CServerSocket();
    }
    return m_pInstance;
}

void CServerSocket::ReleaseInstance()
{
    if (m_pInstance != nullptr) {
        delete m_pInstance;
        m_pInstance = nullptr;
    }
}

CServerSocket::CServerSocket() {
    m_sockServer = INVALID_SOCKET;
    m_sockClient = INVALID_SOCKET;
    if (InitSockEnv() == FALSE) {
        MessageBox(NULL, _T("无法初始化套接字, 请检查网络设置"), _T("初始化错误"), MB_OK | MB_ICONERROR);
        exit(0);
    }
}

CServerSocket::~CServerSocket() {
    WSACleanup();
}

bool CServerSocket::InitSockEnv() {
    WSADATA data;
    if (WSAStartup(MAKEWORD(1, 1), &data) != 0) {
        return false;
    }
    return true;
}

bool CServerSocket::InitSocket()
{
    // 1.创建套接字
    m_sockServer = socket(AF_INET, SOCK_STREAM, 0);
    if (m_sockServer == INVALID_SOCKET) {
        return false;
    }
    // 2.绑定ip和port
    sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(38088);
    if (bind(m_sockServer, (sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
        return false;
    }
    // 3.监听
    if (listen(m_sockServer, 1) == SOCKET_ERROR) {
        return false;
    }
    return true;
}

bool CServerSocket::AcceptClient()
{
    sockaddr_in cli_addr;
    int addr_size = sizeof(sizeof(cli_addr));
    m_sockClient = accept(m_sockServer, (sockaddr*)&cli_addr, &addr_size);
    if (m_sockClient == INVALID_SOCKET) {
        return false;
    }
    return true;
}

int CServerSocket::DealCommand()
{
    if (m_sockClient == INVALID_SOCKET) {
        return false;
    }

    char buffer[1024] = { 0 };
    while (true) {
        size_t len = recv(m_sockClient, buffer, sizeof(buffer), 0);
        if (len < 0) {
            return -1;
        }
        // TODO: 其他处理逻辑
    }
}

bool CServerSocket::SendData(const char* pData, int nSize)
{
    if (m_sockClient == INVALID_SOCKET) {
        return false;
    }
    return send(m_sockClient, pData, nSize, 0) > 0;
}

CServerSocket::CHelper::CHelper()
{
    CServerSocket::GetInstance();
}

CServerSocket::CHelper::~CHelper()
{
    CServerSocket::ReleaseInstance();
}
