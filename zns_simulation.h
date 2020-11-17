/* 2020. 09. 22
 * Creater : Gunhee Choi
 * Modifier : -
 * This file defines the Simulation Class
*/

#ifndef ZNS_simulation_H
#define ZNS_simulation_H

#include "workload_data.h"
#include "m2controller.h"
//#include "u3controller.h"
#include "zns_simulation_datastructure.h"
#include <string>
#include <list>
#include <map>
#include <random>
#include <iostream>

//In Simulation, Set hardware zone spec
#define M2_ZONE_MAX_COUNT 530

//In Simulation, Set default IO size : 4KB = 512 * 8
#define SIM_BLOCK_IO_DEFAULT 8

//In Simulation, Set default Block size, block count : 4KB, 0x35e500
#define ZNS_BLOCK_SIZE 512 * 8
#define ZNS_ZONE_BLOCK_COUNT 0x35e500

//In Simulation, Set Zone spec
#define BLOCK_COUNT_IN_SEGMENT 512
#define SEGMENT_COUNT_IN_ZONE 512

//Control zone, segment, block state
#define VALID_BLOCK 1
#define INVALID_BLOCK 2
#define FREE_BLOCK 0

#define HOT_SEGMENT 0
#define COLD_SEGMENT 1
#define FREE_SEGMENT 2

using namespace std;

class Block_data {
    char data[4096];
};

class ZNS_Simulation {
    //ZNS SSD init variable
    struct zns_share_info * zns_info_list;
    char * workload_type;

    //Setting ZNS SSD Argument
    int Zone_count;
    float Dev_util;
    int Segment_count;
    int Block_count;

    //Simualtion bitmap
    SIM_Zone * Zone_bitmap;
    SIM_Segment * Segment_bitmap;
    SIM_Block * Block_bitmap;
    int total_segment_count;
    int total_block_count;

    //map<start_lab, i_bitmap>
    map <int, int> serialize_map;

    int current_i_block_bitmap;

public :
    ZNS_Simulation(char * path, float dev_util, char * workload_type);
    //init function
    void init_block_bitmap();
    void init_segment_bitmap();
    void init_zone_bitmap();

    int execute_workload_line();
    
    int write_data();
    int read_valid_data(int i_block);
    int basic_zgc();
    int lsm_zgc();

    int request_write(int start_lba, int blocks);

    int init_zones_write(int numofzones);
    void init_zone_reset(int numofzones);
    void init_all_zones_reset();
    
    int print_zns_totalzones();

    int get_offset_in_zone(int i_segment, int i_block);
    int get_i_bitmap(int i_zone, int i_segment, int i_block);

    //test workolad function (not real workload)
    //ref: https://boycoding.tistory.com/192
    int setting_random_bitmap();

    //print function
    void print_block_info(int offset);
    void print_segment_info(int offset);
    void print_zone_info(int offset);
    void print_segment_block_bitmap(int i_segment);
    void print_zone_block_bitmap(int i_zone);
    void print_zone_segment_bitmap(int i_zone);
};

#endif
