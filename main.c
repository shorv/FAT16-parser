#include <stdio.h>
#include "disk_parser.h"

int main() {
    struct disk_t* disk = disk_open_from_file("sail_fat16_volume.img");
    struct volume_t* volume = fat_open(disk, 0);

    fat_close(volume);
    disk_close(disk);
    return 0;
}
