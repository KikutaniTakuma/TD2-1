#include "Post.hlsli"

float4 main(Output input) : SV_TARGET{
    float w, h, levels;
    tex.GetDimensions(0, w,h,levels);

    float dx = 1.0f / w;
    float dy = 1.0f / h;
    float4 ret = float4(0.0f,0.0f,0.0f,0.0f);
    float loopCount = 5.0f;
{
    for(float i =0; i < loopCount; i++){
        ret += tex.Sample(smp, input.uv + float2(0.0f * dx, i * dy));
    }
}
{
    for(float i =0; i < loopCount; i++){
        ret += tex.Sample(smp, input.uv + float2(0.0f * dx, -i * dy));
    }
}

    return ret / (loopCount * 2.0f);
}