/* 2020. 09. 22
 * Creater : Gunhee Choi
 * Modifier : Hojin Shin
 * This file implements the Simulation of Data Structure
*/

#include "zns_simulation_datastructure.h"

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

void SIM_Zone::set_zone_info(int i_zone, int i_start_segment, int i_start_block) {
        this->i_zone = i_zone;
        this->i_start_segment = i_start_segment;
        this->i_start_block = i_start_block;
        this->utilization = 0;
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
    if( status == DEFAULT_STATUS_SEGMENT ) cout<<"DEFAULT"<<endl;
    else if( status == H1_STATUS_SEGMENT ) cout<<"HOT 1"<<endl;
    else if( status == C1_STATUS_SEGMENT ) cout<<"COLD 1"<<endl;
    else if( status == C2_STATUS_SEGMENT ) cout<<"COLD 2"<<endl;
}

//SIM Block Functions
SIM_Block::SIM_Block(int i_block) {
    this->i_block = i_block;
}

void SIM_Block::set_block_info(int _i_block) {
    i_block = _i_block;
    data = EMPTY_DATA;
}

void SIM_Block::print_block_info() {
    cout<< "Block index : " << i_block <<endl;
}

int SIM_Block::get_i_block() {
    return i_block;
}

int SIM_Block::get_data() {
    return data;
}

int SIM_Block::set_data(int data) {
    this->data = data;
}

