#include "controller.h"
#include "zns_simulation.h"
#include "workload_creator.h"
#include <sys/resource.h>
#include <pthread.h>

int main() {
    FILE * workload_file;

    struct workload_data * temp_token_data = malloc(sizeof(struct workload_data));

    printf("Hello ZNS SSD world!!\n\n");

    workload_file = fopen("./test_data/Entries_100000.txt", "r");
    if( !workload_file ) {
        printf("Workload data Open Fail\n");
    }

    get_workload_job(temp_token_data, workload_file);
    print_workload_data_line(temp_token_data);
    
    return 0;
}
