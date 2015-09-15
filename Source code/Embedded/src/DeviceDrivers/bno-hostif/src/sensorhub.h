/* ============================================================================
 *
 * Copyright (c) 2014 Hillcrest Laboratories, Inc.  All rights reserved.
 * HILLCREST LABORATORIES, INC. PROPRIETARY/CONFIDENTIAL
 *
 * ============================================================================*/

#ifndef SENSORHUB_H
#define SENSORHUB_H

#include "sensorhub_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include <stdint.h>

#define SENSORHUB_LIBRARY_VERSION (0x0001)      /* 0.1 */

/**
 * @brief List of sensor types supported by the hub
 *
 * See the SH-1 Reference Manual for more information on each type.
 */
enum sensorhub_Sensor_e {
    SENSORHUB_RAW_ACCELEROMETER = 0x14,
    SENSORHUB_ACCELEROMETER = 0x01,
    SENSORHUB_LINEAR_ACCELERATION = 0x04,
    SENSORHUB_GRAVITY = 0x06,
    SENSORHUB_RAW_GYROSCOPE = 0x15,
    SENSORHUB_GYROSCOPE_CALIBRATED = 0x02,
    SENSORHUB_GYROSCOPE_UNCALIBRATED = 0x07,
    SENSORHUB_RAW_MAGNETOMETER = 0x16,
    SENSORHUB_MAGNETIC_FIELD_CALIBRATED = 0x03,
    SENSORHUB_MAGNETIC_FIELD_UNCALIBRATED = 0x0f,
    SENSORHUB_ROTATION_VECTOR = 0x05,
    SENSORHUB_GAME_ROTATION_VECTOR = 0x08,
    SENSORHUB_GEOMAGNETIC_ROTATION_VECTOR = 0x09,
    SENSORHUB_PRESSURE = 0x0a,
    SENSORHUB_AMBIENT_LIGHT = 0x0b,
    SENSORHUB_HUMIDITY = 0x0c,
    SENSORHUB_PROXIMITY = 0x0d,
    SENSORHUB_TEMPERATURE = 0x0e,
    SENSORHUB_SAR = 0x17,
    SENSORHUB_TAP_DETECTOR = 0x10,
    SENSORHUB_STEP_DETECTOR = 0x18,
    SENSORHUB_STEP_COUNTER = 0x11,
    SENSORHUB_SIGNIFICANT_MOTION = 0x12,
    SENSORHUB_ACTIVITY_CLASSIFICATION = 0x13,
    SENSORHUB_SHAKE_DETECTOR = 0x19,
    SENSORHUB_FLIP_DETECTOR = 0x1a,
    SENSORHUB_PICKUP_DETECTOR = 0x1b,
    SENSORHUB_STABILITY_DETECTOR = 0x1c,
    SENSORHUB_PERSONAL_ACTIVITY_CLASSIFIER = 0x1e,
    SENSORHUB_SLEEP_DETECTOR = 0x1f,
    SENSORHUB_TILT_DETECTOR = 0x20
};
typedef uint8_t sensorhub_Sensor_t;

/**
 * @brief List of FRS types supported by the hub
 *
 * See the SH-1 Reference Manual for more information on each type.
 */
enum sensorhub_FRS_e {
    SENSORHUB_FRS_RECORD_INVALID = 0x0000,
    SENSORHUB_FRS_DCD = 0x1F1F,
    SENSORHUB_FRS_SCD_BACKUP = 0x3030,
    SENSORHUB_FRS_SCD_ACTIVE = 0x7979,      /* Static Cal AGM */
    SENSORHUB_FRS_SCD_NOMINAL = 0x4d4d,
    SENSORHUB_FRS_SCD_EXT = 0xECA1,
    SENSORHUB_FRS_SCD_SRA_ACTIVE = 0x8a8a,
    SENSORHUB_FRS_SCD_SRA_NOMINAL = 0x4e4e,
    SENSORHUB_FRS_CERTIFICATE = 0x5627,
    SENSORHUB_FRS_TRACKING_NUM = 0x4B4B,
    SENSORHUB_FRS_SERIAL_NUM = 0x4B4B,
    SENSORHUB_FRS_NETID_SELF = 0x6565,
    SENSORHUB_FRS_NETID_PARTNER = 0x2929,
    SENSORHUB_FRS_CONFIG_SENSOR = 0x7272,
    SENSORHUB_FRS_CONFIG_BUTTON = 0x5454,
    SENSORHUB_FRS_CONFIG_LED = 0x8383,
    SENSORHUB_FRS_BEHAVIOR_LED = 0x9E9E,
    SENSORHUB_FRS_MAP_BUTTON = 0xADAD,
    SENSORHUB_FRS_BEHAVIOR_PAIR = 0xBCBC,
    SENSORHUB_FRS_EXTERNAL_IMAGE = 0xA1A1,
    SENSORHUB_FRS_MISCELLANEOUS = 0xC1C1,
    SENSORHUB_FRS_DEVICEID = 0xD0D0,
    SENSORHUB_FRS_CAUSE0_LED = 0x3A3A,
    SENSORHUB_FRS_CAUSE1_LED = 0x3B3B,
    SENSORHUB_FRS_CAUSE2_LED = 0x3C3C,
    SENSORHUB_FRS_USB_PID = 0x7A2C,

