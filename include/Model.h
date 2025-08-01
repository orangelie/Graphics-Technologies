#pragma once

#include "ConstantBuffer.h"

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

    uint32 GetStride() const { return _stride; }
    uint32 GetOffset() const { return _offset; }

    wstring name;

    shared_ptr<ModelVertexType> geometry = make_shared<ModelVertexType>();

    wstring materialName = L"";
    shared_ptr<Material> material; // 캐시

    int32 boneIndex;
    shared_ptr<ModelBone> bone; // 캐시

private:
    uint32 _stride = 0;
    uint32 _offset = 0;

};

class RawModel : public enable_shared_from_this<RawModel>
{
public:
    RawModel();
    virtual ~RawModel();

public:
    void ReadMaterial(wstring filename);
    void ReadModel(wstring filename);

public:
    shared_ptr<Material> GetMaterialByName(const wstring& name);

    shared_ptr<ModelBone> GetBoneByIndex(uint32 index);

    vector<shared_ptr<ModelMesh>> GetMeshes() { return _meshes; }
    uint32 GetBoneCount() { return static_cast<uint32>(_bones.size()); }

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

// CB0
struct Transform
{
    Vec3 position;
    Vec3 rotation;
    Vec3 scale = { 1.0F, 1.0F, 1.0F };
};

struct TransformDesc
{
    Matrix World;
    Matrix View;
    Matrix Projection;
};

// CB1
#define MAX_MODEL_TRANSFORMS 700

// CB2
struct BoneDesc
{
    Matrix transforms[MAX_MODEL_TRANSFORMS];
};

// CB2
struct ScalarDesc
{
    int BoneIndex;
    float padding[3];
};

class Model
{
public:
    void Load(const wstring& shaderFilename, const wstring& modelName, const wstring& materialName);
    void Render();

    Transform transform;

private:
    void UploadTransformData();

    // TEST
    void CreateShader(const wstring& filename);
    void UpdateShader();

private:
    shared_ptr<RawModel> _model = nullptr;

    ComPtr<ID3DBlob> _vsBlob = nullptr;
    ComPtr<ID3DBlob> _psBlob = nullptr;

    ComPtr<ID3D11InputLayout> _inputLayout = nullptr;
    ComPtr<ID3D11VertexShader> _vsShader = nullptr;
    ComPtr<ID3D11PixelShader> _psShader = nullptr;

private:
    shared_ptr<ConstantBuffer<TransformDesc>> _transformCB = nullptr;
    shared_ptr<ConstantBuffer<BoneDesc>> _boneCB = nullptr;
    shared_ptr<ConstantBuffer<ScalarDesc>> _scalarCB = nullptr;
    
};
