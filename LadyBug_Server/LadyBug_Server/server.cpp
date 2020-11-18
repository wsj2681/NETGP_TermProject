#define _WINSOCK_DEPRECATED_NO_WARNINGS // 최신 VC++ 컴파일 시 경고 방지
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

//ID랜덤을 위한 변수설정
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

// 소켓 함수 오류 출력 후 종료
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

// 소켓 함수 오류 출력
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

    //송신 반환값
    int sended;
    // 버퍼
    char* ptr = (char*)&randomID;
    // 송신 길이
    int sendlen = sizeof(randomID);

    sended = send(s, ptr, sendlen, flags);

    /////////////////////////////////////////////
    // 이후 송신 클래스 생성시 작성할 부분   ////
    /////////////////////////////////////////////
    return len;

}

int sendDate(SOCKET s, char* buf, int len, int flags) {
    
    //송신 반환값
    int sended;
    // 버퍼
    char* ptr = buf;
    // 송신 길이
    int sendlen = len;

    sended = send(s, ptr, sendlen, flags);

    /////////////////////////////////////////////
    // 이후 송신 클래스 생성시 작성할 부분   ////
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
        //오류 일떄
        if (received == SOCKET_ERROR)
            return SOCKET_ERROR;
        //더이상 데이터를 받아오지 못할떄
        else if (received == 0)
            break;
        // 데이터 존재시 
        left -= received; //남은 데이터양
        ptr += received;  //읽어온 데이터
    }

    return (len - left);
}

DWORD WINAPI Client_Thread(LPVOID arg) {
    
    //소켓 함수 리턴 값
    int retval;

    //
    SOCKET client_sock;
    SOCKADDR_IN client_addr;
    int client_addr_len;
    char buf[BUFSIZE + 1];
    
    //ip주소와 포트번호 담기
    getpeername(client_sock, (SOCKADDR*)&client_addr, &client_addr_len);

    //Id 보내기  - 공유자원 임으로 임게영역 설정해야함
    sendID(client_sock, client_addr_len, 0);

    if (retval == 0)
        return 0;
}


int main(int argc, char* argv[])
{
    int retval;

    // 윈속 초기화
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

    // 데이터 통신에 사용할 변수
    SOCKET client_sock;
    SOCKADDR_IN client_addr;
    int client_addr_len;
    char buf[BUFSIZE + 1];

    HANDLE hThread;
    

    cout << "서버 구동중~~~" << endl;

    while (1) {

        client_addr_len = sizeof(client_addr);
        client_sock = accept(listen_sock, (SOCKADDR*)&client_addr, &client_addr_len);

        

        if (client_sock == INVALID_SOCKET) {
            err_display("accept()");
            break;
        }


        cout << "\n[TCP 서버] 클라이언트 접속: IP 주소= " 
            << inet_ntoa(client_addr.sin_addr) // 클라이언트 주소 네트워크 바이트 정렬
            << ", 포트 번호=" << ntohs(client_addr.sin_port) << endl; // 포트번호 네트워크바이트 정렬

        //쓰레드 생성
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

    // 윈속 종료
    WSACleanup();
    return 0;
}

