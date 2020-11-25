#define _WINSOCK_DEPRECATED_NO_WARNINGS // �ֽ� VC++ ������ �� ��� ����
#pragma comment(lib, "ws2_32")

#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <random>
#include <vector>

using namespace std;

//Server value
constexpr int BUFSIZE = 512;
constexpr int SERVERPORT = 9000;

//Thread Count
constexpr int MAX_THREAD = 10;

//window â ũ��
constexpr int WINDOW_WIDTH = 500;
constexpr int WINDOW_HEIGHT = 800;

struct Object
{
    Object(int id) :CLientID(id) {}
    Object() = default;

    float x, y;
    int width = 30;
    int height = 30;
    bool isCollide = false;
    bool isActive = true;
    int CLientID = 0;
};

//ID������ ���� ��������
default_random_engine dre;
uniform_int_distribution<>uid(1, 100);

uniform_int_distribution<>PositionX(0, 500);
uniform_int_distribution<>PositionY(0, 300);

uniform_int_distribution<>MoveX(-4, +4);
uniform_int_distribution<>MoveY(-4, +4);

int randomFirstID = 0;
int randomSecondID = 0;
char ready[2] = { 0 };
char gameStart = 1;

CRITICAL_SECTION cs; // �Ӱ� ����
vector<Object> Player;
Object Monster[10];
Object Item[2];

DWORD dwThreadID[MAX_THREAD];

void InitObjects();
void ObjectCollisionCheck();
void ObjectUpdate();
void PlayerUpdate(char buf, int ID);

void err_quit(char* msg);
void err_display(char* msg);
void sendID(SOCKET s, int len, int flags);
int sendData(SOCKET s, char* buf, int len, int flags);
int recvn(SOCKET s, char* buf, int len, int flags);

DWORD WINAPI Client_Thread(LPVOID arg);

int main(int argc, char* argv[])
{
    int retval;

    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock == INVALID_SOCKET) 
        err_quit("socket()");

    SOCKADDR_IN serveraddr;
    ZeroMemory(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(SERVERPORT);
    retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR) 
        err_quit("bind()");

    retval = listen(listen_sock, SOMAXCONN);
    if (retval == SOCKET_ERROR) 
        err_quit("listen()");

    SOCKET client_sock;
    SOCKADDR_IN client_addr;
    int client_addr_len;
    char buf[BUFSIZE + 1];

    HANDLE hThread[MAX_THREAD];
    int threadCount = 0;

    cout << "���� ������~~~" << endl;

    while (1) 
    {
        client_addr_len = sizeof(client_addr);
        client_sock = accept(listen_sock, (SOCKADDR*)&client_addr, &client_addr_len);

        if (client_sock == INVALID_SOCKET) 
        {
            err_display("accept()");
            break;
        }

        cout << "\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�= " 
            << inet_ntoa(client_addr.sin_addr)
            << ", ��Ʈ ��ȣ=" << ntohs(client_addr.sin_port) << endl;

        //������ ����
        hThread[threadCount] = CreateThread(NULL, 0, Client_Thread, (LPVOID)client_sock, 0, &dwThreadID[threadCount]);

        threadCount++;
    }
    
    for (auto& i : hThread)
        CloseHandle(i);

    // closesocket()
    closesocket(listen_sock);

    // ���� ����
    WSACleanup();
    return 0;
}

