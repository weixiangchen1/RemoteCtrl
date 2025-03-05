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

void CServerSocket::ReleaseInstance() {
    if (m_pInstance != nullptr) {
        delete m_pInstance;
        m_pInstance = nullptr;
    }
}

CServerSocket::CServerSocket() {
    m_sockServer = INVALID_SOCKET;
    m_sockClient = INVALID_SOCKET;
    if (InitSockEnv() == FALSE) {
        MessageBox(NULL, _T("�޷���ʼ���׽���, ������������"), _T("��ʼ������"), MB_OK | MB_ICONERROR);
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

bool CServerSocket::InitSocket() {
    // 1.�����׽���
    m_sockServer = socket(AF_INET, SOCK_STREAM, 0);
    if (m_sockServer == INVALID_SOCKET) {
        return false;
    }
    // 2.��ip��port
    sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(38088);
    if (bind(m_sockServer, (sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
        return false;
    }
    // 3.����
    if (listen(m_sockServer, 1) == SOCKET_ERROR) {
        return false;
    }
    return true;
}

bool CServerSocket::AcceptClient() {
    sockaddr_in cli_addr;
    int addr_size = sizeof(sizeof(cli_addr));
    m_sockClient = accept(m_sockServer, (sockaddr*)&cli_addr, &addr_size);
    if (m_sockClient == INVALID_SOCKET) {
        return false;
    }
    return true;
}

#define BUFFER_SIZE 4096
int CServerSocket::DealCommand() {
    if (m_sockClient == INVALID_SOCKET) {
        return -1;
    }

    char buffer[BUFFER_SIZE] = { 0 };
    memset(buffer, 0, BUFFER_SIZE); 
    size_t index = 0;
    while (true) {
        size_t len = recv(m_sockClient, buffer + index, BUFFER_SIZE - index, 0);
        if (len < 0) {
            return -2;
        }
        index += len;
        len = index;
        m_packet = CPacket((BYTE*)buffer, len);
        if (len > 0) {
            memmove(buffer, buffer + len, BUFFER_SIZE - len);
            index -= len;
            return m_packet.sCommand;
        }
    }
    return -3;
}

bool CServerSocket::SendData(const char* pData, int nSize) {
    if (m_sockClient == INVALID_SOCKET) {
        return false;
    }
    return send(m_sockClient, pData, nSize, 0) > 0;
}

CServerSocket::CHelper::CHelper() {
    CServerSocket::GetInstance();
}

CServerSocket::CHelper::~CHelper() {
    CServerSocket::ReleaseInstance();
}

CPacket::CPacket()
    :sHead(0), nLength(0), sCommand(0), sChecksum(0) {
}

CPacket::CPacket(const BYTE* pData, size_t& nSize) {
    // Packet: [ head | length | command | data | checksum ]
    // Size:       2       4        2   length-2-2    2
    // length = commandSize + data.size() + checkSize
    size_t headSize = 2, lengthSize = 4, commandSize = 2, checkSize = 2;
    size_t i = 0;

    // Ѱ�����ݰ�ͷ(0xFEFF)
    for (; i < nSize; ++i) {
        if (*(WORD*)(pData + i) == 0xFEFF) {
            sHead = *(WORD*)(pData + i);
            i += headSize;
            break;
        }
    }

    // ���ݰ�����С�ڳ�data��ĳ���(head+length+command+checksum) ���ݰ�������
    if (i + lengthSize + commandSize + checkSize > nSize) {
        nSize = 0;
        return;
    }

    // ȡ�����ݳ���
    nLength = *(DWORD*)(pData + i);
    i += lengthSize;
    // ���ݰ�δ��ȫ���յ�
    // nLength = commandSize + data.size() + checkSize
    if (nLength + i > nSize) {
        nSize = 0;
        return;
    }

    // ȡ��ָ������
    sCommand = *(WORD*)(pData + i);
    i += commandSize;

    // ȡ�����ݰ�����������
    // data.size() = nLength - commandSize - checkSize
    size_t dataSize = nLength - commandSize - checkSize;
    if (dataSize > 0) {
        strData.resize(dataSize);
        memcpy((void*)strData.c_str(), pData + i, dataSize);
        i += dataSize;
    }

    // ȡ��У��λ������У��λ�Ƿ���ȷ
    sChecksum = *(WORD*)(pData + i);
    i += checkSize;
    WORD checkValue = 0;
    for (size_t j = 0; j < strData.size(); ++j) {
        checkValue += BYTE(strData[i]) & 0xFF;
    }
    if (checkValue == sChecksum) {
        nSize = i;
        return;
    }
    nSize = 0;
}

CPacket::CPacket(const CPacket& packet) {
    sHead = packet.sHead;
    nLength = packet.nLength;
    sCommand = packet.sCommand;
    strData = packet.strData;
    sChecksum = packet.sChecksum;
}

CPacket& CPacket::operator=(const CPacket& packet)
{
    if (this != &packet) {
        sHead = packet.sHead;
        nLength = packet.nLength;
        sCommand = packet.sCommand;
        strData = packet.strData;
        sChecksum = packet.sChecksum;
    }
    return *this;
}


CPacket::~CPacket() {
}

