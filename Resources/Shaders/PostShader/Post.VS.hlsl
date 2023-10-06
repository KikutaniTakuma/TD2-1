#include "Post.hlsli"

Output main(float4 pos : POSITION, float2 uv : TEXCOORD){
    Output output;

    output.svPos = pos;
    output.uv = uv;

    return output;
}