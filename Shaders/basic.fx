// Vertex Structs
struct VertexIn
{
    float4 position : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float4 blendIndices : INDICES;
    float4 blendWeights : WEIGHTS;
};

struct VertexOut
{
    float4 position : SV_POSITION;
    float3 worldPosition : POSITION1;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

// Constant Buffers
cbuffer TransformCB : register(b0)
{
    row_major float4x4 World;
    row_major float4x4 View;
    row_major float4x4 Projection;
};

#define MAX_MODEL_TRANSFORMS (250)
cbuffer BoneCB : register(b1)
{
    row_major float4x4 BoneTransforms[MAX_MODEL_TRANSFORMS];
};

cbuffer ScalarCB : register(b2)
{
    int BoneIndex;
    float3 paddings;
};

// Shader Resources
Texture2D DiffuseMap : register(t0);
SamplerState Sampler : register(s0);

VertexOut VS(VertexIn vin)
{
    VertexOut vout;

    vout.position = mul(vin.position, BoneTransforms[BoneIndex]);
    vout.position = mul(vout.position, World);
    vout.worldPosition = vout.position.xyz;
    vout.position = mul(vout.position, View);
    vout.position = mul(vout.position, Projection);
    vout.uv = vin.uv;
    vout.normal = mul(vin.normal, (float3x3)World);
    vout.tangent = mul(vin.tangent, (float3x3)World);

    return vout;
}

float4 PS(VertexOut pin) : SV_TARGET
{
    // return DiffuseMap.Sample(Sampler, pin.uv);
    return float4(1.0F, 0.0F, 0.0F, 1.0F);
}