    SENSORHUB_FRS_SYSTEM_ORIENTATION = 0x2D3E,
    SENSORHUB_FRS_PRIMARY_ACCEL_ORIENTATION = 0x2D41,
    SENSORHUB_FRS_SCREEN_ACCEL_ORIENTATION = 0x2D43,
    SENSORHUB_FRS_GYRO_ORIENTATION = 0x2D46,
    SENSORHUB_FRS_MAG_ORIENTATION = 0x2D4C,
    SENSORHUB_FRS_ARVR_CONFIG = 0x3E2D,
    SENSORHUB_FRS_ARVR_GAME_CONFIG = 0x3E2E,
    SENSORHUB_FRS_SHAKE_DETECTOR_CONFIG = 0x7D7D,
    SENSORHUB_FRS_STEP_DETECTOR_CONFIG = 0xC25A,
    SENSORHUB_FRS_TAP_DETECTOR_CONFIG = 0xC269,
    SENSORHUB_FRS_SIG_MOTION_DETECTOR_CONFIG = 0xC274,
    SENSORHUB_FRS_POWER_CONFIG = 0xD3E2,
    SENSORHUB_FRS_MAX_FUSION_PERIOD = 0xD7D7,
    SENSORHUB_FRS_PRESSURE_CALIBRATION = 0x39AF,
    SENSORHUB_FRS_TEMPERATURE_CALIBRATION = 0x4D20,
    SENSORHUB_FRS_HUMIDITY_CALIBRATION = 0x1AC9,
    SENSORHUB_FRS_AMBIENT_LIGHT_CALIBRATION = 0x39B1,
    SENSORHUB_FRS_PROXIMITY_CALIBRATION = 0x4DA2,
    SENSORHUB_FRS_PICKUP_DETECTOR_CONFIG = 0x1B2A,
    SENSORHUB_FRS_FLIP_DETECTOR_CONFIG = 0xFC94,
    SENSORHUB_FRS_STABILITY_DETECTOR_CONFIG = 0xED85,
    SENSORHUB_FRS_USER_RECORD = 0x74B4,

    SENSORHUB_FRS_META_RAW_ACCELEROMETER = 0xE301,
    SENSORHUB_FRS_META_ACCELERATION = 0xE302,
    SENSORHUB_FRS_META_LINEAR_ACCELERATION = 0xE303,
    SENSORHUB_FRS_META_GRAVITY = 0xE304,
    SENSORHUB_FRS_META_RAW_GYROSCOPE = 0xE305,
    SENSORHUB_FRS_META_GYROSCOPE_CALIBRATED = 0xE306,
    SENSORHUB_FRS_META_GYROSCOPE_UNCALIBRATED = 0xE307,
    SENSORHUB_FRS_META_RAW_MAGNETIC_FIELD = 0xE308,
    SENSORHUB_FRS_META_MAGNETIC_FIELD_CALIBRATED = 0xE309,
    SENSORHUB_FRS_META_MAGNETIC_FIELD_UNCALIBRATED = 0xE30A,
    SENSORHUB_FRS_META_ROTATION_VECTOR = 0xE30B,
    SENSORHUB_FRS_META_GAME_ROTATION_VECTOR = 0xE30C,
    SENSORHUB_FRS_META_GEOMAGNETIC_ROTATION_VECTOR = 0xE30D,
    SENSORHUB_FRS_META_PRESSURE = 0xE30E,
    SENSORHUB_FRS_META_AMBIENT_LIGHT = 0xE30F,
    SENSORHUB_FRS_META_HUMIDITY = 0xE310,
    SENSORHUB_FRS_META_PROXIMITY = 0xE311,
    SENSORHUB_FRS_META_TEMPERATURE = 0xE312,
    SENSORHUB_FRS_META_TAP_DETECTOR = 0xE313,
    SENSORHUB_FRS_META_STEP_DETECTOR = 0xE314,
    SENSORHUB_FRS_META_STEP_COUNTER = 0xE315,
    SENSORHUB_FRS_META_SIGNIFICANT_MOTION = 0xE316,
    SENSORHUB_FRS_META_ACTIVITY_CLASSIFICATION = 0xE317,
    SENSORHUB_FRS_META_SHAKE_DETECTOR = 0xE318,
    SENSORHUB_FRS_META_FLIP_DETECTOR = 0xE319,
    SENSORHUB_FRS_META_PICKUP_DETECTOR = 0xE31A,
    SENSORHUB_FRS_META_STABILITY_DETECTOR = 0xE31B,
};
typedef uint16_t sensorhub_FRS_t;

