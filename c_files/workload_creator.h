#define WORKLOAD_BUFFER_SIZE 1024

struct workload_data {
    int SN;
    double time;
    int PID;
    char event[5];
    int start_lba;
    int blocks;
    char * process;
};

int get_workload_data(char * line, struct workload_data * data);
void get_workload_job(struct workload_data * data, FILE * workload_file);
void print_workload_data_line(struct workload_data * data);