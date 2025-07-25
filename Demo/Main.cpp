#include "pch.h"
#include "01. Forward Lighting/01. ForwardLightingDemo.h"

shared_ptr<Base> engine = make_shared<ForwardLightingDemo>();
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int w, h;
HWND hwnd;

void start(HWND hwnd, UINT width, UINT height)
{
    engine->hwnd = hwnd;
    engine->width = width;
    engine->height = height;

    engine->Start();
}

void update(float dt)
{
    engine->Update(dt);
    engine->Render();
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    Win32Builder builder = {};
    builder.Create(hInstance, L"Graphics-Technologies", 920, 860, start, update, true);
    builder.Run();

    return 0;
}