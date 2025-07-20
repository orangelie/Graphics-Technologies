#pragma once

#include "Win32Builder.h"
#include "Global.h"

class TestDemo
{
public:
    void Test()
    {
        Win32Builder builder;

        shared_ptr<Assimp::Importer> importer = make_unique<Assimp::Importer>();

        auto _scene = importer->ReadFile(
            "../Resources/Models/Test/Mesh.fbx",
            aiProcess_ConvertToLeftHanded |
            aiProcess_Triangulate |
            aiProcess_GenUVCoords |
            aiProcess_GenNormals |
            aiProcess_CalcTangentSpace
        );
    }
};

