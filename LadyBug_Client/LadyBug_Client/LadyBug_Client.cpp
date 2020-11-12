// LadyBug_Client.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//
#if defined(DEBUG) | defined(_DEBUG)
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#endif

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "framework.h"
#include "LadyBug_Client.h"

#define MAX_LOADSTRING 100

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

    int moveX = 0;
    int moveY = 0;
    
    bool is_active = true;


    CImage image;
};

bool KeyInput[4] = { false, false, false, false };

InputFlag input;

CImage imageBackBuffer;
Object BackGround;
CImage imageGameStart;
CImage imageGameResult;

Object Player[2];

Object Monster[3];

Object Item[2];

void GameInit();
void GameRelease();

void LobbyState();
void MainGameState();
void ResultState();

void SendtoServer(const SOCKET& sock, const bool keyInput[]);
void RecvtoServer(const SOCKET& sock);

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
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET)
        return 1;/*err_quit("socket()")*/

    //통신할 서버의 주소 구조체를 설정합니다.
    SOCKADDR_IN server_sock_addr;
    ZeroMemory(&server_sock_addr, sizeof(server_sock_addr));
    server_sock_addr.sin_family = AF_INET;
    server_sock_addr.sin_addr.S_un.S_addr = inet_addr(SERVERIP);
    server_sock_addr.sin_port = ntohs(SERVERPORT);

    //서버에 연결요청을 합니다.
    //returnvalue = connect(sock, (sockaddr*)&server_sock_addr, sizeof(server_sock_addr));
    //if (returnvalue == SOCKET_ERROR)
    //    return 1;/*err_quit("connect()")*/


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
    cout << input << endl;
    HDC hDC, memDC, tempDC;
    PAINTSTRUCT paint;
    static HBITMAP hBit, oldBit;


    input.Init();
    InvalidateRect(hWnd, nullptr, true);
    switch (message)
    {
    case WM_CREATE:
        GameInit();
        break;
    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_UP:
            Player[0].y -= 5.f;
            input.UP = 1;
            break;
        case VK_DOWN:
            Player[0].y += 5.f;
            input.DOWN = 1;
            break;
        case VK_LEFT:
            Player[0].x -= 5.f;
            input.LEFT = 1;
            break;
        case VK_RIGHT:
            Player[0].x += 5.f;
            input.RIGHT = 1;
            break;
        default:
            break;
        }
        
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
            hDC = BeginPaint(hWnd, &paint);
            memDC = CreateCompatibleDC(hDC);

            hBit = CreateCompatibleBitmap(hDC, 1000, 800);
            SelectObject(memDC, hBit);
            
            DrawObject(memDC);
            
            BitBlt(hDC, 0, 0, 500, 800, memDC, 0, 0, SRCCOPY);
            DeleteObject(hBit);
            DeleteDC(memDC);
            EndPaint(hWnd, &paint);

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
    

    imageBackBuffer.Load(_TEXT(""));
    
    
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

    Monster[0].image.Load(_TEXT("Images/Monster.bmp"));
    Monster[0].x = 230;
    Monster[0].y = 100;
    Monster[0].width = 30;
    Monster[0].height = 30;
    Monster[0].picWidth = 76;
    Monster[0].picHeight = 74;

    Item[0].image.Load(_TEXT(""));
    Item[1].image.Load(_TEXT(""));

    imageGameStart.Load(_TEXT(""));
    imageGameResult.Load(_TEXT("Images/Result.png"));

    imageBackBuffer.Create(700, 480, 24, 0);

}

void GameRelease()
{

}

void LobbyState()
{

}

void MainGameState()
{
}

void MainGameState(const SOCKET& sock)
{
    SendtoServer(sock,NULL);
    RecvtoServer(sock);
}

void ResultState()
{

}

void SendtoServer(const SOCKET& sock, const bool keyInput[])
{
}

void RecvtoServer(const SOCKET& sock)
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
            //object.image.Draw(memDC, object.x, object.y, object.width, object.height, object.picX, object.picY, object.picWidth, object.picHeight);
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
