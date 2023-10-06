#include "Line.hlsli"

VertexOutput main(float4 pos : POSITION0, float4 color : COLOR0) {
    VertexOutput output;
    output.pos = mul(mat, pos);
    output.color = color;

    return output;
}