from ctypes import *

dll = CDLL("ad7190_linux/ad7190.so")

# SPI slave device ID
SLAVE_ID         = 1

# AD7190 Register Map
REG_COMM         = 0 # Communications Register (WO, 8-bit) 
REG_STAT         = 0 # Status Register         (RO, 8-bit) 
REG_MODE         = 1 # Mode Register           (RW, 24-bit 
REG_CONF         = 2 # Configuration Register  (RW, 24-bit)
REG_DATA         = 3 # Data Register           (RO, 24/32-bit) 
REG_ID           = 4 # ID Register             (RO, 8-bit) 
REG_GPOCON       = 5 # GPOCON Register         (RW, 8-bit) 
REG_OFFSET       = 6 # Offset Register         (RW, 24-bit 
REG_FULLSCALE    = 7 # Full-Scale Register     (RW, 24-bit)

# Communications Register Bit Designations (AD7190_REG_COMM)
COMM_WEN         = (1 << 7)                     # Write Enable. 
COMM_WRITE       = (0 << 6)                     # Write Operation.
COMM_READ        = (1 << 6)                     # Read Operation. 
COMM_ADDR        = lambda x: (((x) & 0x7) << 3) # Register Address. 
COMM_CREAD       = (1 << 2)                     # Continuous Read of Data Register.

# Status Register Bit Designations (AD7190_REG_STAT)
STAT_RDY         = (1 << 7) # Ready.
STAT_ERR         = (1 << 6) # ADC error bit.
STAT_NOREF       = (1 << 5) # Error no external reference. 
STAT_PARITY      = (1 << 4) # Parity check of the data register. 
STAT_CH2         = (1 << 2) # Channel 2. 
STAT_CH1         = (1 << 1) # Channel 1. 
STAT_CH0         = (1 << 0) # Channel 0. 

# Mode Register Bit Designations (AD7190_REG_MODE)
MODE_SEL         = lambda x: (((x) & 0x7) << 21) # Operation Mode Select.
MODE_DAT_STA     = (1 << 20)                     # Status Register transmission.
MODE_CLKSRC      = lambda x: (((x) & 0x3) << 18) # Clock Source Select.
MODE_SINC3       = (1 << 15)                     # SINC3 Filter Select.
MODE_ENPAR       = (1 << 13)                     # Parity Enable.
MODE_SCYCLE      = (1 << 11)                     # Single cycle conversion.
MODE_REJ60       = (1 << 10)                     # 50/60Hz notch filter.
MODE_RATE        = lambda x: ((x) & 0x3FF)       # Filter Update Rate Select.

# Mode Register: AD7190_MODE_SEL(x) options
MODE_CONT                = 0 # Continuous Conversion Mode.
MODE_SINGLE              = 1 # Single Conversion Mode.
MODE_IDLE                = 2 # Idle Mode.
MODE_PWRDN               = 3 # Power-Down Mode.
MODE_CAL_INT_ZERO        = 4 # Internal Zero-Scale Calibration.
MODE_CAL_INT_FULL        = 5 # Internal Full-Scale Calibration.
MODE_CAL_SYS_ZERO        = 6 # System Zero-Scale Calibration.
MODE_CAL_SYS_FULL        = 7 # System Full-Scale Calibration.

# Mode Register: AD7190_MODE_CLKSRC(x) options
CLK_EXT_MCLK1_2          = 0 # External crystal. The external crystal
                                          # is connected from MCLK1 to MCLK2.
CLK_EXT_MCLK2            = 1 # External Clock applied to MCLK2 
CLK_INT                  = 2 # Internal 4.92 MHz clock. 
                                          # Pin MCLK2 is tristated.
CLK_INT_CO               = 3 # Internal 4.92 MHz clock. The internal
                                          # clock is available on MCLK2.

# Configuration Register Bit Designations (AD7190_REG_CONF)
CONF_CHOP        = (1 << 23)                      # CHOP enable.
CONF_REFSEL      = (1 << 20)                      # REFIN1/REFIN2 Reference Select.
CONF_CHAN        = lambda x: (((x) & 0xFF) << 8)  # Channel select.
CONF_BURN        = (1 << 7)                       # Burnout current enable.
CONF_REFDET      = (1 << 6)                       # Reference detect enable.
CONF_BUF         = (1 << 4)                       # Buffered Mode Enable.
CONF_UNIPOLAR    = (1 << 3)                       # Unipolar/Bipolar Enable.
CONF_GAIN        = lambda x: ((x) & 0x7)          # Gain Select.

