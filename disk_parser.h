/** @file disk_parser.h
 *  @brief Function prototypes for initializing BootSector information and
 *  calculating Volume data based on information transferred from BootSector.
 *
 *  @author Jakub Rychlik (shorv)
 */

#ifndef FAT16PARSER_DISK_PARSER_H
#define FAT16PARSER_DISK_PARSER_H

#define DIR_ENTRY_SIZE 32

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

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

/** @brief Opens block device image file.
 *  @param volume_file_name Name of the block device image file.
 *  @return Pointer to filled Disk structure on success, otherwise null.
 */
Disk *disk_open_from_file(const char *volume_file_name);

/** @brief The function reads sectors_to_read blocks starting from the first_sector block.
 *  @param disk Pointer to initiated Disk structure.
 *  @param first_sector First sector we want to read.
 *  @param buffer Buffer where we want to save the read data.
 *  @param sectors_to_read Number of sectors we want to read starting from the first_sector block.
 *  @return Amount of blocks read on success, otherwise -1.
 */
int disk_read(Disk *disk, int32_t first_sector, void *buffer, int32_t sectors_to_read);

/** @brief Closes block device image file pointer and frees up disk memory.
 *  @param disk Pointer to initiated Disk structure.
 *  @return 0 on success, -1 on failure.
 */
int disk_close(Disk *disk);

/** @brief The function opens and checks the FAT volume available on the disk starting from first_sector.
 *  @param disk Pointer to initiated Disk structure.
 *  @param first_sector The sector from which we want the function to start opening the FAT volume.
 *  @return Pointer to filled Volume structure on success, otherwise null.
 */
Volume *fat_open(Disk *disk, uint32_t first_sector);

/** @brief Fills the BootSector structure with data.
 *  @param boot_sector_buffer The sector buffer where boot sector data is located.
 *  @return Pointer to filled BootSector structure on success, otherwise null.
 */
BootSector *initialize_boot_sector(char boot_sector_buffer[512]);

/** @brief Validates the data with which the BootSector structure has been filled.
 *  @param boot_sector Pointer to the BootSector structure whose data we want to validate.
 *  @return 1 when data is correct, otherwise 0.
 */
int validate_boot_sector(BootSector *boot_sector);

/** @brief Fills the Volume structure with data.
 *  @param boot_sector Pointer to the BootSector structure whose data will be used for calculations.
 *  @param disk Pointer to initiated Disk structure.
 *  @return Pointer to filled Volume structure on success, otherwise null.
 */
Volume *initialize_volume(BootSector *boot_sector, Disk *disk);

/** @brief Calculates correct cluster number of given index based on BootSector and Volume data.
 *  @param volume Pointer to the Volume.
 *  @param index Cluster index we want to be calculate.
 *  @return Fixed cluster index.
 */
uint64_t cluster_of_index(Volume *volume, unsigned int index);

/** @brief Frees up volume memory.
 *  @param volume Pointer to initiated Volume structure.
 *  @return 0 on success, -1 on failure.
 */
int fat_close(Volume *volume);

#endif //FAT16PARSER_DISK_PARSER_H
