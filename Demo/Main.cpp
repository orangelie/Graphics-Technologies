#include "pch.h"
#include "00. Empty/00. EmptyDemo.h"
#include "01. Forward Lighting/01. ForwardLightingDemo.h"

shared_ptr<Base> engine = make_shared<ForwardLightingDemo>();
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int w, h;
HWND hwnd;

void start(HWND hwnd, UINT width, UINT height)
{
    GET_SINGLE(PointerContainer)->application = engine;

    engine->hwnd = hwnd;
    WIDTH = width;
    HEIGHT = height;

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
    builder.Create(hInstance, L"Graphics-Technologies", 1600, 900, start, update, true);
    builder.Run();

    return 0;
}