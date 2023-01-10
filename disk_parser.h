#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#ifndef FAT16PARSER_DISK_MANAGER_H
#define FAT16PARSER_DISK_MANAGER_H

typedef struct boot_sector_t {
    uint8_t jump_code[3];
    char oem_name[8];
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t fat_count;
    uint16_t root_dir_capacity;
    uint16_t logical_sectors16;
    uint8_t media_type;
    uint16_t sectors_per_fat;
    uint16_t chs_sectors_per_track;
    uint16_t chs_tracks_per_cylinder;
    uint32_t hidden_sectors;
    uint32_t logical_sectors32;
    uint8_t media_id;
    uint8_t chs_head;
    uint8_t ext_bpb_signature;
    uint32_t serial_number;
    char volume_label[11];
    char fsid[8];
    uint8_t boot_code[448];
    uint16_t magic;
}__attribute__ (( packed )) BootSector;

typedef struct volume_t {
    struct boot_sector_t *boot_sector;

    struct disk_t *disk;

    uint8_t *fat_table;

    uint64_t first_fat_sector;
    uint64_t first_root_dir_sector;
    uint64_t root_dir_sectors;
    uint64_t first_data_sector;
    uint32_t bytes_per_cluster;
    uint32_t available_clusters;
} __attribute__ (( packed )) Volume;

typedef struct disk_t {
    FILE *file;
} Disk;

struct disk_t *disk_open_from_file(const char *volume_file_name);

int disk_read(struct disk_t *pdisk, int32_t first_sector, void *buffer, int32_t sectors_to_read);

int disk_close(struct disk_t *pdisk);

struct volume_t *fat_open(struct disk_t *pdisk, uint32_t first_sector);

struct super_t *initialize_boot_sector_t(char buffer[512]);

int validate_boot_sector_t(struct boot_sector_t *boot_sector);

uint64_t cluster_of_index(struct volume_t *volume, unsigned int index);

int fat_close(struct volume_t *pvolume);

#endif //FAT16PARSER_DISK_MANAGER_H
