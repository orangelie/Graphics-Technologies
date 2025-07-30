#pragma once

class PointerContainer
{
    DECLARE_SINGLE(PointerContainer);

public:
    ComPtr<ID3D11Device> device = nullptr;
    ComPtr<ID3D11DeviceContext> dc = nullptr;

    uint32 width, height;

};
