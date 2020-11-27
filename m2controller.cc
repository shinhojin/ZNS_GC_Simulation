/* 2020. 09. 22
 * Creater : Gunhee Choi
 * Modifier : Hojin Shin
 * This file is the M.2 ZNS SSD Contorller realization
*/

#include "m2controller.h"
#include <iostream>

using namespace std;

int zns_init(char * dev, struct zns_share_info * zonelist)
{
    int i;
    int fd = open(dev, O_RDWR);
    void * tempdata = malloc(SECTOR_SIZE);
    void * temp_log_data = malloc(LOG_SIZE);
    struct nvme_zone_info_entry * zone_entrys;
    struct nvme_id_ns * id_ns;
    
    memset(tempdata, 0, SECTOR_SIZE);
    zns_info_ns(fd, tempdata);
    id_ns = tempdata;
    
    zonelist->fd = fd;
    zonelist->zonesize = id_ns->zonesize;
    zonelist->activezones = id_ns->activezones;
    zonelist->openzones = id_ns->openzones;
    zonelist->totalzones = id_ns->totalzones;

    zonelist->zone_list = malloc(sizeof(struct zns_zone_info) * zonelist->totalzones);
    
    for(i=0; i<zonelist->totalzones; i++)
    {
        memset(temp_log_data, 0, LOG_SIZE);
        zns_get_log_entry_info(zonelist->fd, temp_log_data, i);
        struct nvme_zone_info_entry  * temp_zone_info_entry = temp_log_data;
        (zonelist->zone_list)[i].zone_entry.zone_condition = temp_zone_info_entry->zone_condition;
        (zonelist->zone_list)[i].zone_entry.zone_capacity = temp_zone_info_entry->zone_capacity;
        (zonelist->zone_list)[i].zone_entry.write_pointer = temp_zone_info_entry->write_pointer;
        (zonelist->zone_list)[i].zone_entry.zone_start_lba = temp_zone_info_entry->zone_start_lba;
        (zonelist->zone_list)[i].zone_entry.cnt_read = temp_zone_info_entry->cnt_read;
        (zonelist->zone_list)[i].zone_entry.cnt_write = temp_zone_info_entry->cnt_write;
        (zonelist->zone_list)[i].zone_entry.cnt_reset = temp_zone_info_entry->cnt_reset;
    }
    
    free(tempdata);
    free(temp_log_data);
    
    return fd;
}

int * zns_init_print(struct zns_share_info * zonelist)
{
    int i;
    struct nvme_zone_info_entry temp_zone_info_entry;
    printf("ZNS SSD Infos\n");
    printf("File Descriptor Number : %d", zonelist->fd);
	printf("Zone Size\t:\t%#"PRIx64"\n", zonelist->zonesize);
	printf("Active Zones\t:\t%#"PRIx64"\n", zonelist->activezones);
	printf("Open Zones\t:\t%#"PRIx64"\n", zonelist->openzones);
	printf("Total Zones\t:\t%#"PRIx64"\n", zonelist->totalzones);
    
    printf("\nZNS SSD Zone Entry Infos\n");
    for(i=0; i<zonelist->totalzones; i++)
    {
        temp_zone_info_entry = zonelist->zone_list->zone_entry;
            printf("Zone %d { Condition : %#"PRIx64",Capacity : %#"PRIx64", SLBA : %#"PRIx64", WP : %#"PRIx64", WriteCnt : %#"PRIx64", ReadCnt : %#"PRIx64", ResetCnt : %#"PRIx64" } \n",
                                i,
                                le64_to_cpu((zonelist->zone_list)[i].zone_entry.zone_condition),
                                le64_to_cpu((zonelist->zone_list)[i].zone_entry.zone_capacity),
                                le64_to_cpu((zonelist->zone_list)[i].zone_entry.zone_start_lba),
                                le64_to_cpu((zonelist->zone_list)[i].zone_entry.write_pointer),
                                le64_to_cpu((zonelist->zone_list)[i].zone_entry.cnt_write),
                                le64_to_cpu((zonelist->zone_list)[i].zone_entry.cnt_read),
                                le64_to_cpu((zonelist->zone_list)[i].zone_entry.cnt_reset));
    }
    
}


