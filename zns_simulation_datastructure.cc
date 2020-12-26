/* 2020. 09. 22
 * Creater : Gunhee Choi
 * Modifier : Hojin Shin
 * This file implements the Simulation of Data Structure
*/

#include "zns_simulation_datastructure.h"
#include "zns_simulation.h"

using namespace std;

//SIM Zone Functions
int SIM_Zone::get_i_zone() {
    return i_zone;
}

int SIM_Zone::get_i_start_segment() {
    return i_start_segment;
}

int SIM_Zone::get_i_start_block() {
    return i_start_block;
}

float SIM_Zone::get_utilization() {
    return utilization;
}

void SIM_Zone::set_utilization(float valid_cnt) {
    float zone_util;
    //zone_util = (valid_cnt / (float)(SEGMENT_COUNT_IN_ZONE * BLOCK_COUNT_IN_SEGMENT)) * 100;
    this->utilization = zone_util;
}

int SIM_Zone::m2_get_valid_blocks(SIM_Zone * Zone_ctl, SIM_Block * Block_ctl, int i_zone) {
    int start_i_block = Zone_ctl[i_zone].get_i_start_block();
    int end_i_block = start_i_block + M2_SEGMENT_COUNT_IN_ZONE * M2_BLOCK_COUNT_IN_SEGMENT;

    for (int block_num = start_i_block; block_num < end_i_block; block_num++) {
        if (Block_ctl[block_num].get_state() == VALID_BLOCK)
            valid_block_count++;
    }

    return valid_block_count;
}

int SIM_Zone::u3_get_valid_blocks(SIM_Zone * Zone_ctl, SIM_Block * Block_ctl, int i_zone) {
    int start_i_block = Zone_ctl[i_zone].get_i_start_block();
    int end_i_block = start_i_block + U3_SEGMENT_COUNT_IN_ZONE * U3_BLOCK_COUNT_IN_SEGMENT;

    for (int block_num = start_i_block; block_num < end_i_block; block_num++) {
        if (Block_ctl[block_num].get_state() == VALID_BLOCK)
            valid_block_count++;
    }

    return valid_block_count;
}

void SIM_Zone::reset_valid_blocks() {
    valid_block_count = 0;
}

void SIM_Zone::set_zone_info(int i_zone, int i_start_segment, int i_start_block) {
        this->i_zone = i_zone;
        this->i_start_segment = i_start_segment;
        this->i_start_block = i_start_block;
        this->utilization = 0;
        this->valid_block_count = 0;
}

void SIM_Zone::print_zone_info() {
    cout<< "Zone index : " << i_zone <<endl;
    cout<< "Zone i_start_segment : "<< i_start_segment <<endl;
    cout<< "Zone i_start_block : " << i_start_block <<endl;
    cout<< "Zone Utilization : " << utilization <<endl;
}

//SIM Segment Functions
int SIM_Segment::get_i_segment() {
    return i_segment;
}

int SIM_Segment::get_i_start_block() {
    return i_start_block;
}

int SIM_Segment::get_status() {
    return status;
}

int SIM_Segment::set_status(int status) {
    this->status = status;
}

void SIM_Segment::set_segment_info(int i_segment, int i_start_block, int status) {
    this->i_segment = i_segment;
    this->i_start_block = i_start_block;
    this->status = status;
}

void SIM_Segment::print_segment_info() {
    cout<< "Segment index : " << i_segment <<endl;
    cout<< "start i_block : " << i_start_block <<endl;
    print_status();
}

void SIM_Segment::print_status() {
    if( status == WARM_SEGMENT ) cout<<"WARM"<<endl;
    else if( status == HOT_SEGMENT ) cout<<"HOT"<<endl;
    else if( status == COLD_SEGMENT ) cout<<"COLD"<<endl;
}

//SIM Block Functions
void SIM_Block::set_block_info(int _i_block) {
    i_block = _i_block;
    state = FREE_BLOCK;
}

void SIM_Block::print_block_info() {
    cout<< "Block index : " << i_block <<endl;
}

int SIM_Block::get_i_block() {
    return i_block;
}

int SIM_Block::get_state() {
    return state;
}

int SIM_Block::set_state(int state) {
    this->state = state;
}
