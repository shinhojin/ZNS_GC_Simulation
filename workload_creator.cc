#include "workload_creator.h"

#include <string.h>
#include <stdio.h>
#include <iostream>
#include <time.h>

Workload_Creator::Workload_Creator(zns_share_info * zonelist, int zone_count, int update_count) {
    this->zns_info_list = zonelist;

    this->Zone_count = zone_count;
    this->Update_count = update_count;
}

int *Workload_Creator::create_sequential_workload(SIM_Zone * Zone_bitmap, SIM_Segment * Segment_bitmap, SIM_Block * Block_bitmap) {
    update_block = new int[Update_count];
    int seq_num;
    int total_block_num = Zone_count * 512 * 512 - 1;

    if (Update_count > total_block_num) {
        cout << "Too many update count!" << endl;
        cout << "Please set update count under " << total_block_num << endl;
        exit(0);
    }



    
}

int *Workload_Creator::create_random_workload(SIM_Zone * Zone_bitmap, SIM_Segment * Segment_bitmap, SIM_Block * Block_bitmap) {
    update_block = new int[Update_count];
    srand(time(NULL));

    int total_block_num = Zone_count * 512 * 512 - 1;

    if (Update_count > total_block_num) {
        cout << "Too many update count!" << endl;
        cout << "Please set update count under " << total_block_num << endl;
        exit(0);
    }

    for(int i  = 0; i < Update_count; i++) {
        update_block[i] = rand() % total_block_num;
    }

    // Sorting Block number
    ascending_sort(update_block);

    return update_block;
}

int *Workload_Creator::create_zipfian_workload(SIM_Zone * Zone_bitmap, SIM_Segment * Segment_bitmap, SIM_Block * Block_bitmap) {
    update_block = new int[Update_count];
}

void Workload_Creator::ascending_sort(int * block_sort) {
    int temp;

    for (int i = 0; i < Update_count; i++) {
        for (int j = i; j < Update_count; j++) {
            if (block_sort[i] > block_sort[j]) {
                temp = block_sort[i];
                block_sort[i] = block_sort[j];
                block_sort[j] = temp;
            }
        }
    }
}

int Workload_Creator::update_block_in_memory(SIM_Zone * Zone_bitmap, SIM_Segment * Segment_bitmap, SIM_Block * Block_bitmap, int * _update_bitmap) {
    int start_block = 0;
    int end_block = Zone_count * 512 * 512;
    int update_cnt = 0;
    int io_result = 0;
    int sel_zone = Zone_count;
    int write_offset = 0;
    int i_current_write_buffer = 0;
    
    void * dummy_data = new char[ZNS_BLOCK_SIZE * 32];
    memset(dummy_data, 66, ZNS_BLOCK_SIZE * 32);

    cout << "Print Zone Utilization Before Update" << endl;
    cout<< "------------------------------------------------------" <<endl;
    for (int i = 0; i < Zone_count; i++) {
        cout << "Zone " << i << " : " << Zone_bitmap[i].get_utilization() << endl; 
    }
    cout<< "------------------------------------------------------" <<endl;

    cout << "Update in memory ...ing" << endl;
    for (int i_block = start_block; i_block < end_block; i_block++) {
        for (int j_block = 0; j_block < Update_count; j_block++) {
            if (i_block == _update_bitmap[j_block]) {
                Block_bitmap[i_block].set_state(INVALID_BLOCK);
                update_cnt++;
                //cout << "through here 1" << endl;
                break;
            }
        }
        if (update_cnt == 32) {
            //cout << "through here 2" << endl;
            io_result = m2_zns_write(zns_info_list, dummy_data, (512 * 8 * 32), sel_zone, write_offset * 8);
            if (io_result == 0) {
                write_offset += update_cnt;
                update_cnt = 0;
            } else {
                cout << "ZNS SSD Write Fail" << endl;
                exit(0);
            }
        }
    }
    if (update_cnt < 32) {
        //io_result = m2_zns_write(zns_info_list, dummy_data, (512 * 8 * 32), sel_zone, write_offset * 8);
        if (io_result == 0) {
            write_offset += update_cnt;
            update_cnt = 0;
        } else {
            cout << "ZNS SSD Write Fail" << endl;
            exit(0);
        }
    }
    cout << "Finish Update in memory" << endl;
    return write_offset;
}