#ifndef workload_creator_H
#define workload_creator_H

#include "zns_simulation_datastructure.h"
#include "zns_simulation.h"
#include <iostream>

//Control zone, segment, block state
#define VALID_BLOCK 1
#define INVALID_BLOCK 2
#define FREE_BLOCK 0

using namespace std;

class Workload_Creator {
    //ZNS SSD variable
    struct zns_share_info * zns_info_list;

    int * update_block;
    int Zone_count;
    int Update_count;

public :
    Workload_Creator(zns_share_info * zonelist, int zone_count, int update_count);

    // create workload function
    int *create_sequential_workload(SIM_Zone * Zone_bitmap, SIM_Segment * Segment_bitmap, SIM_Block * Block_bitmap);
    int *create_random_workload(SIM_Zone * Zone_bitmap, SIM_Segment * Segment_bitmap, SIM_Block * Block_bitmap);
    int *create_zipfian_workload(SIM_Zone * Zone_bitmap, SIM_Segment * Segment_bitmap, SIM_Block * Block_bitmap);

    void ascending_sort(int * block_sort);

    // update block function
    int update_block_in_memory(SIM_Zone * Zone_bitmap, SIM_Segment * Segment_bitmap, SIM_Block * Block_bitmap, int * _update_bitmap);
};

#endif