/**
 * @brief Personal Activity Classification States
 *
 * States for use with the Personal Activity Classifier (0x1E) report
 */
enum sensorhub_PersonalActivityStates_e {
    SENSORHUB_PAC_UNKNOWN = 0,
    SENSORHUB_PAC_IN_VEHICLE = 1,
    SENSORHUB_PAC_ON_BICYCLE = 2,
    SENSORHUB_PAC_ON_FOOT = 3,
    SENSORHUB_PAC_STILL = 4,
    SENSORHUB_PAC_TILTING = 5,
    SENSORHUB_PAC_WALKING = 6,
    SENSORHUB_PAC_RUNNING = 7
};
typedef uint8_t sensorhub_PersonalActivityStates_t;

/**
 * @brief Personal Activity Classficiation MLS Change
 *
 * Bit to set in the Sensor-Specific Configuration Word to impose the
 * constraint that the most-likely state must differ from the last reported
 * most-likely state when set.
 */
#define SENSORHUB_PAC_MLS_CHANGE 0x80000000

/**
 * This structure holds various statistics collected by the Sensor Hub
 * host interface.
 */
typedef struct sensorhub_stats_s {
    int i2cTransfers;
    int i2cRetries;
    int i2cErrors;
} sensorhub_stats_t;

/**
 * This structure holds the configuration for one
 * sensor hub.
 */
typedef struct sensorhub_s {
    /**
     * Set this to the I2C address of the sensor hub. If the system
     * uses 7-bit I2C addresses, it will be 0x48 or 0x49 depending
     * on how the address select line is set. The address is passed
     * through to the i2cTransfer() calls, so if 8-bit I2C addresses
     * are more convenient, then it can be set to that. (e.g., 0x48 << 1)
     */
    uint8_t sensorhubAddress;

    /**
     * Set this to the I2C address of the bootloader. If the system
     * uses 7-bit I2C addresses, it will be 0x28 or 0x29.
     */
    uint8_t bootloaderAddress;

    /**
     * Point this to a location to store host interface statistics.
     */
    sensorhub_stats_t *stats;


    /**
     * Transfer bytes to or from the BNO070.
     *
     * Note: receive transfers can be smart and look at the length field
     *       to determine the actual number of bytes to receive.
     *
     * @param sh the sensorhub
     * @param address the I2C address to send/receive from
     * @param sendData the data to send first
     * @param sendLength the number of bytes to send
     * @param receiveData the data to receive (could be NULL if this transaction is a transmit)
     * @param receiveLength the max length to receive
     * @return 0 on success. <0 will be propogated back through Sensor Hub API.
     */
    int (*i2cTransfer) (const struct sensorhub_s * sh, uint8_t address,
                        const uint8_t * sendData, int sendLength,
                        uint8_t * receiveData, int receiveLength);

    /**
     * Set or clear the RSTN line.
     *
     * @param sh the sensorhub
     * @param value 0 or 1
     */
    void (*setRSTN) (const struct sensorhub_s * sh, int value);

    /**
     * Set or clear the BOOTN line.
     *
     * @param sh the sensorhub
     * @param value 0 or 1
     */
    void (*setBOOTN) (const struct sensorhub_s * sh, int value);

    /**
     * Get the current state of the HOST_INTN line.
     *
     * @param sh the sensorhub
     * @return value 0 or 1 is returned
     */
    int (*getHOST_INTN) (const struct sensorhub_s * sh);

    /**
     * Delay for the specified number of milliseconds
     *
     * @param sh the sensorhub
     * @param milliseconds
     */
    void (*delay) (const struct sensorhub_s * sh, int milliseconds);

    /**
     * Return the current number of ticks. Each tick should increment
     * 1 millisecond. This is used for functions that have timeouts.
     *
     * @param sh the sensorhub
     * @return number of ticks
     */
    uint32_t(*getTick) (const struct sensorhub_s * sh);

    /**
     * Callback when an error occurs. This can be used to log errors
     * or as a convenient breakpoint location when debugging.
     *
     * @param sh the sensorhub
     * @param err the sensor hub error code
     */
    void (*onError) (const struct sensorhub_s * sh, int err);

    /**
     * Callback for issuing debug printfs.
     *
     * @param format printf style format
     * @param args printf style arguments
     */
    void (*debugPrintf) (const char *format, ...);

    /**
     * Failed I2C transactions can be retried. This parameter sets the max
     * number of retries.
     */
    int max_retries;

    /* Optional pointer for callbacks */
    void *cookie;
} sensorhub_t;

