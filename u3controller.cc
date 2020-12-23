/* 2020. 11. 22
 * Creater : Myounghoon Oh
 * Modifier : Hojin Shin
 * This file is the U.3 ZNS SSD Contorller realization
*/

#include "u3controller.h"

void* u3_identify_ns(int fd, void * data)
{	
	int result;
	struct u3_nvme_passthru_cmd cmd = {
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

	result = ioctl(fd, U3_NVME_IOCTL_ADMIN_CMD, &cmd);

	if(result == -1)
	{
		printf("ZNS SSD NS Info Request Fail\n");
		exit(0);
	}

	return data;
}

void* u3_identify_ctrl(int fd, void * data)
{	
	int result;
	struct u3_nvme_passthru_cmd cmd = {
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

	result = ioctl(fd, U3_NVME_IOCTL_ADMIN_CMD, &cmd);

	if(result == -1)
	{
		printf("ZNS SSD Ctrl Info Request Fail\n");
		exit(0);
	}

	return data;
}

int u3_zns_get_info(char * dev)
{
	zns_info = (struct u3_zns_info *)malloc(sizeof(struct u3_zns_info));
	int fd = open(dev, O_RDWR);
	struct u3_nvme_id_ns *id_ns = (struct u3_nvme_id_ns *)malloc(SECTOR_SIZE);
	struct u3_controller_identify *id_ctrl = (struct u3_controller_identify *)malloc(SECTOR_SIZE);

	u3_identify_ns(fd, id_ns);
	u3_identify_ctrl(fd, id_ctrl);

	zns_info -> fd = fd;
	zns_info -> ns_size = id_ns -> nsze;
	zns_info -> ns_cap = id_ns -> ncap;
	zns_info -> ns_use = id_ns -> nuse;

	zns_info -> ns_op_io_bound = id_ns -> noiob;
	//for(int i = 0; i < 16; i++)
	//	 zns_info -> ns_nvm_cap[i] = id_ns -> nvmcap[i];
	zns_info -> ns_pref_wr_gran = id_ns -> npwg;
	zns_info -> ns_pref_wr_align = id_ns -> npwa;
	zns_info -> ns_pref_dealloc_gran = id_ns -> npdg;
	zns_info -> ns_pref_dealloc_align = id_ns -> npda;
	zns_info -> ns_opot_wr_size = id_ns -> nows;

	zns_info -> max_active_res = id_ns -> mar;
	zns_info -> opened_zone_num = 0;
	zns_info -> max_open_res = id_ns -> mor;
	//zns_info -> max_append_size = id_ctrl -> zamds;
	//zns_info -> lbaf = id_ns -> lbaf[0];
	zns_info -> zfi = id_ns -> zfi;
	zns_info -> zonef = id_ns -> zonef[id_ns -> zfi];

	//printf("A\n");
	//printf("%u\n",zns_info -> zonef.zsze);
	zns_info -> max_zone_cnt = (zns_info -> ns_size)/(zns_info -> zonef.zsze);
	zone_desc_list = (struct u3_zone_descriptor *)malloc(zns_info -> max_zone_cnt * sizeof(struct u3_zone_descriptor));
	//printf("B\n");

	return 0;
}

void u3_print_zns_info()
{
	printf("ZNS SSD Info\n");

	printf("Namespace Size\t: %u\n", zns_info -> ns_size);
	printf("Namespace Capacity\t: %u\n", zns_info -> ns_cap);
	printf("Namespace Utilization\t: %u\n", zns_info -> ns_use);
	printf("Namespace Optimal IO Boundary\t: %u\n", zns_info -> ns_op_io_bound);
	//printf("NVM Capacity\t: %u\n", zns_info -> ns_nvm_cap);
	printf("Namespace Preferred Write Gran\t: %u\n", zns_info -> ns_pref_wr_gran);
	printf("Namespace Preferred Write Align\t: %u\n", zns_info -> ns_pref_wr_align);
	printf("Namespace Preferred Dealloc Gran\t: %u\n", zns_info -> ns_pref_dealloc_gran);
	printf("Namespace Preferred Dealloc Align\t: %u\n", zns_info -> ns_pref_dealloc_align);
	printf("Namespace Optimal Write Size\t: %u\n", zns_info -> ns_opot_wr_size);

	printf("Namespace id - Zone Format Index\t: %u\n", zns_info -> zfi);
	printf("Namespace id - Zone Size\t: %u\n", zns_info -> zonef.zsze);
	printf("Namespace id - Max Active Zones\t: %u\n", zns_info -> max_active_res);
	printf("Namespace id - Max Open Zones\t: %u\n", zns_info -> max_open_res);
	//printf("Controller id - Max Append Size\t: %#"PRIx8"\n", zns_info -> max_append_size);
}

int u3_zns_format()
{	
	int result;
	__u32 cdw10 = 0;
	struct u3_nvme_passthru_cmd cmd = {
		.opcode		= 0x80,
		.flags		= 0,
		.rsvd1		= 0,
		.nsid		= 1,
		.cdw2		= 0,
		.cdw3		= 0,
		.metadata	= (__u64)(uintptr_t) 0,
		.addr		= (__u64)(uintptr_t) 0,
		.metadata_len	= 0,
		.data_len	= 0,
		.cdw10		= cdw10,
		.cdw11		= 0,
		.cdw12		= 0,
		.cdw13		= 0,
		.cdw14		= 0,
		.cdw15		= 0,
		.timeout_ms	= 0,
		.result		= 0,
	};

	result = ioctl(zns_info -> fd, U3_NVME_IOCTL_ADMIN_CMD, &cmd);

	if(result == -1)
	{
		printf("ZNS SSD Format Request Fail\n");
		return -1;
	}

	return 0;
}

int u3_zns_management_send(int zone_number, __u8 value)
{	
	int result;
	__le64 slba = 0;
	__u32 cdw13 = 0;

	cdw13 = cdw13 | value;

	if(zone_number < 0)
		//Select All 
		cdw13 = cdw13 | (1 << 8);
	else
	{
		slba = zone_number * (zns_info -> zonef.zsze);

		if(value == MAN_OPEN && zns_info -> opened_zone_num + 1 >= zns_info -> max_open_res)
		{
			printf("Opened zone num : %d\n", zns_info -> opened_zone_num);
			printf("ZNS SSD Zone Management Send ZONE MAX OPEN\n");
			return -1;
		}
	}
	__u64 t = 0xffffffff;
	__le32 slba1 = slba & t;
	__le32 slba2 = slba >> 32;

	struct u3_nvme_passthru_cmd cmd = {
		.opcode		= 0x79,
		.flags		= 0,
		.rsvd1		= 0,
		.nsid		= 1,
		.cdw2		= 0,
		.cdw3		= 0,
		.metadata	= (__u64)(uintptr_t) 0,
		.addr		= (__u64)(uintptr_t) 0,
		.metadata_len	= 0,
		.data_len	= 0,
		.cdw10		= slba1,
		.cdw11		= slba2,
		.cdw12		= 0,
		.cdw13		= cdw13,
		.cdw14		= 0,
		.cdw15		= 0,
		.timeout_ms	= 0,
		.result		= 0,
	};

	result = ioctl(zns_info -> fd, U3_NVME_IOCTL_IO_CMD, &cmd);

	if(result == -1)
	{
		printf("ZNS SSD Zone Management Send Fail\n");
		return -1;
	}

	if(value == MAN_OPEN)
		zns_info -> opened_zone_num += 1;
	else if(value == MAN_CLOSE)
		zns_info -> opened_zone_num -= 1;

	return 0;
}

void u3_zns_set_zone(int zone_number, __u8 value)
{
	u3_zns_management_send(zone_number, value);
}

void* u3_zns_management_recv(unsigned int report_zone_cnt, unsigned int partial, unsigned int option, unsigned int slba)
{	
	int result;
	__u64 t = 0xffffffff;
	__le32 slba1 = slba & t;
	__le32 slba2 = slba >> 32;

	__u32 cdw12 = ((report_zone_cnt + 1)* sizeof(struct u3_zone_descriptor));
	__u32 cdw13 = 0;
	cdw13 = cdw13 | (partial << 16) | (option << 8);

	void *buffer = malloc((report_zone_cnt + 1) * sizeof(struct u3_zone_descriptor));

	struct u3_nvme_passthru_cmd cmd = {
		.opcode		= 0x7A,
		.flags		= 0,
		.rsvd1		= 0,
		.nsid		= 1,
		.cdw2		= 0,
		.cdw3		= 0,
		.metadata	= (__u64)(uintptr_t) 0,
		.addr		= (__u64)(uintptr_t) buffer,
		.metadata_len	= 0,
		.data_len	= (report_zone_cnt + 1) * sizeof(struct u3_zone_descriptor),
		.cdw10		= slba1,
		.cdw11		= slba2,
		.cdw12		= cdw12,
		.cdw13		= cdw13,
		.cdw14		= 0,
		.cdw15		= 0,
		.timeout_ms	= 0,
		.result		= 0,
	};

	result = ioctl(zns_info -> fd, U3_NVME_IOCTL_IO_CMD, &cmd);

	if(result == -1)
	{
		printf("ZNS SSD Zone Management Recv Fail\n");
		exit(0);
	}

	__le64 *temp = (__le64 *)buffer;

	return buffer;
}

void u3_zns_get_zone_desc(unsigned int partial, unsigned int option, unsigned int from_zone, unsigned int report_zone_cnt, bool init)
{
	struct u3_zone_descriptor* desc_buf;
	void* buf = 0;
	__le64* temp = 0;
	__le64 report_zone_num = 0;
	unsigned int slba = from_zone * (zns_info -> zonef.zsze);
	unsigned int real_max_zone_cnt = 0;

	if(init == true)
	{
		partial = REPORT_ALL;
		option = REPORT_ALL_STATE;
		slba = 0;

		for(unsigned int index = 0; index < zns_info -> max_zone_cnt; index += report_zone_num)
		{
			buf = u3_zns_management_recv(REPORT_NUM, partial, option, slba);

			temp = (__le64 *)buf;
			report_zone_num = temp[0];
			//printf("Init - reported zone count : %u\n", index);
			real_max_zone_cnt += report_zone_num;

			desc_buf = (struct u3_zone_descriptor *)buf;
			for(int i = 0; i < report_zone_num; i++)
			{
				memcpy(&zone_desc_list[index + i], &desc_buf[i + 1], sizeof(struct u3_zone_descriptor));
			}

			free(buf);
		}  
		zns_info -> max_zone_cnt = real_max_zone_cnt;
		//printf("Init - max_zone_cnt = %u\n", zns_info -> max_zone_cnt);
	}
	else
	{
		if(partial == REPORT_ALL)
		{     
			unsigned int temp_report_zone_cnt = report_zone_cnt;

			for(unsigned int index = from_zone; index < from_zone + report_zone_cnt; index += report_zone_num)
			{
				if(temp_report_zone_cnt > REPORT_NUM)
					buf = u3_zns_management_recv(REPORT_NUM, partial, option, slba);
				else
					buf = u3_zns_management_recv(temp_report_zone_cnt, partial, option, slba);

				temp = (__le64 *)buf;
				report_zone_num = temp[0];
				real_max_zone_cnt += report_zone_num;

				desc_buf = (struct u3_zone_descriptor *)buf;
				for(int i = 0; i < report_zone_num; i++)
				{
					memcpy(&zone_desc_list[index + i], &desc_buf[i + 1], sizeof(struct u3_zone_descriptor));
				}

				free(buf);
				temp_report_zone_cnt -= report_zone_num;
			}  
		}
		else
			printf("Need to Imple Report Partial\n");
	}
}

void u3_print_zone_desc(unsigned int total_zone)
{   
	printf("ZNS SSD Zone Info\n");
	//printf("%u\n", zns_info -> max_zone_cnt);
	//for(int i = 0; i < 1; i++)
	//for(int i = 0; i < zns_info -> max_zone_cnt; i++)
	for(int i = 0; i < total_zone; i++)
	{
		printf("------------- Zone %u -------------\n", i);
		if(zone_desc_list[i].state == STATE_EMPTY)
			printf("Zone State\t: EMPTY\n");
		else if(zone_desc_list[i].state == STATE_IMPL_OPEN)
			printf("Zone State\t: IMPLE_OPEN\n");
		else if(zone_desc_list[i].state == STATE_EXPL_OPEN)
			printf("Zone State\t: EXPL_OPEN\n");
		else if(zone_desc_list[i].state == STATE_CLOSED)
			printf("Zone State\t: CLOSE\n");
		else if(zone_desc_list[i].state == STATE_READ_ONLY)
			printf("Zone State\t: READ_ONLY\n");
		else if(zone_desc_list[i].state == STATE_FULL)
			printf("Zone State\t: FULL\n");
		else if(zone_desc_list[i].state == STATE_OFFLINE)
			printf("Zone State\t: OFFLINE\n");
		printf("Zone Capacity\t: %u\n", zone_desc_list[i].capacity);
		printf("Zone Start LBA\t: %u\n", zone_desc_list[i].start_lba);
		printf("Zone Write Pointer\t: %u\n", zone_desc_list[i].wp);
		printf("Finish Zone Recommended\t: %#"PRIx8"\n", zone_desc_list[i].fzr);
		printf("Reset Zone Recommended\t: %#"PRIx8"\n", zone_desc_list[i].rzr);
		printf("-----------------------------------\n\n");
	}
}


int u3_zns_write_request(void * write_data, __le16 nblocks, __le32 data_size, __u64 slba)
{
	int result;

	struct u3_nvme_user_io io= {
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
		.apptag		= 0,
		.appmask	= 0,
	};

	result = ioctl(zns_info -> fd, U3_NVME_IOCTL_SUBMIT_IO, &io);
	if(result != 0)
	{
		printf("ZNS SSD Write Fail, error code : %#"PRIx64"\n", result);
		return result;
	}

	return 0;
}

int u3_zns_write(void * write_data, int data_size, int zone_number)
{
	int i;
	int result;
	__le16 nblocks;

	if(data_size % BLOCK_SIZE == 0)
		nblocks = data_size / 512 - 1;
	else
		nblocks = data_size / 512;

	result = u3_zns_write_request(write_data, nblocks, data_size, zone_desc_list[zone_number].wp);
	zone_desc_list[zone_number].wp += nblocks + 1;

	return result;
}

int u3_zns_read_request(void * read_data, int nblocks, __u64 slba)
{
	int result;

	struct u3_nvme_user_io io = {
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
		.apptag		= 0,
		.appmask	= 0,
	};

	result = ioctl(zns_info -> fd, U3_NVME_IOCTL_SUBMIT_IO, &io);
	if(result == -1)
	{
		printf("ZNS SSD Read Fail : %#x\n");
		return -1;
	}

	return 0;
}

int u3_zns_read(void * read_data, int data_size, int zone_number, __u64 offset)
{
	int i;
	int result;
	int nblocks;
	__u64 read_lba;

	if(data_size % BLOCK_SIZE == 0)
		nblocks = data_size / 512 - 1;
	else
		nblocks = data_size / 512;

	read_lba = zone_desc_list[zone_number].start_lba + offset;
	result = u3_zns_read_request(read_data, nblocks, read_lba);

	return result;
}

int u3_zns_set_zone_change_notification()
{
	int result;
	__u32 cdw10 = 0x0B;
	__u32 cdw11 = 0;
	cdw11 = cdw11 | 1 << 27;

	struct u3_nvme_passthru_cmd cmd = {
		.opcode		= 0x09,
		.flags		= 0,
		.rsvd1		= 0,
		.nsid		= 1,
		.cdw2		= 0,
		.cdw3		= 0,
		.metadata	= (__u64)(uintptr_t) 0,
		.addr		= (__u64)(uintptr_t) 0,
		.metadata_len	= 0,
		.data_len	= 4096,
		.cdw10		= cdw10,
		.cdw11		= cdw11,
		.cdw12		= 0,
		.cdw13		= 0,
		.cdw14		= 0,
		.cdw15		= 0,
		.timeout_ms	= 0,
		.result		= 0,
	};

	result = ioctl(zns_info -> fd, U3_NVME_IOCTL_ADMIN_CMD, &cmd);

	if(result == -1)
	{
		printf("ZNS SSD Set Feature Fail\n");
		return -1;
	}

	return 0;
}





int zns_get_async_event()
{}

int zns_get_log(int fd, void * data, __u64 zid)
{}

int check_completion_queue()
{}
