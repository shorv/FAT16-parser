#include "disk_parser.h"

Disk *disk_open_from_file(const char *volume_file_name) {
    if (!volume_file_name) {
        errno = EFAULT;
        return NULL;
    }

    FILE *file;
    file = fopen(volume_file_name, "rb");
    if (!file) {
        errno = ENOENT;
        return NULL;
    }

    Disk *disk = calloc(1, sizeof(Disk));
    if (!disk) {
        errno = ENOMEM;
        return NULL;
    }

    disk->file = file;
    return disk;
}

int disk_read(Disk *pdisk, int32_t first_sector, void *buffer, int32_t sectors_to_read) {
    if (!pdisk || !pdisk->file || first_sector < 0 || !buffer || sectors_to_read <= 0) {
        errno = EFAULT;
        return -1;
    }

    fseek(pdisk->file, first_sector * 512, SEEK_SET);
    if (fread(buffer, 1, sectors_to_read * 512, pdisk->file) != (size_t) sectors_to_read * 512) {
        errno = ERANGE;
        return -1;
    }

    return 0;
}

int disk_close(Disk *pdisk) {
    if (!pdisk || !pdisk->file) {
        errno = EFAULT;
        return -1;
    }

    fclose(pdisk->file);
    free(pdisk);

    return 0;
}

Volume *fat_open(Disk *pdisk, uint32_t first_sector) {
    if (!pdisk || !pdisk->file) {
        errno = EFAULT;
        return NULL;
    }

    char boot_sector_bytes[512];
    if (disk_read(pdisk, first_sector, boot_sector_bytes, 1) != 0) {
        errno = EINVAL;
        return NULL;
    }

    if (((uint8_t *) boot_sector_bytes)[510] != 0x55 && ((uint8_t *) boot_sector_bytes)[511] != 0xaa) {
        errno = EFAULT;
        return NULL;
    }

    BootSector *boot_sector = initialize_boot_sector_t(boot_sector_bytes);
    if (!boot_sector) {
        return NULL;
    }

    if (!validate_boot_sector_t(boot_sector)) {
        errno = EINVAL;
        return NULL;
    }

    Volume *volume = calloc(1, sizeof(Volume));
    if (!volume) {
        errno = ENOMEM;
        return NULL;
    }

    volume->disk = pdisk;
    volume->boot_sector = boot_sector;
    volume->first_fat_sector = boot_sector->reserved_sectors;
    volume->first_root_dir_sector = volume->first_fat_sector + boot_sector->fat_count * boot_sector->sectors_per_fat;
    volume->root_dir_sectors = ((boot_sector->root_dir_capacity * DIR_ENTRY_SIZE) / boot_sector->bytes_per_sector);

    if ((boot_sector->root_dir_capacity * DIR_ENTRY_SIZE) % boot_sector->bytes_per_sector != 0) {
        volume->root_dir_sectors++;
    }

    volume->first_data_sector = volume->first_root_dir_sector + volume->root_dir_sectors;

    uint64_t volume_size =
            boot_sector->logical_sectors16 == 0 ? boot_sector->logical_sectors32 : boot_sector->logical_sectors16;
    uint64_t user_size =
            volume_size - (boot_sector->fat_count * boot_sector->sectors_per_fat) - boot_sector->reserved_sectors -
            volume->root_dir_sectors;
    volume->available_clusters = user_size / boot_sector->sectors_per_cluster;

    volume->fat_table = calloc(boot_sector->bytes_per_sector * boot_sector->sectors_per_fat, sizeof(uint16_t));
    volume->bytes_per_cluster = boot_sector->sectors_per_cluster * boot_sector->bytes_per_sector;

    disk_read(pdisk, volume->first_fat_sector, volume->fat_table, boot_sector->sectors_per_fat);

    return volume;
}

BootSector *initialize_boot_sector_t(char boot_sector_bytes[512]) {
    BootSector *boot_sector = calloc(1, sizeof(BootSector));
    if (!boot_sector) {
        errno = ENOMEM;
        return NULL;
    }

    strncpy(boot_sector->oem_name, boot_sector_bytes + 0x03, 8);
    boot_sector->bytes_per_sector = *(uint16_t *) (boot_sector_bytes + 0x0B);
    boot_sector->sectors_per_cluster = *(uint8_t *) (boot_sector_bytes + 0x0D);
    boot_sector->reserved_sectors = *(uint16_t *) (boot_sector_bytes + 0x0E);
    boot_sector->fat_count = *(uint8_t *) (boot_sector_bytes + 0x10);
    boot_sector->root_dir_capacity = *(uint16_t *) (boot_sector_bytes + 0x11);
    boot_sector->logical_sectors16 = *(uint16_t *) (boot_sector_bytes + 0x13);
    boot_sector->media_type = *(uint8_t *) (boot_sector_bytes + 0x15);
    boot_sector->sectors_per_fat = *(uint16_t *) (boot_sector_bytes + 0x16);
    boot_sector->chs_sectors_per_track = *(uint16_t *) (boot_sector_bytes + 0x18);
    boot_sector->chs_tracks_per_cylinder = *(uint16_t *) (boot_sector_bytes + 0x1A);
    boot_sector->hidden_sectors = *(uint32_t *) (boot_sector_bytes + 0x1C);
    boot_sector->logical_sectors32 = *(uint32_t *) (boot_sector_bytes + 0x20);
    boot_sector->media_id = *(uint8_t *) (boot_sector_bytes + 0x24);
    boot_sector->chs_head = *(uint8_t *) (boot_sector_bytes + 0x25);
    boot_sector->ext_bpb_signature = *(uint8_t *) (boot_sector_bytes + 0x26);
    boot_sector->serial_number = *(uint32_t *) (boot_sector_bytes + 0x27);
    strncpy(boot_sector->volume_label, boot_sector_bytes + 0x2B, 11);
    strncpy(boot_sector->fsid, boot_sector_bytes + 0x36, 8);
    boot_sector->magic = *(uint16_t *) (boot_sector_bytes + 0xFE);

    return boot_sector;
}

int validate_boot_sector_t(BootSector *boot_sector) {
    if (!boot_sector) {
        errno = EFAULT;
        return 0;
    }

    return 1;
}

uint64_t cluster_of_index(struct volume_t *volume, unsigned int index) {
    return volume->first_data_sector + (index - 2) * volume->boot_sector->sectors_per_cluster;
}

int fat_close(struct volume_t *pvolume) {
    if (!pvolume) {
        errno = EFAULT;
        return -1;
    }

    return 0;
}
