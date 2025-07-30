#include "pch.h"
#include "01. ForwardLightingDemo.h"
#include "PointerContainer.h"

void ForwardLightingDemo::Start()
{
    // DirectX11 Initialization
    {
        CreateD3D11DeviceAndSwapChain();
        CreateMainRTVAndDSV();

        // AlphaBlend
        // RasterizerState

        // Viewport
        {
            _viewport.MinDepth = 0.0F;
            _viewport.MaxDepth = 1.0F;
            _viewport.TopLeftX = 0;
            _viewport.TopLeftY = 0;
            _viewport.Width = width;
            _viewport.Height = height;
        }
    }

    // Create Meshes
    {
        AnimationImporter ai;
        /*ai.ReadAssetFile(L"Genshin/navia.fbx");
        ai.ExportModelData(L"Genshin/NAVIA");
        ai.ExportMaterialData(L"Genshin/NAVIA");*/

        /*ai.ReadAssetFile(L"S/S.fbx");
        ai.ExportModelData(L"S/S");
        ai.ExportMaterialData(L"S/S");*/

        ai.ReadAssetFile(L"Kachujin/Mesh.fbx");
        ai.ExportModelData(L"Kachujin/Kachujin");
        ai.ExportMaterialData(L"Kachujin/Kachujin");

        _model = make_shared<Model>();
        _model->Start();
        _model->transform.scale = { 0.01F, 0.01F, 0.01F };
        _model->transform.rotation = { 0.0F, 0.0F, 0.0F };
        _model->transform.position = { 0.0F, 0.0F, 0.0F };
    }
}

void ForwardLightingDemo::Update(float dt)
{

}

void ForwardLightingDemo::Render()
{
    const float clearColor[4] = { 0.0F, 0.0F, 0.0F, 1.0F };

    _dc->ClearRenderTargetView(_rtv.Get(), clearColor);
    _dc->ClearDepthStencilView(_dsv.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0F, 0);
    _dc->OMSetRenderTargets(1, _rtv.GetAddressOf(), _dsv.Get());
    
    _dc->RSSetViewports(1, &_viewport);

    {
        _model->Render();
    }

    _swapChain->Present(1, 0);
}

// =================================================================================================
// =================================================================================================
// =================================================================================================

void ForwardLightingDemo::CreateD3D11DeviceAndSwapChain()
{
    DXGI_SWAP_CHAIN_DESC desc = {};
    {
        desc.BufferDesc.Width = width;
        desc.BufferDesc.Height = height;
        desc.BufferDesc.RefreshRate.Numerator = 120;
        desc.BufferDesc.RefreshRate.Denominator = 1;
        desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        desc.BufferCount = 2;
        desc.OutputWindow = hwnd;
        desc.Windowed = TRUE;
        desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    }

    CHECK(::D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &desc,
        _swapChain.GetAddressOf(),
        _device.GetAddressOf(),
        nullptr,
        _dc.GetAddressOf()
    ));

    DEVICE = _device;
    DC = _dc;
}

void ForwardLightingDemo::CreateMainRTVAndDSV()
{
    {
        ComPtr<ID3D11Texture2D> _renderTarget = nullptr;

        CHECK(_swapChain->GetBuffer(0, IID_PPV_ARGS(_renderTarget.GetAddressOf())));
        CHECK(_device->CreateRenderTargetView(_renderTarget.Get(), nullptr, _rtv.GetAddressOf()));
    }

   {
       D3D11_TEXTURE2D_DESC desc = { 0 };
       ZeroMemory(&desc, sizeof(desc));
       desc.Width = static_cast<uint32>(width);
       desc.Height = static_cast<uint32>(height);
       desc.MipLevels = 1;
       desc.ArraySize = 1;
       desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
       desc.SampleDesc.Count = 1;
       desc.SampleDesc.Quality = 0;
       desc.Usage = D3D11_USAGE_DEFAULT;
       desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
       desc.CPUAccessFlags = 0;
       desc.MiscFlags = 0;

       HRESULT hr = DEVICE->CreateTexture2D(&desc, nullptr, _depthStencilTexture.GetAddressOf());
       CHECK(hr);
   }

   {
       D3D11_DEPTH_STENCIL_VIEW_DESC desc;
       ZeroMemory(&desc, sizeof(desc));
       desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
       desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
       desc.Texture2D.MipSlice = 0;

       HRESULT hr = DEVICE->CreateDepthStencilView(_depthStencilTexture.Get(), &desc, _dsv.GetAddressOf());
       CHECK(hr);
   }
}
