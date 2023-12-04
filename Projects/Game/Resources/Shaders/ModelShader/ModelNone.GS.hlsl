#include "Model.hlsli"

[maxvertexcount(3)]
void main(triangle VertexShaderOutput input[3], inout TriangleStream<GeometoryOutPut> outStream){
    GeometoryOutPut output[3];
   for(int i=0;i<3;i++){
        output[i].position = input[i].position;
        output[i].worldPosition = input[i].worldPosition;
        output[i].normal = input[i].normal;
        output[i].uv = input[i].uv;
        outStream.Append(output[i]);
    }
    outStream.RestartStrip();
}