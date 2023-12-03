cbuffer Mat : register(b0){
    float4x4 mat;
}

struct VertexOutput {
    float4 pos : SV_POSITION;
    float4 color : COLOR;
};