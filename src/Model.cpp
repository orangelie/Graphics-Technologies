#include "Global.h"
#include "Model.h"
#include "Material.h"
#include "Resources.h"
#include "PointerContainer.h"

RawModel::RawModel()
{
}

RawModel::~RawModel()
{
}

void RawModel::ReadMaterial(wstring filename)
{
    wstring fullPath = _texturePath + filename + L".xml";
    auto parentPath = filesystem::path(fullPath).parent_path();

    tinyxml2::XMLDocument* document = new tinyxml2::XMLDocument();
    tinyxml2::XMLError error = document->LoadFile(ToString(fullPath).c_str());
    assert(error == tinyxml2::XML_SUCCESS);

    tinyxml2::XMLElement* root = document->FirstChildElement();
    tinyxml2::XMLElement* materialNode = root->FirstChildElement();

    while (materialNode)
    {
        shared_ptr<Material> material = make_shared<Material>();

        tinyxml2::XMLElement* node = nullptr;

        node = materialNode->FirstChildElement();
        material->SetName(ToWString(node->GetText()));

        // Diffuse Texture
        node = node->NextSiblingElement();
        if (node->GetText())
        {
            wstring textureStr = ToWString(node->GetText());
            if (textureStr.length() > 0)
            {
                auto texture = RESOURCES->GetOrAddTexture(textureStr, (parentPath / textureStr).wstring());
                material->SetDiffuseMap(texture);
            }
        }

        // Specular Texture
        node = node->NextSiblingElement();
        if (node->GetText())
        {
            wstring texture = ToWString(node->GetText());
            if (texture.length() > 0)
            {
                wstring textureStr = ToWString(node->GetText());
                if (textureStr.length() > 0)
                {
                    auto texture = RESOURCES->GetOrAddTexture(textureStr, (parentPath / textureStr).wstring());
                    material->SetSpecularMap(texture);
                }
            }
        }

        // Normal Texture
        node = node->NextSiblingElement();
        if (node->GetText())
        {
            wstring textureStr =ToWString(node->GetText());
            if (textureStr.length() > 0)
            {
                auto texture = RESOURCES->GetOrAddTexture(textureStr, (parentPath / textureStr).wstring());
                material->SetNormalMap(texture);
            }
        }

        // Ambient
        {
            node = node->NextSiblingElement();

            Color color;
            color.x = node->FloatAttribute("R");
            color.y = node->FloatAttribute("G");
            color.z = node->FloatAttribute("B");
            color.w = node->FloatAttribute("A");
            material->GetMaterialDesc().ambient = color;
        }

        // Diffuse
        {
            node = node->NextSiblingElement();

            Color color;
            color.x = node->FloatAttribute("R");
            color.y = node->FloatAttribute("G");
            color.z = node->FloatAttribute("B");
            color.w = node->FloatAttribute("A");
            material->GetMaterialDesc().diffuse = color;
        }

        // Specular
        {
            node = node->NextSiblingElement();

            Color color;
            color.x = node->FloatAttribute("R");
            color.y = node->FloatAttribute("G");
            color.z = node->FloatAttribute("B");
            color.w = node->FloatAttribute("A");
            material->GetMaterialDesc().specular = color;
        }

        // Emissive
        {
            node = node->NextSiblingElement();

            Color color;
            color.x = node->FloatAttribute("R");
            color.y = node->FloatAttribute("G");
            color.z = node->FloatAttribute("B");
            color.w = node->FloatAttribute("A");
            material->GetMaterialDesc().emissive = color;
        }

        _materials.push_back(material);

        // Next Material
        materialNode = materialNode->NextSiblingElement();
    }

    BindCacheInfo();
}

