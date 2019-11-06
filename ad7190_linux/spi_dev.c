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

    printf("spi mode: 0x%x\n", dev->mode);
    printf("bits per word: %d\n", dev->bits);
    printf("max speed: %d Hz (%d KHz)\n", dev->speed, dev->speed/1000);
    printf("delay: %d\n", dev->delay);

    return 0;

close_fd:
    spi_dev_close(dev);
exit:
    return ret;
}

int spi_dev_close(SpiDev *dev) {
    close(dev->fd);
    dev->fd = -1;
}

int spi_dev_transfer(SpiDev *dev, uint8_t const *tx, uint8_t const *rx, size_t len)
{
    int ret = 0;
    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)tx,
        .rx_buf = (unsigned long)rx,
        .len = len,
        .delay_usecs = dev->delay,
        .speed_hz = dev->speed,
        .bits_per_word = dev->bits,
    };

    if (dev->mode & SPI_TX_QUAD)
        tr.tx_nbits = 4;
    else if (dev->mode & SPI_TX_DUAL)
        tr.tx_nbits = 2;
    if (dev->mode & SPI_RX_QUAD)
        tr.rx_nbits = 4;
    else if (dev->mode & SPI_RX_DUAL)
        tr.rx_nbits = 2;
    if (!(dev->mode & SPI_LOOP)) {
        if (dev->mode & (SPI_TX_QUAD | SPI_TX_DUAL))
            tr.rx_buf = 0;
        else if (dev->mode & (SPI_RX_QUAD | SPI_RX_DUAL))
            tr.tx_buf = 0;
    }

    ret = ioctl(dev->fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret < 1) {
        perror("can't send spi message");
    }

    return ret;
}
