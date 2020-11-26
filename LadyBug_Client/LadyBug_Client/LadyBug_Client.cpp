// LadyBug_Client.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//
#if defined(DEBUG) | defined(_DEBUG)
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#endif

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "framework.h"
#include "LadyBug_Client.h"

#define MAX_LOADSTRING 100

#pragma pack(push, 1)
struct Object2
{
    short x = 0;
    short y = 0;
    int width = 30;
    int height = 30;

    bool isCollide = false;
    bool isActive = true;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct Input
{
    char UP = 0x01;
    char DOWN = 0x02;
    char LEFT = 0x04;
    char RIGHT = 0x08;
};
#pragma pack(pop)

//constexpr char UP = 0x01;
//constexpr char DOWN = 0x02;
//constexpr char LEFT = 0x04;
//constexpr char RIGHT = 0x08;
//constexpr char IDLE = 0x00;

//TODO : local ip -> network ip
const char* SERVERIP = "127.0.0.1";
constexpr int SERVERPORT = 9000;
constexpr int BUFSIZE = 512;

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

struct Object
{
    float x = 0.f;
    float y = 0.f;
    
    int width = 0;
    int height = 0;

    int picX = 0;
    int picY = 0;
    int picWidth = 0;
    int picHeight = 0;

    bool is_active = true;

    CImage image;
};

Input input;

Object BackGround;
CImage Backbuff;
CImage imageGameStart;
CImage imageGameResult;

Object Player[2];

Object Monster[10];

Object Item[2];

bool GameEnd = false;

SOCKET sock;

void GameInit();
void GameRelease();

int Recvn(SOCKET sock, char* buf, int len, int flags);
void RecvObjects();

void DrawObject(HDC memDC);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.
    
    //서버와 통신할 소켓 프로토콜을 초기화 합니다.
    int returnvalue = 0;

    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    //소켓을 생성합니다.
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET)
        return 1;/*err_quit("socket()")*/

    //통신할 서버의 주소 구조체를 설정합니다.
    SOCKADDR_IN server_sock_addr;
    ZeroMemory(&server_sock_addr, sizeof(server_sock_addr));
    server_sock_addr.sin_family = AF_INET;
    server_sock_addr.sin_addr.S_un.S_addr = inet_addr(SERVERIP);
    server_sock_addr.sin_port = ntohs(SERVERPORT);

    //서버에 연결요청을 합니다.
    returnvalue = connect(sock, (sockaddr*)&server_sock_addr, sizeof(server_sock_addr));
    if (returnvalue == SOCKET_ERROR)
        return 1;/*err_quit("connect()")*/

    char data[9];

    Object2 object;

    returnvalue = recv(sock, (char*)&object, sizeof(object), 0);

    Object2 Player[2];
    for (const auto& i : Player)
    {
        if (recv(sock, (char*)&i, sizeof(i), 0) == -1)
        {
            cout << "Send Fail\n";
        }
    }
    Object2 Monster[10];
    for (const auto& i : Monster)
    {
        if (recv(sock, (char*)&i, sizeof(i), 0) == -1)
        {
            cout << "Send Fail\n";
        }
    }
    Object2 Item[3];
    for (const auto& i : Item)
    {
        if (recv(sock, (char*)&i, sizeof(i), 0) == -1)
        {
            cout << "Send Fail\n";
        }
    }

    returnvalue = send(sock, (char*)"Ready", sizeof("Ready"), 0);


    char GameStart[10];
    recv(sock, (char*)GameStart, sizeof(GameStart), 0);

    input.UP = 1;
    input.DOWN = 2;
    input.LEFT = 4;
    input.RIGHT = 16;

   


    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LADYBUGCLIENT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LADYBUGCLIENT));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LADYBUGCLIENT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_LADYBUGCLIENT);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, L"LadyBug", WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 500, 800, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM) 
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT paint;

    InvalidateRect(hWnd, nullptr, false);

    send(sock, (char*)&input, sizeof(input), 0);
    
    input.UP = false;
    input.DOWN = false;
    input.LEFT = false;
    input.RIGHT = false;

    switch (message)
    {
    case WM_CREATE:
        GameInit();
        break;
    case WM_KEYDOWN:
        if (wParam == VK_UP)
        {
            input.UP = true;
            //send(sock, (char*)UP, sizeof(char), 0);
        }

        else if (wParam == VK_DOWN)
            input.DOWN = true;
        else if (wParam == VK_LEFT)
            input.LEFT = true;
        else if (wParam == VK_RIGHT)
            input.RIGHT = true;
     

        //switch (wParam)
        //{
        //case VK_UP:
        //    Player[0].y -= 5.f;
        //    input.UP = 1;
        //    send(sock, (char*)(0x01), sizeof(char), 0);
        //    break;
        //case VK_DOWN:
        //    Player[0].y += 5.f;
        //    input.DOWN = 1;
        //    break;
        //case VK_LEFT:
        //    Player[0].x -= 5.f;
        //    input.LEFT = 1;
        //    break;
        //case VK_RIGHT:
        //    Player[0].x += 5.f;
        //    input.RIGHT = 1;
        //    break;
        //default:
        //    break;
        //}
        
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            ////더블버퍼링
            RECT Rclient;
            GetClientRect(hWnd, &Rclient);
            HDC hdc = BeginPaint(hWnd, &paint);
            HDC memDC = Backbuff.GetDC();

            FillRect(memDC, &Rclient, (HBRUSH)(GetStockObject(WHITE_BRUSH)));
            DrawObject(memDC);

            Backbuff.Draw(hdc, 0, 0);
            Backbuff.ReleaseDC();

           
            break;         
        }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