typedef struct sensorhub_RawAccelerometer {
    /* Units are ADC counts */
    int16_t x;
    int16_t y;
    int16_t z;

    /* Microseconds */
    uint32_t timestamp;
} sensorhub_RawAccelerometer_t;

typedef struct sensorhub_Accelerometer {
    /* Units are in m/s^2 */
    int16_t x_16Q8;
    int16_t y_16Q8;
    int16_t z_16Q8;
} sensorhub_Accelerometer_t;

typedef struct sensorhub_RawGyroscope {
    /* Units are ADC counts */
    int16_t x;
    int16_t y;
    int16_t z;
    int16_t temperature;

    /* Microseconds */
    uint32_t timestamp;
} sensorhub_RawGyroscope_t;

typedef struct sensorhub_Gyroscope {
    /* Units are rad/s */
    int16_t x_16Q9;
    int16_t y_16Q9;
    int16_t z_16Q9;
} sensorhub_Gyroscope_t;

typedef struct sensorhub_GyroscopeUncalibrated {
    /* Units are rad/s */
    int16_t x_16Q9;
    int16_t y_16Q9;
    int16_t z_16Q9;
    int16_t biasx_16Q9;
    int16_t biasy_16Q9;
    int16_t biasz_16Q9;
} sensorhub_GyroscopeUncalibrated_t;

typedef struct sensorhub_RawMagnetometer {
    /* Units are ADC counts */
    int16_t x;
    int16_t y;
    int16_t z;

    /* Microseconds */
    uint32_t timestamp;
} sensorhub_RawMagnetometer_t;

typedef struct sensorhub_MagneticField {
    /* Units are uTesla */
    int16_t x_16Q4;
    int16_t y_16Q4;
    int16_t z_16Q4;
} sensorhub_MagneticField_t;

typedef struct sensorhub_MagneticFieldUncalibrated {
    /* Units are uTesla */
    int16_t x_16Q5;
    int16_t y_16Q5;
    int16_t z_16Q5;
    int16_t biasx_16Q5;
    int16_t biasy_16Q5;
    int16_t biasz_16Q5;
} sensorhub_MagneticFieldUncalibrated_t;

typedef struct sensorhub_StepCounter {
    uint32_t detectLatency;
    uint16_t steps;
    uint16_t reserved;
} sensorhub_StepCounter_t;

typedef struct sensorhub_RotationVectorWAcc {
    int16_t i_16Q14;
    int16_t j_16Q14;
    int16_t k_16Q14;
    int16_t real_16Q14;
    int16_t accuracy_16Q12;
} sensorhub_RotationVectorWAcc_t;

typedef struct sensorhub_RotationVector {
    int16_t i_16Q14;
    int16_t j_16Q14;
    int16_t k_16Q14;
    int16_t real_16Q14;
} sensorhub_RotationVector_t;

typedef struct sensorhub_PersonalActivityClassifier {
    uint8_t pageNumberEOS; // MSB set if this is the last input report, LSBs is page number
    sensorhub_PersonalActivityStates_t mostLikelyState;
    uint8_t confidence[10];
} sensorhub_PersonalActivityClassifier_t;

