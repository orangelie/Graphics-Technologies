#pragma once

#include "../00. Base.h"

#include "AnimationImporter.h"
#include "Model.h"

class ForwardLightingDemo : public Base
{
public:
    virtual void Start() override;
    virtual void Update(float dt) override;
    virtual void Render() override;

private:
    void CreateD3D11DeviceAndSwapChain();
    void CreateMainRTVAndDSV();

private:
    ComPtr<IDXGISwapChain> _swapChain = nullptr;
    ComPtr<ID3D11Device> _device = nullptr;
    ComPtr<ID3D11DeviceContext> _dc = nullptr;

private:
    ComPtr<ID3D11RenderTargetView> _rtv = nullptr;

    ComPtr<ID3D11Texture2D> _depthStencilTexture = nullptr;
    ComPtr<ID3D11DepthStencilView> _dsv = nullptr;

    D3D11_VIEWPORT _viewport = {};

private:
    shared_ptr<Model> _model = nullptr;

};

