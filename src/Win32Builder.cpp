#include "Global.h"
#include "Win32Builder.h"
#include "Timer.h"
#include "Input.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void Win32Builder::Create(HINSTANCE hInst, const wstring& title, int width, int height, function<void(HWND hwnd, UINT width, UINT height)> start, function<void(float dt)> update, bool use_console)
{
    if (use_console)
        CreateConsole();

    _hInst = hInst;

    WNDCLASSEXW wcex = {};

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInst;
    wcex.hIcon = 0;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = 0;
    wcex.lpszMenuName = 0;
    wcex.lpszClassName = title.c_str();
    wcex.hIconSm = 0;

    RegisterClassExW(&wcex);

    RECT rc = { 0, 0, width, height };
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

    _hWnd = CreateWindowW(title.c_str(), title.c_str(), WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rc.right - rc.left, rc.bottom - rc.top,
        nullptr, nullptr, hInst, nullptr);
    assert(_hWnd);

    ShowWindow(_hWnd, SW_SHOW);
    UpdateWindow(_hWnd);

    INPUT->Init(_hWnd);
    TIME->Init();

    _start = start;
    _update = update;

    _start(_hWnd, rc.right - rc.left, rc.bottom - rc.top);
}

void Win32Builder::Run()
{
    MSG msg = {};

    for (;;)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (msg.message == WM_QUIT)
            break;

        INPUT->Update();
        TIME->Update();

        _update(TIME->GetDeltaTime());
    }
}

void Win32Builder::CreateConsole()
{
    AllocConsole();

    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);   // printf, std::cout
    freopen_s(&fp, "CONOUT$", "w", stderr);   // std::cerr
    freopen_s(&fp, "CONIN$", "r", stdin);     // std::cin

    SetConsoleOutputCP(CP_UTF8); // 콘솔 출력 인코딩을 UTF-8로 설정
}

//void DestroyConsole()
//{
//    FreeConsole();  // 콘솔 해제
//}

LRESULT CALLBACK Win32Builder::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
        return true;

    switch (message)
    {
    case WM_SIZE:
        if (wParam != SIZE_MINIMIZED)
        {
            
        }

        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}
