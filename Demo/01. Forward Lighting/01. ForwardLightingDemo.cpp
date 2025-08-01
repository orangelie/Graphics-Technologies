#include "pch.h"
#include "01. ForwardLightingDemo.h"
#include "Camera.h"
#include "Input.h"
#include "PointerContainer.h"

void ForwardLightingDemo::Start()
{
    // DirectX11 Initialization
    {
        CreateD3D11DeviceAndSwapChain();
        CreateMainRTVAndDSV();
        CreateRasterizerState();
        CreateSamplerState();

        // Viewport
        {
            _viewport.MinDepth = 0.0F;
            _viewport.MaxDepth = 1.0F;
            _viewport.TopLeftX = 0;
            _viewport.TopLeftY = 0;
            _viewport.Width = WIDTH;
            _viewport.Height = HEIGHT;
        }

        // IMGUI
        {
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO();
            (void)io;

            ImGui_ImplWin32_Init(hwnd);
            ImGui_ImplDX11_Init(_device.Get(), _dc.Get());
            ImGui::StyleColorsDark();
        }
    }

    // Create Meshes
    {
        {
            //AnimationImporter ai;
            /*ai.ReadAssetFile(L"Genshin/navia.fbx");
            ai.ExportModelData(L"Genshin/NAVIA");
            ai.ExportMaterialData(L"Genshin/NAVIA");*/
            /*ai.ReadAssetFile(L"Test/test.fbx");
            ai.ExportModelData(L"Test/Test");
            ai.ExportMaterialData(L"Test/Test");*/
        }
        
        _model = make_shared<Model>();
        _model->Load(L"../Shaders/00. Empty/00. Empty.fx", L"Test/Test", L"Test/Test");

        _model->transform.scale = Vec3(0.1F);
        _model->transform.rotation = { 0.0F, 0.0F, 0.0F };
        _model->transform.position = { 0.0F, -15.0F, 5.0F };
    }
}

void ForwardLightingDemo::Update(float dt)
{
    // Camera Movement
    const float speed = 10.0F;
    if (INPUT->GetButton(KEY_TYPE::W))
        CAMERA->transform.position += GetLocalDirectionVector(-Vec3::Forward) * dt * speed;
    if (INPUT->GetButton(KEY_TYPE::S))
        CAMERA->transform.position += GetLocalDirectionVector(Vec3::Forward) * dt * speed;
    if (INPUT->GetButton(KEY_TYPE::A))
        CAMERA->transform.position += GetLocalDirectionVector(-Vec3::Right) * dt * speed;
    if (INPUT->GetButton(KEY_TYPE::D))
        CAMERA->transform.position += GetLocalDirectionVector(Vec3::Right) * dt * speed;

    const Vec2 mouseDelta = INPUT->GetMouseDelta();
    const float wheelRatio = 0.2F;

    // CAMERA ROTATION
    if (INPUT->GetButtonDown(KEY_TYPE::K))
        _freezeCamRot = !_freezeCamRot;

    if (!_freezeCamRot)
    {
        CAMERA->transform.rotation.x += mouseDelta.x * dt * wheelRatio;
        CAMERA->transform.rotation.y += mouseDelta.y * dt * wheelRatio;
        CAMERA->transform.rotation.y = clamp(CAMERA->transform.rotation.y, -XM_PIDIV4, XM_PIDIV4);
    }

    // TODO

}

