# Virtual COM Port
The OSVR unit provides a com port on the USB interface.  Commands begin with # and are terminated with \n.
All command parsing is performed in SerialStateMachine.c  Parsing functions generally work on one character, then pass control to a sub-parser.

# Command Set
Commands can be given in upper or lower case.  For the descriptions below, however, I've used upper case for the literal command and lower case to denote parameters of commands.

## Parameters:
n - single numeric digit
aa - two character hex address
xx, yy, zz, ww - two character hex value.

## Info commands
```
#?V - display version info
#?C - display clock
#?B1948 - enter bootloader
```

## BNO070 Commands

```
#BDExx - Set DCD Cal enable flags to hex xx.
#BDS   - Save the current DCD values in non-volatile storage.
#BMExx - Enable/disable Mag sensor (xx=00 disable, anything else = enable)
#BMQ   - Query the Mag sensor status
#BSQ   - Query status.
#BVVxx - Pretty print events on the serial port (xx=00 disable,
         anything else = enable)
#BRI   - Re-init BNO with the default settings
#BRH   - Hard reset the BNO
```

## SPI Commands

```
#SIn -
#SWnaammcc
#SRn
#SRnaa
#SUn
#SN
#SF
#SDn
#S0
#S1n
```

## I2C Commands
```
#I...
```

## FPGA Commands

```
#F...
```
## TMDS Commands

```
#T...
```
## HDMI Commands

```
#H...
```
## PWM Settings

```
#Pxx
```

## Echo on/off and EEProm Functions

```
#EI
#EWaaxxyyzzww
#EV
#ERaa
```

## Debug Commands
```
#Dxx - set debug level to xx
```