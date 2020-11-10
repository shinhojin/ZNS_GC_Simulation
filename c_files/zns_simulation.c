#include <pthread.h>
#include <sys/time.h>
#include "controller.h"
#include "zns_simulation.h"

int overprovisioning_zone = OVERPROVISIONING_STRART_ZONE;
int overprovisioning_offset = 0; 

/*
Init Stage Function
*/
int init_zone_reset(struct zns_share_info * zns_info_list, int zones)
{
    int i;

    for(i=0; i<zones; i++)
    {
        zns_zone_reset(zns_info_list, i);
    }

    overprovisioning_zone = OVERPROVISIONING_STRART_ZONE;
    overprovisioning_offset = 0;
    
    return 0;
}

int init_zones_write(struct zns_share_info * zns_info_list, char * block_bitmap)
{
    int i_zone, i_segment, i_block;
    int offset;
    int write_result;
    
    //block size : 512, 512 * 8 = 4KB = I/O Basic Size
    void * test_data = malloc(SIM_BLOCK_SIZE);
    memset(test_data, 66, SIM_BLOCK_SIZE);

    for(i_zone=0; i_zone<MAX_ZONE_COUNT; i_zone++)
    {
	    printf("Fill Block BitMap zone %d start\n", i_zone);
	    //printf("Zone slab : %#"PRIx64"\n", get_zone_to_slba(zns_info_list, i_zone));
        for(i_segment=0; i_segment<SIM_ZONE_IN_SEGMENT_COUNT; i_segment++)
        {
            //printf("Zone %d, segment %d write start!\n", i_zone, i_segment);

            for(i_block=0; i_block<SIM_SEGMENT_IN_BLOCK_COUNT; i_block++)
            {
                offset = get_block_offset(i_segment, i_block);
                
                write_result = zns_write(zns_info_list,
                                        test_data, SIM_BLOCK_SIZE,
                                        i_zone, offset);
                
                if(write_result == 0)
                    block_bitmap[(get_startblock_zone_index(i_zone) + (offset/SIM_BLOCK_IO_DEFAULT))] = 1;
            } //end of i_blcok
        }// end of i_segment
	    zns_zone_finish(zns_info_list, i_zone);
        //printf("Zone %d finish\n\n", i_zone);
    }// end of i_zone 

    return 0;
}

int init_zones_write_segment(struct zns_share_info * zns_info_list, char * block_bitmap)
{
    int i_zone, i_segment, i_block;
    int offset;
    int offset_segment;
    int write_result;

    //block size : 512, 512 * 8 = 4KB = I/O Basic Size
    void * test_data = malloc(ZNS_BLOCK_SIZE*SIM_SEGMENT_IN_BLOCK_COUNT);
    memset(test_data, 66, ZNS_BLOCK_SIZE*SIM_SEGMENT_IN_BLOCK_COUNT);
    
    for(i_zone=0; i_zone<MAX_ZONE_COUNT; i_zone++)
    {
	    printf("Fill Block BitMap zone %d start\n", i_zone);
	    //printf("Zone slab : %#"PRIx64"\n", get_zone_to_slba(zns_info_list, i_zone));
        offset_segment = 0;
        for(i_segment=0; i_segment<SIM_ZONE_IN_SEGMENT_COUNT; i_segment++)
        {
            //printf("Zone %d, segment %d write start!\n", i_zone, i_segment);
            
            for(i_block=0; i_block<SIM_SEGMENT_IN_BLOCK_COUNT; i_block++)
            {
                offset = get_block_offset(i_segment, i_block);
                block_bitmap[(get_startblock_zone_index(i_zone) + (offset/SIM_BLOCK_IO_DEFAULT))] = 1;
            } //end of i_blcok
        }// end of i_segment
        //printf("Zone %d finish\n\n", i_zone);
    }// end of i_zone 

    for(i_zone=0; i_zone<MAX_ZONE_COUNT; i_zone++)
    {
	    printf("Zone %d write start\n", i_zone);
	    //printf("Zone slab : %#"PRIx64"\n", get_zone_to_slba(zns_info_list, i_zone));
        offset_segment = 0;
        for(i_segment=0; i_segment<SIM_ZONE_IN_SEGMENT_COUNT*8; i_segment++)
        {          
            offset_segment = i_segment * ZNS_BLOCK_SIZE;
            write_result = zns_write(zns_info_list,
                            test_data, ZNS_BLOCK_SIZE*SIM_SEGMENT_IN_BLOCK_COUNT,
                            i_zone, offset_segment);
        }// end of i_segment
	    zns_zone_finish(zns_info_list, i_zone);
        //printf("Zone %d finish\n\n", i_zone);
    }// end of i_zone 

    return 0;
}

