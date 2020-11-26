#define _WINSOCK_DEPRECATED_NO_WARNINGS

#if defined(DEBUG) | defined(_DEBUG)
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#endif

#include "framework.h"
#include "Project_Client.h"

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

Input input;
Object_Info BackGround;
CImage Backbuff;
CImage imageGameStart;
CImage imageGameResult;

// 그리기 전용 오브젝트
Object_Info Player[2];
Object_Info Monster[10];
Object_Info Item[3];

// 데이터 받기 전용 오브젝트
Object_Data PlayerData[2];
Object_Data MonsterData[10];
Object_Data ItemData[3];


// 서버 소켓
SOCKET sock;

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void GameInit();
void GameRelease();
void RecvObjects();
void DrawObjects(HDC memDC);

void err_quit(const char* msg);
void err_display(const char* msg);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    int retval = 0;

    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET)
        err_quit("socket()");

    SOCKADDR_IN server_sock_addr;
    ZeroMemory(&server_sock_addr, sizeof(server_sock_addr));
    server_sock_addr.sin_family = AF_INET;
    server_sock_addr.sin_addr.S_un.S_addr = inet_addr(SERVERIP);
    server_sock_addr.sin_port = ntohs(SERVERPORT);

    retval = connect(sock, (sockaddr*)&server_sock_addr, sizeof(server_sock_addr));
    if (retval == SOCKET_ERROR)
        err_quit("connect()");

    GameInit();

    retval = send(sock, (char*)"Ready", sizeof("Ready"), 0);

    char GameStart[10];
    recv(sock, (char*)GameStart, sizeof(GameStart), 0);

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_PROJECTCLIENT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PROJECTCLIENT));

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

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PROJECTCLIENT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_PROJECTCLIENT);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, L"LadyBug", WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, WINDOWWIDTH, WINDOWHEIGHT, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT paint;

    InvalidateRect(hWnd, nullptr, false);

    send(sock, (char*)&input, sizeof(input), 0);

    input.UP = true;
    input.DOWN = false;
    input.LEFT = true;
    input.RIGHT = false;

    RecvObjects();

    switch (message)
    {
    case WM_CREATE:
        break;

    case WM_KEYDOWN:
        if (wParam == VK_UP)
        {
            input.UP = true;
        }
        break;

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
            RECT Rclient;
            GetClientRect(hWnd, &Rclient);
            HDC hdc = BeginPaint(hWnd, &paint);
            HDC memDC = Backbuff.GetDC();

            FillRect(memDC, &Rclient, (HBRUSH)(GetStockObject(WHITE_BRUSH)));
            DrawObjects(memDC);

            Backbuff.Draw(hdc, 0, 0);
            Backbuff.ReleaseDC();
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

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
    // Init Player
    for (const auto& i : PlayerData)
    {
        if (recv(sock, (char*)&i, sizeof(i), 0) == -1)
        {
            cout << "Send Fail\n";
        }
    }

    for (int i = 0; i < 2; ++i)
    {
        Player[i] = PlayerData[i];
        Player[i].picWidth = 76;
        Player[i].picHeight = 74;
    }

    Player[0].image.Load(_TEXT("Images/Player1.bmp"));
    Player[1].image.Load(_TEXT("Images/Player2.png"));

    // Init Monster
    for (const auto& i : MonsterData)
    {
        if (recv(sock, (char*)&i, sizeof(i), 0) == -1)
        {
            cout << "Send Fail\n";
        }
    }

    for (int i = 0; i < 10; ++i)
    {
        Monster[i] = MonsterData[i];
        Monster[i].picWidth = 76;
        Monster[i].picHeight = 74;
        Monster[i].image.Load(_TEXT("Images/Monster.bmp"));
    }

    // Init Item
    for (const auto& i : ItemData)
    {
        if (recv(sock, (char*)&i, sizeof(i), 0) == -1)
        {
            cout << "Send Fail\n";
        }
    }

    for (int i = 0; i < 3; ++i)
    {
        Item[i] = ItemData[i];
        Item[i].picWidth = 76;
        Item[i].picHeight = 74;
    }
    Item[0].image.Load(_TEXT(""));
    Item[1].image.Load(_TEXT(""));

    imageGameStart.Load(_TEXT(""));
    imageGameResult.Load(_TEXT("Images/Result.png"));
}

void GameRelease()
{
}

void RecvObjects()
{
    //Recv Player
    for (const auto& i : PlayerData)
    {
        if (recv(sock, (char*)&i, sizeof(i), 0) == -1)
        {
            cout << "Recv Fail\n";
        }
    }

    for (int i = 0; i < 2; ++i)
    {
        Player[i] = PlayerData[i];
    }

    //Recv Monster
    for (const auto& i : MonsterData)
    {
        if (recv(sock, (char*)&i, sizeof(i), 0) == -1)
        {
            cout << "Recv Fail\n";
        }
    }

    for (int i = 0; i < 10; ++i)
    {
        Monster[i] = MonsterData[i];
    }

    //Recv Item
    for (const auto& i : ItemData)
    {
        if (recv(sock, (char*)&i, sizeof(i), 0) == -1)
        {
            cout << "Recv Fail\n";
        }
    }

    for (int i = 0; i < 3; ++i)
    {
        Item[i] = ItemData[i];
    }
}

void DrawObjects(HDC memDC)
{
    BackGround.image.Draw(memDC, BackGround.x, BackGround.y, BackGround.width, BackGround.height, BackGround.picX, BackGround.picY, BackGround.picWidth, BackGround.picHeight);

    for (const auto& object : Player)
    {
        if (object.isActive)
        {
            object.image.Draw(memDC, object.x, object.y, object.width, object.height, object.picX, object.picY, object.picWidth, object.picHeight);
        }
    }

    for (const auto& object : Monster)
    {
        if (object.isActive)
        {
            object.image.Draw(memDC, object.x, object.y, object.width, object.height, object.picX, object.picY, object.picWidth, object.picHeight);
        }
    }

    for (const auto& object : Item)
    {
        if (object.isActive)
        {
            /*Draw*/
        }
    }
}

void err_quit(const char* msg)
{
}

void err_display(const char* msg)
{
}
