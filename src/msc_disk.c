/* 
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include "hardware/flash.h"
#include "hardware/sync.h"
#include "bsp/board.h"

#include "tusb.h"

#include "msc_disk.h"
#include <mrubyc.h>

// whether host does safe-eject
static bool ejected = false;

#include "version.h"

#define PRK_DESCRIPTION \
"PRK Firmware " PRK_VERSION " (" PRK_BUILDDATE " revision " PRK_REVISION ")"

/* PRK_DESCRIPTION should be located at the beginning of README. see msc_init() */
#define README_CONTENTS \
PRK_DESCRIPTION "\n\nWelcome to PRK Firmware!\n\n\
Usage:\n\
- Drag and drop your `keymap.rb` into this directory\n\
- Then, your keyboard will be automatically rebooted. That's all!\n\n\
Notice:\n\
- Make sure you always have a backup of your `keymap.rb`\n\
  because upgrading prk_firmware-*.uf2 will remove it from flash\n\n\
https://github.com/picoruby/prk_firmware\n"

#define README_LENGTH (sizeof(README_CONTENTS) - 1)
#define FAT_START ( (uint8_t*)FLASH_MMAP_ADDR+FLASH_SECTOR_SIZE*1 )

void msc_write_file(const char *filename, const uint8_t *data, uint16_t length);
void c_find_file(mrb_vm *vm, mrb_value *v, int argc);
void c_read_file(mrb_vm *vm, mrb_value *v, int argc);

uint8_t msc_disk[4][SECTOR_SIZE] =
{
  //------------- Block0: Boot Sector -------------//
  // byte_per_sector    = SECTOR_SIZE; fat12_sector_num_16  = SECTOR_COUNT;
  // sector_per_cluster = 1; reserved_sectors = 1;
  // fat_num            = 1; fat12_root_entry_num = 16;
  // sector_per_fat     = 1; sector_per_track = 1; head_num = 1; hidden_sectors = 0;
  // drive_number       = 0x80; media_type = 0xf8; extended_boot_signature = 0x29;
  // filesystem_type    = "FAT12   "; volume_serial_number = 0x1234; volume_label = "PRKFirmware";
  {
    0xEB, 0x3C, 0x90,       /*    BS_jmpBoot */
    0x4D, 0x53, 0x44, 0x4F, 0x53, 0x35, 0x2E, 0x30,   /* BS_OEMName "MSDOS5.0" */
    0x00, 0x10,             /* ** BPB_BytePerSec ** */
    0x01,                   /*    BPB_SecPerClus */
    0x01, 0x00,             /*    BPB_RsvdSecCnt */
    0x01,                   /*    BPB_NumFATs */
    0x80, 0x00,             /* ** BPB_RootEntCnt ** */
    0x00, 0x01,             /* ** BPB_TotSec16 ** */
    0xF8,                   /*    BPB_Media  */
    0x01, 0x00,             /*    BPB_FATSz16 */
    0x01, 0x00,             /*    BPB_SecPerTrk */
    0x01, 0x00,             /*    BPB_NumHeads */
    0x00, 0x00, 0x00, 0x00, /*    BPB_HiddSec */
    0x00, 0x00, 0x00, 0x00, /*    BPB_TotSec32 */
    0x80, 0x00, 0x29, 0x34, 0x12, 0x00, 0x00, 'P' , 'R' , 'K' , 'F' , 'i' ,
    'r' , 'm' , 'w' , 'a' , 'r' , 'e' , 0x46, 0x41, 0x54, 0x31, 0x32, 0x20, 0x20, 0x20, 0x00, 0x00,
    // Zerofill until BS_BootSign (0xAA55 at offset 510)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0xAA
  },

  //------------- Block1: FAT12 Table -------------//
  {
    0xF8, 0xFF, 0xFF, 0xFF, 0x0F // // first 2 entries must be F8FF, third entry is cluster end of readme file
  },

  //------------- Block2: Root Directory -------------//
  {
    // first entry is volume label
    'P' , 'R' , 'K' , 'F' , 'i' , 'r' , 'm' , 'w' , 'a' , 'r' , 'e' , 0x08, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4F, 0x6D, 0x65, 0x43, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    // second entry is readme file
    'R' , 'E' , 'A' , 'D' , 'M' , 'E' , ' ' , ' ' , 'T' , 'X' , 'T' , 0x20, 0x00, 0xC6, 0x52, 0x6D,
    0x65, 0x43, 0x65, 0x43, 0x00, 0x00, 0x88, 0x6D, 0x65, 0x43, 0x02, 0x00,
    // readme's files size (4 Bytes little endian)
    README_LENGTH & 0xFF,
    README_LENGTH >> 8 & 0xFF,
    README_LENGTH >> 16 & 0xFF,
    README_LENGTH >> 24
  },

  //------------- Block3: Readme Content -------------//
  README_CONTENTS
};