void RawModel::ReadModel(wstring filename)
{
    wstring fullPath = _modelPath + filename + L".mesh";

    shared_ptr<FileUtils> file = make_shared<FileUtils>();
    file->Open(fullPath, FileMode::Read);

    // Bones
    {
        const uint32 count = file->Read<uint32>();

        for (uint32 i = 0; i < count; i++)
        {
            shared_ptr<ModelBone> bone = make_shared<ModelBone>();
            bone->index = file->Read<int32>();
            bone->name = ToWString(file->Read<string>());
            bone->parentIndex = file->Read<int32>();
            bone->transform = file->Read<Matrix>();

            _bones.push_back(bone);
        }
    }

    // Mesh
    {
        const uint32 count = file->Read<uint32>();

        for (uint32 i = 0; i < count; i++)
        {
            shared_ptr<ModelMesh> mesh = make_shared<ModelMesh>();

            mesh->name = ToWString(file->Read<string>());
            mesh->boneIndex = file->Read<int32>();

            // Material
            mesh->materialName = ToWString(file->Read<string>());

            //VertexData
            {
                const uint32 count = file->Read<uint32>();
                vector<VertexType> vertices;
                vertices.resize(count);

                void* data = vertices.data();
                file->Read(&data, sizeof(VertexType) * count);
                mesh->geometry->AddVertices(vertices);
            }

            //IndexData
            {
                const uint32 count = file->Read<uint32>();

                vector<uint32> indices;
                indices.resize(count);

                void* data = indices.data();
                file->Read(&data, sizeof(uint32) * count);
                mesh->geometry->AddIndices(indices);
            }

            mesh->CreateBuffers(mesh->geometry->vertices.size(), mesh->geometry->indices.size());

            _meshes.push_back(mesh);
        }
    }

    BindCacheInfo();
}

shared_ptr<Material> RawModel::GetMaterialByName(const wstring& name)
{
    for (auto& material : _materials)
    {
        if (material->GetName() == name)
            return material;
    }

    return nullptr;
}

shared_ptr<ModelBone> RawModel::GetBoneByIndex(uint32 index)
{
    return (index < 0 || index >= _bones.size() ? nullptr : _bones[index]);
}

void RawModel::BindCacheInfo()
{
    // Mesh에 Material 캐싱
    for (const auto& mesh : _meshes)
    {
        // 이미 찾았으면 스킵
        if (mesh->material != nullptr)
            continue;

        mesh->material = GetMaterialByName(mesh->materialName);
    }

    // Mesh에 Bone 캐싱
    for (const auto& mesh : _meshes)
    {
        // 이미 찾았으면 스킵
        if (mesh->bone != nullptr)
            continue;

        mesh->bone = GetBoneByIndex(mesh->boneIndex);
    }

    // Bone 계층 정보 채우기
    if (_root == nullptr && _bones.size() > 0)
    {
        _root = _bones[0];

        for (const auto& bone : _bones)
        {
            if (bone->parentIndex >= 0)
            {
                bone->parent = _bones[bone->parentIndex];
                bone->parent->children.push_back(bone);
            }
            else
            {
                bone->parent = nullptr;
            }
        }
    }
}

void ModelMesh::CreateBuffers(uint32 vCount, uint32 iCount)
{
    D3D11_BUFFER_DESC desc = {};

    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    desc.ByteWidth = sizeof(VertexType) * vCount;
    desc.Usage = D3D11_USAGE_IMMUTABLE;

    D3D11_SUBRESOURCE_DATA data = {};
    data.pSysMem = geometry->vertices.data();

    CHECK(DEVICE->CreateBuffer(&desc, &data, geometry->vertexBuffer.GetAddressOf()));

    _stride = sizeof(VertexType);
    _offset = 0;

    desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    desc.ByteWidth = sizeof(uint32) * iCount;
    desc.Usage = D3D11_USAGE_IMMUTABLE;

    data.pSysMem = geometry->indices.data();

    CHECK(DEVICE->CreateBuffer(&desc, &data, geometry->indexBuffer.GetAddressOf()));
}

void ModelVertexType::AddVertices(vector<VertexType> vertices)
{
    this->vertices.insert(this->vertices.end(), vertices.begin(), vertices.end());
}

void ModelVertexType::AddIndices(vector<uint32> indices)
{
    this->indices.insert(this->indices.end(), indices.begin(), indices.end());
}

void Model::Start()
{
    // Model
    _model = make_shared<RawModel>();
    /*_model->ReadModel(L"Genshin/NAVIA");
    _model->ReadMaterial(L"Genshin/NAVIA");*/
    _model->ReadModel(L"Kachujin/Kachujin");
    _model->ReadMaterial(L"Kachujin/Kachujin");

    CreateShader();

    // Material
    /*const auto& materials = _rawModel->GetMaterials();
    for (auto& material : materials)
    {
        material->SetShader(_shader);
    }*/

    // CB
    _transformCB = make_shared<ConstantBuffer<TransformDesc>>();
    _transformCB->Create();

    _boneCB = make_shared<ConstantBuffer<BoneDesc>>();
    _boneCB->Create();

    _scalarCB = make_shared<ConstantBuffer<ScalarDesc>>();
    _scalarCB->Create();
}

