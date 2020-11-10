#include "m2controller.h"
#include "workload_data.h"

void Workload_Data::print_workload_data(){
    cout<< SN << " "
        << time << " "
        << PID << " "
        << event << " "
        << start_lba << " "
        << blocks << " "
        << process << " "
    <<endl;
};

int Workload_Data::get_SN() {
    return this->SN;
}

double Workload_Data::get_time() {
    return this->time;
}

int Workload_Data::get_PID() {
    return this->PID;
}

string Workload_Data::get_event() {
    return this->event;
}

long Workload_Data::get_start_lba() {
    return this->start_lba;
}

long Workload_Data::get_blocks() {
    return this->blocks;
}

string Workload_Data::get_process() {
    return this->process;
}