#pragma once




#include <sstream>
#include <fstream>
#include <math.h>
#include <vector>
#include "PF_types.h"

class Map {
public:

    struct single_landmark_s {

        int id_i; // Landmark ID
        float x_f; // Landmark x-position in the map (global coordinates)
        float y_f; // Landmark y-position in the map (global coordinates)
    };

    std::vector<single_landmark_s> landmark_list; // List of landmarks in the map

};

bool read_map_data(std::string filename, Map& map);
bool read_control_data(std::string filename, std::vector<control_t>& position_meas);
bool read_gt_data(std::string filename, std::vector<particle_t>& gt);
bool read_landmark_data(std::string filename, std::vector<landmark_t>& observations);