void Model::Render()
{
    if (_model == nullptr)
        return;

    UpdateShader();

    // Bones
    BoneDesc boneDesc;

    const uint32 boneCount = _model->GetBoneCount();
    for (uint32 i = 0; i < boneCount; i++)
    {
        shared_ptr<ModelBone> bone = _model->GetBoneByIndex(i);
        boneDesc.transforms[i] = bone->transform;
    }
    
    // _boneCB->PushBoneData(boneDesc);
    _boneCB->CopyData(boneDesc);
    DC->VSSetConstantBuffers(1, 1, _boneCB->GetComPtr().GetAddressOf());

    // Transform
    UploadTransformData();

    const auto& meshes = _model->GetMeshes();
    for (auto& mesh : meshes)
    {
        if (mesh->material)
            mesh->material->UploadTexture();

        {
            // BoneIndex
            ScalarDesc desc = {};
            desc.BoneIndex = mesh->boneIndex;
            _scalarCB->CopyData(desc);
            DC->VSSetConstantBuffers(2, 1, _scalarCB->GetComPtr().GetAddressOf());
        }

        uint32 stride = mesh->GetStride();
        uint32 offset = mesh->GetOffset();

        DC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        DC->IASetVertexBuffers(0, 1, mesh->geometry->vertexBuffer.GetAddressOf(), &stride, &offset);
        DC->IASetIndexBuffer(mesh->geometry->indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

        DC->DrawIndexedInstanced(mesh->geometry->indices.size(), 1, 0, 0, 0);
    }
}

void Model::UploadTransformData()
{
    TransformDesc desc = {};
    {
        Matrix S, R, T;
        S = Matrix::CreateScale(transform.scale.x, transform.scale.y, transform.scale.z);
        R = Matrix::CreateRotationX(transform.rotation.x);
        R *= Matrix::CreateRotationY(transform.rotation.y);
        R *= Matrix::CreateRotationZ(transform.rotation.z);
        T = Matrix::CreateTranslation(transform.position.x, transform.position.y, transform.position.z);

        desc.World = S * R * T;
    }
    {
        Vec3 position = Vec3(0.0F, 0.0F, -50.0F); // TEMP
        desc.View = Matrix::CreateLookAt(position, position + Vec3(0.0F, 0.0F, 0.1F), Vec3::Up);
    }
    desc.Projection = Matrix::CreatePerspective(WIDTH, HEIGHT, 0.1F, 1000.0F);

    _transformCB->CopyData(desc);
    DC->VSSetConstantBuffers(0, 1, _transformCB->GetComPtr().GetAddressOf());
}

void Model::CreateShader()
{
    const wstring filename = L"../Shaders/basic.fx";

    // Shader (VS)
    DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(_DEBUG)
    shaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ComPtr<ID3DBlob> errorBlob = nullptr;
    HRESULT hr = D3DCompileFromFile(
        filename.c_str(),
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "VS",
        "vs_5_0",
        shaderFlags,
        0,
        _vsBlob.GetAddressOf(),
        errorBlob.GetAddressOf());

    if (FAILED(hr))
    {
        if (errorBlob)
        {
            std::cerr << (char*)errorBlob->GetBufferPointer() << std::endl;
        }

        assert(false);
    }

    CHECK(DEVICE->CreateVertexShader(_vsBlob->GetBufferPointer(), _vsBlob->GetBufferSize(), nullptr, _vsShader.GetAddressOf()));

    // Shader (PS)
    errorBlob.Reset();

    hr = D3DCompileFromFile(
        filename.c_str(),
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "PS",
        "ps_5_0",
        shaderFlags,
        0,
        _psBlob.GetAddressOf(),
        errorBlob.GetAddressOf());

    if (FAILED(hr))
    {
        if (errorBlob)
        {
            std::cerr << (char*)errorBlob->GetBufferPointer() << std::endl;
        }

        assert(false);
    }

    CHECK(DEVICE->CreatePixelShader(_psBlob->GetBufferPointer(), _psBlob->GetBufferSize(), nullptr, _psShader.GetAddressOf()));

    vector<D3D11_INPUT_ELEMENT_DESC> input =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,   0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,   0, 12,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT,   0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "INDICES", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,   0, 44,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "WEIGHTS",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 60, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    CHECK(DEVICE->CreateInputLayout(input.data(), input.size(), _vsBlob->GetBufferPointer(), _vsBlob->GetBufferSize(), _inputLayout.GetAddressOf()));
}

void Model::UpdateShader()
{
    DC->VSSetShader(_vsShader.Get(), nullptr, 0);
    DC->PSSetShader(_psShader.Get(), nullptr, 0);
    DC->IASetInputLayout(_inputLayout.Get());
}