__u64 get_zone_to_slba(struct zns_share_info * zonelist, int zonenumber)
{
    return (zonelist->zone_list)[zonenumber].zone_entry.zone_start_lba;
}

void * zns_info_ctrl(int fd, void * data)
{	
    int result;
    struct nvme_passthru_cmd cmd = {
		.opcode		= 0x06,
		.flags		= 0,
		.rsvd1		= 0,
		.nsid		= 0,
		.cdw2		= 0,
		.cdw3		= 0,
		.metadata	= (__u64)(uintptr_t) 0,
		.addr		= (__u64)(uintptr_t) data,
		.metadata_len	= 0,
		.data_len	= 4096,
		.cdw10		= 1,
		.cdw11		= 0,
		.cdw12		= 0,
		.cdw13		= 0,
		.cdw14		= 0,
		.cdw15		= 0,
		.timeout_ms	= 0,
		.result		= 0,
	};

    result = ioctl(fd, NVME_IOCTL_ADMIN_CMD, &cmd);

    if(result == -1)
    {
        printf("ZNS SSD Ctrl Info Request Fail\n");
    }
        return -1;

    return data;
}

void * zns_info_ctrl_print(void * data)
{
    struct nvme_id_ctrl * id_ctrl = data;

    printf("vid\t:\t%#"PRIx64"\n", le64_to_cpu(id_ctrl->vid));
	printf("ssvid\t:\t%#"PRIx64"\n", le64_to_cpu(id_ctrl->ssvid));
	printf("oncs\t:\t%#"PRIx64"\n", le64_to_cpu(id_ctrl->oncs));
}

void * zns_info_ns(int fd, void * data)
{	
    int result;
    struct nvme_passthru_cmd cmd = {
		.opcode		= 0x06,
		.flags		= 0,
		.rsvd1		= 0,
		.nsid		= 1,
		.cdw2		= 0,
		.cdw3		= 0,
		.metadata	= (__u64)(uintptr_t) 0,
		.addr		= (__u64)(uintptr_t) data,
		.metadata_len	= 0,
		.data_len	= 4096,
		.cdw10		= 0,
		.cdw11		= 0,
		.cdw12		= 0,
		.cdw13		= 0,
		.cdw14		= 0,
		.cdw15		= 0,
		.timeout_ms	= 0,
		.result		= 0,
	};

    result = ioctl(fd, NVME_IOCTL_ADMIN_CMD, &cmd);

    if(result == -1)
    {
        printf("ZNS SSD Ctrl Info Request Fail\n");
    }
        return -1;

    return data;
}


void * zns_info_ns_print(void * data)
{
    struct nvme_id_ns * id_ns = data;

    printf("nsze\t\t:\t%#"PRIx64"\n", le64_to_cpu(id_ns->nsze));
	printf("Zone Size\t:\t%#"PRIx64"\n", id_ns->zonesize);
	printf("activezones\t:\t%#"PRIx64"\n", id_ns->activezones);
	printf("openzones\t:\t%#"PRIx64"\n", id_ns->openzones);
	printf("totalzones\t:\t%#"PRIx64"\n", id_ns->totalzones);
}

int zns_write_request(int fd, void * write_data, int nblocks, __u64 slba)
{
    int result;

    struct nvme_user_io io= {
            .opcode		= 0x01,
            .flags		= 0,
            .control	= 0x0400,
            .nblocks	= nblocks,
            .rsvd		= 0,
            .metadata	= (__u64)(uintptr_t) 0,
            .addr		= (__u64)(uintptr_t) write_data,
            .slba		= slba,
            .dsmgmt		= 0,
            .reftag		= 0,
            .appmask	= 0,
            .apptag		= 0,
        };

    result = ioctl(fd, NVME_IOCTL_SUBMIT_IO, &io);
    if(result != 0)
    {
        printf("ZNS SSD Write Fail, error code : %#"PRIx64"\n", result);
        return result;
    }

    return 0;
}

