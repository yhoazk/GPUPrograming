// inicializando un constant buffer
cbuffer  PARAMS:register(b0)
{
    float2 Offset;
};

// el fractal de julia se define asi:
// z0 =  z  #cualquier posicion en la pantalla
// zn = f(zn) + c #  cualquier funcion mas una constante
// 
// si z0, z1, ..., zn forman una serie acotada se dice que pertenece al conjunto de julia
// de lo contrario la serie diverge y no pertenece al conjunto

// en direct compute no existe la multiplicacion de quaternios


/* Declaracion de funcion en HLSL*/

float2 ComplexMul(float2 a, float2 b)
{
    float2 r;
    r.x = a.x * b.x - a.y * b.y;
    r.y = a.x * b.y + a.y * b.x;
    return r;
}

RWTexture2D<float4> Output:register(u0);
[numthreads (64,16,1)] // tiene solo vigencia de una linea, debe ir antes de la funcion main
void main(uint3 id:SV_DispatchThreadID)
{
    float2 z = ((id.xy/1024.0f) - 0.5f) * 2; // si se quita la f de los numeros flotantes´puede asignarlo como double y la operacion tarda 4 veces mas en algunas arch.
	int i;
	for (i; i < 256; i++)
	{
		z = ComplexMul(z, ComplexMul(z, z)) + Offset;
		// verificar si z diverge
		if((z.x*z.x + z.y*z.y) > 4)
		{
			break;
		}
	}
	float theta =  2* 3.141592f * i / 256.0f;
	Output[id.xy] =1- (float4(0.5 * cos(theta), 0.5 * cos(3 * theta), 0.5 * cos(7 * theta),1) + 0.5);
	/*if((z.x*z.x + z.y*z.y )> 4)
	{
		Output[id.xy] = float4(0, 0, 0, 0);
	}
	else
	{
		Output[id.xy] = float4(1, 1, 1,1);
	}*/
}
