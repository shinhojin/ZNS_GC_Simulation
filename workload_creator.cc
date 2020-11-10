#include "m2controller.h"
#include "workload_creator.h"
#include <fstream>
#include <string.h>

Workload_Data * Workload_Creator::get_workload_data(string data) {
    char * token_buffer = new char[200];
    int token_SN;
    double token_time;
    int token_PID;
    string token_event;
    int token_start_lba;
    int token_blocks;
    string token_process;

    static int count = 0;

    strcpy(token_buffer, data.c_str());
    
    char * token = strtok(token_buffer, " ");
    token_SN = atoi(token);

    token = strtok(NULL, " ");
    token_time = atof(token);

    token = strtok(NULL, " ");
    token_PID = atoi(token);

    //pass
    token = strtok(NULL, " ");
    
    token = strtok(NULL, " ");
    token_event = string(token);

    token = strtok(NULL, " ");
    token_start_lba = atoi(token);

    token = strtok(NULL, " ");
    token_blocks = atoi(token);

    //pocess temp
    token_process = string("NA");

    delete token_buffer;

    count++;
    return new Workload_Data(token_SN, token_time, token_PID, token_event, token_start_lba, token_blocks, token_process);
}

void Workload_Creator::get_workload_job(string path) {
    ifstream readFile;
    string raw_data;
    Workload_Data * data;

    int count = 0;

    readFile.open(path);

    if(!readFile.is_open()) {
        cout<< "fail open workload data file" <<endl;
    }
    
    while(getline(readFile, raw_data)) {
        data = get_workload_data(raw_data);
        //data->print_workload_data();
        workload_list->push_back(data);
        count++;
    }

    cout<<"Data Line : "<< count <<endl;
    readFile.close();
}

void Workload_Creator::print_all_workload() {
    if(workload_list->size() == 0) {
        cout<< "Workload empty" <<endl;
        return;
    }

    list<Workload_Data * >::iterator iter;
    for (iter = workload_list->begin(); iter != workload_list->end(); ++iter){
        (*iter)->print_workload_data();
    }
}

list<Workload_Data * > * Workload_Creator::getWorkloadlist() {
    return workload_list;
}