// Invoked when received SCSI_CMD_INQUIRY
// Application fill vendor id, product id and revision with string up to 8, 16, 4 characters respectively
void
tud_msc_inquiry_cb(uint8_t lun, uint8_t vendor_id[8], uint8_t product_id[16], uint8_t product_rev[4])
{
  (void) lun;

  const char vid[] = "TinyUSB";
  const char pid[] = "Mass Storage";
  const char rev[] = "1.0";

  memcpy(vendor_id  , vid, strlen(vid));
  memcpy(product_id , pid, strlen(pid));
  memcpy(product_rev, rev, strlen(rev));
}

// Invoked when received Test Unit Ready command.
// return true allowing host to read/write this LUN e.g SD card inserted
bool
tud_msc_test_unit_ready_cb(uint8_t lun)
{
  (void) lun;

  // RAM disk is ready until ejected
  if (ejected) {
    tud_msc_set_sense(lun, SCSI_SENSE_NOT_READY, 0x3a, 0x00);
    return false;
  }

  return true;
}

// Invoked when received SCSI_CMD_READ_CAPACITY_10 and SCSI_CMD_READ_FORMAT_CAPACITY to determine the disk size
// Application update block count and block size
void
tud_msc_capacity_cb(uint8_t lun, uint32_t* block_count, uint16_t* block_size)
{
  (void) lun;

  *block_count = SECTOR_COUNT;
  *block_size  = SECTOR_SIZE;
}

// Invoked when received Start Stop Unit command
// - Start = 0 : stopped power mode, if load_eject = 1 : unload disk storage
// - Start = 1 : active mode, if load_eject = 1 : load disk storage
bool
tud_msc_start_stop_cb(uint8_t lun, uint8_t power_condition, bool start, bool load_eject)
{
  (void) lun;
  (void) power_condition;

  if ( load_eject )
  {
    if (start)
    {
      // load disk storage
      ejected = false;
    }else
    {
      // unload disk storage
      ejected = true;
    }
  }

  return true;
}

// Callback invoked when received READ10 command.
// Copy disk's data to buffer (up to bufsize) and return number of copied bytes.
int32_t
tud_msc_read10_cb(uint8_t lun, uint32_t lba, uint32_t offset, void* buffer, uint32_t bufsize)
{
  (void) lun;
  // out of ramdisk
  if ( lba >= SECTOR_COUNT ) return -1;
  memcpy(buffer, (void *)(FLASH_MMAP_ADDR + lba * SECTOR_SIZE + offset), bufsize);
  return bufsize;
}

bool
tud_msc_is_writable_cb (uint8_t lun)
{
  (void) lun;
#ifdef PICORUBY_MSC_READONLY
  return false;
#else
  return true;
#endif
}

// Callback invoked when received WRITE10 command.
// Process data in buffer to disk's storage and return number of written bytes
int32_t
tud_msc_write10_cb(uint8_t lun, uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t bufsize)
/*
 * TODO: Cache coherency problem can be avoided if bufsize == SECTOR_SIZE(4094)
 */
{
  (void) lun;
  // out of ramdisk
  if ( lba >= SECTOR_COUNT ) return -1;
  uint32_t ints = save_and_disable_interrupts();
  if (offset == 0) {
    flash_range_erase(FLASH_TARGET_OFFSET + lba * SECTOR_SIZE, SECTOR_SIZE);
  }
  flash_range_program(FLASH_TARGET_OFFSET + lba * SECTOR_SIZE + offset, buffer, bufsize);
  restore_interrupts(ints);
  return bufsize;
}

