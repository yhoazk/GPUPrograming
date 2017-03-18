
//Define el número y estructura del grupo
//grupo, es un equipo de hilos que variables globales.


struct VECTOR
{
    float x, y, z, w;
};

StructuredBuffer<VECTOR> Input:register(t0);
//RWStructuredBuffer<VECTOR> Output:register(u0);
RWTexture2D<float4>   Output:register(u0);   //debe ser de tipo nativo, por default toma el de la vista, float4      uint1,2,3,4
int x;     //x y z    hilos y grupos
[numthreads(16,16,1)]
void main(uint3 id:SV_DispatchThreadID)
{               // id is a 3 dimensional input

    /*
    Output[id.x].x = Input[id.x].x + 1;
    Output[id.x].y = Input[id.x].y + 1;
    Output[id.x].z = Input[id.x].xz + 1;
    Output[id.x].w = Input[id.x].w+ 1;         
    */

    //Output[id.xy] = float4(Input[id.x].x, 255, Input[id.x].z, Input[id.x].w); //      barra verde
    //Output[id.xy] = float4(Input[id.x].x, Input[id.x].y, Input[id.x].z, Input[id.x].w); // 
    //Output[id.xy] = float4((id.x % 16) / 16.0, (id.y % 16.0) / 16.0, 0, 0); // barras rojas cone el modulo del identificador de hilo cuadros
    int r = 160;
    int x = id.x - 160;
    int y = id.y - 160;
    if (x*x + y*y < r*r)
    {
        Output[id.xy] = float4((id.x % 256) / 16.0, (id.y % 16.0) / 16.0, (id.x*id.y)%256, 0)/256.0; // barras rojas cone el modulo del identificador de hilo

    }
    else
    {
        //Output[id.xy] = float4(0,100, 0, 0); // barras rojas cone el modulo del identificador de hilo
        Output[id.xy] = float4( (id.y % 16.0) / 16.0, (id.x % 256) / 16.0, (id.x*id.y) % 256, 0) / 256.0; // barras rojas cone el modulo del identificador de hilo


    }
}