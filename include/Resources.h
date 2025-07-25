#pragma once

class Texture
{
public:
    ComPtr<ID3D11ShaderResourceView> srv;
    uint32 width, height;
};

class Resources
{
    DECLARE_SINGLE(Resources);

public:
    shared_ptr<Texture> Get(const wstring& name);
    shared_ptr<Texture> GetOrAddTexture(const wstring& name, const wstring& path);

private:
    map<wstring, shared_ptr<Texture>> _textures;

};