DWORD WINAPI Client_Thread(LPVOID arg) 
{
    int retval;

    SOCKET client_sock = (SOCKET)arg;
    SOCKADDR_IN client_addr;
    int client_addr_len;
    char buf[BUFSIZE + 1];

    ZeroMemory(&buf, sizeof(buf));
    getpeername(client_sock, (SOCKADDR*)&client_addr, &client_addr_len);

    //EnterCriticalSection(&cs);
    ////Id ������  - �����ڿ� ������ �ӰԿ��� �����ؾ���
    //sendID(client_sock, client_addr_len, 0);
    //LeaveCriticalSection(&cs);

    Object player;

    Player.push_back(player);

    InitObjects();

    char buffer[2];

    retval = recv(client_sock, (char*)buffer, sizeof(char), 0);
    cout << "���� �� - " << "[" << buffer << "]" << endl;

                
    while (true)
    {
        ZeroMemory(&buffer, sizeof(buffer));
        retval = recv(client_sock, (char*)buffer, sizeof(char), 0);
        cout << "���� �� - " << "[" << buffer << "]" << endl;

    }

    int clientId = uid(dre);

    //Ŭ���̾�Ʈ ���̵� �ޱ�
    //recvn(client_sock, (char*)&clientId, sizeof(clientId), 0);


    //EnterCriticalSection(&cs);
    // �غ� ���� �ޱ�
    if (clientId == randomFirstID && ready[0] == 0) {
        retval = recvn(client_sock, (char*)&ready[0], sizeof(ready[0]), 0);

    }
    else if (clientId == randomSecondID && ready[1] == 0) {


        retval = recvn(client_sock, (char*)&ready[1], sizeof(ready[1]), 0);


    }
    //LeaveCriticalSection(&cs);
    //���̻� ���� �����Ͱ� ������

    //�غ� �Ϸ�̴ٸ� ���� ���� ������
    if (ready[0] != 0 && ready[1] != 0)
        send(client_sock, (char*)&gameStart, sizeof(gameStart), 0);

    // ���� ����
    while (ready[0] != 0 && ready[1] != 0) 
    {
        //recv(buf)
        //EnterCriticalSection(&cs);
        //PlayerUpdate(buf, clientId);
        //LeaveCriticalSection(&cs);
        ObjectUpdate();
        ObjectCollisionCheck();
        //sendData()

        //if (Player.isCollide);
            //send(Gameover)

        //if (recv(CloseGame));
            //return 0;
    }

    if (retval == 0)
        return 0;


}

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

void InitObjects()
{
    for (auto i = 0; i < Player.size(); ++i)
    {
        Player.data()[i].x = 230 + (i * 5);
        Player.data()[i].y = 650;
    }

    for (auto& i : Monster)
    {
        i.x = PositionX(dre);
        i.y = PositionY(dre);
    }

    Item[0].x = 200;
    Item[0].y = 400;

    Item[1].x = 300;
    Item[1].y = 400;
}

void ObjectCollisionCheck()
{
    //TODO : �浹üũ �ϱ� ���ؼ� �̹��� ������ �˾ƾ���
    for (auto& mon : Monster)
    {
        for (auto& pla : Player)
        {
            if (mon.x + mon.width >= pla.x && mon.x <= pla.x + pla.width && mon.y + mon.height >= pla.y && mon.y <= pla.y + pla.height)
            {
                mon.isCollide = true;
                pla.isCollide = true;
            }
        }
    }
}

void ObjectUpdate()
{
    for (auto& i : Monster)
    {
        if (i.x >= 0 && i.x <= WINDOW_WIDTH && i.y > 0 && i.y <= WINDOW_HEIGHT)
        {
            i.x += MoveX(dre);
            i.y += MoveY(dre);
        }
        else
        {
            continue;// â ������ �������� ó��
        }
    }

    for (auto& i : Item)
    {
        if (i.x >= 0 && i.x <= WINDOW_WIDTH && i.y > 0 && i.y <= WINDOW_HEIGHT)
        {
            i.x += MoveX(dre);
            i.y += MoveY(dre);
        }
        else
        {
            continue;// â ������ �������� ó��
        }
    }
}

void PlayerUpdate(char buf, int ID)
{
    //TODO : ID CHECK
    for (auto& pla : Player)
    {
        if (pla.CLientID = ID)
        {
            switch (buf)
            {
            case 0x01:
                pla.y -= 5;
                break;
            case 0x02:
                pla.y += 5;
                break;
            case 0x04:
                pla.x -= 5;
                break;
            case 0x05:
                pla.y -= 5;
                pla.x -= 5;
                break;
            case 0x06:
                pla.y += 5;
                pla.x -= 5;
                break;
            case 0x08:
                pla.x += 5;
                break;
            case 0x09:
                pla.y -= 5;
                pla.x += 5;
                break;
            case 0x0a:
                pla.y += 5;
                pla.x += 5;
                break;
            }
        }
    }
}

void sendID(SOCKET s, int len, int flags) {


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

}
int sendData(SOCKET s, char* buf, int len, int flags) {

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