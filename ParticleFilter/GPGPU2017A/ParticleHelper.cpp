#include "stdafx.h"
#include "ParticleHelper.h"




bool read_map_data(std::string filename, Map& map) {

    // Get file of map:
    std::ifstream in_file_map(filename.c_str(), std::ifstream::in);
    // Return if we can't open the file.
    if (!in_file_map) {
        return false;
    }

    // Declare single line of map file:
    std::string line_map;

    // Run over each single line:
    while (getline(in_file_map, line_map)) {

        std::istringstream iss_map(line_map);

        // Declare landmark values and ID:
        float landmark_x_f, landmark_y_f;
        int id_i;

        // Read data from current line to values::
        iss_map >> landmark_x_f;
        iss_map >> landmark_y_f;
        iss_map >> id_i;

        // Declare single_landmark:
        Map::single_landmark_s single_landmark_temp;

        // Set values
        
        single_landmark_temp.x = landmark_x_f;
        single_landmark_temp.y = landmark_y_f;
        single_landmark_temp.id = id_i;
        // Add to landmark list of map:
        map.landmark_list.push_back(single_landmark_temp);
    }
    return true;
}




bool read_control_data(std::string filename, std::vector<control_t>& position_meas) {

    // Get file of position measurements:
    std::ifstream in_file_pos(filename.c_str(), std::ifstream::in);
    // Return if we can't open the file.
    if (!in_file_pos) {
        return false;
    }

    // Declare single line of position measurement file:
    std::string line_pos;

    // Run over each single line:
    while (getline(in_file_pos, line_pos)) {

        std::istringstream iss_pos(line_pos);

        // Declare position values:
        double velocity, yaw_rate;

        // Declare single control measurement:
        control_t meas;

        //read data from line to values:

        iss_pos >> velocity;
        iss_pos >> yaw_rate;


        // Set values
        meas.velocity = velocity;
        meas.yaw_rate = yaw_rate;

        // Add to list of control measurements:
        position_meas.push_back(meas);
    }
    return true;
}


bool read_gt_data(std::string filename, std::vector<particle_t>& gt) {

    // Get file of position measurements:
    std::ifstream in_file_pos(filename.c_str(), std::ifstream::in);
    // Return if we can't open the file.
    if (!in_file_pos) {
        return false;
    }

    // Declare single line of position measurement file:
    std::string line_pos;

    // Run over each single line:
    while (getline(in_file_pos, line_pos)) {

        std::istringstream iss_pos(line_pos);

        // Declare position values:
        double x, y, azimuth;

        // Declare single ground truth:
        particle_t single_gt;

        //read data from line to values:
        iss_pos >> x;
        iss_pos >> y;
        iss_pos >> azimuth;

        // Set values
        single_gt.x = x;
        single_gt.y = y;
        single_gt.th = azimuth;

        // Add to list of control measurements and ground truth:
        gt.push_back(single_gt);
    }
    return true;
}


bool read_landmark_data(std::string filename, std::vector<landmark_t>& observations) {

    // Get file of landmark measurements:
    std::ifstream in_file_obs(filename.c_str(), std::ifstream::in);
    // Return if we can't open the file.
    if (!in_file_obs) {
        return false;
    }

    // Declare single line of landmark measurement file:
    std::string line_obs;

    // Run over each single line:
    while (getline(in_file_obs, line_obs)) {

        std::istringstream iss_obs(line_obs);

        // Declare position values:
        double local_x, local_y;

        //read data from line to values:
        iss_obs >> local_x;
        iss_obs >> local_y;

        // Declare single landmark measurement:
        landmark_t meas;

        // Set values
        meas.x = local_x;
        meas.y = local_y;
        meas.id = 0;

        // Add to list of control measurements:
        observations.push_back(meas);
    }
    return true;
}

//#endif /*__PARTICLE_HELPER__*/