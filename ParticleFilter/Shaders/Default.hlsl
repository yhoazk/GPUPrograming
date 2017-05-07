
//Define el número y estructura del grupo
//grupo, es un equipo de hilos que variables globales.

struct VECTOR	
{
	float x, y, z, w;
};
StructuredBuffer<VECTOR> Input:register(t0);
RWStructuredBuffer<VECTOR> Output:register(u0);
[numthreads(1,1,1)]
void main(uint3 id:SV_DispatchThreadID)
{
	VECTOR A, B,C;
	A = Input[id.x * 2];
	B = Input[id.x * 2 + 1];
	C.x = A.x + B.x;
	C.y = A.y + B.y;
	C.z = A.z + B.z;
	C.w = A.w + B.w;
	Output[id.x] = C;
}