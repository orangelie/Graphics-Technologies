#include "Global.h"
#include "Material.h"
#include "PointerContainer.h"
#include "Resources.h"

void Material::SetName(const wstring& name)
{
    _name = name;
}

void Material::UploadTexture()
{
    if (_diffuse)
    {
        cout << _diffuse->width << endl;
        DC->PSSetShaderResources(0, 1, _diffuse->srv.GetAddressOf());
    }
}

void Material::SetDiffuseMap(shared_ptr<Texture> diffuse)
{
    _diffuse = diffuse;
}

void Material::SetSpecularMap(shared_ptr<Texture> specular)
{
    _specular = specular;
}

void Material::SetNormalMap(shared_ptr<Texture> normal)
{
    _normal = normal;
}
