#pragma once
/* Types implemented for the particle filter */
/* Shared structure between for a particle  */

#include "PF_cfg.h"

struct particle_t {
    float_t x;
    float_t y;
    float_t th;
    float_t w;
    int32_t  id;
};
/* Shared struct for observation and landmark info */
typedef struct {
    float_t x;
    float_t y;
    float_t id;
}landmark_t;


typedef struct {
    float_t velocity;
    float_t yaw_rate;
}control_t;







extern const float_t sigma_meas[3];


extern const float_t sigma_landmark[2];