int zns_write(struct zns_share_info * zonelist, void * write_data, int data_size, int zone_number, __u64 offset)
{
    int i;
    int result;
    int nblocks;
    __u64 write_lba;

    if(data_size % BLOCK_SIZE == 0)
        nblocks = data_size / 512 - 1;
    else
        nblocks = data_size / 512;
    
    //printf("nblocks : %d\n", nblocks);

    write_lba = get_zone_to_slba(zonelist, zone_number) + offset;

    result = zns_write_request(zonelist->fd, write_data, nblocks, write_lba);
    zns_update_zone_info(zonelist, zone_number);

    return result;
}


int zns_read_request(int fd, void * read_data, int nblocks, __u64 slba)
{
    int result;

    struct nvme_user_io io = {
            .opcode		= 0x02,
            .flags		= 0,
            .control	= 0,
            .nblocks	= nblocks,
            .rsvd		= 0,
            .metadata	= 0,
            .addr		= (__u64)(uintptr_t) read_data,
            .slba		= slba,
            .dsmgmt		= 0,
            .reftag		= 0,
            .appmask	= 0,
            .apptag		= 0,
        };

    result = ioctl(fd, NVME_IOCTL_SUBMIT_IO, &io);
    if(result == -1)
    {
        printf("ZNS SSD Read Fail : %#x\n");
        return -1;
    }

    return 0;
}

int zns_read(struct zns_share_info * zonelist, void * write_data, int data_size, int zone_number, __u64 offset)
{
    int i;
    int result;
    int nblocks;
    __u64 write_lba;

    if(data_size % BLOCK_SIZE == 0)
        nblocks = data_size / 512 - 1;
    else
        nblocks = data_size / 512;
    
    write_lba = get_zone_to_slba(zonelist, zone_number) + offset;
    
    result = zns_read_request(zonelist->fd, write_data, nblocks, write_lba);
    zns_update_zone_info(zonelist, zone_number);

    return result;
}

int zns_get_log_entry_info(int fd, void * data, __u64 zid)
{
    __u32 data_len = 64;
    int result;

    struct nvme_admin_cmd cmd = {
		.opcode = 0x02,
		.nsid = 1,
		.addr = (__u64)(uintptr_t) data,
		.data_len = data_len,
	};

    __u32 numd = (data_len >> 2) - 1;
	__u16 numdu = numd >> 16, numdl = numd & 0xffff;
	__u8 log_id = 0x82;
	__u8 lsp = 0;
	__u64 lpo = (zid + 1) * 64;
	bool rae = 0;
	__u16 lsi = 0;

	cmd.cdw10 = log_id | (numdl << 16) | (rae ? 1 << 15 : 0);
	if (lsp)
        cmd.cdw10 |= lsp << 8;
	cmd.cdw11 = numdu | (lsi << 16);
	cmd.cdw12 = lpo;
	cmd.cdw13 = (lpo >> 32);

    result = ioctl(fd, NVME_IOCTL_ADMIN_CMD, &cmd);
    if(result == -1)
    {
        printf("Get log entry fail\n");
        return -1;
    }

    return 0;
}

int zns_get_total_log_entry_info(int fd, int nzones)
{
    int i;
    void * data = malloc(64);

    for(i=0; i<nzones; i++)
    {
        memset(data, 0, 64);
        zns_get_log_entry_info(fd, data, i);
        zns_log_info_entry_print(i, data);
    }

    return 0;
}

