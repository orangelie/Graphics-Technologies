#pragma once

#include "Base.h"

#include "AnimationImporter.h"
#include "Model.h"

class EmptyDemo : public Base
{
public:
    virtual void Start() override;
    virtual void Update(float dt) override;
    virtual void Render() override;

    virtual void Resize(uint32 width, uint32 height) override;

private:
    void CreateD3D11DeviceAndSwapChain();
    void CreateMainRTVAndDSV();
    void CreateRasterizerState();
    void CreateSamplerState();

private:
    Vec3 GetLocalDirectionVector(Vec3 dir);

private:
    ComPtr<IDXGISwapChain> _swapChain = nullptr;
    ComPtr<ID3D11Device> _device = nullptr;
    ComPtr<ID3D11DeviceContext> _dc = nullptr;

private:
    // SAMPLER STATE
    ComPtr<ID3D11SamplerState> _samplerState = nullptr;

    // RASTERERIZER STATE
    ComPtr<ID3D11RasterizerState> _solidRasterState = nullptr;
    ComPtr<ID3D11RasterizerState> _wireframeRasterState = nullptr;
    int _rasterMode = 0;

    // RTV & DSV
    ComPtr<ID3D11RenderTargetView> _rtv = nullptr;
    ComPtr<ID3D11Texture2D> _depthStencilTexture = nullptr;
    ComPtr<ID3D11DepthStencilView> _dsv = nullptr;

    // VIEWPORT
    D3D11_VIEWPORT _viewport = {};

    // d
    bool _freezeCamRot = false;

private:
    // CUSTOM
    shared_ptr<Model> _model = nullptr;

};

