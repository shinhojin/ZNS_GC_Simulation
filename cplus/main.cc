#include "controller.h"

int main()
{
    //Basic Data Setting
    //block size : 512, 512 * 8 = 4KB = I/O Basic Size
    char * test_data = new char[BLOCK_SIZE * 4];
    memset(test_data, 5, BLOCK_SIZE * 4);
    strcpy(test_data, "ZNS SSD Write Test!");

    //Start
    printf("Hello ZNS SSd world!!\n\n");
    
    //ZNS SSD Init
    struct zns_share_info * zns_info_list = new struct zns_share_info;
    zns_init("/dev/nvme0n1", zns_info_list);
    //ZNS SSD Info print
    //zns_init_print(zns_info_list);

    zns_get_total_log_entry_info(zns_info_list->fd, 530);

    //Read/Write Test
    //zns_write(zns_info_list, test_data, BLOCK_SIZE * 4, 0, 0);
    //memset(test_data, 0, BLOCK_SIZE * 8);
    //zns_read(zns_info_list, test_data, BLOCK_SIZE * 8, 0, 0);
    //printf("Read Result : %s\n", test_data);
    //zns_zone_open(zns_info_list, 0);
    /*for (int i = 0; i <= 530; i++) {
    	zns_zone_reset(zns_info_list, i);
    }*/
    //zns_get_total_log_entry_info(zns_info_list->fd, 30);

    

    return 0;
}
