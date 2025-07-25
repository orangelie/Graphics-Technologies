#pragma once

struct asBone
{
    string name;

    int32 index = -1;
    int32 parent = -1;

    Matrix transform;
};

struct asMesh
{
    string name;

    aiMesh* mesh;
    vector<VertexType> vertices;
    vector<uint32> indices;

    int32 boneIndex;
    string materialName;
};

struct asMaterial
{
    string name;

    Color ambient;
    Color diffuse;
    Color specular;
    Color emissive;

    string diffuseFile;
    string specularFile;
    string normalFile;
};

class AnimationImporter
{
public:
    AnimationImporter();
    virtual ~AnimationImporter();

    void ReadAssetFile(const wstring& filename);
    void ExportModelData(const wstring& savePath);
    void ExportMaterialData(const wstring& savePath);

private:
    void ReadModelData(aiNode* node, int32 index, int32 parent);
    void ReadMeshData(aiNode* node, int32 bone);
    void WriteModelFile(const wstring& finalPath);

private:
    void ReadMaterialData();
    void WriteMaterialData(const wstring& finalPath);
    string WriteTexture(const string& saveFolder, const string& file);

private:
    wstring _assetPath = L"../Resources/Assets/";
    wstring _modelPath = L"../Resources/Models/";
    wstring _texturePath = L"../Resources/Textures/";

private:
    shared_ptr<Assimp::Importer> _importer = nullptr;
    const aiScene* _scene;

private:
    vector<shared_ptr<asBone>> _bones;
    vector<shared_ptr<asMesh>> _meshes;
    vector<shared_ptr<asMaterial>> _materials;


};