void ForwardLightingDemo::Render()
{
    const float clearColor[4] = { 0.0F, 0.0F, 0.0F, 1.0F };

    _dc->ClearRenderTargetView(_rtv.Get(), clearColor);
    _dc->ClearDepthStencilView(_dsv.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0F, 0);
    _dc->OMSetRenderTargets(1, _rtv.GetAddressOf(), _dsv.Get());
    
    _dc->RSSetViewports(1, &_viewport);
    if (_rasterMode == 0)
        _dc->RSSetState(_solidRasterState.Get());
    else if (_rasterMode == 1)
        _dc->RSSetState(_wireframeRasterState.Get());

    _dc->PSSetSamplers(0, 1, _samplerState.GetAddressOf());

    // TODO
    {
        _model->Render();
    }

    // IMGUI
    {
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Help");
        {
            ImGui::Text("WASD > Move camera");
            ImGui::Text("K > Freeze/Unfreeze camera rotation");
        }
        ImGui::End();

        ImGui::Begin("Graphics Settings");
        {
            ImGui::Text("Choose Rasterizer Mode:");
            ImGui::RadioButton("Solid Mode", _rasterMode == 0); if (ImGui::IsItemClicked()) _rasterMode = 0;
            ImGui::RadioButton("Wireframe Mode", _rasterMode == 1); if (ImGui::IsItemClicked()) _rasterMode = 1;
            ImGui::Text("Selected Rasterizer Mode: %d", _rasterMode);
        }
        ImGui::End();

        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }

    _swapChain->Present(1, 0);
}

// =================================================================================================
// =================================================================================================
// =================================================================================================

void ForwardLightingDemo::Resize(uint32 width, uint32 height)
{
    if (_device != nullptr)
    {
        // SETUP
        WIDTH = width;
        HEIGHT = height;

        // RTV
        _dc->OMSetRenderTargets(0, nullptr, nullptr);
        _dc->Flush();

        _rtv.Reset();
        _dsv.Reset();
        _depthStencilTexture.Reset();

        _swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
        CreateMainRTVAndDSV();

        // VIEWPORT
        _viewport.Width = static_cast<float>(width);
        _viewport.Height = static_cast<float>(height);

        // IMGUI
        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2((float)width, (float)height);

        _dc->OMSetRenderTargets(1, _rtv.GetAddressOf(), _dsv.Get());
    }
}

void ForwardLightingDemo::CreateD3D11DeviceAndSwapChain()
{
    DXGI_SWAP_CHAIN_DESC desc = {};
    {
        desc.BufferDesc.Width = WIDTH;
        desc.BufferDesc.Height = HEIGHT;
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
       desc.Width = static_cast<uint32>(WIDTH);
       desc.Height = static_cast<uint32>(HEIGHT);
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

void ForwardLightingDemo::CreateRasterizerState()
{
    D3D11_RASTERIZER_DESC desc = {};
    desc.FillMode = D3D11_FILL_SOLID;
    desc.CullMode = D3D11_CULL_BACK;
    desc.FrontCounterClockwise = FALSE;
    desc.DepthClipEnable = TRUE;

    CHECK(_device->CreateRasterizerState(&desc, _solidRasterState.GetAddressOf()));

    desc.FillMode = D3D11_FILL_WIREFRAME;
    desc.CullMode = D3D11_CULL_BACK;
    desc.FrontCounterClockwise = FALSE;
    desc.DepthClipEnable = TRUE;

    CHECK(_device->CreateRasterizerState(&desc, _wireframeRasterState.GetAddressOf()));
}

void ForwardLightingDemo::CreateSamplerState()
{
    D3D11_SAMPLER_DESC samplerDesc = {};
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    CHECK(_device->CreateSamplerState(&samplerDesc, _samplerState.GetAddressOf()));
}

Vec3 ForwardLightingDemo::GetLocalDirectionVector(Vec3 dir)
{
    Quaternion qPitch = Quaternion::CreateFromAxisAngle(Vec3::Right, CAMERA->transform.rotation.y); // y
    Quaternion qYaw = Quaternion::CreateFromAxisAngle(Vec3::Up, CAMERA->transform.rotation.x); // x
    Quaternion rotation = qPitch * qYaw;

    Matrix rotationMatrix = Matrix::CreateFromQuaternion(rotation);
    return Vec3::Transform(dir , rotationMatrix);
}