int setup_util_zone(struct zns_share_info * zns_info_list, char * block_bitmap, int i_zone, int goal_util)
{
    int i;
    int result;
    int i_start_block = get_startblock_zone_index(i_zone);
    int deleted_block_count = 0;
    int i_random_offset;
    int i_block;

    int seed_arr[MAX_RANDOM_ARR];
    printf("Zone %d util start\n", i_zone);
    if(goal_util == 0) return 0;

    while(1)//deleted_block_count < goal_util);
    {
        int zone_in_blocks = SIM_SEGMENT_IN_BLOCK_COUNT * SIM_ZONE_IN_SEGMENT_COUNT;
        srand(time(NULL));
        for(i=0; i<MAX_RANDOM_ARR; i++)
        {
            seed_arr[i] = rand() % zone_in_blocks;
        }

        for(i=0; i<MAX_RANDOM_ARR; i++)
        {
            i_random_offset = seed_arr[i];
            i_block = i_random_offset + i_start_block;
            
            if(block_bitmap[i_block] == 1)
            {
                block_bitmap[i_block] = 0;
                deleted_block_count++;
                //printf("deleted data count : %d\n", deleted_block_count);

                if(deleted_block_count == goal_util)
                    break;
            }
        }
        
        if(deleted_block_count == goal_util)
            break;
    }
    printf("Zone %d util end\n", i_zone);

    return 0;
}

int setup_skewed_data(struct zns_share_info * zns_info_list, char * block_bitmap, int * segment_bitmap, int hot_goal)
{
    int i, j;
    int count;
    int seed_segs[512];
    int seed_blocks[512];
    int temp, temp_a, temp_b;
    int temp_segs[512];
    int temp_blocks[512];
    int i_segment, i_block;
    int goal;

    setup_util_zone(zns_info_list, block_bitmap, 0, Pecentage_10 * 5);
    
    goal =  Percentage_10_seg * (5-hot_goal) * Percentage_10_seg * hot_goal;
    printf("segment select for 20%\n"); 
    for(i=0; i<512; i++)
        seed_segs[i] = i;
    
    srand(time(NULL));
    for(i=0; i<5000; i++)
    {
        temp_a = rand() % 512;
        temp_b = rand() % 512;
        temp = seed_segs[temp_a];
        seed_segs[temp_a] = seed_segs[temp_b];
        seed_segs[temp_b] = temp;
    }

    printf("20%% down start!\n");
    
    i_segment = 0;
    i_block = 0;
    count = 0;
    while(1)
    {
         for(i=0; i<Percentage_10_seg * hot_goal; i++)
         {
                for(i_block=0; i_block<512; i_block++)
                {
                    if(block_bitmap[ seed_segs[i] * 512 + i_block] == 1)
                    {
                        block_bitmap[ seed_segs[i] * 512 + i_block ] = 0;
                        count++;
                        break;
                    }
                }
                if(count == goal)
                       break;
         }
         
         if(count == goal)
             break;
    }

    printf("80%% up start!\n");
    count=0;
    while(1)
    {
        for(i=Percentage_10_seg; i<512; i++)
        {
            for(i_block=0; i<512; i_block++)
            {
                if(block_bitmap[ seed_segs[i] * 512 + i_block] == 0)
                {
                    block_bitmap[ seed_segs[i] * 512 + i_block] = 1;
                    count++;
                    break;
                }
            }
            if(count == goal)
                break;
        }
        if(count == goal)
            break;
    }

}


/*
I/O Function
*/
int get_startblock_zone_index(int zone_number)
{   
    return zone_number * SIM_SEGMENT_IN_BLOCK_COUNT * SIM_ZONE_IN_SEGMENT_COUNT;
}

int get_startblock_segment_index(int zone_number, int i_segment)
{
    return (get_startblock_zone_index(zone_number) + i_segment * SIM_SEGMENT_IN_BLOCK_COUNT);
}

int get_block_offset(int i_segment, int i_block)
{
    return i_segment * SIM_ZONE_IN_SEGMENT_COUNT * SIM_BLOCK_IO_DEFAULT + i_block * SIM_BLOCK_IO_DEFAULT;
}

/*
int write_block(struct zns_share_info * zns_info_list, char * block_bitmap, int i_zone, int i_segment, int i_block, void * data, int data_size)
{
    int result;
    int offset = i_segment * 512 * 8 + i_block * 8;
    result = zns_write(zns_info_list, data, data_size, i_zone, offset);
    
    if(result == 0)
        block_bitmap[get_startblock_zone_index(i_zone) + (offset/8)] = 1;
    
    return result;
}

int read_block(struct zns_share_info * zns_info_list, char * block_bitmap, int i_zone, int i_segment, int i_block, void * data, int data_size)
{
    int result;
    int offset = i_segment * 512 * 8 + i_block * 8;
    result- zns_read(zns_info_list, data, data_size, i_zone, offset);
    
    return result;
}
*/

/*
Data Print Function
*/
void print_utilization_segments_in_zone(int i_zone, int * segment_bitmap, char * block_bitmap)
{
    int i_segment;

    for(i_segment=0; i_segment<SIM_ZONE_IN_SEGMENT_COUNT; i_segment++)
    {
        printf("%d\t", segment_bitmap[(i_zone * SIM_ZONE_IN_SEGMENT_COUNT + i_segment)]);
        //printf("%d\t",i_zone * SIM_ZONE_IN_SEGMENT_COUNT + i_segment);
        if((i_segment+1)%10 == 0)
            printf("\n");
    }
    printf("\n");
}

