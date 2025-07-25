#pragma once

class Material;

struct ModelVertexType
{
    void AddVertices(vector<VertexType> vertices);
    void AddIndices(vector<uint32> indices);

    ComPtr<ID3D11Buffer> vertexBuffer;
    ComPtr<ID3D11Buffer> indexBuffer;

    vector<VertexType> vertices;
    vector<uint32> indices;
};

struct ModelBone
{
    wstring name;
    int32 index;
    int32 parentIndex;
    shared_ptr<ModelBone> parent; // 캐시

    Matrix transform;
    vector<shared_ptr<ModelBone>> children; // 캐시
};

struct ModelMesh
{
    void CreateBuffers(uint32 vCount, uint32 iCount);

    wstring name;

    shared_ptr<ModelVertexType> geometry = make_shared<ModelVertexType>();

    wstring materialName = L"";
    shared_ptr<Material> material; // 캐시

    int32 boneIndex;
    shared_ptr<ModelBone> bone; // 캐시
};

class RawModel : public enable_shared_from_this<RawModel>
{
public:
    RawModel();
    virtual ~RawModel();

public:
    void ReadMaterial(wstring filename);
    void ReadModel(wstring filename);

private:
    shared_ptr<Material> GetMaterialByName(const wstring& name);

    shared_ptr<ModelBone> GetBoneByIndex(uint32 index);

private:
    void BindCacheInfo();

private:
    wstring _modelPath = L"../Resources/Models/";
    wstring _texturePath = L"../Resources/Textures/";

private:
    shared_ptr<ModelBone> _root;
    vector<shared_ptr<Material>> _materials;
    vector<shared_ptr<ModelBone>> _bones;
    vector<shared_ptr<ModelMesh>> _meshes;

};

class Model
{
public:
    void Start();
    void Render();

private:
    shared_ptr<RawModel> _rawModel = nullptr;

    ComPtr<ID3DBlob> _vsBlob = nullptr;
    ComPtr<ID3DBlob> _psBlob = nullptr;
    
};
