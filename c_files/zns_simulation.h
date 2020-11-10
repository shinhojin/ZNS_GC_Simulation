//Real ZNS SSD R/W size 512bytes -> WP 1
//Real ZNS SSD Zone block count : 0x35e500
#define ZNS_BLOCK_SIZE 512
#define ZNS_ZONE_BLOCK_COUNT 0x35e000
#define MAX_ZONE_COUNT 516

//overprovisioning zones info
#define OVERPROVISIONING_STRART_ZONE 200
#define OVERPROVISIONING_END_ZONE 300

//simulation I/O size : 512 bytes * 8 = 4096 bytes = 4KB
//simulation Segment size : 512 blocks = 512 * 4096 = 2MB
#define SIM_BLOCK_SIZE ZNS_BLOCK_SIZE*8
#define SIM_SEGMENT_SIZE SIM_BLOCK_SIZE*512

#define SIM_BLOCK_IO_DEFAULT 8
#define SIM_SEGMENT_IN_BLOCK_COUNT 512
#define SIM_ZONE_IN_SEGMENT_COUNT 512

//GC
#define DELETE_SEGMENT_COUNT 100
#define DELETE_ZONE_COUNT 1

//Utilization
#define Pecentage_10 51*512
#define MAX_RANDOM_ARR 10000
#define Percentage_10_seg 51
#define MAX_RANDOM_SEGMENT_ARR 500

#define TOTAL_BLOCKS SIM_SEGMENT_IN_BLOCK_COUNT * SIM_ZONE_IN_SEGMENT_COUNT * MAX_ZONE_COUNT
#define TOTAL_SEGMENTS SIM_ZONE_IN_SEGMENT_COUNT * MAX_ZONE_COUNT

//New
#define HOT_SEGMENT 0
#define COLD_SEGMENT 1
#define FREE_SEGMENT 2

#define VALID_BLCOK 1
#define INVALID_BLOCK 2
#define FREE_BLOCK 0

#define FREE_ZONE -1
#define HOT_ZONE 0
#define COLD_L1_ZONE 1
#define COLD_L2_ZONE 2

#define SEED_MAX 10000

#define ZONE_COUNT 512
#define BLOCK_COUNT 512 * 512 * ZONE_COUNT
#define OP_ZONE_COUNT 4
#define TOTAL_BLOCK_COUNT (BLOCK_COUNT + (OP_ZONE_COUNT * 512 * 512))

#define START_OP_ZONE 517

//Data Structure
typedef struct Segment_data
{
    int utilization;
    int index;
} Segment_data;

typedef struct Zone_data
{
    int utilization;
    int index;
} Zone_data;

typedef struct Block_data
{
    char data[4096];
} Block_data;

typedef struct Big_data
{
    Block_data data[32];
} Big_data;

typedef struct MultipleArg {
    int zone_number;
    struct zns_share_info * zns_info_list;
} MultipleArg;


//Init Stage Function
int init_zone_reset(struct zns_share_info * zns_info_list, int zones);
int init_zones_write(struct zns_share_info * zns_info_list, char * block_bitmap);
int setup_util_zone(struct zns_share_info * zns_info_list, char * block_bitmap, int i_zone, int goal_util);
int setup_skewed_data(struct zns_share_info * zns_info_list, char * block_bitmap, int * segment_bitmap, int hot_goal);

//I/O Function
int get_startblock_zone_index(int zone_number);
int get_startblock_segment_index(int zone_number, int i_segment);
int get_block_offset(int i_segment, int i_block);
//int write_block(struct zns_share_info * zns_info_list, int i_zone, int offset, void * data, int data_size);
//int read_block(struct zns_share_info * zns_info_list, int i_zone, int offset, void * data, int data_size);

//Data Print Function
void print_utilization_segments_in_zone(int i_zone, int * segment_bitmap, char * block_bitmap);
void print_total_utilization_segments_in_zone(int * segment_bitmap, int * block_bitmap);
void print_utilization_zone(int * segment_bitmap);

//Bitmap Update
int check_segment_util(char * block_bitmap, int i_zone, int i_segment);
void update_bitmap(int * zone_bitmap, int * segment_bitmap, char * block_bitmap);
void update_segment_bitmap(int * segment_bitmap, char * block_bitmap);
void update_zone_bitmap(int * zone_bitmap, int * segment_bitmap);

//GC Function

//etc.
int compare_segement(const void *a, const void *b);
int compare_zone(const void *a, const void *b);


//Bitmap Sturcture
typedef struct ZNS_BLOCK
{
    char vaild;
} ZNS_Block;

typedef struct ZNS_SEGMENT
{
    //ZNS_Block blocks[512];
    
    int i_start_block;
    int i_end_block;

    int vaild_blocks;
    int hot_cold_state;
} ZNS_Segment;

typedef struct ZNS_ZONE
{
    ZNS_Segment segments[512];
    int state;
    int valid_blocks;

    int i_start_block;
    int i_end_block;
    
    int zone_number;
} ZNS_Zone;


//New
int init_overprovision_zones(struct zns_share_info * zns_info_list);
int zone_bitmap_init(ZNS_Zone * zone, int zone_number, char * block_bitmap);
void print_segment_bitmap_in_zone(ZNS_Zone * zone);
int setup_block_bitmap_utilization(char * block_bitmap, int goal, int n_zones, int setup_state);
//int setup_zone_bitmap_utilization(ZNS_Zone * zone, int goal);
void print_block_bitmap_in_segment(ZNS_Zone * zone, int i_segment, char * block_bitmap);
void print_multi_segment_in_zone(ZNS_Zone * zone, int count);
void print_zones_info(ZNS_Zone * zone, int count);
int compare_zone_v2(const void *a, const void *b);
void sort_zones(ZNS_Zone * zones);
int get_read_block_count(char * block_bitmap, int i_block);
int basic_garbage_collection(struct zns_share_info * zns_info_list, ZNS_Zone * zones, int n_zones, char * block_bitmap, int goal);
int lsm_garbage_collection(struct zns_share_info * zns_info_list, ZNS_Zone * zones, int n_zones, char * block_bitmap, int goal);
void fill_zns_ssd_128KB(struct zns_share_info * zns_info_list);
void fill_zns_ssd_zone_128KB(struct zns_share_info * zns_info_list, int zone_number);
void worker_read(struct zns_share_info * zns_info_list, int zone_number);
void worker_write(struct zns_share_info * zns_info_list, int zone_number);
void fill_zns_ssd_128KB(struct zns_share_info * zns_info_list);
void fill_zns_ssd_zone_128KB(struct zns_share_info * zns_info_list, int zone_number);

void worker_write_thread(void * arg);