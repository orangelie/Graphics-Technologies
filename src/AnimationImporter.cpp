#include "Global.h"
#include "AnimationImporter.h"
#include "PointerContainer.h"

AnimationImporter::AnimationImporter()
{
    _importer = make_shared<Assimp::Importer>();
}

AnimationImporter::~AnimationImporter()
{
}

void AnimationImporter::ReadAssetFile(const wstring& filename)
{
    wstring fileStr = _assetPath + filename;
    filesystem::path p = filesystem::path(fileStr);
    assert(filesystem::exists(p));

    _scene = _importer->ReadFile(
        ToString(fileStr),
        aiProcess_ConvertToLeftHanded | 
        aiProcess_Triangulate | 
        aiProcess_GenNormals | 
        aiProcess_GenUVCoords | 
        aiProcess_CalcTangentSpace
    );

    assert(_scene != nullptr);
}

void AnimationImporter::ExportModelData(const wstring& savePath)
{
    wstring finalPath = _modelPath + savePath + L".mesh";
    ReadModelData(_scene->mRootNode, -1, -1);
    WriteModelFile(finalPath);
}

void AnimationImporter::ExportMaterialData(const wstring& savePath)
{
    wstring finalPath = _texturePath + savePath + L".xml";
    ReadMaterialData();
    WriteMaterialData(finalPath);
}

void AnimationImporter::ReadModelData(aiNode* node, int32 index, int32 parent)
{
    shared_ptr<asBone> bone = make_shared<asBone>();
    bone->index = index;
    bone->parent = parent;
    bone->name = node->mName.C_Str();

    Matrix transform(node->mTransformation[0]);
    bone->transform = transform.Transpose();

    Matrix matParent = Matrix::Identity;
    if (parent >= 0)
        matParent = _bones[parent]->transform;

    bone->transform = bone->transform * matParent;

    _bones.push_back(bone);

    // Mesh
    ReadMeshData(node, index);

    for (uint32 i = 0; i < node->mNumChildren; ++i)
    {
        ReadModelData(node->mChildren[i], _bones.size(), index);
    }
}

void AnimationImporter::ReadMeshData(aiNode* node, int32 bone)
{
    if (node->mNumMeshes < 1)
        return;

    shared_ptr<asMesh> mesh = make_shared<asMesh>();
    mesh->name = node->mName.C_Str();
    mesh->boneIndex = bone;

    for (uint32 i = 0; i < node->mNumMeshes; ++i)
    {
        uint32 index = node->mMeshes[i];
        const aiMesh* srcMesh = _scene->mMeshes[index];

        // Material Name
        const aiMaterial* material = _scene->mMaterials[srcMesh->mMaterialIndex];
        mesh->materialName = material->GetName().C_Str();

        const uint32 startVertex = mesh->vertices.size();

        for (uint32 v = 0; v < srcMesh->mNumVertices; ++v)
        {
            // Vertex
            VertexType vertex;
            ::memcpy(&vertex.position, &srcMesh->mVertices[v], sizeof(Vec3));

            //// UV
            if (srcMesh->HasTextureCoords(0))
                ::memcpy(&vertex.uv, &srcMesh->mTextureCoords[0][v], sizeof(Vec2));

            // Normal
            if (srcMesh->HasNormals())
                ::memcpy(&vertex.normal, &srcMesh->mNormals[v], sizeof(Vec3));

            mesh->vertices.push_back(vertex);
        }

        for (uint32 f = 0; f < srcMesh->mNumFaces; ++f)
        {
            aiFace& face = srcMesh->mFaces[f];

            for (uint32 k = 0; k < face.mNumIndices; ++k)
            {
                mesh->indices.push_back(face.mIndices[k] + startVertex);
            }
        }
    }

    _meshes.push_back(mesh);
}

void AnimationImporter::WriteModelFile(const wstring& finalPath)
{
    auto path = filesystem::path(finalPath);

    filesystem::create_directory(path.parent_path());

    shared_ptr<FileUtils> file = make_shared<FileUtils>();
    file->Open(finalPath, FileMode::Write);

    file->Write<uint32>(_bones.size());
    for (shared_ptr<asBone>& bone : _bones)
    {
        file->Write<int32>(bone->index);
        file->Write<string>(bone->name);
        file->Write<int32>(bone->parent);
        file->Write<Matrix>(bone->transform);
    }

    file->Write<uint32>(_meshes.size());
    for (shared_ptr<asMesh>& mesh : _meshes)
    {
        file->Write<string>(mesh->name);
        file->Write<int32>(mesh->boneIndex);
        file->Write<string>(mesh->materialName);

        file->Write<uint32>(mesh->vertices.size());
        file->Write(&mesh->vertices[0], sizeof(VertexType) * mesh->vertices.size());

        file->Write<uint32>(mesh->indices.size());
        file->Write(&mesh->indices[0], sizeof(uint32) * mesh->indices.size());
    }
}

