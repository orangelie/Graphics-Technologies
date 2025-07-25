#pragma once
#pragma warning(disable:4996)

#define WIN32_LEAN_AND_MEAN
#define _HAS_STD_BYTE 0

#include <Windows.h>
#include <windowsx.h>
#include <wrl.h>

using namespace Microsoft::WRL;

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <array>
#include <memory>
#include <algorithm>
#include <functional>
#include <filesystem>

using namespace std;

#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3d11shader.h>
#include <d3d11.h>

#include <DirectXMath.h>
#include <DirectXTex/DirectXTex.h>
#include <DirectXTex/DirectXTex.inl>
#include "SimpleMath.h"

using namespace DirectX;

// assmip
#include <Assimp/Importer.hpp>
#include <Assimp/scene.h>
#include <Assimp/postprocess.h>

// imgui
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

// tinyxml2
#include "tinyxml2.h"

// Libs
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

#ifdef _DEBUG
#pragma comment(lib, "DirectXTex/DirectXTex_debug.lib")
#pragma comment(lib, "Assimp/assimp-vc143-mtd.lib")
#else
#pragma comment(lib, "DirectXTex/DirectXTex.lib")
#pragma comment(lib, "Assimp/assimp-vc143-mt.lib")
#endif

using int8 = __int8;
using int16 = __int16;
using int32 = __int32;
using int64 = __int64;
using uint8 = unsigned __int8;
using uint16 = unsigned __int16;
using uint32 = unsigned __int32;
using uint64 = unsigned __int64;

using Color = DirectX::XMFLOAT4;

using Vec2 = DirectX::SimpleMath::Vector2;
using Vec3 = DirectX::SimpleMath::Vector3;
using Vec4 = DirectX::SimpleMath::Vector4;
using Matrix = DirectX::SimpleMath::Matrix;
using Quaternion = DirectX::SimpleMath::Quaternion;

#define CHECK(p)	assert(SUCCEEDED(p))

#define DECLARE_SINGLE(classname)			\
private:									\
	classname() { }							\
public:										\
	static classname* GetInstance()			\
	{										\
		static classname s_instance;		\
		return &s_instance;					\
	}

#define GET_SINGLE(classname)	(classname::GetInstance())

#define TIME            GET_SINGLE(Timer)
#define INPUT           GET_SINGLE(Input)
#define DEVICE          PointerContainer::GetInstance()->device
#define DC              PointerContainer::GetInstance()->dc
#define RESOURCES       GET_SINGLE(Resources)

wstring ToWString(const string& value);
string ToString(const wstring& value);
void Replace(OUT string& str, IN const string& comp, IN const string& rep);
void Replace(OUT wstring& str, IN const wstring& comp, IN const wstring& rep);

struct VertexType
{
    Vec3 position = {};
    Vec2 uv = {};
    Vec3 normal = {};
    Vec3 tangent = {};
    Vec4 blendIndices = {};
    Vec4 blendWeights = {};
};

#include "FileUtils.h"