typedef struct sensorhub_Event {
    /* The sensor generating the event. */
    sensorhub_Sensor_t sensor;

    /* 8-bit unsigned integer used to track reports. The
     * sequence number increments once for each report sent.
     * Gaps in the sequence numbers indicate missing or
     * dropped reports.
     */
    uint8_t sequenceNumber;

    /* Bits 1:0 – indicate the status of a sensor.
     *   0 – Unreliable
     *   1 – Accuracy low
     *   2 – Accuracy medium
     *   3 – Accuracy high
     * Bits 4:2 – delay exponent
     * Bits 7:5 - reserved
     */
    uint8_t status;

    /* 8-bit significand representing the delay from when a
     * sensor sample is taken until that sample is reported to the
     * host. The delay is scaled by the delay exponent. The total
     * delay is given by delay * (2 ^ delay exponent). The units
     * are microseconds. Delays greater than 32640 us are reported
     * as 32640 us.
     */
    uint8_t delay;

    /* Event report. Use the structure based on the value of the sensor
     * field. See the SH-1 Reference Manual for reports.
     */
    union {
        sensorhub_RawAccelerometer_t rawAccelerometer;
        sensorhub_Accelerometer_t accelerometer;
        sensorhub_Accelerometer_t linearAcceleration;
        sensorhub_Accelerometer_t gravity;
        sensorhub_RawGyroscope_t rawGyroscope;
        sensorhub_Gyroscope_t gyroscope;
        sensorhub_GyroscopeUncalibrated_t gyroscopeUncal;
        sensorhub_RawMagnetometer_t rawMagnetometer;
        sensorhub_MagneticField_t magneticField;
        sensorhub_MagneticFieldUncalibrated_t magneticFieldUncal;
        sensorhub_RotationVectorWAcc_t rotationVector;
        sensorhub_RotationVector_t gameRotationVector;
        sensorhub_RotationVectorWAcc_t geoMagRotationVector;
        sensorhub_StepCounter_t stepCounter;
        sensorhub_PersonalActivityClassifier_t personalActivityClassifier;

        /* Internal fields */
        uint16_t field16[6];        /* Many reports are 3 to 6 16-bit ints */
        uint32_t field32[3];        /* Some reports just contain 32-bit ints */
    } un;
} sensorhub_Event_t;

/**
 * Feature settings on a sensor.
 */
typedef struct sensorhub_SensorFeature {
    /* Change sensitivity enabled */
    bool changeSensitivityEnabled;

    /* Change sensitivity - true if relative; false if absolute */
    bool changeSensitivityRelative;

    /* Wake-up enabled */
    bool wakeupEnabled;

    /* 16-bit signed fixed point integer representing the value a
     * sensor output must exceed in order to trigger another input
     * report. A setting of 0 causes all reports to be sent.
     */
    uint16_t changeSensitivity;

    /* Interval in microseconds between asynchronous input reports. */
    uint32_t reportInterval;

    /* Interval in microseconds between batches of input reports. */
    uint32_t batchInterval;

    /* Sensor specific configuration word  */
    uint32_t sensorSpecificConfiguration;

} sensorhub_SensorFeature_t;

/**
 * @brief Return values for sensor hub functions
 */
enum sensorhub_Status_e {
    /* Warnings (>0) */
	SENSORHUB_STATUS_MORE_EVENTS_PENDING = 1,
    SENSORHUB_STATUS_NO_REPORT_PENDING = 2,
	SENSORHUB_STATUS_HUB_RESET = 3,

    /* Success */
    SENSORHUB_STATUS_SUCCESS = 0,         /**< The operation was successful */

