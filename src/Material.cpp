#include "Global.h"
#include "Material.h"

void Material::SetName(const wstring& name)
{
    _name = name;
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