void print_total_utilization_segments_in_zone(int * segment_bitmap, int * block_bitmap)
{
    int i_zone;

    for(i_zone=0; i_zone<MAX_ZONE_COUNT; i_zone++)
    {
        printf("\nZone : %d\n", i_zone);
        print_utilization_segments_in_zone(i_zone, segment_bitmap, block_bitmap);
        printf("\n");
    }
}

void print_utilization_zone(int * segment_bitmap)
{
    int i_zone, i_segment;
    int sum_valid_segment = 0;

    for(i_zone=0; i_zone<MAX_ZONE_COUNT; i_zone++)
    {
        sum_valid_segment = 0;
        for(i_segment=0; i_segment<SIM_ZONE_IN_SEGMENT_COUNT; i_segment++)
        {
            if(segment_bitmap[i_zone * SIM_ZONE_IN_SEGMENT_COUNT + i_segment] != 0)
                sum_valid_segment++;
        }
        
        printf("%d\t", sum_valid_segment);
        if((i_zone+1)%10 == 0)
            printf("\n");
    }

    printf("\n");
}

/*
Bitmap Update
*/
int check_segment_util(char * block_bitmap, int i_zone, int i_segment)
{
    int i;
    int sum_valid_data = 0;
    int i_start_block = get_startblock_segment_index(i_zone, i_segment);
    
    for(i=0; i<SIM_SEGMENT_IN_BLOCK_COUNT; i++)
        sum_valid_data += block_bitmap[i_start_block+i];

    //printf("Zone %d, segment %d, sum %d\n", i_zone, i_segment, sum_valid_data);
    return sum_valid_data;
}

void update_bitmap(int * zone_bitmap, int * segment_bitmap, char * block_bitmap)
{
    update_segment_bitmap(segment_bitmap, block_bitmap);
    update_zone_bitmap(zone_bitmap, segment_bitmap);
}

void update_segment_bitmap(int * segment_bitmap, char * block_bitmap)
{
    int i_zone, i_segment;

    for(i_zone=0; i_zone<MAX_ZONE_COUNT; i_zone++)
    {
        for(i_segment=0; i_segment<SIM_ZONE_IN_SEGMENT_COUNT; i_segment++)
        {
            segment_bitmap[i_zone*SIM_ZONE_IN_SEGMENT_COUNT + i_segment] = check_segment_util(block_bitmap, i_zone, i_segment);
        }
    }
}

void update_zone_bitmap(int * zone_bitmap, int * segment_bitmap)
{
    int i_zone, i_segment;
    int sum_valid_segment = 0;

    for(i_zone=0; i_zone<MAX_ZONE_COUNT; i_zone++)
    {   
        sum_valid_segment = 0;
        for(i_segment=0; i_segment<SIM_ZONE_IN_SEGMENT_COUNT; i_segment++)
        {
            if(segment_bitmap[i_zone * SIM_ZONE_IN_SEGMENT_COUNT + i_segment] != 0)
                sum_valid_segment++;
        }
        zone_bitmap[i_zone] = sum_valid_segment;
    }
}

/*
GC Function
*/




/*
etc.
*/
int compare_segement(const void *a, const void *b)
{
    int num1 = ((Segment_data *)a)->utilization;    // void 포인터를 int 포인터로 변환한 뒤 역참조하여 값을 가져옴
    int num2 = ((Segment_data *)b)->utilization;   // void 포인터를 int 포인터로 변환한 뒤 역참조하여 값을 가져옴

    if (num1 < num2)    // a가 b보다 작을 때는
        return -1;      // -1 반환
    
    if (num1 > num2)    // a가 b보다 클 때는
        return 1;       // 1 반환
    
    return 0;    // a와 b가 같을 때는 0 반환
}

int compare_zone(const void *a, const void *b)
{
    int num1 = ((Zone_data *)a)->utilization;    // void 포인터를 int 포인터로 변환한 뒤 역참조하여 값을 가져옴
    int num2 = ((Zone_data *)b)->utilization;   // void 포인터를 int 포인터로 변환한 뒤 역참조하여 값을 가져옴

    if (num1 < num2)    // a가 b보다 작을 때는
        return -1;      // -1 반환
    
    if (num1 > num2)    // a가 b보다 클 때는
        return 1;       // 1 반환
    
    return 0;    // a와 b가 같을 때는 0 반환
}




//New
int init_overprovision_zones(struct zns_share_info * zns_info_list) {
    int i;

    for(i=START_OP_ZONE; i<525; i++)
    {
        zns_zone_reset(zns_info_list, i);
    }
    
    return 0;
}