    /* Errors (<0) */
    SENSORHUB_STATUS_ERROR_I2C_IO = -1,
    SENSORHUB_STATUS_REPORT_LEN_TOO_LONG = -2,      /* a report was received with an invalid length */
    SENSORHUB_STATUS_REPORT_INVALID_LEN = -3,       /* a report has an unexpected length for its type */
    SENSORHUB_STATUS_REPORT_UNKNOWN = -4,   /* a report with an unknown type was received */
    SENSORHUB_STATUS_RESET_FAIL_1 = -5,     /* reseting the BNO070 failed */
    SENSORHUB_STATUS_RESET_FAIL_2 = -6,     /* reseting the BNO070 failed */
    SENSORHUB_STATUS_RESET_INTN_BROKE = -7, /* clearing the BNO070 interrupt line failed */
    SENSORHUB_STATUS_FRS_READ_BAD_OFFSET = -8,      /* received an out of order FRS read response */
    SENSORHUB_STATUS_FRS_READ_BAD_LENGTH = -9,      /* received an FRS read response with a bad length field */
    SENSORHUB_STATUS_FRS_READ_BAD_TYPE = -10,       /* received an FRS read response with a bad type field */
    SENSORHUB_STATUS_FRS_READ_UNRECOGNIZED_FRS = -11,
    SENSORHUB_STATUS_FRS_READ_BUSY = -12,
    SENSORHUB_STATUS_FRS_READ_DEVICE_ERROR = -13,
    SENSORHUB_STATUS_FRS_READ_UNKNOWN_ERROR = -14,
    SENSORHUB_STATUS_FRS_READ_EMPTY = -15,
    SENSORHUB_STATUS_FRS_READ_OFFSET_OUT_OF_RANGE = -16,
    SENSORHUB_STATUS_FRS_READ_UNEXPECTED_LENGTH = -17,      /* expected to only get so many responses, but it looks like we'll get more. */
    SENSORHUB_STATUS_UNEXPECTED_REPORT = -18,
    SENSORHUB_STATUS_FRS_WRITE_BUSY = -19,
    SENSORHUB_STATUS_FRS_WRITE_BAD_TYPE = -20,
    SENSORHUB_STATUS_FRS_WRITE_BAD_LENGTH = -21,
    SENSORHUB_STATUS_FRS_WRITE_DEVICE_ERROR = -22,
    SENSORHUB_STATUS_FRS_WRITE_BAD_STATUS = -23,
    SENSORHUB_STATUS_FRS_WRITE_BAD_MODE = -24,
    SENSORHUB_STATUS_FRS_WRITE_FAILED = -25,
    SENSORHUB_STATUS_FRS_READ_ONLY = -26,
    SENSORHUB_STATUS_FRS_INVALID_RECORD = -27,
    SENSORHUB_STATUS_FRS_WRITE_NOT_ENOUGH = -28,
    SENSORHUB_STATUS_NOT_AN_EVENT = -29,
    SENSORHUB_STATUS_UNEXPECTED_DFU_STREAM_TYPE = -30,
    SENSORHUB_STATUS_DFU_STREAM_SIZE_WRONG = -31,
    SENSORHUB_STATUS_DFU_RECEIVED_NAK = -32,
    SENSORHUB_STATUS_INVALID_HID_DESCRIPTOR = -33,
    SENSORHUB_STATUS_OP_FAILED = -34,
};

enum sensorhub_FRS_ReadStatus_e {
    SENSORHUB_FRP_RD_NO_ERR = 0,    // no error, more data
    SENSORHUB_FRP_RD_BAD_TYPE = 1,  // FRS record type not recognized
    SENSORHUB_FRP_RD_BUSY = 2,      // FRS resource busy
    SENSORHUB_FRP_RD_COMPLETE = 3,  // no error, end of data
    SENSORHUB_FRP_RD_BAD_OFFSET = 4,        // requested offset out of range
    SENSORHUB_FRP_RD_RECORD_EMPTY = 5,      // record type not found in FRS
    SENSORHUB_FRP_RD_BLOCK_DONE = 6,        // requested block is done
    SENSORHUB_FRP_RD_BLOCK_REC_DONE = 7,    // requested block done and record complete
    SENSORHUB_FRP_RD_DEVICE_ERROR = 8,      // device error (for DFU)
};
typedef uint8_t sensorhub_FRS_ReadStatus_t;

typedef struct sensorhub_FRSReadResponse {
    uint8_t length;
    sensorhub_FRS_ReadStatus_t status;
    uint16_t offset;
    uint32_t data[2];
    sensorhub_FRS_t recordType;
    uint16_t reserved;
} sensorhub_FRSReadResponse_t;

enum sensorhub_FRS_WriteStatus_e {
    SENSORHUB_FRP_WR_ACK = 0,       // no error, acknowledge data received
    SENSORHUB_FRP_WR_BAD_TYPE = 1,  // FRS record type not recognized
    SENSORHUB_FRP_WR_BUSY = 2,      // FRS resource busy
    SENSORHUB_FRP_WR_COMPLETE = 3,  // no error, write operation successful
    SENSORHUB_FRP_WR_READY = 4,     // write mode entered, ready for data
    SENSORHUB_FRP_WR_FAILED = 5,    // FRS write request failed
    SENSORHUB_FRP_WR_BAD_MODE = 6,  // data received but not in write mode
    SENSORHUB_FRP_WR_BAD_LENGTH = 7,        // write request length not valid
    SENSORHUB_FRP_WR_REC_VALID = 8, // no error, record passed internal checks
    SENSORHUB_FRP_WR_REC_INVALID = 9,       // record failed internal checks
    SENSORHUB_FRP_WR_DEVICE_ERROR = 10,     // device error (for DFU)
    SENSORHUB_FRP_WR_READONLY = 11, // record is read only
};
typedef uint8_t sensorhub_FRS_WriteStatus_t;

