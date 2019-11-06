// SPDX-License-Identifier: GPL-2.0-only
/*
 * SPI testing utility (using SpiDev driver)
 *
 * Copyright (c) 2007  MontaVista Software, Inc.
 * Copyright (c) 2007  Anton Vorontsov <avorontsov@ru.mvista.com>
 *
 * Cross-compile with cross-gcc -I/path/to/cross-kernel/include
 */

#include "spi_dev.h"

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <time.h>
#include <sys/ioctl.h>
#include <linux/ioctl.h>
#include <sys/stat.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#define DEBUG


static void hex_dump(const void *src, size_t length, size_t line_size, char *prefix) {
    int i = 0;
    const unsigned char *address = src;
    const unsigned char *line = address;
    unsigned char c;

    printf("%s | ", prefix);
    while (length-- > 0) {
        printf("%02X ", *address++);
        if (!(++i % line_size) || (length == 0 && i % line_size)) {
            if (length == 0) {
                while (i++ % line_size)
                    printf("__ ");
            }
            printf(" |");
            while (line < address) {
                c = *line++;
                printf("%c", (c < 32 || c > 126) ? '.' : c);
            }
            printf("|\n");
            if (length > 0)
                printf("%s | ", prefix);
        }
    }
}

int spi_dev_open(SpiDev *dev, const char *file) {
    int ret = 0;

    dev->fd = open(file, O_RDWR);
    if (dev->fd < 0) {
        perror("can't open device");
        goto exit;
    }

    /*
     * spi mode
     */
    ret = ioctl(dev->fd, SPI_IOC_WR_MODE32, &dev->mode);
    if (ret == -1) {
        perror("can't set spi mode");
        goto close_fd;
    }

    ret = ioctl(dev->fd, SPI_IOC_RD_MODE32, &dev->mode);
    if (ret == -1) {
        perror("can't get spi mode");
        goto close_fd;
    }

    /*
     * bits per word
     */
    ret = ioctl(dev->fd, SPI_IOC_WR_BITS_PER_WORD, &dev->bits);
    if (ret == -1) {
        perror("can't set bits per word");
        goto close_fd;
    }

    ret = ioctl(dev->fd, SPI_IOC_RD_BITS_PER_WORD, &dev->bits);
    if (ret == -1) {
        perror("can't get bits per word");
        goto close_fd;
    }

    /*
     * max speed hz
     */
    ret = ioctl(dev->fd, SPI_IOC_WR_MAX_SPEED_HZ, &dev->speed);
    if (ret == -1) {
        perror("can't set max speed hz");
        goto close_fd;
    }

    ret = ioctl(dev->fd, SPI_IOC_RD_MAX_SPEED_HZ, &dev->speed);
    if (ret == -1) {
        perror("can't get max speed hz");
        goto close_fd;
    }

#ifdef DEBUG
    printf("spi mode: 0x%x\n", dev->mode);
    printf("bits per word: %d\n", dev->bits);
    printf("max speed: %d Hz (%d KHz)\n", dev->speed, dev->speed/1000);
    printf("delay: %d\n", dev->delay);
#endif /* DEBUG */

    return 0;

close_fd:
    spi_dev_close(dev);
exit:
    return 1;
}

int spi_dev_close(SpiDev *dev) {
    if (dev->fd < 0) {
        perror("device is not initialized");
        return 1;
    } else {
        close(dev->fd);
        dev->fd = -1;
        return 0;
    }
}

int spi_dev_transfer(SpiDev *dev, uint8_t const *tx, uint8_t const *rx, size_t len) {
    int ret = 0;

    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)tx,
        .rx_buf = (unsigned long)rx,
        .len = len,
        .delay_usecs = dev->delay,
        .speed_hz = dev->speed,
        .bits_per_word = dev->bits,
    };

    if (dev->fd < 0) {
        perror("device is not initialized");
        return 1;
    }

#ifdef DEBUG
    if (tx) {
        hex_dump(tx, len, 8, "TX");
    }
#endif /* DEBUG */

    ret = ioctl(dev->fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret < 1) {
        perror("can't send spi message");
        return 1;
    }

#ifdef DEBUG
    if (rx) {
        hex_dump(rx, len, 8, "RX");
    }
#endif /* DEBUG */

    return 0;
}
