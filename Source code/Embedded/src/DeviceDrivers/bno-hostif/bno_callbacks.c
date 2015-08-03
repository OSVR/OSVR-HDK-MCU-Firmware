#include "string.h"

#include "sensorhub.h"

#include "GlobalOptions.h"
#include "my_hardware.h"
#include "Console.h"

#include <stdio.h>
#include <ioport.h>
#include "util/delay.h"

#include "twi_master.h"

#ifdef BNO070

#define BNO070_APP_I2C_8BIT_ADDR (0x48)
#define BNO070_BOOTLOADER_I2C_8BIT_ADDR (0x28)

static void debugPrintf(const char *format, ...)
{
#if 1
  static char buffer[256];

  va_list ap;
  va_start(ap, format);
  vsprintf(buffer, format, ap);
  static volatile int test;
  ++test;
  WriteLn(buffer);
  va_end(ap);
#endif
}

static void logError(const struct sensorhub_s *sh, int err)
{
  /* Report the error */
  //HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET); // Turn on red LED

  /* Send this to the debugger's Terminal I/O window */
  static volatile int lastErr = 0;
  lastErr = err;
  debugPrintf("BNO070 error: %d\r\n", lastErr);
}

static int i2cTransfer(const struct sensorhub_s *sh,
                       uint8_t address,
                       const uint8_t *sendData,
                       int sendLength,
                       uint8_t *receiveData,
                       int receiveLength)
{
    // TODO: Support case where sendLength > 0 && sendLength <= 3 && receiveLength > 0
    if ((sendLength > 0) && (receiveLength > 0)) {
        if (sendLength <= 3) {
            twi_package_t packet_read = {
                .addr[0]      = sendData[0], //regNum,      // TWI slave memory address data
                .addr[1]      = (sendLength <= 2) ? sendData[1] : 0, //regNum,      // TWI slave memory address data
                .addr[2]      = (sendLength <= 3) ? sendData[2] : 0, //regNum,      // TWI slave memory address data
                .addr_length  = sendLength, // TWI slave memory address data size
                .chip         = address,      // TWI slave bus address
                .buffer       = receiveData, // transfer data source buffer
                .length       = receiveLength  // transfer data size (bytes)
            };
            if (twi_master_read(TWI_BNO070_PORT, &packet_read)!=STATUS_OK) {
                return SENSORHUB_STATUS_ERROR_I2C_IO;
            } else {
                return SENSORHUB_STATUS_SUCCESS;
            }
        } else {
            return SENSORHUB_STATUS_ERROR_I2C_IO;  // Don't support this yet!!!
        }
    }

    if (sendLength > 0) {
        twi_package_t packet_write = {
            .addr[0]      = 0, //regNum,      // TWI slave memory address data
            .addr[1]      = 0, //regNum,      // TWI slave memory address data
            .addr_length  = 0, // TWI slave memory address data size
            .chip         = address,      // TWI slave bus address
            .buffer       = (uint8_t*)sendData, // transfer data source buffer
            .length       = sendLength  // transfer data size (bytes)
        };
        if (twi_master_write(TWI_BNO070_PORT, &packet_write)!=STATUS_OK) {
            return SENSORHUB_STATUS_ERROR_I2C_IO;
        }
    }
    if (receiveLength > 0) {
        twi_package_t packet_read = {
            .addr[0]      = 0, //regNum,      // TWI slave memory address data
            .addr[1]      = 0, //regNum,      // TWI slave memory address data
            .addr_length  = 0, // TWI slave memory address data size
            .chip         = address,      // TWI slave bus address
            .buffer       = receiveData, // transfer data source buffer
            .length       = receiveLength  // transfer data size (bytes)
        };
        if (twi_master_read(TWI_BNO070_PORT, &packet_read)!=STATUS_OK) {
            return SENSORHUB_STATUS_ERROR_I2C_IO;
        }
    }
    return SENSORHUB_STATUS_SUCCESS;
}

static void gpioSetRSTN(const struct sensorhub_s *sh, int value)
{
    if (value) {
		// set to high state by configuring as input.  External pullup brings it high.
		ioport_configure_pin(BNO_070_Reset_Pin,IOPORT_DIR_INPUT);
    } else {
		// set to low state by configuring as output, setting it low.
		ioport_configure_pin(BNO_070_Reset_Pin,IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
        ioport_set_pin_low(BNO_070_Reset_Pin);
    }
}

static void gpioSetBOOTN(const struct sensorhub_s *sh, int value)
{
    if (value) {
        ioport_set_pin_high(Side_by_side_A);
    } else {
        ioport_set_pin_low(Side_by_side_A);
    }
}

static int gpioGetHOST_INTN(const struct sensorhub_s *sh)
{
    return ioport_get_value(Int_BNO070);
}

static void delay(const struct sensorhub_s *sh, int milliseconds)
{
    delay_ms(milliseconds);
}

static uint32_t getTick(const struct sensorhub_s *sh)
{
    static uint32_t currTick = 0;
    currTick += 10;
    delay_ms(10);
    return currTick;
}


static sensorhub_stats_t sensorhubStats;

sensorhub_t sensorhub = {
    BNO070_APP_I2C_8BIT_ADDR,
    BNO070_BOOTLOADER_I2C_8BIT_ADDR,
    &sensorhubStats,
    i2cTransfer,
    gpioSetRSTN,
    gpioSetBOOTN,
    gpioGetHOST_INTN,
    delay,
    getTick,
    logError,
    debugPrintf,
    5,                          /* I2C retries */
    NULL                        /* cookie */
};

#endif // BNO