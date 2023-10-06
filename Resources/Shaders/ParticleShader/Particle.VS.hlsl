#include "Particle.hlsli"

StructuredBuffer<float4x4> mat : register(t1);

Output main(float4 pos : POSITION, float2 uv : TEXCOORD, uint32_t instanceId : SV_InstanceID){
    Output output;

    output.svPos = mul(mat[instanceId], pos);
    output.uv = uv;

    return output;
}