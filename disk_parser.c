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

int disk_read(struct disk_t *pdisk, int32_t first_sector, void *buffer, int32_t sectors_to_read) {
    if (!pdisk || first_sector < 0 || !buffer || sectors_to_read <= 0) {
        errno = EFAULT;
        return -1;
    }

    return 0;
}

int disk_close(struct disk_t *pdisk) {
    if (!pdisk) {
        errno = EFAULT;
        return -1;
    }

    return 0;
}

struct volume_t *fat_open(struct disk_t *pdisk, uint32_t first_sector) {
    if (!pdisk) {
        errno = EFAULT;
        return NULL;
    }

    struct volume_t *volume = calloc(1, sizeof(struct volume_t));
    if (!volume) {
        errno = ENOMEM;
        return NULL;
    }

    return volume;
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