int zns_log_info_entry_print(int num, void * data)
{   
    struct nvme_zone_info_entry * zone_entry = data;
    printf("Zone %d { Condition : %#"PRIx64", Capacity : %#"PRIx64", SLBA : %#"PRIx64", WP : %#"PRIx64", WriteCnt : %#"PRIx64", ReadCnt : %#"PRIx64", ResetCnt : %#"PRIx64" } \n",
                                num,
                                le64_to_cpu(zone_entry->zone_condition),
                                le64_to_cpu(zone_entry->zone_capacity),
                                le64_to_cpu(zone_entry->zone_start_lba),
                                le64_to_cpu(zone_entry->write_pointer),
                                le64_to_cpu(zone_entry->cnt_write),
                                le64_to_cpu(zone_entry->cnt_read),
                                le64_to_cpu(zone_entry->cnt_reset));
    return 0;
}

int zns_zone_io_managemnet(int fd, __u64 slba, __u64 action)
{
    int result;
    __u64 t = 0xffffffff;

    struct nvme_passthru_cmd cmd = {
  		.opcode = 0x20,
		.nsid = 1,
    };
    slba |= action << 61;
    cmd.cdw10 = slba & t;
    cmd.cdw11 = slba >> 32;

    result = ioctl(fd, NVME_IOCTL_IO_CMD, &cmd);
    
    if(result == -1)
    {
        printf("Get finish fail\n");
        return -1;
    }

    return 0;
}

int zns_update_zone_info(struct zns_share_info * zonelist, int zonenumber)
{
    void * temp_log_data = malloc(LOG_SIZE);
    struct nvme_zone_info_entry * zone_entrys;

    zns_get_log_entry_info(zonelist->fd, temp_log_data, zonenumber);
    struct nvme_zone_info_entry  * temp_zone_info_entry = temp_log_data;
    (zonelist->zone_list)[zonenumber].zone_entry.zone_condition = temp_zone_info_entry->zone_condition;
    (zonelist->zone_list)[zonenumber].zone_entry.zone_capacity = temp_zone_info_entry->zone_capacity;
    (zonelist->zone_list)[zonenumber].zone_entry.write_pointer = temp_zone_info_entry->write_pointer;
    (zonelist->zone_list)[zonenumber].zone_entry.zone_start_lba = temp_zone_info_entry->zone_start_lba;
    (zonelist->zone_list)[zonenumber].zone_entry.cnt_read = temp_zone_info_entry->cnt_read;
    (zonelist->zone_list)[zonenumber].zone_entry.cnt_write = temp_zone_info_entry->cnt_write;
    (zonelist->zone_list)[zonenumber].zone_entry.cnt_reset = temp_zone_info_entry->cnt_reset;

    free(temp_log_data);
    
    return 0;
}

int zns_zone_finish_request(int fd, __u64 slba)
{
    return zns_zone_io_managemnet(fd, slba, 0x02);
}

int zns_zone_open_request(int fd, __u64 slba)
{
    return zns_zone_io_managemnet(fd, slba, 0x03);
}

int zns_zone_reset_request(int fd, __u64 slba)
{
    return zns_zone_io_managemnet(fd, slba, 0x04);
}

int zns_zone_finish(struct zns_share_info * zonelist, int zonenumber)
{
    int result;
    result = zns_zone_finish_request(zonelist->fd, get_zone_to_slba(zonelist, zonenumber));
    zns_update_zone_info(zonelist, zonenumber);
    return result;
}

int zns_zone_open(struct zns_share_info * zonelist, int zonenumber)
{
    int result;
    result = zns_zone_open_request(zonelist->fd, get_zone_to_slba(zonelist, zonenumber));
    zns_update_zone_info(zonelist, zonenumber);
    return result;
}

int zns_zone_reset(struct zns_share_info * zonelist, int zonenumber)
{
    int result;
    result = zns_zone_reset_request(zonelist->fd, get_zone_to_slba(zonelist, zonenumber));
    zns_update_zone_info(zonelist, zonenumber);
    return result;
}