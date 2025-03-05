// RemoteCtrl.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include "framework.h"
#include "RemoteCtrl.h"
#include "ServerSocket.h"
#include <direct.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 唯一的应用程序对象

CWinApp theApp;

using namespace std;

void Dump(BYTE* pData, size_t nSize) {
    std::string strOut;
    for (size_t i = 0; i < nSize; ++i) {
        char buffer[8] = { 0 };
        if (i > 0 && (i % 16 == 0)) {
            strOut += "\n";
        }
        snprintf(buffer, sizeof(buffer), "%02X ", pData[i] & 0xFF);
        strOut += buffer;
    }
    strOut += "\n";
    OutputDebugStringA(strOut.c_str());
}

int MakeDriverInfo() {
    std::string strResult;
    // 1 = A盘，2 = B盘，3 = C盘 ....
    for (int i = 1; i <= 26; ++i) {
        if (_chdrive(i) == 0) {
            if (strResult.size() > 0) {
                strResult += ',';
            }
            strResult += 'A' + i - 1;
        }
    }

    CPacket packet(1, (BYTE*)strResult.c_str(), strResult.size());
    Dump((BYTE*)packet.Data(), packet.Size());
    
    return 0;
}


int main() {
    int nRetCode = 0;

    HMODULE hModule = ::GetModuleHandle(nullptr);

    if (hModule != nullptr) {
        // 初始化 MFC 并在失败时显示错误
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0)) {
            // TODO: 在此处为应用程序的行为编写代码。
            wprintf(L"错误: MFC 初始化失败\n");
            nRetCode = 1;
        }
        else {
            // TODO: 在此处为应用程序的行为编写代码。
            //if (!CServerSocket::GetInstance()->InitSocket()) {
            //    MessageBox(NULL, _T("网络初始化失败，请检查网络设置"), _T("网络初始化失败"), MB_OK | MB_ICONERROR);
            //    exit(0);
            //}
            //int iCount = 0;
            //while (CServerSocket::GetInstance()) {
            //    if (!CServerSocket::GetInstance()->AcceptClient()) {
            //        if (iCount == 5) {
            //            MessageBox(NULL, _T("连接客户端失败，结束程序"), _T("连接客户端失败"), MB_OK | MB_ICONERROR);
            //            exit(0);
            //        }
            //        MessageBox(NULL, _T("连接客户端失败，重新连接"), _T("连接客户端失败"), MB_OK | MB_ICONERROR);
            //        iCount++;
            //    }
            //    // TODO: 调用DealCommand处理业务逻辑
            //    // CServerSocket::GetInstance()->DealCommand()
            //}
            MakeDriverInfo();
        }
    } 
    else
    {
        // TODO: 更改错误代码以符合需要
        wprintf(L"错误: GetModuleHandle 失败\n");
        nRetCode = 1;
    }

    return nRetCode;
}
