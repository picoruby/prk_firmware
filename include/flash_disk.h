#ifndef FLASH_DISK_DEFINED_H_
#define FLASH_DISK_DEFINED_H_

#include <hardware/flash.h>
#include <ff.h>
#include <diskio.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * 1280 KiB + 768 KiB == 2 MiB
 * Fully exhausts Raspberry Pi Pico ROM because it's 2 MiB.
 * (Other RP2040 board may have a bigger ROM)
 */
#define FLASH_TARGET_OFFSET  0x00140000  /* 1280 KiB for program code */
#define FLASH_MMAP_ADDR      (XIP_BASE + FLASH_TARGET_OFFSET)
/* 4096 * 192 = 768 KiB */
#define SECTOR_SIZE          4096 /* == FLASH_SECTOR_SIZE */
#define SECTOR_COUNT         192  /* Seems FatFS allows 192 as the minimum */

int FLASH_disk_initialize(void);
int FLASH_disk_status(void);
int FLASH_disk_read(BYTE *buff, LBA_t sector, UINT count);
int FLASH_disk_write(const BYTE *buff, LBA_t sector, UINT count);
DRESULT FLASH_disk_ioctl(BYTE cmd, void *buff);

#ifdef __cplusplus
}
#endif

#endif /* FLASH_DISK_DEFINED_H_ */

