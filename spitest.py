import spidev

spi = spidev.SpiDev(1, 0)
spi.max_speed_hz = 1000000
print(spi.xfer([0xAA]))
