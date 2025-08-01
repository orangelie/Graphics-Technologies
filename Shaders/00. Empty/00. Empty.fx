#include "../Global.fx"

VertexOut VS(VertexIn vin)
{
    VertexOut vout;

    vout.position = mul(float4(vin.position.xyz, 1.0F), BoneTransforms[BoneIndex]);
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
    return DiffuseMap.Sample(Sampler, pin.uv);
}