int zone_bitmap_init(ZNS_Zone * zone, int zone_number, char * block_bitmap) {
    int i_segment, i_block;

    zone->zone_number = zone_number;
    
    //get N zone index block
    zone->i_start_block = zone_number * 512 * 512;
    zone->i_end_block = zone->i_start_block + 512 * 512;
    
    zone->valid_blocks = 0;
    for(i_segment = 0; i_segment<512; i_segment++) {
        //get N zone, i segment index block
        zone->segments[i_segment].i_start_block = zone->i_start_block + i_segment * 512;
        zone->segments[i_segment].i_end_block = zone->segments[i_segment].i_start_block + 512;
        
        zone->segments[i_segment].vaild_blocks = 0;
        zone->segments[i_segment].hot_cold_state = FREE_SEGMENT;

        for(i_block=0; i_block<512; i_block++) {
            if(block_bitmap[zone->segments[i_segment].i_start_block + i_block] == VALID_BLCOK) {
                zone->segments[i_segment].vaild_blocks++;
            }
        }//end of block

        if(zone->segments[i_segment].vaild_blocks > 408) {
            zone->segments[i_segment].hot_cold_state = COLD_SEGMENT;
        } else {
            zone->segments[i_segment].hot_cold_state = HOT_SEGMENT;
        }

        zone->valid_blocks = zone->valid_blocks + zone->segments[i_segment].vaild_blocks;
        
    }//end of segment
    
    //To modify ...ing
    zone->state = FREE_ZONE;

    return 0;
}

int setup_block_bitmap_utilization(char * block_bitmap, int goal, int n_zones, int setup_state) {
    int i;
    int i_block;
    int state = setup_state;
    int change_block_count = 0;
    
    int seed_variable = 512 * 512 * n_zones;
    int seed_block_arr[SEED_MAX];

    printf("Setup block bitmap\n");

    //Start Block Select
    while(1) {
        //Select Block Seed
        srand(time(NULL));
        for(i=0; i<SEED_MAX; i++)
            seed_block_arr[i] = rand() % seed_variable;
        
        //Select Valid block
        for(i=0; i<SEED_MAX; i++) {
            if(block_bitmap[seed_block_arr[i]] != state) {
                block_bitmap[seed_block_arr[i]]  = state;
                //printf("Valid block count %d\n", change_block_count);
                change_block_count++;

                if(goal == change_block_count)
                    goto Setup_End;
            } 
            
        }

    }//end of while

Setup_End :
    printf("Setup End block bitmap\n");
}


/*
int setup_zone_bitmap_utilization(ZNS_Zone * zone, int goal) {
    int i;
    int i_segment, i_block;
    int vaild_block_count = 0;

    int seed_segment_arr[SEED_MAX];
    int seed_block_arr[SEED_MAX];

    printf("Setup Zone %d Utilization\n", zone->zone_number);

    //Start Block Select
    while(1) {

        //Select Segment Seed
        srand(time(NULL));
        for(i=0; i<SEED_MAX; i++)
            seed_segment_arr[i] = rand() % 512;
        
        sleep(1);

        //Select Block Seed
        srand(time(NULL));
        for(i=0; i<SEED_MAX; i++)
            seed_block_arr[i] = rand() % 512;
        
        //Select Valid block
        for(i=0; i<SEED_MAX; i++) {
            if(zone->segments[seed_segment_arr[i]].blocks[seed_block_arr[i]].vaild == 0) {
                zone->segments[seed_segment_arr[i]].blocks[seed_block_arr[i]].vaild = 1;
                
                zone->segments[seed_segment_arr[i]].vaild_blocks++;
                zone->valid_blocks++;
                vaild_block_count++;
                
                //printf("Valid block count %d\n", vaild_block_count);

                if(goal == vaild_block_count)
                    goto Setup_End;
            } 
            
        }

    }//end of while

Setup_End :

    //Start Setup Zone Uilization
    for(i_segment=0; i_segment<512; i_segment++) {
        if(zone->segments[i_segment].vaild_blocks > 300)
            zone->segments[i_segment].hot_cold_state = HOT_SEGMENT;
    }

    printf("Setup End Zone %d Utilization\n", zone->zone_number);
}
*/


/*
   print segment Uitilization in zone unit
*/
void print_segment_bitmap_in_zone(ZNS_Zone * zone) {
    int i_segment;

    printf("Zone %d\n", zone->zone_number);
    for(i_segment = 0; i_segment<512; i_segment++) {
        printf("%d\t", zone->segments[i_segment].vaild_blocks);
        if((i_segment+1)%10 == 0)
            printf("\n");
    }
    printf("\n");
}

void print_block_bitmap_in_segment(ZNS_Zone * zone, int i_segment, char * block_bitmap) {
    int i_block;

    printf("Zone %d, Segemnt %d\n", zone->zone_number, i_segment);
    for(i_block = 0; i_block<512; i_block++) {
        printf("%d\t", block_bitmap[zone->segments[i_segment].i_start_block + i_block]);
        if((i_block+1)%10 == 0)
            printf("\n");
    }
    printf("\n");
}

