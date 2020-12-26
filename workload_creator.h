#ifndef workload_creator_H
#define workload_creator_H

#include "zns_simulation.h"
#include <iostream>

//Control zone, segment, block state
#define VALID_BLOCK 1
#define INVALID_BLOCK 2
#define FREE_BLOCK 0

using namespace std;

class Workload_Creator {
    //ZNS SSD variable
    struct m2_zns_share_info * zns_info_list;

    int Dev_num;
    int * update_block;
    int Zone_count;
    int Update_count;
    float zone_util;
    int cal_util_block;

public :
    // Constructor for M2 ZNS SSD
    Workload_Creator(m2_zns_share_info * zonelist, int zone_count, int dev_num, float zone_util);
    // Constructor for U3 ZNS SSD
    Workload_Creator(int zone_count, int dev_num, float zone_util);

    // create workload function
    int *create_sequential_workload(SIM_Zone * Zone_bitmap, SIM_Segment * Segment_bitmap, SIM_Block * Block_bitmap);
    int *create_random_workload(SIM_Zone * Zone_bitmap, SIM_Segment * Segment_bitmap, SIM_Block * Block_bitmap);

    // update block function
    pair<int,int>m2_update_block_in_memory(SIM_Zone * Zone_bitmap, SIM_Segment * Segment_bitmap, SIM_Block * Block_bitmap, int * _update_bitmap);
    pair<int,int>u3_update_block_in_memory(SIM_Zone * Zone_bitmap, SIM_Segment * Segment_bitmap, SIM_Block * Block_bitmap, int * _update_bitmap);
};

#endif