// Callback invoked when received an SCSI command not in built-in list below
// - READ_CAPACITY10, READ_FORMAT_CAPACITY, INQUIRY, MODE_SENSE6, REQUEST_SENSE
// - READ10 and WRITE10 has their own callbacks
int32_t
tud_msc_scsi_cb (uint8_t lun, uint8_t const scsi_cmd[16], void* buffer, uint16_t bufsize)
{
  // read10 & write10 has their own callback and MUST not be handled here

  void const* response = NULL;
  int32_t resplen = 0;

  // most scsi handled is input
  bool in_xfer = true;

  switch (scsi_cmd[0])
  {
    case SCSI_CMD_PREVENT_ALLOW_MEDIUM_REMOVAL:
      // Host is about to read/write etc ... better not to disconnect disk
      resplen = 0;
    break;

    default:
      // Set Sense = Invalid Command Operation
      tud_msc_set_sense(lun, SCSI_SENSE_ILLEGAL_REQUEST, 0x20, 0x00);

      // negative means error -> tinyusb could stall and/or response with failed status
      resplen = -1;
    break;
  }

  // return resplen must not larger than bufsize
  if ( resplen > bufsize ) resplen = bufsize;

  if ( response && (resplen > 0) )
  {
    if(in_xfer)
    {
      memcpy(buffer, response, resplen);
    }else
    {
      // SCSI output
    }
  }

  return resplen;
}

void
msc_init(void)
{
#ifndef FORCE_FORMAT_FLASH
  if (
      strncmp(
        (void *)(FLASH_MMAP_ADDR + SECTOR_SIZE * 3),
        PRK_DESCRIPTION,
        sizeof(PRK_DESCRIPTION) - 1 /* Skips null term */
      )
     )
#endif
  {
    /* These functions know XIP_BASE so you just have to give them FLASH_TARGET_OFFSET */
    uint32_t ints = save_and_disable_interrupts();
    flash_range_erase(FLASH_TARGET_OFFSET, SECTOR_SIZE * 4);
    flash_range_program(FLASH_TARGET_OFFSET, msc_disk[0], SECTOR_SIZE * 4);
    restore_interrupts(ints);
  }
#ifdef DEBUG_FAT
  uint8_t buf_rootdir[FLASH_SECTOR_SIZE], buf_fat[FLASH_SECTOR_SIZE];
  memcpy(buf_rootdir, (uint8_t*)(FLASH_MMAP_ADDR+SECTOR_SIZE*2), FLASH_SECTOR_SIZE);
  memcpy(buf_fat, (uint8_t*)(FLASH_MMAP_ADDR+SECTOR_SIZE*1), FLASH_SECTOR_SIZE);
  uint32_t ints = save_and_disable_interrupts();
  msc_write_file("FAT     BIN", buf_fat, FLASH_SECTOR_SIZE-1);
  msc_write_file("ROOTDIR BIN", buf_rootdir, FLASH_SECTOR_SIZE-1);
  restore_interrupts(ints);
#endif
  mrbc_define_method(0, mrbc_class_object, "write_file_internal", c_write_file_internal);
  mrbc_define_method(0, mrbc_class_object, "find_file", c_find_file);
  mrbc_define_method(0, mrbc_class_object, "read_file", c_read_file);
}

/*
 * Only works in top directory
 * Only works if SFN (short file name)
 */
void
msc_findDirEnt(const char *filename, DirEnt *entry)
{
  void *addr;
  for (
        addr = (void *)(FLASH_MMAP_ADDR + (SECTOR_SIZE * 2) + 32);
        (uint32_t)addr < FLASH_MMAP_ADDR + SECTOR_SIZE * SECTOR_COUNT;
        addr += 32
      )
  {
    entry->Name[0] = '\0';
    memcpy(entry, addr, 32);
    if (entry->Name[0] == 0xe5) continue;
    if (entry->Name[0] == '\0') return;
    if (strncmp(filename, entry->Name, 11) == 0 && entry->Attr == 0x20) return;
  }
}

void c_find_file(mrb_vm *vm, mrb_value *v, int argc) {
  uint8_t *rb_filename = GET_STRING_ARG(1);
  DirEnt entry;
  mrbc_value return_val = mrbc_array_new(vm, 2);
  mrbc_array *rb_array = return_val.array;

  msc_findDirEnt(rb_filename, &entry);

  if(strncmp(rb_filename, entry.Name, 11)==0) {
    rb_array->n_stored = 2;
    mrbc_set_integer( rb_array->data, entry.FstClusLO );
    mrbc_set_integer( rb_array->data+1, entry.FileSize );
    SET_RETURN(return_val);
  } else {
    SET_NIL_RETURN();
  }
}

uint8_t*
msc_read_sector(uint16_t sector)
{
  return (uint8_t*)(FLASH_MMAP_ADDR + (SECTOR_SIZE * (1+sector)));
}

