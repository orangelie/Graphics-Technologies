#include "Global.h"
#include "Resources.h"
#include "PointerContainer.h"

shared_ptr<Texture> Resources::Get(const wstring& name)
{
    auto it = _textures.find(name);
    if (it != _textures.end())
        return it->second;

    return nullptr;
}

shared_ptr<Texture> Resources::GetOrAddTexture(const wstring& name, const wstring& path)
{
    auto it = _textures.find(name);
    if (it != _textures.end())
        return it->second;

    shared_ptr<Texture> texture = make_shared<Texture>();

    DirectX::TexMetadata md;
    DirectX::ScratchImage img;

    HRESULT hr = ::LoadFromWICFile(path.c_str(), WIC_FLAGS_NONE, &md, img);
    CHECK(hr);

    hr = ::CreateShaderResourceView(DEVICE.Get(), img.GetImages(), img.GetImageCount(), md, texture->srv.GetAddressOf());
    CHECK(hr);

    texture->width = md.width;
    texture->height = md.height;

    _textures[name] = texture;

    return texture;
}
