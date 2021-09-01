# Serial command outline

This is based on my study and refactoring of the code. It is not complete but it
is complete enough to be useful. The [VirtualSerialPort](VirtualSerialPort.md)
document is more BNO070-centric.

- ? - info commands
  - v
    - show version (including BNO firmware version, if applicable)
  - f
    - show firmware variant/details (in very recent versions)
  - b1948
    - enter bootloader
- b - BNO070 commands (if present)
  - d - DCD stuff
    - e*XX*
      - DCD enable, set DCD/calibration enable flags to hex byte XX
    - s
      - DCD save (forced - done automatically when on a table for some amount of
        time)
    - c
      - DCD clear (not sure how safe this is!)
  - t
    - Output timing debug data - only in builds with MeasurePerformance turned
      on (which is not default because it of course impacts performance)
  - m - Mag stuff
    - e*XX*
      - mag enable if hex byte XX > 0
    - q
      - Mag status:  0 - Unreliable, 1 - Low, 2 - Medium, 3 - High Accuracy.
  - v and later - haven't documented yet
- s - Generic display (originally SPI/Solomon) commands
  - i*X*
    - Display_Init for x=1, 2 referring to Display1 or Display2
  - n
    - Display_On - assumes display1
  - g1
    - Set game rotation vector mode (BNO) - this does not use the mag
  - g0
    - Set rotation vector mode (BNO) - this uses the mag
  - f
    - Display_off - assumes display1
  - 0*X* (zero)
    - display reset for 0-index hex value
  - 1*X*
    - display powercycle for 1-indexed hex value
  - Additional commands on Toshiba (HDK2):
    - r1*xxxx*
      - read 1 byte from register *xxxx*
    - r2*xxxx*
      - read 2 bytes starting at register *xxxx*
    - r4*xxxx*
      - read 4 bytes starting at register *xxxx*
    - w1*xxxxyy*
      - write 1 byte, *yy*, to register *xxxx*
    - w2*xxxxyyyy*
      - write 2 bytes, *yyyy*, to register *xxxx*
    - w4*xxxxyyyyyyyy*
      - write 4 bytes, *yyyyyyyy*, to register *xxxx*
- i
  - I2C commands
- f*X*
  - FPGA commands - on non-hdk2 - on 1-indexed FPGA number *X* (though this
    number frequently ignored except for range-check)
    - s
      - Toggle side-by-side mode
    - 0
      - Perform FPGA reset.
    - n
      - Turn FPGA on/end reset
    - f
      - Turn FPGA off/start reset
- f
  - HDK2: "Factory Settings" commands
    - s
      - serial number
      - r
        - read serial number, check crc, print NG if crc bad, otherwise print serial number
    - t
      - calls OSVR_HDK_EDID() to send the EDID data to the toshiba chip (for debugging)
- t - dSight-only, TMDS (video switch) commands
- h - HDMI (generic VideoInput) commands
  - i
    - Initialize (VideoInput_Init())
  - r
    - Report status
      - For NXP-using devices (this excludes the HDK 2), see below.
      - Toshiba-using devices (HDK2) - this may vary from useless/nonfunctional
        (1.01) to printing a variety of information on whether or not video is
        being received, various chip registers that seemed interesting at that
        point in development, etc.
  - t
    - set HDMI_task to true - should be unnecessary
  - 0*xx*
    - reset the given video input channel
  - ... more that is NXP (non-hdk2) only
- p*xxyy*
  - takes two hex values to set PWM - presumably only applies to dSight, but PWM
    code appears disabled currently in dSight
- e - Described as "eeprom and echo" commands
  - Only eeprom commands currently found
  - i
    - write sensics ID signature to EEPROM - not sure why, but hey, it's a command
  - v
    - Verify sensics ID signature in eeprom
  - w*xxaabbccdd*
    - Writes the given four bytes of data to eeprom at address *xx*
  - r*xx*
    - Reads four bytes of data from eeprom starting at *xx* (prints in decimal!)
- d*xx*
  - Sets debug level/mask in hex

## Report Status `#hr` Details

For NXP-using devices (this excludes the HDK 2) output takes the form

```none
#hr
*known_resolution*
*activity_detected*
Res0 *resolution_id*
L *num_lines* P *num_pixels* I *interlace*
*video_mode*</code>
```

If a known resolution is detected, *known_resolution* is `Known res 0`;
otherwise, nothing is printed.

*interlace* may be one of the following values:

- `BSLHDMIRX_ASD_FRAME_PROGRESSIVE = 0x00 // 0`
- `BSLHDMIRX_ASD_FRAME_INTERLACED  = 0x80 // 128`

If video activity is detected, *activity_detected* is `Activity 0`; otherwise
nothing is printed.

If the resolution is unknown or not set, *resolution_id* is set to 255 (-1).
Otherwise, it is set to the ID of the current resolution. IDs are defined by the
[`tmdlHdmiRxResolutionID_t` enum](https://github.com/OSVR/OSVR-HDK-MCU-Firmware/blob/main/Source%20code/Embedded/src/NXP/tmdlHdmiRx_Types.h#L332).

*video_mode* may be one of the following:

- `No Video detected`
- `Portrait mode video`
- `Landscape mode video`

### HDK 1.4 in landscape

```none
Activity 0 
Res0 255 
L 1125 P 2200 I 0 
Landscape mode video
```

### HDK 1.2 in portrait

```none
Activity 0
Res0 255
L 1943 P 1128 I 0
Portrait mode video
```
