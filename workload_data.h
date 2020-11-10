#ifndef Workload_data_H
#define Workload_data_H

#include <string>

using namespace std;

class Workload_Data {
    int SN;
    double time;
    int PID;
    string event;
    long start_lba;
    long blocks;
    string process;
public :
    ~Workload_Data() {};
    Workload_Data() {};
    Workload_Data(int SN, double time, int PID, string event, int start_lba, int blocks, string process) {
        this->SN = SN;
        this->time = time;
        this->PID = PID;
        this->event = event;
        this->start_lba = start_lba;
        this->blocks = blocks;
        this->process = process;
    };
    void print_workload_data();
    int get_SN();
    double get_time();
    int get_PID();
    string get_event();
    long get_start_lba();
    long get_blocks();
    string get_process();
};

#endif