void
c_read_file(mrb_vm *vm, mrb_value *v, int argc) {
  uint16_t sector = GET_INT_ARG(1);
  uint16_t length = GET_INT_ARG(2);

  mrbc_value rb_val_array = mrbc_array_new(vm, length);
  mrbc_array *rb_array = rb_val_array.array;
  uint8_t *data = msc_read_sector(sector);

  rb_array->n_stored = length;
  for(uint16_t i=0; i<length; i++) {
    mrbc_set_integer( (rb_array->data)+i, data[i] );
  }

  SET_RETURN(rb_val_array);
}

void
msc_write_file(const char *filename, const uint8_t *data, uint16_t length)
{
  DirEnt entry;
  void* addr;
  /*
  Sector#0 : Boot (Reserved)
  Sector#1 : FAT (Cluster#0)
  Sector#2 : RootDirectory (Cluster#1)
  Sector#3 : Cluster#2
  Sector#4 : Cluster#3
   */
  uint8_t dir_entry_index_to_write = 0;
  uint16_t cluster_id_to_write = 0;
  bool is_update = false;
  
  // search empty entry
  for (uint8_t i=1; i<128; i++ )
  {
    addr = (void *)(FLASH_MMAP_ADDR + (SECTOR_SIZE * 2) + 32*i);
    memcpy(&entry, addr, 32);
    if (strncmp(entry.Name, filename, 11)==0)
    {
      dir_entry_index_to_write = i;
      cluster_id_to_write = entry.FstClusLO;
      is_update = true;
      break;
    }
    else if (entry.Name[0] == 0xe5 && dir_entry_index_to_write == 0)
    {
      dir_entry_index_to_write = i;

      // continue to search exist "update" entry
      continue; 
    }
    else if (entry.Name[0] == '\0')
    {
      if(dir_entry_index_to_write==0) {
        dir_entry_index_to_write = i;
      }
      break;
    }
  }
  
  if(! is_update) {
    for(uint8_t i=0; i<128; i++) {
      uint16_t fatusage;

      if(i%2==0) {
        fatusage = ((uint16_t)*(FAT_START+i/2*3))<<4 | ((*(FAT_START+i/2*3+1)) & 0x0F);
      } else {
        fatusage = ( (uint16_t)(*(FAT_START+i/2*3+1)) &0xF0 )<<4 | (*(FAT_START+i/2*3+2));
      }

      if(fatusage==0) {
        cluster_id_to_write = i;
        break;
      }
    }
  }

  if(cluster_id_to_write && dir_entry_index_to_write) {
    uint8_t buf_rootdir[FLASH_SECTOR_SIZE];
    uint8_t buf_fat[FLASH_SECTOR_SIZE];
    // There are (1) Reserved Sector
    // copy RootDir to erase
    memcpy(buf_rootdir, (void*)(FLASH_MMAP_ADDR+FLASH_SECTOR_SIZE*2), FLASH_SECTOR_SIZE);

    // copy DirEntry from README.TXT
    if(is_update) {
      memcpy(&entry, buf_rootdir+32*dir_entry_index_to_write, 32);
    } else {
      // copy FAT to erase
      memcpy(buf_fat, (void*)(FLASH_MMAP_ADDR+FLASH_SECTOR_SIZE*1), FLASH_SECTOR_SIZE);
      
      if(cluster_id_to_write%2==0) {
        buf_fat[cluster_id_to_write/2*3] = 0xFF;
        buf_fat[cluster_id_to_write/2*3+1] = 0x0F | (0xF0 & buf_fat[cluster_id_to_write/2*3+1]);  
      } else {
        buf_fat[cluster_id_to_write/2*3+1] = 0xF0 | (0x0F & buf_fat[cluster_id_to_write/2*3+1]); 
        buf_fat[cluster_id_to_write/2*3+2] = 0xFF;
      }

      memcpy(&entry, buf_rootdir+32, 32);
      entry.FstClusLO = cluster_id_to_write;
      entry.NTRes = 0x18; // Filename is small-case
      memcpy(&entry.Name, filename, 11);
    }
    entry.FileSize = length;
    memcpy(buf_rootdir+32*dir_entry_index_to_write, &entry, 32);

    uint32_t ints = save_and_disable_interrupts();
    // write RootDir
    // RootDirSector is Sector#1
    flash_range_erase(FLASH_TARGET_OFFSET+FLASH_SECTOR_SIZE*2, FLASH_SECTOR_SIZE);
    flash_range_program(FLASH_TARGET_OFFSET+FLASH_SECTOR_SIZE*2, buf_rootdir, FLASH_SECTOR_SIZE);

    // write data
    flash_range_erase(FLASH_TARGET_OFFSET+FLASH_SECTOR_SIZE*(1+cluster_id_to_write), FLASH_SECTOR_SIZE);
    flash_range_program(FLASH_TARGET_OFFSET+FLASH_SECTOR_SIZE*(1+cluster_id_to_write), data, (length/FLASH_PAGE_SIZE+1)*FLASH_PAGE_SIZE);
    if(! is_update) {
      // write FAT
      flash_range_erase(FLASH_TARGET_OFFSET+FLASH_SECTOR_SIZE*1, FLASH_SECTOR_SIZE);
      flash_range_program(FLASH_TARGET_OFFSET+FLASH_SECTOR_SIZE*1, buf_fat, FLASH_SECTOR_SIZE);
    }
    restore_interrupts(ints);
  }
}

