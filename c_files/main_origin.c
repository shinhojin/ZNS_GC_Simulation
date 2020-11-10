#include "controller.h"
#include "zns_simulation.h"
#include <sys/resource.h>
#include <pthread.h>

int main() {
    //ZNS SSD Variable
    struct zns_share_info * zns_info_list;

    //Time Variable
    struct timeval time_start, time_end;
    double time_result=0;

    //Experiment Variable
    void * buffer_128KB_write = malloc(512 * 8 * 32);
    void * buffer_128KB_read = malloc(512 * 8 * 32);

    void * write_init_data = malloc(512 * 8 * 32);
    memset(write_init_data, 99, 512 * 8 * 32);

    //Bitmap Variable
    int i_zone, i_segment, i_block;
    char * block_bitmap = (char *) malloc(sizeof(char) * 512 * 512 * MAX_ZONE_COUNT);
    ZNS_Zone * zones = malloc(sizeof(ZNS_Zone) * MAX_ZONE_COUNT);

    //Temp Variable
    int i,u;
    int temp=0;
    int start, end;
    int count;
    printf("Hello ZNS SSD world!!\n\n");

    //ZNS SSD init
    zns_info_list = malloc(sizeof(struct zns_share_info));
    zns_init("/dev/nvme0n1", zns_info_list);
    //init_zone_reset(zns_info_list, 530);
    init_overprovision_zones(zns_info_list);

    //Bitmap setting
    gettimeofday(&time_start, NULL);

    //read bitmap set
    printf("data set up start!\n");
    setup_bitmap_read_file(block_bitmap);
    printf("data set up end!\n");

    for(i_zone=0; i_zone<MAX_ZONE_COUNT; i_zone++)
        zone_bitmap_init(&zones[i_zone], i_zone, block_bitmap);

    //128KB zones init
    //fill_zns_ssd_128KB(zns_info_list);

    gettimeofday(&time_end, NULL);
    time_result = time_end.tv_sec + time_end.tv_usec / 1000000.0 - time_start.tv_sec - time_start.tv_usec/10000000.0;
    printf("Utilization Setting Time %lf\n\n", time_result);

    //print_multi_segment_in_zone(zones, 10);

    sort_zones(zones);
    print_zones_info(zones, 8);
    //print_block_bitmap_in_segment(&zones[1], 2, block_bitmap);


/*
    //Basic gc
    printf("Start Basic GC!!\n");
    gettimeofday(&time_start, NULL);

    for(i=0; i<2; i++) {
        basic_garbage_collection(zns_info_list, zones, MAX_ZONE_COUNT, block_bitmap, 512*512);
        init_overprovision_zones(zns_info_list);
    }

    gettimeofday(&time_end, NULL);
    time_result = time_end.tv_sec + time_end.tv_usec / 1000000.0 - time_start.tv_sec - time_start.tv_usec/10000000.0;
    printf("Basic GC Time %lf\n\n", time_result);
*/

  //LSM gc
    printf("Start LSM GC!!\n");
    gettimeofday(&time_start, NULL);

    for(i=0; i<2; i++) {
        lsm_garbage_collection(zns_info_list, zones, MAX_ZONE_COUNT, block_bitmap, 512*512);
        init_overprovision_zones(zns_info_list);
    }
    
    gettimeofday(&time_end, NULL);
    time_result = time_end.tv_sec + time_end.tv_usec / 1000000.0 - time_start.tv_sec - time_start.tv_usec/10000000.0;
    printf("LSM GC Time %lf\n\n", time_result); 

    //Start Recover
    //pthread_join(p_t[0], (void **)&status);
    printf("Start GC recover\n");
    for(i=0; i<520; i++) {
        if((zns_info_list->zone_list)[i].zone_entry.write_pointer == 0) {
            fill_zns_ssd_zone_128KB(zns_info_list, i);
        }
    }

    free(zones);
    free(block_bitmap);
    printf("End ZNS SSD\n");
    return 0;
}