void GameInit()
{
    BackGround.image.Load(_TEXT("Images/BackGround.bmp"));
    BackGround.x = 0;
    BackGround.y = 0;
    BackGround.width = 500;
    BackGround.height = 800;
    BackGround.picX = 0;
    BackGround.picY = 0;
    BackGround.picWidth = 500;
    BackGround.picHeight = 800;

    Player[0].image.Load(_TEXT("Images/Player1.bmp"));
    Player[0].x = 235;
    Player[0].y = 650;
    Player[0].width = 30;
    Player[0].height = 30;
    Player[0].picWidth = 76;
    Player[0].picHeight = 74;

    Player[1].image.Load(_TEXT("Images/Player2.png"));
    Player[1].x = 230;
    Player[1].y = 500;
    Player[1].width = 30;
    Player[1].height = 30;
    Player[1].picWidth = 76;
    Player[1].picHeight = 74;

    for (int i = 0; i < 3; ++i)
    {
        Monster[i].image.Load(_TEXT("Images/Monster.bmp"));
        Monster[i].x = 230;
        Monster[i].y = 100 + i*30;
        Monster[i].width = 30;
        Monster[i].height = 30;
        Monster[i].picWidth = 76;
        Monster[i].picHeight = 74;
    }
   
    //백버퍼 생성
    Backbuff.Create(500, 800, 24, 0);

    Item[0].image.Load(_TEXT(""));
    Item[1].image.Load(_TEXT(""));

    imageGameStart.Load(_TEXT(""));
    imageGameResult.Load(_TEXT("Images/Result.png"));
}

int Recvn(SOCKET sock, char* buf, int len, int flags)
{
    int received;
    char* ptr = buf;

    int left = len;

    while (left > 0)
    {
        received = recv(sock, ptr, left, flags);

        if (received == SOCKET_ERROR)
            return SOCKET_ERROR;
        else if (received == 0)
            break;

        left -= received;
        ptr += received;
    }

    return (len - left);
}

void RecvObjects()
{
    
}

void DrawObject(HDC memDC)
{
    BackGround.image.Draw(memDC, BackGround.x, BackGround.y, BackGround.width, BackGround.height, BackGround.picX, BackGround.picY, BackGround.picWidth, BackGround.picHeight);

    for (const auto& object : Player)
    {
        if (object.is_active)
        {
            object.image.Draw(memDC, object.x, object.y, object.width, object.height, object.picX, object.picY, object.picWidth, object.picHeight);
        }
    }

    for (const auto& object : Monster)
    {
        if (object.is_active)
        {
            object.image.Draw(memDC, object.x, object.y, object.width, object.height, object.picX, object.picY, object.picWidth, object.picHeight);
        }
    }

    for (const auto& object : Item)
    {
        if (object.is_active)
        {
            /*Draw*/
        }
    }
}
