/* 2020. 09. 22
 * Creater : Gunhee Choi
 * Modifier : -
 * This file defines the Simulation of Data Structure
*/

#include <iostream>

#ifndef ZNS_Simulation_datastructure_H
#define ZNS_Simulation_datastructure_H

//Define Segment State
#define DEFAULT_STATUS_SEGMENT 0
#define H1_STATUS_SEGMENT 1
#define C1_STATUS_SEGMENT 2
#define C2_STATUS_SEGMENT 3

//Define Block Data State
#define EMPTY_DATA 0
#define VALID_DATA 1
#define INVALID_DATA 2

using namespace std;

//Zone
class SIM_Zone {
    int i_zone;
    int i_start_segment;
    int i_start_block;
    float utilization;
    int vaild_block_count;

public :
    SIM_Zone() {};
    void set_zone_info(int _i_zone, int _i_start_segment, int _i_start_block);
    void print_zone_info();

    int get_i_zone();
    int get_i_start_segment();
    int get_i_start_block();
    float get_utilization();
};

//Segment
class SIM_Segment {
    int i_segment;
    int i_start_block;
    int status;
    int vaild_block_count;

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

//Block
class SIM_Block {
    int i_block;
    int data;
public :
    SIM_Block() {};
    SIM_Block(int i_block);

    void set_block_info(int i_block);
    void print_block_info();

    int get_i_block();
    int get_data();
    int set_data(int data);
};

#endif
