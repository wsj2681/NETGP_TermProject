#define _WINSOCK_DEPRECATED_NO_WARNINGS // �ֽ� VC++ ������ �� ��� ����
#pragma comment(lib, "ws2_32")

#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>



constexpr int BUFSIZE = 512;
constexpr int SERVERPORT = 9000;

using namespace std;



// ���� �Լ� ���� ��� �� ����
void err_quit(char* msg)
{
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);
    MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
    LocalFree(lpMsgBuf);
    exit(1);
}

// ���� �Լ� ���� ���
void err_display(char* msg)
{
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);
    printf("[%s] %s", msg, (char*)lpMsgBuf);
    LocalFree(lpMsgBuf);
}

DWORD WINAPI Client_Thread(LPVOID arg) {
    
    //���� �Լ� ���� ��
    int retval;

    //
    SOCKET client_sock;
    SOCKADDR_IN client_addr;
    int client_addr_len;
    char buf[BUFSIZE + 1];
    
    //ip�ּҿ� ��Ʈ��ȣ ���
    getpeername(client_sock, (SOCKADDR*)&client_addr, &client_addr_len);


    if (retval == 0)
        return 0;
}


int main(int argc, char* argv[])
{
    int retval;

    // ���� �ʱ�ȭ
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    // socket()
    SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock == INVALID_SOCKET) err_quit("socket()");

    // bind()
    SOCKADDR_IN serveraddr;
    ZeroMemory(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(SERVERPORT);
    retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR) err_quit("bind()");

    // listen()
    retval = listen(listen_sock, SOMAXCONN);
    if (retval == SOCKET_ERROR) err_quit("listen()");

    // ������ ��ſ� ����� ����
    SOCKET client_sock;
    SOCKADDR_IN client_addr;
    int client_addr_len;
    char buf[BUFSIZE + 1];

    HANDLE hThread;
    

    cout << "���� ������~~~" << endl;

    while (1) {




        cout << "\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�= " 
            << inet_ntoa(client_addr.sin_addr) // Ŭ���̾�Ʈ �ּ� ��Ʈ��ũ ����Ʈ ����
            << ", ��Ʈ ��ȣ=" << ntohs(client_addr.sin_port) << endl; // ��Ʈ��ȣ ��Ʈ��ũ����Ʈ ����

        //������ ����
        hThread = CreateThread(NULL, 0, Client_Thread, (LPVOID)client_sock, 0, NULL);
        if (hThread == NULL) {
            closesocket(client_sock);
        }
        else {
            CloseHandle(hThread);
        }

    }
    

    // closesocket()
    closesocket(listen_sock);

    // ���� ����
    WSACleanup();
    return 0;
}