void c_write_file_internal(mrb_vm *vm, mrb_value *v, int argc) {
  uint16_t limit;
  uint8_t *rb_filename = GET_STRING_ARG(1);
  uint8_t *c_data;
  mrbc_object value = GET_ARG(2);
  
  if(value.tt == MRBC_TT_STRING) {
    mrbc_string* str = value.string;
    c_data = str->data;
    limit = str->size;
  } else {
    return;
  }

  if(limit>FLASH_SECTOR_SIZE) {
    limit = FLASH_SECTOR_SIZE;
  }
  
  msc_write_file(rb_filename, c_data, limit);
}

/*
 * FAT12 cluster chain structure: Each entry consists of 12 bit data
 * 0  00000000  LSB of FAT[0]
 * 1  11110000  LSB of FAT[1] and MSB of FAT[0]
 * 2  11111111  MSB of FAT[1]
 * 3  22222222  LSB of FAT[2]
 * 4  33332222  LSB of FAT[3] and MSB of FAT[2]
 * 5  33333333  MSB of FAT[3]
 * ^  ^^^^^^^^
 * |  entry data
 * byte offset
 */
void
msc_loadFile(uint8_t *file, DirEnt *entry)
{
  uint16_t clusNum = entry->FstClusLO;
  uint16_t nextClusNum;
  uint8_t *lsb, *clusAddr;
  uint16_t copySize;
  for (int i = 0; clusNum < 0xFF8; i++) {
    if (clusNum & 1) { // Odd number in FAT12
      lsb = (uint8_t *)(FLASH_MMAP_ADDR + SECTOR_SIZE + (clusNum - 1) / 2 * 3 + 1);
      nextClusNum = (uint16_t)((*(lsb + 1) << 4) + (uint16_t)((*lsb >> 4) & 0b1111));
    } else {           // Even number in FAT12
      lsb = (uint8_t *)(FLASH_MMAP_ADDR + SECTOR_SIZE + clusNum / 2 * 3);
      nextClusNum = (uint16_t)((*(lsb + 1) & 0b1111) << 8) + (uint16_t)*lsb;
    }
    clusAddr = (uint8_t *)(FLASH_MMAP_ADDR + SECTOR_SIZE * (1 + clusNum));
    copySize = (nextClusNum < 0xFF8) ? SECTOR_SIZE : (entry->FileSize % SECTOR_SIZE);
    memcpy(file + i * SECTOR_SIZE, clusAddr, copySize);
    clusNum = nextClusNum;
  }
}

//volatile char ppp;
void
tud_msc_write10_complete_cb(uint8_t lun)
{
  (void)lun;
  autoreload_state = AUTORELOAD_READY;
}


//--------------------------------------------------------------------+
// USB CDC
//--------------------------------------------------------------------+
void
c_cdc_task(mrb_vm *vm, mrb_value *v, int argc)
{
  // connected() check for DTR bit
  // Most but not all terminal client set this when making connection
  // if ( tud_cdc_connected() )
  {
    // connected and there are data available
    if ( tud_cdc_available() )
    {
      // read datas
      char buf[64];
      uint32_t count = tud_cdc_read(buf, sizeof(buf));
      (void) count;

      // Echo back
      // Note: Skip echo by commenting out write() and write_flush()
      // for throughput test e.g
      //    $ dd if=/dev/zero of=/dev/ttyACM0 count=10000
      //tud_cdc_write(buf, count);
      //tud_cdc_write_flush();
    }
  }
}

// Invoked when cdc when line state changed e.g connected/disconnected
void
tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts)
{
  (void) itf;
  (void) rts;

  // TODO set some indicator
  if ( dtr )
  {
    // Terminal connected
  }else
  {
    // Terminal disconnected
  }
}

// Invoked when CDC interface received data from host
void
tud_cdc_rx_cb(uint8_t itf)
{
  (void) itf;
}
