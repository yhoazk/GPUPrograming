

cbuffer PARAMS:register(b0)
{
	float2 Offset;
};

/* Fractal de Julia se define así:
     z0 = z    
	 zn = f(zn) + c

	 Si z0,z1,...,zn forman una serie acotada se dice que pertenece al conjunto de julia
	 en caso contrario la serie diverge y no pertenece al conjunto.
*/

float2 ComplexMul(float2 a, float2 b)
{
	// (Ax,Ay*i)*(Bx,By*i)
	float2 r;
	r.x = a.x*b.x - a.y*b.y;
	r.y = a.x*b.y + a.y*b.x;
	return r;
}
float2 Exp(float2 a)
{
	// exp(z) = e^z= e^(x+yi) = e^x*e^yi = e^x*(cos(y)+sin(y)i) 
	return float2(exp(a.x)*cos(a.y), exp(a.x)*sin(a.y));
}
RWTexture2D<float4> Output:register(u0);
[numthreads(16,16,1)]
void main(uint3 id:SV_DispatchThreadID)
{
	float2 z = ((id.xy /1920.0f) - 0.5f) * 2.0f;
	float2 c = Offset.xy;
	int i;
	for (i = 0; i < 256; i++)
	{
		z =ComplexMul(z,z) + c;
		if ((z.x*z.x + z.y*z.y) > 4)
			break;
	}
	float theta = 2*3.141592f* i / 256.0f;
	Output[id.xy] =1- 
		(float4(0.5*cos(theta), 0.5*cos(2 * theta), 0.5*cos(3* theta), 1) + 0.5);
}