void print_multi_segment_in_zone(ZNS_Zone * zone, int count) {
    int i_zone, i_segment;

    for(i_zone = 0; i_zone<count; i_zone++) {
        printf("Zone %d\n", zone[i_zone].zone_number);
        for(i_segment = 0; i_segment<512; i_segment++) {
            printf("%d  ", zone[i_zone].segments[i_segment].vaild_blocks);
            if((i_segment+1)%100 == 0)
                printf("\n");
        }
        printf("\n");
    }
}

void print_zones_info(ZNS_Zone * zone, int count) {
    int i_zone, i_segment;
    double utilization;

    for(i_zone = 0; i_zone<count; i_zone++) {
        if(zone[i_zone].valid_blocks == 0) continue;
        printf("Zone Number : %d\n", zone[i_zone].zone_number);
        //printf("Zone State : %d\n", zone[i_zone].state);
        printf("Zone valid_blocks : %d\n", zone[i_zone].valid_blocks);
        utilization = ((double)zone[i_zone].valid_blocks) / (512.0 * 512.0) * 100.0;
        printf("Zone Utilization : %lf\n", utilization);
        
        printf("\n");
    }   
}

void print_zones_info_zone(ZNS_Zone * zone) {
    int i_zone, i_segment;
    double utilization;

        printf("Zone Number : %d\n", zone->zone_number);
        printf("Zone State : %d\n", zone->state);
        printf("Zone valid_blocks : %d\n", zone->valid_blocks);
        utilization = ((double)zone->valid_blocks) / (512.0 * 512.0) * 100.0;
        printf("Zone Utilization : %lf\n", utilization);
        
        printf("\n");
}

/*
I/O func
*/
int get_read_block_count(char * block_bitmap, int i_block) {
    int index = i_block;
    int read_count = 0;

    while(1) {
        if(block_bitmap[index] == VALID_BLCOK) {
            read_count++;
            index++;
        } else break;

        if(read_count == 32) break;
    }

    return read_count;
}

int get_read_block_count_LSM(char * block_bitmap, int i_start, int i_end) {
    int read_count = 0;
    int i;

    for(i = i_start; i<i_end; i++) {
        if(block_bitmap[i] == VALID_BLCOK) {
            read_count++;
        } else break;
    }

    return read_count;
}

/*
GC Func
*/

int basic_garbage_collection(struct zns_share_info * zns_info_list, ZNS_Zone * zones, int n_zones, char * block_bitmap, int goal) {
    int i;
    int i_zone, i_segment, i_block;

    int collection_invalid_count = 0;
    int i_bitmap_current;
    int read_count;
    int io_result = 0;

    int i_zone_start_block;
    int i_zone_end_block;
    
    int write_zone = START_OP_ZONE;
    int offset_write_zone = 0;
    int i_current_write_buffer = 0; //MAX 32
    Block_data * buffer_128KB_read = (Block_data *) malloc(512 * 8 * 32);
    Block_data * buffer_128KB_write = (Block_data *) malloc(512 * 8 * 32);

    int buffer_write_temp_offset;
    int remain_read_offset;

    memset(buffer_128KB_read, 0, 512 * 8 * 32);
    memset(buffer_128KB_write, 0, 512 * 8 * 32);

    printf("Basic GC Start!\n");

    for(i_zone=0; i_zone<n_zones; i_zone++) {
        i_zone_start_block = zones[i_zone].i_start_block;
        i_zone_end_block = zones[i_zone].i_end_block;
        if(zones[i_zone].valid_blocks == 0) continue;
        printf("i_zone : %d, select zone : %d\n",i_zone, zones[i_zone].zone_number);
        printf("collection_invalid_count : %d\n", collection_invalid_count);

        for(i_bitmap_current = i_zone_start_block; i_bitmap_current <= i_zone_end_block; ) {
            //printf("i_bitmap_current : %d\n", i_bitmap_current);
            //blocks read
            if(block_bitmap[i_bitmap_current] == INVALID_BLOCK) {
                collection_invalid_count++;
                i_bitmap_current++;
                continue;
            }

            read_count = get_read_block_count(block_bitmap, i_bitmap_current);
            io_result = zns_read(zns_info_list, buffer_128KB_read, 512 * 8 * read_count, zones[i_zone].zone_number, (i_bitmap_current - 512*512*zones[i_zone].zone_number) * 8);
            if(io_result == 0) {
                i_bitmap_current += read_count;
                //printf("read_count : %d\n", read_count);
                //printf("IO result : %#x\n", io_result);
            } else {
                printf("goto Basic_GC_end\n");
                printf("read_count : %d\n", read_count);
                printf("IO result : %#x\n", io_result);
                goto Basic_GC_end;
            }

            //buffer write
            if( (i_current_write_buffer + read_count) < 32 ) { //check 128KB
                memcpy(&buffer_128KB_write[i_current_write_buffer], buffer_128KB_read, 512 * 8 * read_count);
                i_current_write_buffer += read_count;
            } else {
                if((i_current_write_buffer + read_count) == 32) { //128KB write
                    memcpy(&buffer_128KB_write[i_current_write_buffer], buffer_128KB_read, 512 * 8 * read_count);
                    io_result = zns_write(zns_info_list, buffer_128KB_write, 512 * 8 * 32, write_zone, offset_write_zone * 8);
                    if(io_result == 0) {
                        offset_write_zone += 32;
                        i_current_write_buffer = 0;
                    } else {
                        printf("write IO result : %#x\n", io_result);
                    }
                } else {
                    remain_read_offset = i_current_write_buffer + read_count - 32;
                    //printf("i_current_write_buffer : %d, read_count: %d, remain_read_offset : %d\n",i_current_write_buffer, read_count, remain_read_offset);
                    memcpy(&buffer_128KB_write[i_current_write_buffer], buffer_128KB_read, 512 * 8 * (read_count-remain_read_offset));
                    io_result = zns_write(zns_info_list, buffer_128KB_write, 512 * 8 * 32, write_zone, offset_write_zone * 8);
                    if(io_result == 0) {
                        offset_write_zone += 32;
                        i_current_write_buffer = 0;
                    } else {
                        printf("write IO result : %#x\n", io_result);
                    }

                    memcpy(&buffer_128KB_write[i_current_write_buffer], &buffer_128KB_read[read_count-remain_read_offset], 512 * 8 * remain_read_offset);
                    i_current_write_buffer += remain_read_offset;
                }
            }

            if( offset_write_zone  == (512*512) ) {
                zns_zone_finish(zns_info_list, write_zone);
                write_zone++;
                offset_write_zone = 0;
                printf("write zone : %d\n", write_zone);
            }

            if(collection_invalid_count >= goal) {
                if(i_current_write_buffer != 0) {
                    io_result = zns_write(zns_info_list, buffer_128KB_write, 512 * 8 * i_current_write_buffer, write_zone, offset_write_zone * 8);
                }
                goto Basic_GC_end;
            }
        }//end of zone blocks

        zns_zone_reset(zns_info_list, zones[i_zone].zone_number);
        zones[i_zone].valid_blocks = 0;
    }//end of zones

Basic_GC_end: 
    printf("Basic GC End\n");

    return 0;
}