void AnimationImporter::ReadMaterialData()
{
    for (uint32 i = 0; i < _scene->mNumMaterials; ++i)
    {
        aiMaterial* srcMaterial = _scene->mMaterials[i];

        shared_ptr<asMaterial> material = make_shared<asMaterial>();
        material->name = srcMaterial->GetName().C_Str();

        aiColor3D color;
        // Ambient
        srcMaterial->Get(AI_MATKEY_COLOR_AMBIENT, color);
        material->ambient = Color(color.r, color.g, color.b, 1.0F);
        // Diffuse
        srcMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color);
        material->diffuse = Color(color.r, color.g, color.b, 1.0F);
        // Specular
        srcMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color);
        material->specular = Color(color.r, color.g, color.b, 1.0F);
        srcMaterial->Get(AI_MATKEY_SHININESS, material->specular.w);
        // Emmisive
        srcMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, color);
        material->emissive = Color(color.r, color.g, color.b, 1.0F);

        aiString file;
        // Diffuse Texture
        srcMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &file);
        material->diffuseFile = file.C_Str();
        // Specular Texture
        srcMaterial->GetTexture(aiTextureType_SPECULAR, 0, &file);
        material->specularFile = file.C_Str();
        // Normal Texture
        srcMaterial->GetTexture(aiTextureType_NORMALS, 0, &file);
        material->normalFile = file.C_Str();

        _materials.push_back(material);

        /*cout << material->diffuseFile << endl;
        printf("[%d] diffuse: (%f, %f, %f)\n", i, material->diffuse.x, material->diffuse.y, material->diffuse.z);*/
    }
}

void AnimationImporter::WriteMaterialData(const wstring& finalPath)
{
    filesystem::path path = filesystem::path(finalPath);
    filesystem::create_directory(path.parent_path());

    string folder = path.parent_path().string();

    shared_ptr<tinyxml2::XMLDocument> document = make_shared<tinyxml2::XMLDocument>();
    tinyxml2::XMLDeclaration* decl = document->NewDeclaration();
    document->LinkEndChild(decl);

    tinyxml2::XMLElement* root = document->NewElement("Materials");
    document->LinkEndChild(root);

    for (shared_ptr<asMaterial> material : _materials)
    {
        tinyxml2::XMLElement* node = document->NewElement("Material");
        root->LinkEndChild(node);

        tinyxml2::XMLElement* element = nullptr;

        element = document->NewElement("Name");
        element->SetText(material->name.c_str());
        node->LinkEndChild(element);

        element = document->NewElement("DiffuseFile");
        element->SetText(WriteTexture(folder, material->diffuseFile).c_str());
        node->LinkEndChild(element);

        element = document->NewElement("SpecularFile");
        element->SetText(WriteTexture(folder, material->specularFile).c_str());
        node->LinkEndChild(element);

        element = document->NewElement("NormalFile");
        element->SetText(WriteTexture(folder, material->normalFile).c_str());
        node->LinkEndChild(element);

        element = document->NewElement("Ambient");
        element->SetAttribute("R", material->ambient.x);
        element->SetAttribute("G", material->ambient.y);
        element->SetAttribute("B", material->ambient.z);
        element->SetAttribute("A", material->ambient.w);
        node->LinkEndChild(element);

        element = document->NewElement("Diffuse");
        element->SetAttribute("R", material->diffuse.x);
        element->SetAttribute("G", material->diffuse.y);
        element->SetAttribute("B", material->diffuse.z);
        element->SetAttribute("A", material->diffuse.w);
        node->LinkEndChild(element);

        element = document->NewElement("Specular");
        element->SetAttribute("R", material->specular.x);
        element->SetAttribute("G", material->specular.y);
        element->SetAttribute("B", material->specular.z);
        element->SetAttribute("A", material->specular.w);
        node->LinkEndChild(element);

        element = document->NewElement("Emissive");
        element->SetAttribute("R", material->emissive.x);
        element->SetAttribute("G", material->emissive.y);
        element->SetAttribute("B", material->emissive.z);
        element->SetAttribute("A", material->emissive.w);
        node->LinkEndChild(element);
    }

    document->SaveFile(ToString(finalPath).c_str());
}

string AnimationImporter::WriteTexture(const string& saveFolder, const string& file)
{
    string fileName = filesystem::path(file).filename().string();
    string folderName = filesystem::path(saveFolder).filename().string();

    const aiTexture* srcTexture = _scene->GetEmbeddedTexture(file.c_str());
    if (srcTexture)
    {
        string pathStr = (filesystem::path(saveFolder) / fileName).string();

        if (srcTexture->mHeight == 0)
        {
            shared_ptr<FileUtils> file = make_shared<FileUtils>();
            file->Open(ToWString(pathStr), FileMode::Write);
            file->Write(srcTexture->pcData, srcTexture->mWidth);
        }
        else
        {
            D3D11_TEXTURE2D_DESC desc;
            ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
            desc.Width = srcTexture->mWidth;
            desc.Height = srcTexture->mHeight;
            desc.MipLevels = 1;
            desc.ArraySize = 1;
            desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            desc.SampleDesc.Count = 1;
            desc.SampleDesc.Quality = 0;
            desc.Usage = D3D11_USAGE_IMMUTABLE;

            D3D11_SUBRESOURCE_DATA subResource = { 0 };
            subResource.pSysMem = srcTexture->pcData;

            ComPtr<ID3D11Texture2D> texture;
            HRESULT hr = DEVICE->CreateTexture2D(&desc, &subResource, texture.GetAddressOf());
            CHECK(hr);

            DirectX::ScratchImage img;
            ::CaptureTexture(DEVICE.Get(), DC.Get(), texture.Get(), img);

            // Save To File
            hr = DirectX::SaveToDDSFile(*img.GetImages(), DirectX::DDS_FLAGS_NONE, ToWString(fileName).c_str());
            CHECK(hr);
        }
    }
    else
    {
        string originStr = (filesystem::path(_assetPath) / folderName / file).string();
        Replace(originStr, "\\", "/");

        string pathStr = (filesystem::path(saveFolder) / fileName).string();
        Replace(pathStr, "\\", "/");

        ::CopyFileA(originStr.c_str(), pathStr.c_str(), false);
    }

    return string();
}
