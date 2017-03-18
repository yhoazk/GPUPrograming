/* Postprocess.hlsl */


Texture2D Input : register(t0);
// RWTexture2D se le debe especificar de que tipo es la salida,
RWTexture2D<float4> Output : register(u0);

// variables inicializadas aqui no son visibles para el gpu, solo es visible 
// para el metacompilador, pero el gpu no la ve. Puede ser usada para decidir 
// si el hlsl tiene diferentes caracteristicas y tomar decisiones acorde a ese valor

//groupshared float4 Data[16][16];
[numthreads(16,16,1)]
void main(uint3 id : SV_DispatchThreadID, uint3 lid : SV_GroupThreadID)// SV_GroupThreadID Defines the thread offset within the group, per dimension of the group. Available as input to the compute shader. (read only)
{
    //Data[lid.y][lid.x] = Input[id.xy];
    GroupMemoryBarrierWithGroupSync();
    Output[id.xy] = 0.0f;
    //Input[id.xy].r * 0.2126f + Input[id.xy].g * 0.7152f + Input[id.xy].b * 0.0722f;


}