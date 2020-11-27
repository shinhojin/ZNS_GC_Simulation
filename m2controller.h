/* 2020. 09. 22
 * Creater : Gunhee Choi
 * Modifier : Hojin Shin
 * This file is the M.2 ZNS SSD Contorller header
*/

#ifndef m2controller_H
#define m2controller_H

#ifdef _cpluscplus
extern "C" {
#endif

#include <assert.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <errno.h>
#include <getopt.h>
#include <fcntl.h>
#include <inttypes.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <asm/types.h>
#include <linux/posix_types.h>
#include <uuid/uuid.h>
#include <stdbool.h>
#include <stdint.h>
#include <endian.h>

#define BLOCK_SIZE 512
#define SECTOR_SIZE (BLOCK_SIZE * 8)
#define LOG_SIZE 64

#define unlikely(x) x

#ifdef __CHECKER__
#define __bitwise__ __attribute__((bitwise))
#else
#define __bitwise__
#endif
#define __bitwise __bitwise__

#ifdef __CHECKER__
#define __force       __attribute__((force))
#else
#define __force
#endif

typedef __u16 __bitwise __le16;
typedef __u16 __bitwise __be16;
typedef __u32 __bitwise __le32;
typedef __u32 __bitwise __be32;
typedef __u64 __bitwise __le64;
typedef __u64 __bitwise __be64;

typedef __u16 __bitwise __sum16;
typedef __u32 __bitwise __wsum;

#define __aligned_u64 __u64 __attribute__((aligned(8)))
#define __aligned_be64 __be64 __attribute__((aligned(8)))
#define __aligned_le64 __le64 __attribute__((aligned(8)))

#ifndef _ASM_GENERIC_INT_LL64_H
#define _ASM_GENERIC_INT_LL64_H

#include <asm/bitsperlong.h>

#ifndef __ASSEMBLY__

typedef __signed__ char __s8;
typedef unsigned char __u8;

typedef __signed__ short __s16;
typedef unsigned short __u16;

typedef __signed__ int __s32;
typedef unsigned int __u32;

#ifdef __GNUC__
__extension__ typedef __signed__ long long __s64;
__extension__ typedef unsigned long long __u64;
#else
typedef __signed__ long long __s64;
typedef unsigned long long __u64;
#endif

#endif /* __ASSEMBLY__ */

#endif /* _ASM_GENERIC_INT_LL64_H */

typedef __u16 __bitwise __le16;
typedef __u16 __bitwise __be16;
typedef __u32 __bitwise __le32;
typedef __u32 __bitwise __be32;
typedef __u64 __bitwise __le64;
typedef __u64 __bitwise __be64;

typedef __u16 __bitwise __sum16;
typedef __u32 __bitwise __wsum;

static inline __le16 cpu_to_le16(uint16_t x)
{
	return (__force __le16)htole16(x);
}
static inline __le32 cpu_to_le32(uint32_t x)
{
	return (__force __le32)htole32(x);
}
static inline __le64 cpu_to_le64(uint64_t x)
{
	return (__force __le64)htole64(x);
}

static inline uint16_t le16_to_cpu(__le16 x)
{
	return le16toh((__force __u16)x);
}
static inline uint32_t le32_to_cpu(__le32 x)
{
	return le32toh((__force __u32)x);
}
static inline uint64_t le64_to_cpu(__le64 x)
{
	return le64toh((__force __u64)x);
}

struct nvme_passthru_cmd {
	__u8	opcode;
	__u8	flags;
	__u16	rsvd1;
	__u32	nsid;
	__u32	cdw2;
	__u32	cdw3;
	__u64	metadata;
	__u64	addr;
	__u32	metadata_len;
	__u32	data_len;
	__u32	cdw10;
	__u32	cdw11;
	__u32	cdw12;
	__u32	cdw13;
	__u32	cdw14;
	__u32	cdw15;
	__u32	timeout_ms;
	__u32	result;
};

struct nvme_user_io {
	__u8	opcode;
	__u8	flags;
	__u16	control;
	__u16	nblocks;
	__u16	rsvd;
	__u64	metadata;
	__u64	addr;
	__u64	slba;
	__u32	dsmgmt;
	__u32	reftag;
	__u16	apptag;
	__u16	appmask;
};

enum nvme_opcode {
	nvme_cmd_flush		= 0x00,
	nvme_cmd_write		= 0x01,
	nvme_cmd_read		= 0x02,
	nvme_cmd_write_uncor	= 0x04,
	nvme_cmd_compare	= 0x05,
	nvme_cmd_zone_info	= 0x06,
	nvme_cmd_write_zeroes	= 0x08,
	nvme_cmd_dsm		= 0x09,
	nvme_cmd_verify		= 0x0c,
	nvme_cmd_resv_register	= 0x0d,
	nvme_cmd_resv_report	= 0x0e,
	nvme_cmd_resv_acquire	= 0x11,
	nvme_cmd_resv_release	= 0x15,
};

enum nvme_admin_opcode {
	nvme_admin_delete_sq		= 0x00,
	nvme_admin_create_sq		= 0x01,
	nvme_admin_get_log_page		= 0x02,
	nvme_admin_delete_cq		= 0x04,
	nvme_admin_create_cq		= 0x05,
	nvme_admin_identify		= 0x06,
	nvme_admin_abort_cmd		= 0x08,
	nvme_admin_set_features		= 0x09,
	nvme_admin_get_features		= 0x0a,
	nvme_admin_async_event		= 0x0c,
	nvme_admin_ns_mgmt		= 0x0d,
	nvme_admin_activate_fw		= 0x10,
	nvme_admin_download_fw		= 0x11,
	nvme_admin_dev_self_test	= 0x14,
	nvme_admin_ns_attach		= 0x15,
	nvme_admin_keep_alive		= 0x18,
	nvme_admin_directive_send	= 0x19,
	nvme_admin_directive_recv	= 0x1a,
	nvme_admin_virtual_mgmt		= 0x1c,
	nvme_admin_nvme_mi_send		= 0x1d,
	nvme_admin_nvme_mi_recv		= 0x1e,
	nvme_admin_dbbuf		= 0x7C,
	nvme_admin_format_nvm		= 0x80,
	nvme_admin_security_send	= 0x81,
	nvme_admin_security_recv	= 0x82,
	nvme_admin_sanitize_nvm		= 0x84,
};

struct nvme_id_power_state {
	__le16			max_power;	/* centiwatts */
	__u8			rsvd2;
	__u8			flags;
	__le32			entry_lat;	/* microseconds */
	__le32			exit_lat;	/* microseconds */
	__u8			read_tput;
	__u8			read_lat;
	__u8			write_tput;
	__u8			write_lat;
	__le16			idle_power;
	__u8			idle_scale;
	__u8			rsvd19;
	__le16			active_power;
	__u8			active_work_scale;
	__u8			rsvd23[9];
};

struct nvme_id_ctrl {
	__le16			vid;
	__le16			ssvid;
	char			sn[20];
	char			mn[40];
	char			fr[8];
	__u8			rab;
	__u8			ieee[3];
	__u8			cmic;
	__u8			mdts;
	__le16			cntlid;
	__le32			ver;
	__le32			rtd3r;
	__le32			rtd3e;
	__le32			oaes;
	__le32			ctratt;
	__le16			rrls;
	__u8			rsvd102[154];
	__le16			oacs;
	__u8			acl;
	__u8			aerl;
	__u8			frmw;
	__u8			lpa;
	__u8			elpe;
	__u8			npss;
	__u8			avscc;
	__u8			apsta;
	__le16			wctemp;
	__le16			cctemp;
	__le16			mtfa;
	__le32			hmpre;
	__le32			hmmin;
	__u8			tnvmcap[16];
	__u8			unvmcap[16];
	__le32			rpmbs;
	__le16			edstt;
	__u8			dsto;
	__u8			fwug;
	__le16			kas;
	__le16			hctma;
	__le16			mntmt;
	__le16			mxtmt;
	__le32			sanicap;
	__le32			hmminds;
	__le16			hmmaxd;
	__le16			nsetidmax;
	__u8			rsvd340[2];
	__u8			anatt;
	__u8			anacap;
	__le32			anagrpmax;
	__le32			nanagrpid;
	__u8			rsvd352[160];
	__u8			sqes;
	__u8			cqes;
	__le16			maxcmd;
	__le32			nn;
	__le16			oncs;
	__le16			fuses;
	__u8			fna;
	__u8			vwc;
	__le16			awun;
	__le16			awupf;
	__u8			nvscc;
	__u8			nwpc;
	__le16			acwu;
	__u8			rsvd534[2];
	__le32			sgls;
	__le32			mnan;
	__u8			rsvd544[224];
	char			subnqn[256];
	__u8			rsvd1024[768];
	__le32			ioccsz;
	__le32			iorcsz;
	__le16			icdoff;
	__u8			ctrattr;
	__u8			msdbd;
	__u8			rsvd1804[244];
	struct nvme_id_power_state	psd[32];
	__u8			vs[1024];
};

struct nvme_lbaf {
	__le16			ms;
	__u8			ds;
	__u8			rp;
};

struct nvme_id_ns {
	__le64			nsze;
	__le64			ncap;
	__le64			nuse;
	__u8			nsfeat;
	__u8			nlbaf;
	__u8			flbas;
	__u8			mc;
	__u8			dpc;
	__u8			dps;
	__u8			nmic;
	__u8			rescap;
	__u8			fpi;
	__u8			dlfeat;
	__le16			nawun;
	__le16			nawupf;
	__le16			nacwu;
	__le16			nabsn;
	__le16			nabo;
	__le16			nabspf;
	__le16			noiob;
	__u8			nvmcap[16];
	//Gunhee, Choi zns ssd command --start
	__u64			zonesize;
	__u32			activezones;
	__u32			openzones;
	//--end
	__u8			rsvd80[6];
	//Gunhee, Choi zns ssd command --start
	__u16			totalzones;
	//--end
	__u8			rsvd88[4];
	__le32			anagrpid;
	__u8			rsvd96[3];
	__u8			nsattr;
	__le16			nvmsetid;
	__le16			endgid;
	__u8			nguid[16];
	__u8			eui64[8];
	struct nvme_lbaf	lbaf[16];
	__u8			rsvd192[192];
	__u8			vs[3712];
};

struct nvme_zone_info_entry {
	/*Gunhee, Choi*/
	/*Zone Information Enry Structure (total 64 bytes)*/
	__u8		zone_condition_rsvd : 4;
	__u8		rsvd0 : 4;
	__u8		rsvd1 : 4;
	__u8		zone_condition : 4;
	__u8		rsvd8[6];
	__u64		zone_capacity;
	__u64		zone_start_lba;
	__u64		write_pointer;
	__u64		cnt_read;
	__u64		cnt_write;
	__u32		cnt_reset;
	__u8		rsvd56[12];
};

struct nvme_zone_info {
	/*Gunhee, Choi*/
	/*Zone Information Enry Structure (total 64 bytes)*/
	__u64	number_zones;
	__u8	rsvd8[56];
	struct nvme_zone_info_entry * zone_entrys;
};

struct zns_zone_info {
	struct nvme_zone_info_entry zone_entry;
	int zone_number;
	int level;
};

struct zns_share_info {
	int fd;
	__u64 zonesize;
	__u32 activezones;
	__u32 openzones;
	__u16 totalzones;
	struct zns_zone_info * zone_list;
};

struct zns_block {
	__u8 data[BLOCK_SIZE];
};

struct zns_sector {
	struct zns_block data[8];
};

#define nvme_admin_cmd nvme_passthru_cmd

#define NVME_IOCTL_ID		_IO('N', 0x40)
#define NVME_IOCTL_ADMIN_CMD	_IOWR('N', 0x41, struct nvme_admin_cmd)
#define NVME_IOCTL_SUBMIT_IO	_IOW('N', 0x42, struct nvme_user_io)
#define NVME_IOCTL_IO_CMD	_IOWR('N', 0x43, struct nvme_passthru_cmd)
#define NVME_IOCTL_RESET	_IO('N', 0x44)
#define NVME_IOCTL_SUBSYS_RESET	_IO('N', 0x45)
#define NVME_IOCTL_RESCAN	_IO('N', 0x46)

void * zns_info_ctrl(int fd, void * data);
void * zns_info_ctrl_print(void * data);

void * zns_info_ns(int fd, void * data);
void * zns_info_ns_print(void * data);

int zns_init(char * dev, struct zns_share_info * zonelist);
int * zns_init_print(struct zns_share_info * zonelist);

__u64 get_zone_to_slba(struct zns_share_info * zonelist, int zonenumber);

int zns_write_request(int fd, void * write_data, int nblocks, __u64 slba);
int zns_write(struct zns_share_info * zonelist, void * write_data, int data_size, int zone_number, __u64 offset);
int zns_read_request(int fd, void * read_data, int nblocks, __u64 slba);
int zns_read(struct zns_share_info * zonelist, void * write_data, int data_size, int zone_number, __u64 offset);

int zns_get_log_entry_info(int fd, void * data, __u64 zid);
int zns_get_total_log_entry_info(int fd, int nzones);
int zns_log_info_entry_print(int num, void * data);

int zns_update_zone_info(struct zns_share_info * zonelist, int zonenumber);

int zns_zone_io_managemnet(int fd, __u64 slba, __u64 action);
int zns_zone_finish_request(int fd, __u64 slba);
int zns_zone_open_request(int fd, __u64 slba);
int zns_zone_reset_request(int fd, __u64 slba);
int zns_zone_finish(struct zns_share_info * zonelist, int zonenumber);
int zns_zone_open(struct zns_share_info * zonelist, int zonenumber);
int zns_zone_reset(struct zns_share_info * zonelist, int zonenumber);

#ifdef __cpluscplus
}
#endif
#endif /*m2controller.h*/