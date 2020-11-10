#ifndef Workload_creator_H
#define Workload_creator_H

#include <list>
#include "workload_data.h"

#define WORKLOAD_BUFFER_SIZE 1024

using namespace std;

class Workload_Creator {
    list<Workload_Data * > * workload_list;
public :
    ~Workload_Creator() {};
    Workload_Creator() {
        workload_list = new list<Workload_Data * >;
    };
    Workload_Data * get_workload_data(string data);
    void get_workload_job(string path);
    void print_all_workload();
    list<Workload_Data * > * getWorkloadlist();
};

#endif