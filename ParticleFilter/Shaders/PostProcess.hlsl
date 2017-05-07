Texture2D Input:register(t0);
RWTexture2D<float4> Output:register(u0);
groupshared float4 Data[16][16];
[numthreads(16,16,1)]
void main(uint3 id:SV_DispatchThreadID,uint3 lid:SV_GroupThreadID)
{
	Data[lid.y][lid.x] = Input[id.xy];
	Output[id.xy] = float4(0, 0, 0, 0);
	if (lid.x==0 && lid.y==0) //Divergencia
	{
		float4 Suma = 0;
		for (int j = 0; j < 16; j++)
			for (int i = 0; i < 16; i++)
				Suma += Data[j][i];
		Output[id.xy/16] = Suma / (16*16);
	}
}