typedef struct sensorhub_FRSWriteResponse {
    sensorhub_FRS_WriteStatus_t status;
    uint16_t offset;
} sensorhub_FRSWriteResponse_t;

enum sensorhub_Cmd_e {
	CMD_TARE = 0x03,
	CMD_SAVE_DCD = 0x06,
	CMD_CONFIG_ME_CAL = 0x07,
    CMD_CONFIG_DCD_SAVE = 0x09,
};
typedef uint8_t sensorhub_Cmd_t;

enum sensorhub_Subcmd_e {
	SUBCMD_TARE_NOW = 0x00,
	SUBCMD_TARE_PERSIST = 0x01,
	SUBCMD_TARE_SET_REORIENTATION = 0x02,
};
typedef uint8_t sensorhub_Subcmd_t;

// for sensorhub_calEnable
#define ACCEL_CAL_EN 1
#define GYRO_CAL_EN 2
#define MAG_CAL_EN 4

#define TARE_AXIS_X 1
#define TARE_AXIS_Y 2
#define TARE_AXIS_Z 4

#define TARE_BASIS_RV 0
#define TARE_BASIS_GAME_RV 1
#define TARE_BASIS_GEOMAG_RV 2

typedef struct sensorhub_ProductID {
    uint8_t resetCause;
    uint8_t swVersionMajor;
    uint8_t swVersionMinor;
    uint32_t swPartNumber;
    uint32_t swBuildNumber;
    uint16_t swVersionPatch;
} sensorhub_ProductID_t;

#define DFU_MAX_PAGES (16)
typedef struct avrDfuStream_s {
  unsigned long totalLength;
  unsigned long pageSize;
  unsigned long numPages;
  /* const int_farptr_t page[DFU_MAX_PAGES]; */
} avrDfuStream_t;

extern bool sensorhub_needs_init;
extern uint32_t sensorhub_resets;
extern uint32_t sensorhub_events;
extern uint32_t sensorhub_empty_events;

/**
 * Reset the sensor hub and initialize it over the I2C bus. This
 * function should be called on power up or if the sensor hub's
 * configuration should be reset. The sensor hub will not send
 * any notifications until sensors have been configured.
 *
 * @param sh the sensor hub configuration
 * @return 0 on success; negative on failure
 */
int sensorhub_probe(const sensorhub_t * sh);

/**
 * @brief Set a sensor's configuration
 * @param sh the sensor hub
 * @param sensor which sensor to set
 * @param features the settings for that sensor
 * @return 0 on success; negative on failure
 */
int sensorhub_setDynamicFeature(const sensorhub_t * sh,
                                sensorhub_Sensor_t sensor,
                                const sensorhub_SensorFeature_t *
                                features);

/**
 * @brief Get a sensor's configuration
 * @param sh the sensor hub
 * @param sensor which sensor to set
 * @param features the settings for that sensor are returned
 * @return 0 on success; negative on failure
 */
int sensorhub_getDynamicFeature(const sensorhub_t * sh,
                                sensorhub_Sensor_t sensor,
                                sensorhub_SensorFeature_t * features);

/**
 * This function should be called after a Sensor Hub interrupt has
 * occurred, but in a regular thread context. This function
 * will issue I2C reads and writes and take longer than desirable
 * in an interrupt context. If no events are available from the
 * Sensor Hub, it will return successfully with numEvents = 0.
 *
 * @param events an array of sensorhub_Event_t to store events
 * @param maxEvents the max number of events that can be returned
 * @param numEvents the number of events actually returned
 * @return 0 on success
 *         SENSORHUB_STATUS_MORE_EVENTS_PENDING if there are more events
 */
int sensorhub_poll(const sensorhub_t * sh, sensorhub_Event_t * events,
                   int maxEvents, int *numEvents);

/* This function will establish the current device attitude as the reference orientation */	
int sensorhub_tareNow(const sensorhub_t * sh, uint8_t axes, uint8_t basis);
	
/* This function will save the current reference orientation in non-volatile storage */	
int sensorhub_tarePersist(const sensorhub_t * sh);
	
