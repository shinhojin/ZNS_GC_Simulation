/* 2020. 09. 22
 * Creater : Gunhee Choi
 * Modifier : -
 * This file defines the Simulation of Data Structure
*/

#ifndef ZNS_Simulation_datastructure_H
#define ZNS_Simulation_datastructure_H

#include <iostream>

using namespace std;

//Block
class SIM_Block {
    int i_block;
    int state;

public :
    SIM_Block() {};

    void set_block_info(int i_block);
    void print_block_info();

    int get_i_block();
    int get_state();
    int set_state(int state);
};

//Segment
class SIM_Segment {
    int i_segment;
    int i_start_block;
    int status;
    int valid_block_count;

public :
    SIM_Segment() {};
    void set_segment_info(int i_segment, int i_start_block, int status);
    void print_segment_info();
    void print_status();
    int get_i_segment();
    int get_i_start_block();
    int get_status();
    int set_status(int status);
};

//Zone
class SIM_Zone {
    int i_zone;
    int i_start_segment;
    int i_start_block;
    float utilization;
    int valid_block_count;

public :
    SIM_Zone() {};
    void set_zone_info(int _i_zone, int _i_start_segment, int _i_start_block);
    void print_zone_info();

    int get_i_zone();
    int get_i_start_segment();
    int get_i_start_block();
    float get_utilization();
    void set_utilization(float valid_cnt);
    int m2_get_valid_blocks(SIM_Zone * Zone_ctl, SIM_Block * Block_ctl, int i_zone);
    int u3_get_valid_blocks(SIM_Zone * Zone_ctl, SIM_Block * Block_ctl, int i_zone);
    void reset_valid_blocks();
};

#endif
