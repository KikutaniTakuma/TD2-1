#include "Post.hlsli"

float4 main(Output input) : SV_TARGET{
    float4 color =  tex.Sample(smp, input.uv);

    // 輝度法によるグレースケール
    float4 finalColor;
    finalColor.xyz = dot(color.xyz, float3(0.21f, 0.72f, 0.07f ));

    finalColor.w = color.w;

    clip(dot(color.xyz, float3(1.0f, 1.0f, 1.0f )) - 0.01f);

    return finalColor;
}