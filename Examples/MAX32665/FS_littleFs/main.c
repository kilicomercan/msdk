/******************************************************************************
 *
 * Copyright (C) 2022-2023 Maxim Integrated Products, Inc. (now owned by 
 * Analog Devices, Inc.),
 * Copyright (C) 2023-2024 Analog Devices, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************/

/**
 * @file    main.c
 * @brief   FS_littleFs!
 * @details This example uses littleFs file system and the w25 
 *          external spi device.
 */

/***** Includes *****/
#include <stdio.h>
#include <stdint.h>
#include "mxc_device.h"
#include "led.h"
#include "board.h"
#include "mxc_delay.h"
#include "max32665.h"
#include "spixf.h"
#include "lfs.h"
#include "Ext_Flash.h"

// *****************************************************************************
// variables used by the filesystem
lfs_t lfs;
lfs_file_t file;

#define BLOCK_SIZE 4096
#define BLOCK_COUNT 2048

static int __read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size){
    // Calculate the flash address
    uint32_t addr = (block * c->block_size) + off;

    // Use the SPIXF driver to read data
    if ( Ext_Flash_Read(addr, buffer, size, Ext_Flash_DataLine_Single) != 0) {
        return LFS_ERR_IO; // Return an error if the read operation fails
    }

    return 0; // Success
    return 0;
}

static int __prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size){
    // Calculate the flash address
    uint32_t addr = (block * c->block_size) + off;

    // Use the SPIXF driver to write data
    if (Ext_Flash_Program_Page(addr, buffer, size, Ext_Flash_DataLine_Single) != 0) {
        return LFS_ERR_IO; // Return an error if the write operation fails
    }
    return 0;
}

static int __erase(const struct lfs_config *c, lfs_block_t block){
    // Calculate the flash address for the block
    uint32_t addr = block * c->block_size;

    // Use the SPIXF driver to erase the block
    if (Ext_Flash_Erase(addr, c->block_size) != 0) {
        return LFS_ERR_IO; // Return an error if the erase operation fails
    }

    return 0;
}

static int __sync(const struct lfs_config *c){
    /**
     * @todo implement your own sync.
     */
    return 0;
}

// configuration of the filesystem is provided by this struct
const struct lfs_config cfg = {
    // block device operations
    .read  = __read,
    .prog  = __prog,
    .erase = __erase,
    .sync  = __sync,

    // block device configuration
    .read_size = 16,
    .prog_size = 16,
    .block_size = BLOCK_SIZE,       // Each sector is around 4KB
    .block_count = BLOCK_COUNT,     // We have 2048 sector each with 4KB 
    .cache_size = 16,
    .lookahead_size = 16,
    .block_cycles = 500,
};

uint8_t data[100] = {0};

int main(void) {

    printf("%s\r\n", "We read from file a number uint32_t first than write to the same region by incrementing it.");
    int err = Ext_Flash_Init();
    if(0 != err){
        printf("External Flash Initialization Error (Code -> %d)\r\n", err);
        return err;
    }

    // mount the filesystem
    err = lfs_mount(&lfs, &cfg);

    // reformat if we can't mount the filesystem
    // this should only happen on the first boot
    if (err) {
        lfs_format(&lfs, &cfg);
        lfs_mount(&lfs, &cfg);
    }

    // read current count
    uint32_t boot_count = 0;
    lfs_file_open(&lfs, &file, "boot_count", LFS_O_RDWR | LFS_O_CREAT);

    lfs_file_read(&lfs, &file, &boot_count, sizeof(boot_count));
    printf("Boot count read %d\r\n", boot_count);

    // update boot count
    boot_count += 1;
    lfs_file_rewind(&lfs, &file);
    lfs_file_write(&lfs, &file, &boot_count, sizeof(boot_count));
    
    // remember the storage is not updated until the file is closed successfully
    lfs_file_close(&lfs, &file);

    // release any resources we were using
    lfs_unmount(&lfs);

    // print the boot count
    printf("What we see in the next read on the same address : %d\r\n", boot_count);
    while(1){}

    return 0;
}