/* Postprocess.hlsl */


/*
 Problem Statement:
 	Construya un procesador de imágenes que reciba cualquier archivo BMP solicitado por el usuario y reduzca el color a escala de grises. La imagen procesada en el GPU debe presentarse en pantalla en escala de grises y no es necesario almacenar a disco los cambios.
Luma=0.2126R+0.7152G+0.0722B
Donde R, G, B son los canales de color de cada pixel.

*/


Texture2D Input : register(t0);
// RWTexture2D se le debe especificar de que tipo es la salida,
RWTexture2D<float4> Output : register(u0);

// variables inicializadas aqui no son visibles para el gpu, solo es visible 
// para el metacompilador, pero el gpu no la ve. Puede ser usada para decidir 
// si el hlsl tiene diferentes caracteristicas y tomar decisiones acorde a ese valor

groupshared float4 Data[16][16];
[numthreads(16,16,1)]
void main(uint3 id : SV_DispatchThreadID)// SV_GroupThreadID Defines the thread offset within the group, per dimension of the group. Available as input to the compute shader. (read only)
{
    /* The dot product multiplies and then sums the result to get one single value, 
       Then the result is casted to a float4 with the same value in all the elements.
      */
    Output[id.xy] = dot(Input[id.xy], float4(0.212f, 0.7152f, 0.0722f, 0));

}