/**
 * This function will issue the Configure ME Calibration command and
 * wait for the response.  
 * 
 * @param flags are ACCEL_CAL_EN, GYRO_CAL_EN, MAG_CAL_EN
 * @return 0 on success, -ERR on error
 */	
int sensorhub_calEnable(const sensorhub_t *sh, uint8_t flags);

/* Save the current dynamic calibration data in non-volatile storage. */	
int sensorhub_saveDcd(const sensorhub_t * sh);
	
/**
 * This function will wait for an event. It only returns when it either
 * has an event or has encountered an error.
 *
 * @param event where to store the event
 * @param timeout how many milliseconds to wait
 * @return 0 on success; negative on failure;
 *         SENSORHUB_STATUS_NO_REPORT_PENDING if nothing
 */
int sensorhub_waitForEvent(const sensorhub_t * sh,
                           sensorhub_Event_t * event,
                           uint32_t timeout);

/**
 * Read all events until there are no more.
 *
 * @return 0 on success; negative on failure
 */
int sensorhub_flushEvents(const sensorhub_t * sh);

/**
 * Read an FRS record from the Sensor Hub.
 *
 * @param recordType the type of record to read
 * @param data where to store the data
 * @param offset the offset to start reading at in 32-bit words (normally 0)
 * @param maxLength the length of the data array in 32-bit words
 * @param actualLength where to store the data that was read
 * @return 0 on success; negative on failure
 */
int sensorhub_readFRS(const sensorhub_t * sh,
                      sensorhub_FRS_t recordType,
                      uint32_t * data,
                      uint16_t offset,
                      uint16_t maxLength, uint16_t * actualLength);

/**
 * Write an FRS record to the Sensor Hub.
 *
 * @param recordType the type of record to read
 * @param data the data to write
 * @param length the length of the data to write in 32-bit words
 * @return 0 on success; negative on failure
 */
int sensorhub_writeFRS(const sensorhub_t * sh,
                       sensorhub_FRS_t recordType,
                       const uint32_t * data, uint16_t length);

/**
 * Retrieve the SensorHub's Product ID.
 *
 * @param pid the product IDs are stored here
 * @return 0 on success; negative on failure
 */
int sensorhub_getProductID(const sensorhub_t * sh,
                           sensorhub_ProductID_t * pid);

/**
 * Communicate with the Sensor Hub
 *
 * This routine should normally not be called by user code, but it
 * exists in case there's some transaction with the sensor hub that isn't
 * supported by the API.
 *
 * @param sh the sensorhub
 * @param address either sh->sensorhubAddress or sh->bootloaderAddress
 * @param sendData the data to send first
 * @param sendLength the number of bytes to send
 * @param receiveData the data to receive (could be NULL if this transaction is a transmit)
 * @param receiveLength the max length to receive
 * @return 0 on success. <0 are errors
 */
int sensorhub_i2cTransferWithRetry(const sensorhub_t * sh,
                                   uint8_t address,
                                   const uint8_t * sendData,
                                   int sendLength,
                                   uint8_t * receiveData,
                                   int receiveLength);

/**
 * Update the firmware on the sensor hub. After this
 * completes, call sensorhub_probe() to reinitialize;
 * the sensorhub and use it.
 *
 * This function takes a while! Expect to wait 35-40 seconds.
 *
 * @param dfuStream the firmware in DFU stream format (see util/hex2dfu.py)
 * @param length the length of the firmware
 * @return 0 on success; negative on failure
 */
int sensorhub_dfu(const sensorhub_t * sh,
                  const uint8_t * dfuStream, int length);

/**
 * Update the firmware on the sensor hub. After this
 * completes, call sensorhub_probe() to reinitialize
 * the sensorhub and use it.
 *
 * This function takes a while! Expect to wait 35-40 seconds.
 *
 * @param dfuStream the firmware in AVR DFU stream format (see util/hex2dfu.py)
 * @param length the length of the firmware
 * @return 0 on success; negative on failure
 */
int sensorhub_dfu_avr(const sensorhub_t * sh,
		      const avrDfuStream_t * dfuStream);
			  
uint32_t dfuAddr(uint32_t index);

/**
 * Turn on/off automatic saving of DCD (Dynamic Cal Data)
 *
 * @param sh the sensorhub
 * @param state false to disable auto-save, true to enable it.
 * @return 0 on success, negative on failure.
 */
int sensorhub_dcdAutoSave(const sensorhub_t *sh, bool state);
	

#ifdef __cplusplus
}
#endif
#endif                          // SENSORHUB_H
