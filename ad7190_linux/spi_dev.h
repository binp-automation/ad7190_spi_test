// SPDX-License-Identifier: GPL-2.0-only
/*
 * SPI testing utility (using spidev driver)
 *
 * Copyright (c) 2007  MontaVista Software, Inc.
 * Copyright (c) 2007  Anton Vorontsov <avorontsov@ru.mvista.com>
 *
 * Cross-compile with cross-gcc -I/path/to/cross-kernel/include
 */
#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <linux/spi/spidev.h>


typedef struct SpiDev {
    int fd;
    uint32_t mode;
    uint8_t bits;
    uint32_t speed;
    uint16_t delay;
} SpiDev;

int spi_dev_open(SpiDev *dev, const char *file);
int spi_dev_close(SpiDev *dev);
int spi_dev_transfer(SpiDev *dev, uint8_t const *tx, uint8_t const *rx, size_t len);
