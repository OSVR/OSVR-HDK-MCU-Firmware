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
#BDExx - BNO070 DCD Enable, set flags as xx
#BDS   - BNO070 DCD Save Now
#BMExx - Enable Mag sensor for xx samples (to facilitate mag cal.)
#BMQ   - Query the Mag sensor status. (Format TBD)
#BSQ   - Query status.
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