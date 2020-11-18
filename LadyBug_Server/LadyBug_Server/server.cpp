#define _WINSOCK_DEPRECATED_NO_WARNINGS // �ֽ� VC++ ������ �� ��� ����
#pragma comment(lib, "ws2_32")

#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <random>



constexpr int BUFSIZE = 512;
constexpr int SERVERPORT = 9000;

using namespace std;

//ID������ ���� ��������
default_random_engine dre;
uniform_int_distribution<>uid(1, 100);

int randomFirstID = 0;
int randomSecondID = 0;


//Player
class Player {
    int palyerX;
    int platerY;

    int playerID;

};
//Monster
class Monster {
    int monsterX;
    int monsterY;

};
// iTem
class Item {
    int itemX;
    int itemY;

};

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

int sendID(SOCKET s, int len, int flags) {

    
    int randomID = uid(dre);
    if (randomFirstID == 0)
        randomFirstID = randomID;
    else if (randomSecondID == 0)
        randomSecondID = randomID;

    //�۽� ��ȯ��
    int sended;
    // ����
    char* ptr = (char*)&randomID;
    // �۽� ����
    int sendlen = sizeof(randomID);

    sended = send(s, ptr, sendlen, flags);

    /////////////////////////////////////////////
    // ���� �۽� Ŭ���� ������ �ۼ��� �κ�   ////
    /////////////////////////////////////////////
    return len;

}

int sendDate(SOCKET s, char* buf, int len, int flags) {
    
    //�۽� ��ȯ��
    int sended;
    // ����
    char* ptr = buf;
    // �۽� ����
    int sendlen = len;

    sended = send(s, ptr, sendlen, flags);

    /////////////////////////////////////////////
    // ���� �۽� Ŭ���� ������ �ۼ��� �κ�   ////
    /////////////////////////////////////////////
    return len;
}

int recvn(SOCKET s, char* buf, int len, int flags)
{
    int received;
    char* ptr = buf;
    int left = len;

    while (left > 0) {
        received = recv(s, ptr, left, flags);
        //���� �ϋ�
        if (received == SOCKET_ERROR)
            return SOCKET_ERROR;
        //���̻� �����͸� �޾ƿ��� ���ҋ�
        else if (received == 0)
            break;
        // ������ ����� 
        left -= received; //���� �����;�
        ptr += received;  //�о�� ������
    }

    return (len - left);
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

    //Id ������  - �����ڿ� ������ �ӰԿ��� �����ؾ���
    sendID(client_sock, client_addr_len, 0);

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

        client_addr_len = sizeof(client_addr);
        client_sock = accept(listen_sock, (SOCKADDR*)&client_addr, &client_addr_len);

        

        if (client_sock == INVALID_SOCKET) {
            err_display("accept()");
            break;
        }


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