int lsm_garbage_collection(struct zns_share_info * zns_info_list, ZNS_Zone * zones, int n_zones, char * block_bitmap, int goal) {
    int i;
    int i_zone, i_segment, i_block;

    int collection_invalid_count = 0;
    int i_bitmap_current;
    int read_count;
    int io_result = 0;

    int i_zone_start_block;
    int i_zone_end_block;
    
    int write_zone = START_OP_ZONE;
    int offset_write_zone = 0;
    int i_current_write_buffer = 0; //MAX 32
    int i_current_read_offset=0;

    Block_data * buffer_128KB_read = (Block_data *) malloc(512 * 8 * 32);
    Block_data * buffer_128KB_write = (Block_data *) malloc(512 * 8 * 32);

    int buffer_write_temp_offset;
    int remain_read_offset;

    memset(buffer_128KB_read, 0, 512 * 8 * 32);
    memset(buffer_128KB_write, 0, 512 * 8 * 32);

    printf("LSM GC Start!\n");

    for(i_zone=0; i_zone<n_zones; i_zone++) {
        i_zone_start_block = zones[i_zone].i_start_block;
        i_zone_end_block = zones[i_zone].i_end_block;
        if(zones[i_zone].valid_blocks == 0)
            continue;
        printf("i_zone : %d, select zone : %d\n",i_zone, zones[i_zone].zone_number);
        printf("collection_invalid_count : %d\n", collection_invalid_count);
        for(i_bitmap_current = i_zone_start_block; i_bitmap_current <= i_zone_end_block; i_bitmap_current += 32) {
            
            //blocks read
            io_result = zns_read(zns_info_list, buffer_128KB_read, 512 * 8 * 32, zones[i_zone].zone_number, (i_bitmap_current - 512*512*zones[i_zone].zone_number) * 8);
            if(io_result != 0) {
                printf("LSM GC Read X\n");
                goto LSM_GC_end;
            }
            //printf("128KB Read Start\n");
            i_current_read_offset=0;
            for(i=i_bitmap_current; i<i_bitmap_current+32;) { //128KB Read
                if(block_bitmap[i] == INVALID_BLOCK) {
                    collection_invalid_count++;
                    i++;
                    //printf("invaild 0, i : %d\n", i);
                } else {
                    read_count = get_read_block_count_LSM(block_bitmap, i, i_bitmap_current+32);
                    memcpy(&buffer_128KB_write[i_current_write_buffer], &buffer_128KB_read[i_current_read_offset], 512 * 8 * read_count);
                    //printf("read count : %d\n", read_count);
                    i = i + read_count;
                    i_current_read_offset = i_current_read_offset + read_count;
                    //printf("i : %d, i_current_read_offset : %d, read_count : %d\n",i,i_current_read_offset,read_count);
                }
            }
            //printf("128KB Read end\n");
            //printf("i : %d, i_current_read_offset: %d, i_current_write_buffer : %d\n",i, i_current_read_offset, i_current_write_buffer);
            //printf("\n");
            //printf("test6 i_current_write_buffer : %d\n", i_current_write_buffer);
             //buffer write
            if( (i_current_write_buffer + i_current_read_offset) < 32 ) { //check 128KB
                memcpy(&buffer_128KB_write[i_current_write_buffer], buffer_128KB_read, 512 * 8 * i_current_read_offset);
                i_current_write_buffer += i_current_read_offset;
            } else {
                //printf("test1 i_current_write_buffer : %d\n", i_current_write_buffer);
                if((i_current_write_buffer + i_current_read_offset) == 32) { //128KB write
                    memcpy(&buffer_128KB_write[i_current_write_buffer], buffer_128KB_read, 512 * 8 * i_current_read_offset);
                    io_result = zns_write(zns_info_list, buffer_128KB_write, 512 * 8 * 32, write_zone, offset_write_zone * 8);
                    offset_write_zone += 32;
                    i_current_write_buffer = 0;
                    
/*
                    if(io_result == 0) {
                        offset_write_zone += 32;
                        i_current_write_buffer = 0;
                    } else {
                        printf("write IO result : %#x\n", io_result);
                    }
*/
                } else {
                    remain_read_offset = i_current_write_buffer + i_current_read_offset - 32;
                    //printf("i_current_write_buffer : %d, i_current_read_offset: %d, remain_read_offset : %d, (i_current_write_buffer + i_current_read_offset) %d\n",i_current_write_buffer, i_current_read_offset, remain_read_offset, (i_current_write_buffer + i_current_read_offset));
                    memcpy(&buffer_128KB_write[i_current_write_buffer], buffer_128KB_read, 512 * 8 * (i_current_read_offset-remain_read_offset));
                    io_result = zns_write(zns_info_list, buffer_128KB_write, 512 * 8 * 32, write_zone, offset_write_zone * 8);
                    offset_write_zone += 32;
                    i_current_write_buffer = 0;
                    /*
                    if(io_result == 0) {
                        printf("test\n");
                        printf("test1 i_current_write_buffer : %d\n", i_current_write_buffer);
                        offset_write_zone += 32;
                        i_current_write_buffer = 0;
                        printf("test2 i_current_write_buffer : %d\n", i_current_write_buffer);
                    } else {
                        printf("write IO result : %#x\n", io_result);
                    }
*/
                    memcpy(&buffer_128KB_write[i_current_write_buffer], &buffer_128KB_read[i_current_read_offset-remain_read_offset], 512 * 8 * remain_read_offset);
                    //printf("test2 i_current_write_buffer : %d\n", i_current_write_buffer);
                    i_current_write_buffer += remain_read_offset;
                    //printf("test3 i_current_write_buffer : %d\n", i_current_write_buffer);
                }
            }
            if( offset_write_zone  == (512*512) ) {
                zns_zone_finish(zns_info_list, write_zone);
                write_zone++;
                offset_write_zone = 0;
                printf("write zone : %d\n", write_zone);
            }
            if(collection_invalid_count >= goal) {
                if(i_current_write_buffer != 0) {
                    io_result = zns_write(zns_info_list, buffer_128KB_write, 512 * 8 * i_current_write_buffer, write_zone, offset_write_zone * 8);
                }
                goto LSM_GC_end;
            }
        }//end of zone blocks

        zns_zone_reset(zns_info_list, zones[i_zone].zone_number);
        zones[i_zone].valid_blocks = 0;
    }//end of zones

LSM_GC_end: 
    printf("LSM GC End\n");

    return 0;
}

