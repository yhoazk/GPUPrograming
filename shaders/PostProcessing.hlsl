/* Postprocess.hlsl */


Texture2D Input : register(t0);
// RWTexture2D se le debe especificar de que tipo es la salida,
RWTexture2D<float4> Output : register(u0);

// variables inicializadas aqui no son visibles para el gpu, solo es visible 
// para el metacompilador, pero el gpu no la ve. Puede ser usada para decidir 
// si el hlsl tiene diferentes caracteristicas y tomar decisiones acorde a ese valor

groupshared float4 Data[16][16];
[numthreads(16,16,1)]
void main(uint3 id:SV_DispatchThreadID, uint3 lid:SV_GroupThreadID)
{
    Data[lid.y][lid.x] = Input[id.xy];
    GroupMemoryBarrierWithGroupSync();
    Output[id.xy] = 0;
    if(lid.x== 0 && lid.y ==0) // aqui se presetna un problema de divergencia, cuando not todos los hilos hacen lo mismo debido a una bifurcacion
    {
        // TAREA: Reducir la divergencia de este algoritmo.
        // Suma de fibonacci hasta N 
        // Tamano de unidades de ejecucion para intel 256/ amd 64/ nvidia 32
        float4 suma=0;
        for (int j = 0; j < 16; j++)
        {
            for (int i = 0; i < 16; i++)
            {
                suma += Data[j][i];
            }
        }
    //GroupMemoryBarrierWithGroupSync();
        Output[id.xy/16] = suma / 256;
    }
}
