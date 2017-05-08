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
    uint  observations; //number of located landmarks in this step
    uint map_marks; // number of landmarks in the current map
    LANDMARK obs[MAX_OBS_POINTS]; /* Array of observations */
};

float getMVN_weight(float xi, float yi, float uix, float uiy, float cov_xx, float cov_yy)
{
    float x_d = xi - uix;
    float y_d = yi - uiy;
    float X = (x_d * x_d) / (cov_xx*cov_xx);
    float Y = (y_d * y_d) / (cov_yy*cov_yy);
    /* The term involving rho=0 and thus the cross covariance is eliminated */

    //return  (exp(-0.5f *(X + Y)) / (2.0f * 3.14159f * cov_xx *cov_yy));
    //return  exp((X + Y)); // (exp(-0.5f *(X + Y)) / (2.0f * 3.14159f * cov_xx *cov_yy));
    // return the logarithm instead of the e
    return x_d;// (-1.0 * log(2.0f * 3.14159f * cov_xx *cov_yy)) - 0.5*(X + Y);
}

/**/
//Particles buffer

StructuredBuffer<PARTICLE> particles_in:register(t0);
StructuredBuffer<LANDMARK> map_data:register(t1);
RWStructuredBuffer<PARTICLE> particles_out:register(u0);
[numthreads(64,1,1)]
void predict(uint3 id:SV_DispatchThreadID)
{
    PARTICLE p = particles_in[id.x];
    LANDMARK transformed_obs[MAX_OBS_POINTS];
    
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
    }                        
    /* TODO: copy the weigth every time  */
    particles_out[id.x].w = p.w;
    /*wait for all the particles to finish the update procedure */
    AllMemoryBarrierWithGroupSync();
    /* Rotate and translate each observation to match the vehicles location */
    for (uint i = 0; i < observations; ++i)
    {
        transformed_obs[i].x = obs[i].x * cos(p.th) - obs[i].y * sin(p.th) + p.x;
        transformed_obs[i].y = obs[i].x * sin(p.th) + obs[i].y * cos(p.th) + p.y;
    }
    AllMemoryBarrierWithGroupSync();
    /* Data association pahse: get the id of the landmakr with smallest euclidean distance */
    float min_dist = 1e100; // set minimun distance to a large number
    float current_dist = min_dist;
    for (uint j = 0; j < observations; ++j)
    {
        for (uint i = 0; i < map_marks; ++i)
        {
            current_dist = distance(float2(transformed_obs[j].x, transformed_obs[j].y),float2(map_data[i].x, map_data[i].y));
            if (current_dist < min_dist)
            {
                transformed_obs[j].id = map_data[i].id;
                particles_out[id.x].id = map_data[i].id; // for test
                min_dist = current_dist;
            }
        }
    } 
    AllMemoryBarrierWithGroupSync();
    float x_landmrk;
    float y_landmrk;
    /*Update weigths according to the mutlivariate normal distribution*/
    for (uint k = 0; k < observations; k++)
    {   /* The id of the landmark matches with its position in the array -1 */
        x_landmrk = map_data[transformed_obs[k].id-1].x;
        y_landmrk = map_data[transformed_obs[k].id-1].y;
        /* As we applied the log to the function instead of multiply we add */
        particles_out[id.x].w += getMVN_weight(transformed_obs[k].x, transformed_obs[k].y, x_landmrk, y_landmrk, x_noise, y_noise);
        
    }
    particles_out[id.x].w = exp(particles_out[id.x].w);

    //particles_out[id.x].w = log(exp(8));// transformed_obs[1].id;
    /* Association  */

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
