/*
Particle filter HLSL implementation
*/

#define MAX_OBS_POINTS (12)
#define EPSILON (1e-3)

struct PARTICLE {
    float x;
    float y;
    float th;
    float w;
    uint  id;
    //float3 Padding;
};

struct LANDMARK {
    float x;
    float y;
    uint id;
    
};

struct SPARK
{
	float4 Position;
	float4 Velocity;
	float4 Color;
	float  MaxAge;
};

cbuffer PF_STEP_DATA
{
    float velocity; /* Control input speed */
    float yaw_rate; /* Control input yaw rate */
    float x_noise;  /* Current measurement noise */
    float y_noise;  /* Current measurement noise */
    float th_noise; /* Current measurement noise */
    float sensor_range;
    float delta_t;
    uint  time; /* Time stamp */
    LANDMARK obs[MAX_OBS_POINTS]; /* Array of observations */
};


/**/
//Particles buffer

StructuredBuffer<PARTICLE> particles_in:register(t0);
StructuredBuffer<LANDMARK> map_data:register(t1);
RWStructuredBuffer<PARTICLE> particles_out:register(u0);
[numthreads(64,1,1)]
void predict(uint3 id:SV_DispatchThreadID)
{
    PARTICLE p = particles_in[id.x];
                   /*
    if (abs(yaw_rate) <= EPSILON)
    {
        particles_out[id.x].x = p.x + velocity * delta_t * cos(p.th);
        particles_out[id.x].y = p.y + velocity * delta_t * sin(p.th);
        particles_out[id.x].th = 0.0f;
    }
    else
    {
        particles_out[id.x].x = p.x + ((velocity / yaw_rate) * (sin(p.th + yaw_rate * delta_t) - sin(p.th)));
        particles_out[id.x].y = p.y = p.y + ((velocity / yaw_rate) * (cos(p.th) - cos(p.th + yaw_rate * delta_t)));
        particles_out[id.x].th = p.th + yaw_rate * delta_t;
    }                        */
    particles_out[id.x].x = map_data[id.x].x;
    particles_out[id.x].y = map_data[id.x].y;
    particles_out[id.x].id = id.x; //map_data[id.x+40].id;
    AllMemoryBarrierWithGroupSync();
    if (id.x == 0)
    {
        for (int i = 0; i < 64; i++)
        {
            particles_out[0].id += particles_out[i].id;
        }
    }




    /* Translate from particle coordinates to map location */


   // p.x = p.x + velocity*delta_t;
   // float d = distance(float2(p.x, p.y), float2(p.x, p.y));
    //float4 gI = float4(0, 0, 0, 0);
    //float I = 0; // calcula la distancia entre dos vecotres con el teorema de pitagoras
}

#if 0
RWTexture2D<PARTICLE> Map:register(u1);                // TODO Change to landmark
[numthreads(64, 1, 1)]
void update_weight(uint3 id:SV_DispatchThreadID)
{
}
RWTexture2D<PARTICLE> new_particles:register(u0);
[numthreads(64, 1, 1)]
void resample(uint3 id:SV_DispatchThreadID)
{
}


Texture2D<float4> Source:register(t0);
[numthreads(64, 1, 1)]
void HBloor(uint3 id:SV_DispatchThreadID)
{
    Sky[id.xy] =
        Source[id.xy - int2(-4, 0)] * 0.004902046 +
        Source[id.xy - int2(-3, 0)] * 0.028975468 +
        Source[id.xy - int2(-2, 0)] * 0.103088739 +
        Source[id.xy - int2(-1, 0)] * 0.220759647 +
        Source[id.xy - int2(0, 0)] * 0.284548201 +
        Source[id.xy - int2(4, 0)] * 0.004902046 +
        Source[id.xy - int2(3, 0)] * 0.028975468 +
        Source[id.xy - int2(2, 0)] * 0.103088739 +
        Source[id.xy - int2(1, 0)] * 0.220759647;
}
[numthreads(1, 64, 1)]
void VBloor(uint3 id:SV_DispatchThreadID)
{
    Sky[id.xy] =
        Source[id.xy - int2(0, -4)] * 0.004902046 +
        Source[id.xy - int2(0, -3)] * 0.028975468 +
        Source[id.xy - int2(0, -2)] * 0.103088739 +
        Source[id.xy - int2(0, -1)] * 0.220759647 +
        Source[id.xy - int2(0, 0)] * 0.284548201 +
        Source[id.xy - int2(0, 4)] * 0.004902046 +
        Source[id.xy - int2(0, 3)] * 0.028975468 +
        Source[id.xy - int2(0, 2)] * 0.103088739 +
        Source[id.xy - int2(0, 1)] * 0.220759647;
}


[numthreads(64, 1, 1)]
void NBody(uint3 id:SV_DispatchThreadID)
{
    float4 Iij = 0.0f;
    for (uint j = 0; j < nParticles; j++)
    {
        if (id.x == j)
            continue;

        float4 Xij = Sparks[j].Position - Sparks[id.x].Position;
        float Squares = dot(Xij, Xij);
        if (Squares < 0.01)
        {
            continue;
        }
        float4 Dij = normalize(Xij); // calcular la direccion del cuerpo
        // calcular la distancia
        float4 d = length(Xij);

        Iij += Dij * kg / (1.0f + d);
    }
    Sparks[id.x].Velocity = Sparks[id.x].Velocity + Iij * dt;

}/**/

#endif // 0
