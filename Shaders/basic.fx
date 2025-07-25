struct VertexIn
{

};

struct VertexOut
{

};

VertexOut VS(VertexIn vin)
{
    VertexOut vout;

    return vout;
}

float4 PS() : SV_TARGET
{
    return float4(1.0F, 0.0F, 0.0F, 1.0F);
}