# Configuration Register: AD7190_CONF_CHAN(x) options
CH_AIN1P_AIN2M      = 0 # AIN1(+) - AIN2(-)       
CH_AIN3P_AIN4M      = 1 # AIN3(+) - AIN4(-)       
CH_TEMP_SENSOR      = 2 # Temperature sensor       
CH_AIN2P_AIN2M      = 3 # AIN2(+) - AIN2(-)       
CH_AIN1P_AINCOM     = 4 # AIN1(+) - AINCOM       
CH_AIN2P_AINCOM     = 5 # AIN2(+) - AINCOM       
CH_AIN3P_AINCOM     = 6 # AIN3(+) - AINCOM       
CH_AIN4P_AINCOM     = 7 # AIN4(+) - AINCOM

# Configuration Register: AD7190_CONF_GAIN(x) options
#                                             ADC Input Range (5 V Reference)
CONF_GAIN_1      = 0 # Gain 1    +-5 V
CONF_GAIN_8      = 3 # Gain 8    +-625 mV
CONF_GAIN_16     = 4 # Gain 16   +-312.5 mV
CONF_GAIN_32     = 5 # Gain 32   +-156.2 mV
CONF_GAIN_64     = 6 # Gain 64   +-78.125 mV
CONF_GAIN_128    = 7 # Gain 128  +-39.06 mV

# ID Register Bit Designations (AD7190_REG_ID)
ID               = 0x4
ID_MASK          = 0x0F

# GPOCON Register Bit Designations (AD7190_REG_GPOCON)
GPOCON_BPDSW     = (1 << 6) # Bridge power-down switch enable
GPOCON_GP32EN    = (1 << 5) # Digital Output P3 and P2 enable
GPOCON_GP10EN    = (1 << 4) # Digital Output P1 and P0 enable
GPOCON_P3DAT     = (1 << 3) # P3 state
GPOCON_P2DAT     = (1 << 2) # P2 state
GPOCON_P1DAT     = (1 << 1) # P1 state
GPOCON_P0DAT     = (1 << 0) # P0 state


# Writes data into a register.
dll.AD7190_SetRegisterValue.restype = None
dll.AD7190_SetRegisterValue.argtypes = [c_ubyte, c_ulong, c_ubyte, c_ubyte]
def set_register_value(*args):
    return dll.AD7190_SetRegisterValue(*args);

# Reads the value of a register.
dll.AD7190_GetRegisterValue.restype = c_ulong
dll.AD7190_GetRegisterValue.argtypes = [c_ubyte, c_ubyte, c_ubyte];
def get_register_value(*args):
    return dll.AD7190_GetRegisterValue(*args)

# Checks if the AD7139 part is present.
dll.AD7190_Init.restype = c_ubyte
dll.AD7190_Init.argtypes = [];
def init(*args):
    return dll.AD7190_Init(*args)

# Resets the device.
dll.AD7190_Reset.restype = None
dll.AD7190_Reset.argtypes = [];
def reset(*args):
    return dll.AD7190_Reset(*args)

# Set device to idle or power-down.
dll.AD7190_SetPower.restype = None
dll.AD7190_SetPower.argtypes = [c_ubyte];
def set_power(*args):
    return dll.AD7190_SetPower(*args)

# Waits for RDY pin to go low.
dll.AD7190_WaitRdyGoLow.restype = None
dll.AD7190_WaitRdyGoLow.argtypes = [];
def wait_rdy_go_low(*args):
    return dll.AD7190_WaitRdyGoLow(*args)

# Selects the channel to be enabled.
dll.AD7190_ChannelSelect.restype = None
dll.AD7190_ChannelSelect.argtypes = [c_ushort];
def channel_select(*args):
    return dll.AD7190_ChannelSelect(*args)

# Performs the given calibration to the specified channel.
dll.AD7190_Calibrate.restype = None
dll.AD7190_Calibrate.argtypes = [c_ubyte, c_ubyte];
def calibrate(*args):
    return dll.AD7190_Calibrate(*args)

# Selects the polarity of the conversion and the ADC input range.
dll.AD7190_RangeSetup.restype = None
dll.AD7190_RangeSetup.argtypes = [c_ubyte, c_ubyte];
def range_setup(*args):
    return dll.AD7190_RangeSetup(*args)

# Returns the result of a single conversion.
dll.AD7190_SingleConversion.restype = c_ulong
dll.AD7190_SingleConversion.argtypes = [];
def single_conversion(*args):
    return dll.AD7190_SingleConversion(*args)

# Returns the average of several conversion results.
dll.AD7190_ContinuousReadAvg.restype = c_ulong
dll.AD7190_ContinuousReadAvg.argtypes = [c_ubyte];
def continuous_read_avg(*args):
    return dll.AD7190_ContinuousReadAvg(*args)

# Read data from temperature sensor and converts it to Celsius degrees.
dll.AD7190_TemperatureRead.restype = c_ulong
dll.AD7190_TemperatureRead.argtypes = [];
def temperature_read(*args):
    return dll.AD7190_TemperatureRead(*args)
