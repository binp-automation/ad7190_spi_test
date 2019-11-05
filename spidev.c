// SPDX-License-Identifier: GPL-2.0-only
/*
 * SPI testing utility (using spidev driver)
 *
 * Copyright (c) 2007  MontaVista Software, Inc.
 * Copyright (c) 2007  Anton Vorontsov <avorontsov@ru.mvista.com>
 *
 * Cross-compile with cross-gcc -I/path/to/cross-kernel/include
 */

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


static void pabort(const char *s)
{
    perror(s);
    abort();
}

struct spi_dev {
    int fd;
    uint32_t mode;
    uint8_t bits;
    uint32_t speed;
}

static void spi_dev_transfer(spi_dev *dev, uint8_t const *tx, uint8_t const *rx, size_t len)
{
    int ret;
    int out_fd;
    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)tx,
        .rx_buf = (unsigned long)rx,
        .len = len,
        .delay_usecs = delay,
        .speed_hz = speed,
        .bits_per_word = bits,
    };

    if (mode & SPI_TX_QUAD)
        tr.tx_nbits = 4;
    else if (mode & SPI_TX_DUAL)
        tr.tx_nbits = 2;
    if (mode & SPI_RX_QUAD)
        tr.rx_nbits = 4;
    else if (mode & SPI_RX_DUAL)
        tr.rx_nbits = 2;
    if (!(mode & SPI_LOOP)) {
        if (mode & (SPI_TX_QUAD | SPI_TX_DUAL))
            tr.rx_buf = 0;
        else if (mode & (SPI_RX_QUAD | SPI_RX_DUAL))
            tr.tx_buf = 0;
    }

    ret = ioctl(dev->fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret < 1)
        pabort("can't send spi message");
}

int spi_dev_open(spi_dev *dev, const char *file) {
    int ret = 0;

    dev->fd = open(file, O_RDWR);
    if (dev->fd < 0)
        pabort("can't open device");

    /*
     * spi mode
     */
    ret = ioctl(dev->fd, SPI_IOC_WR_MODE32, &dev->mode);
    if (ret == -1)
        pabort("can't set spi mode");

    ret = ioctl(dev->fd, SPI_IOC_RD_MODE32, &dev->mode);
    if (ret == -1)
        pabort("can't get spi mode");

    /*
     * bits per word
     */
    ret = ioctl(dev->fd, SPI_IOC_WR_BITS_PER_WORD, &dev->bits);
    if (ret == -1)
        pabort("can't set bits per word");

    ret = ioctl(dev->fd, SPI_IOC_RD_BITS_PER_WORD, &dev->bits);
    if (ret == -1)
        pabort("can't get bits per word");

    /*
     * max speed hz
     */
    ret = ioctl(dev->fd, SPI_IOC_WR_MAX_SPEED_HZ, &dev->speed);
    if (ret == -1)
        pabort("can't set max speed hz");

    ret = ioctl(dev->fd, SPI_IOC_RD_MAX_SPEED_HZ, &dev->speed);
    if (ret == -1)
        pabort("can't get max speed hz");

    printf("spi mode: 0x%x\n", dev->mode);
    printf("bits per word: %d\n", dev->bits);
    printf("max speed: %d Hz (%d KHz)\n", dev->speed, dev->speed/1000);

    transfer(dev->fd, default_tx, default_rx, sizeof(default_tx));

    close(dev->fd);

    return ret;
}

int spi_dev_close(spi_dev *dev) {
    close(dev->fd);
    dev->fd = -1;
}