int compare_zone_v2(const void *a, const void *b) {
    int num1 = ((ZNS_Zone *)a)->valid_blocks;
    int num2 = ((ZNS_Zone *)b)->valid_blocks;

    if (num1 < num2)    // a가 b보다 작을 때는
        return -1;      // -1 반환
    
    if (num1 > num2)    // a가 b보다 클 때는
        return 1;       // 1 반환
    
    return 0;    // a와 b가 같을 때는 0 반환
}


void sort_zones(ZNS_Zone * zones) {
    qsort(zones, MAX_ZONE_COUNT, sizeof(ZNS_Zone), compare_zone_v2);
}

void setup_bitmap_read_file(char * block_bitmap) {
    FILE *fp;
    int index=0;
    int i;
    char * buffer = (char *) malloc(TOTAL_BLOCK_COUNT);
    memset(buffer, 0, TOTAL_BLOCK_COUNT);

    fp = fopen("/home/choigunhee/mnt/result_cs_th_80/HC_U_80/BitmapLog_700.csv", "r");
    lseek(fp, TOTAL_BLOCK_COUNT, SEEK_SET);
    fread(buffer, 1, TOTAL_BLOCK_COUNT, fp);
    for(i=0; i<TOTAL_BLOCK_COUNT; i++) {
        block_bitmap[i] = buffer[i];
        if(block_bitmap[i] == FREE_BLOCK) {
            block_bitmap[i] = INVALID_BLOCK;
        } else {
           //printf("tsestst : %d, i : %d\n", block_bitmap[i], i);
        }
        //printf("data : %d\n", block_bitmap[i]);
    }

    fclose(fp);
}

