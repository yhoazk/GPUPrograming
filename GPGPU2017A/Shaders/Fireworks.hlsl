/* Fireworks */

/*

Consiste de 2 etapas, aplicar la cinetica y la segunda hacer rendering de la particula
*/

struct SPARK
{
    float4 Position;
    float4 Velocidad;
    float4 Color;
    float MaxAge;

};


cbuffer PARAMS
{
    float4 g; // gravity
    float dt;
    float t;
}

RWStructuredBuffer<SPARK> Sparks : register(U0);

[numthreads(64,1,1)]
void Animate(uint3 id:SV_DispatchThreadID)
{
    SPARK Spark = Sparks[id.x];
    Spark.Position += Spark.Velocidad * dt;
    Spark.Velocidad += g * dt;
    Sparks[id.x] = Spark;
}

RWTexture2D<float4> Sky : register(u1);
[numthreads(64,1,1)]
void Render(uint3 id:SV_DispatchThreadID)
{
    int2 coordenada = (int2)Sparks[id.x].Position.xy;
    Sky[coordenada] = Sparks[id.x].Color;
}