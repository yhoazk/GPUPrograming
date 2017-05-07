#pragma once
/* PF specific configurations */

/* Defines the uncertainty in the location measurements and yaw measurement */
#define PF_SIGMA_MEAS_X    (0.3f)
#define PF_SIGMA_MEAS_Y    (0.3f)
#define PF_SIGMA_MEAS_TH   (0.01f)
/* Defines the uncertainty in the location of the landmark */
#define PF_SIGMA_LNDMRK_X  (0.3f)
#define PF_SIGMA_LNDMRK_Y  (0.3f)
/* Defines the number of particles used */
#define PF_PARTICLE_NUMBER (100U)
/* Defines the max obersvations to be used */
#define MAX_OBS_POINTS     (9U)