#pragma once

class Base;

class PointerContainer
{
    DECLARE_SINGLE(PointerContainer);

public:
    ComPtr<ID3D11Device> device = nullptr;
    ComPtr<ID3D11DeviceContext> dc = nullptr;

    shared_ptr<Base> application = nullptr;

    uint32 width = 0;
    uint32 height = 0;

};
