#include <pthread.h>
#include <sys/time.h>
#include "controller.h"
#include "workload_creator.h"

int get_workload_data(char * line, struct workload_data * data) {
    
    char * token_data;
    int token_SN;
    double token_time;
    int token_PID;
    char token_event[5];
    int token_start_lba;
    int token_blocks;

    token_data = strtok(line, " ");
    token_SN = atoi(token_data);
    data->SN = token_SN;

    token_data = strtok(NULL, " ");
    token_time = atof(token_data);
    data->time = token_time;

    token_data = strtok(NULL, " ");
    token_PID = atoi(token_data);
    data->PID = token_PID;

    //pass
    token_data = strtok(NULL, " ");
    
    token_data = strtok(NULL, " ");
    strcpy(data->event, token_data);

    token_data = strtok(NULL, " ");
    token_start_lba = atoi(token_data);
    data->start_lba = token_start_lba;

    token_data = strtok(NULL, " ");
    token_blocks = atoi(token_data);
    data->blocks = token_blocks;

    return 0;
}

void get_workload_job(struct workload_data * data, FILE * workload_file) {
    char workload_buffer[WORKLOAD_BUFFER_SIZE];
    fgets(workload_buffer, WORKLOAD_BUFFER_SIZE, workload_file);
    get_workload_data(workload_buffer, data);
}


void print_workload_data_line(struct workload_data * data) {
    printf("data : %d, %f, %d, %s, %d, %d\n",
            data->SN,
            data->time,
            data->PID,
            data->event,
            data->start_lba,
            data->blocks);
}