#ifndef FAT16PARSER_FILE_PARSER_H
#define FAT16PARSER_FILE_PARSER_H

#include <stdio.h>
#include <stdint.h>
#include "disk_parser.h"

typedef struct clusters_chain_t {
    uint16_t *clusters;
    size_t size;
} ClustersChain;

typedef struct file_t {
    uint16_t first_cluster;
    struct clusters_chain_t *clusters_chain;
    long offset;
    size_t size;
    Volume *volume;
} File;

typedef struct dir_entry_t {
    char name[12];
    size_t size;
    int is_archived;
    int is_readonly;
    int is_system;
    int is_hidden;
    int is_directory;
} DirEntry;

File *file_open(Volume *volume, const char *file_name);

int file_close(FILE *stream);

size_t file_read(void *ptr, size_t size, size_t nmemb, File *stream);

int32_t file_seek(File *stream, int32_t offset, int whence);

#endif //FAT16PARSER_FILE_PARSER_H