//128KB total write
void fill_zns_ssd_128KB(struct zns_share_info * zns_info_list) {
    int i;
    int i_zone;
    void * write_init_data = malloc(512 * 8 * 32);
    memset(write_init_data, 99, 512 * 8 * 32);

    printf("Start All zone fill\n");
    for(i_zone=0; i_zone<MAX_ZONE_COUNT; i_zone++) {
        printf("Zone %d Start\n", i_zone);
        for(i=0; i<512*512/32; i++)
            zns_write(zns_info_list, write_init_data, 512 * 8 * 32, i_zone, i * 8 * 32);
        zns_zone_finish(zns_info_list, i_zone);
    }
    printf("End All zone fill\n");
}

//128KB one zone write
void fill_zns_ssd_zone_128KB(struct zns_share_info * zns_info_list, int zone_number) {
    int i;
    int i_zone;
    void * write_init_data = malloc(512 * 8 * 32);
    memset(write_init_data, 99, 512 * 8 * 32);

    printf("Zone %d Start\n", zone_number);
    for(i=0; i<512*512/32; i++)
        zns_write(zns_info_list, write_init_data, 512 * 8 * 32, zone_number, i * 8 * 32);
    zns_zone_finish(zns_info_list, zone_number);

    printf("Zone %d End\n", zone_number);
}

void worker_write(struct zns_share_info * zns_info_list, int zone_number) {
    int i, j;

    //block size : 512, 512 * 8 = 4KB = I/O Basic Size
    void * write_data = malloc(SIM_BLOCK_SIZE);
    memset(write_data, 66, SIM_BLOCK_SIZE);

    //block size : 512, 512 * 8 * 32 = 128KB = I/O Max Size
    void * big_write_data = malloc(SIM_BLOCK_SIZE * 32);
    memset(big_write_data, 66, SIM_BLOCK_SIZE * 32);

    //zns_zone_reset(zns_info_list, zone_number);
    for(i=0; i<512*512;) {
        
        for(j=i; j<i+32; j++) {
            zns_write(zns_info_list, write_data, 512*8, zone_number, j*8);
        }
        i+=32;
        //printf("4KB i : %d, j : %d\n", i, j);
        zns_write(zns_info_list, big_write_data, 512*8*32, zone_number, i*8);
        i+=32;
        //printf("4KB i : %d\n", i);
    }
}

void worker_write_thread(void * arg) {
    int i, j;
    MultipleArg *multiple_arg = (MultipleArg *) arg;
    struct zns_share_info * zns_info_list = multiple_arg->zns_info_list;
    int zone_number = multiple_arg->zone_number;

    //Time Variable
    struct timeval time_start, time_end;
    double time_result=0;

    //block size : 512, 512 * 8 = 4KB = I/O Basic Size
    void * write_data = malloc(SIM_BLOCK_SIZE);
    memset(write_data, 66, SIM_BLOCK_SIZE);

    //block size : 512, 512 * 8 * 32 = 128KB = I/O Max Size
    void * big_write_data = malloc(SIM_BLOCK_SIZE * 32);
    memset(big_write_data, 66, SIM_BLOCK_SIZE * 32);

    //zns_zone_reset(zns_info_list, zone_number);

    printf("Start Worker %d\n", zone_number);
    gettimeofday(&time_start, NULL);
    for(i=0; i<512*512;) {
        
        for(j=i; j<i+32; j++) {
            zns_write(zns_info_list, write_data, 512*8, zone_number, j*8);
        }
        i+=32;
        //printf("4KB i : %d, j : %d\n", i, j);
        zns_write(zns_info_list, big_write_data, 512*8*32, zone_number, i*8);
        i+=32;
        //printf("4KB i : %d\n", i);
    }
    gettimeofday(&time_end, NULL);
    time_result = time_end.tv_sec + time_end.tv_usec / 1000000.0 - time_start.tv_sec - time_start.tv_usec/10000000.0;
    printf("Worker %d, Time %lf\n\n", zone_number, time_result);
}

void worker_read(struct zns_share_info * zns_info_list, int zone_number) {
    int i, j;

    //block size : 512, 512 * 8 = 4KB = I/O Basic Size
    void * read_data = malloc(SIM_BLOCK_SIZE);
    memset(read_data, 0, SIM_BLOCK_SIZE);

    //block size : 512, 512 * 8 * 32 = 128KB = I/O Max Size
    void * big_read_data = malloc(SIM_BLOCK_SIZE * 32);
    memset(big_read_data, 0, SIM_BLOCK_SIZE * 32);

    zns_zone_reset(zns_info_list, zone_number);
    for(i=0; i<512*512;) {
        
        for(j=i; j<i+32; j++) {
            zns_read(zns_info_list, read_data, 512*8, zone_number, j*8);
        }
        i+=32;
        //printf("4KB i : %d, j : %d\n", i, j);
        zns_read(zns_info_list, big_read_data, 512*8*32, zone_number, i*8);
        i+=32;
        //printf("4KB i : %d\n", i);
    }
}
