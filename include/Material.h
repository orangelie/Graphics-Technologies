#pragma once

class Texture;

struct MaterialDesc
{
    Color ambient = Color(0.f, 0.f, 0.f, 1.f);
    Color diffuse = Color(1.f, 1.f, 1.f, 1.f);
    Color specular = Color(0.f, 0.f, 0.f, 1.f);
    Color emissive = Color(0.f, 0.f, 0.f, 1.f);
};

class Material
{
public:
    void SetName(const wstring& name);
    wstring GetName() const { return _name; }

    void UploadTexture();

public:
    void SetDiffuseMap(shared_ptr<Texture> diffuse);
    void SetSpecularMap(shared_ptr<Texture> specular);
    void SetNormalMap(shared_ptr<Texture> normal);

    MaterialDesc& GetMaterialDesc() { return _mat; }

public:
    wstring _name;

    shared_ptr<Texture> _diffuse = nullptr;
    shared_ptr<Texture> _specular = nullptr;
    shared_ptr<Texture> _normal = nullptr;

    MaterialDesc _mat = {};

};

