#pragma once

class Win32Builder
{
public:
    void Create(HINSTANCE hInst, const wstring& title, int width, int height, function<void(HWND hwnd, UINT width, UINT height)> start, function<void(float dt)> update, bool use_console = true);
    void Run();

private:
    void CreateConsole();

private:
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
    function<void(HWND hwnd, UINT width, UINT height)> _start = nullptr;
    function<void(float dt)> _update = nullptr;
    HWND _hWnd = nullptr;
    HINSTANCE _hInst = nullptr;

};
