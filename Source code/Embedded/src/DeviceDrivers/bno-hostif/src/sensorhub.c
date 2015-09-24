/* ============================================================================
 *
 * Copyright (c) 2014 Hillcrest Laboratories, Inc.  All rights reserved.
 * HILLCREST LABORATORIES, INC. PROPRIETARY/CONFIDENTIAL
 *
 * ============================================================================*/
#include "sensorhub.h"
#include "sensorhub_hid.h"
#include <progmem.h>
#include <string.h>

#define SENSORHUB_CMD_LEN 12 // TODO-DW : Find a home for this.

uint32_t sensorhub_resets = 0;
uint32_t sensorhub_events = 0;
uint32_t sensorhub_empty_events = 0;

static int sensorhub_pollForReport(const sensorhub_t * sh, uint8_t * report);


static int checkError(const sensorhub_t * sh, int rc)
{
    if (rc < 0 && sh->onError)
        sh->onError(sh, rc);

    return rc;
}

int sensorhub_i2cTransferWithRetry(const sensorhub_t * sh,
                                   uint8_t address,
                                   const uint8_t * sendData,
                                   int sendLength,
                                   uint8_t * receiveData,
                                   int receiveLength)
{
    int rc;
    int retries = 0;

    for (;;) {
        sh->stats->i2cTransfers++;
        rc = sh->i2cTransfer(sh, address, sendData, sendLength, receiveData,
                             receiveLength);
        if (rc >= 0)
            break;

        sh->stats->i2cErrors++;

        if (retries >= sh->max_retries)
            break;

        sh->stats->i2cRetries++;
        retries++;
    }

    return rc;
}

static int sensorhub_i2c_handshake(const sensorhub_t * sh) {

    int rc;

    //BUILD_BUG_ON(sizeof(desc) != BNO070_DESC_V1_LEN);
    uint8_t const cmd[2] = {
        BNO070_REGISTER_HID_DESCRIPTOR & 0xFF,
        (BNO070_REGISTER_HID_DESCRIPTOR >> 8) & 0xFF
    };
    union {
        hid_descriptor_t desc;
        uint8_t descBuf[sizeof(hid_descriptor_t)];
    } u;

    // Call I2C directly with no retries.
    rc = sh->i2cTransfer(sh, sh->sensorhubAddress, cmd, sizeof(cmd),
                                        u.descBuf, sizeof(u.descBuf));
    if (rc < 0) {
        return rc;
    }

    if (sh->debugPrintf) {
#if 0 // Full Print
       sh->debugPrintf(
             "I2C Hid Descriptor:\r\n"
             "    wHIDDescLength            = %04x\r\n"
             "    bcdVersion                = %04x\r\n"
             "    wReportDescriptorLength   = %04x\r\n"
             "    wReportDescriptorRegister = %04x\r\n"
             "    wInputRegister            = %04x\r\n"
             "    wMaxInputLength           = %04x\r\n"
             "    wOutputRegister           = %04x\r\n"
             "    wMaxOutputLength          = %04x\r\n"
             "    wCommandRegister          = %04x\r\n"
             "    wDataRegister             = %04x\r\n"
             "    wVendorID                 = %04x\r\n"
             "    wProductID                = %04x\r\n"
             "    wVersionID                = %04x\r\n"
             , (unsigned int) u.desc.wHIDDescLength
             , (unsigned int) u.desc.bcdVersion
             , (unsigned int) u.desc.wReportDescriptorLength
             , (unsigned int) u.desc.wReportDescriptorRegister
             , (unsigned int) u.desc.wInputRegister
             , (unsigned int) u.desc.wMaxInputLength
             , (unsigned int) u.desc.wOutputRegister
             , (unsigned int) u.desc.wMaxOutputLength
             , (unsigned int) u.desc.wCommandRegister
             , (unsigned int) u.desc.wDataRegister
             , (unsigned int) u.desc.wVendorID
             , (unsigned int) u.desc.wProductID
             , (unsigned int) u.desc.wVersionID
#else // Reduced print
       sh->debugPrintf(
           "I2C Hid Descriptor:\r\n"
           "    wHIDDescLength            = %04x\r\n"
           "    bcdVersion                = %04x\r\n"
           , (unsigned int) u.desc.wHIDDescLength
           , (unsigned int) u.desc.bcdVersion
              );
#endif
    }

    if (u.desc.wHIDDescLength != BNO070_DESC_V1_LEN) {
        return checkError(sh, SENSORHUB_STATUS_INVALID_HID_DESCRIPTOR);
    }

    if (u.desc.bcdVersion != BNO070_DESC_V1_BCD) {
        return checkError(sh, SENSORHUB_STATUS_INVALID_HID_DESCRIPTOR);
    }

    return SENSORHUB_STATUS_SUCCESS;
}

static int sensorhub_probe_internal(const sensorhub_t * sh, bool reset)
{
    int i;
    int rc;

    if (reset) {
        bool host_intn_pulled_low;

        /* Put the BNO070 into reset */
        sh->setRSTN(sh, 0);

        /* BNO070 BOOTN high (no bootloader mode) */
        sh->setBOOTN(sh, 1);

        /* Let the BNO sit in reset for a little while */
        sh->delay(sh, 10);

        /* Check if HOST_INTN is incorrectly pulled low. */
        host_intn_pulled_low = !sh->getHOST_INTN(sh);

        /* Take the BNO070 out of reset */
        sh->setRSTN(sh, 1);

        if (host_intn_pulled_low) {
            /* If HOST_INTN is pulled low, wait for the BNO to set
             * it high. Once the BNO inits HOST_INTN high, it leaves
             * it high for 8-10 ms before it's ready for commands.
             * Wait for a max of 100 ms, since the BNO will surely be
             * ready by then and we probably missed the short time
             * that it wasn't.
             */
            for (i = 0; i < 100; i++) {
                if (sh->getHOST_INTN(sh))
                    break;
                sh->delay(sh, 1);
            }
        }
    }

    /* Wait for the BNO070 to boot and check the HID descriptor */
    for (i = 0; i < 100; i++) {
        rc = sensorhub_i2c_handshake(sh);
        if (rc != SENSORHUB_STATUS_ERROR_I2C_IO) break;
        sh->delay(sh, 10);
    }
    if (rc < 0) {
        return checkError(sh, rc);
    }
	
	/* Wait up to 100ms for INTN to be asserted for first time */
	for (i = 0; (i < 100) && (sh->getHOST_INTN(sh)); i++) {
		sh->delay(sh, 1);
	}

    if (!sh->getHOST_INTN(sh)) {
        /* Clear the interrupt by reading up to 10 pending reports */
        for (i = 0; i < 10; i++) {
            uint8_t report[BNO070_MAX_INPUT_REPORT_LEN];
            int rc;
			
			rc = sensorhub_pollForReport(sh, report);
            if (rc == SENSORHUB_STATUS_NO_REPORT_PENDING) {
                break;
            }
        }

#if 0
        /* Check that the interrupt was cleared. */
        if (!sh->getHOST_INTN(sh)) {
            /* Not expecting HOST_INTN. It should have been cleared. */
            return checkError(sh, SENSORHUB_STATUS_RESET_INTN_BROKE);
        }
#endif

    }

    /* We're ready to go. */
    return SENSORHUB_STATUS_SUCCESS;
}

int sensorhub_probe(const sensorhub_t * sh) {
  return sensorhub_probe_internal(sh, true);
}


static inline uint16_t read16(const uint8_t * buffer)
{
    return ((uint16_t) buffer[0]) | ((uint16_t) (buffer[1]) << 8);
}

static inline void write16(uint8_t * buffer, uint16_t value)
{
    buffer[0] = (uint8_t) (value);
    buffer[1] = (uint8_t) (value >> 8);
}

static inline uint32_t read32(const uint8_t * buffer)
{
    return ((uint32_t) buffer[0]) |
           ((uint32_t) (buffer[1]) << 8) |
           ((uint32_t) (buffer[2]) << 16) | ((uint32_t) (buffer[3]) << 24);
}

static inline uint32_t read32be(const uint8_t * buffer)
{
    return ((uint32_t) buffer[3]) |
           ((uint32_t) (buffer[2]) << 8) |
           ((uint32_t) (buffer[1]) << 16) | ((uint32_t) (buffer[0]) << 24);
}

static inline void write32(uint8_t * buffer, uint32_t value)
{
    buffer[0] = (uint8_t) (value);
    buffer[1] = (uint8_t) (value >> 8);
    buffer[2] = (uint8_t) (value >> 16);
    buffer[3] = (uint8_t) (value >> 24);
}

int sensorhub_setDynamicFeature(const sensorhub_t * sh,
                                sensorhub_Sensor_t sensor,
                                const sensorhub_SensorFeature_t * settings)
{
    uint8_t payload[14];

    payload[0] = (settings->changeSensitivityRelative ? 0x1 : 0x0) |
                 (settings->changeSensitivityEnabled ? 0x2 : 0x0) |
                 (settings->wakeupEnabled ? 0x4 : 0x0);
    write16(&payload[1], settings->changeSensitivity);
    write32(&payload[3], settings->reportInterval);
    write32(&payload[7], settings->batchInterval);
    write32(&payload[10], settings->sensorSpecificConfiguration);
    return checkError(sh, shhid_setReport(sh,
                                          HID_REPORT_TYPE_FEATURE,
                                          sensor,
                                          payload, sizeof(payload)));
}

int sensorhub_getDynamicFeature(const sensorhub_t * sh,
                                sensorhub_Sensor_t sensor,
                                sensorhub_SensorFeature_t * settings)
{
    uint8_t payload[17];
    int rc;

    rc = shhid_getReport(sh,
                         HID_REPORT_TYPE_FEATURE,
                         sensor, payload, sizeof(payload));
    if (rc != SENSORHUB_STATUS_SUCCESS)
        return checkError(sh, rc);

    settings->changeSensitivityRelative = ((payload[2] & 0x1) != 0);
    settings->changeSensitivityEnabled = ((payload[2] & 0x2) != 0);
    settings->wakeupEnabled = ((payload[2] & 0x4) != 0);

    settings->changeSensitivity = read16(&payload[3]);
    settings->reportInterval = read32(&payload[5]);
    settings->batchInterval = read32(&payload[9]);
    settings->sensorSpecificConfiguration = read32(&payload[13]);

    return SENSORHUB_STATUS_SUCCESS;
}

static int sensorhub_decodeEvent(const sensorhub_t * sh,
                                 const uint8_t * report,
                                 sensorhub_Event_t * event)
{
    /* Get the length. Technically, it's 2 bytes, but valid reports won't come
       close to using the 2nd byte */
    int length = report[0];
    if (length > BNO070_MAX_INPUT_REPORT_LEN || report[1] != 0)
        return checkError(sh, SENSORHUB_STATUS_REPORT_LEN_TOO_LONG);

    // Look for reset indications
    if ((report[2] == SENSORHUB_CMD_RESP) &&
        ((report[4] & 0x7F) == 0x04)) {
	    // The sensorhub says it was reset
	    sensorhub_resets++;
		return SENSORHUB_STATUS_HUB_RESET;
    }

    /* Fill out common fields */
    event->sensor = report[2];
    event->sequenceNumber = report[3];
    event->status = report[4];
    event->delay = report[5];

    switch (event->sensor) {
        /* Reports that are 1 16-bit integer */
    case SENSORHUB_HUMIDITY:
    case SENSORHUB_PROXIMITY:
    case SENSORHUB_TEMPERATURE:
    case SENSORHUB_SIGNIFICANT_MOTION:
    case SENSORHUB_SHAKE_DETECTOR:
    case SENSORHUB_FLIP_DETECTOR:
    case SENSORHUB_PICKUP_DETECTOR:
    case SENSORHUB_STEP_DETECTOR:
    case SENSORHUB_STABILITY_DETECTOR:
        if (length < 8)
            return checkError(sh, SENSORHUB_STATUS_REPORT_INVALID_LEN);

        event->un.field16[0] = read16(&report[6]);
        break;

        /* Reports that are 1 32-bit integer */
    case SENSORHUB_PRESSURE:
    case SENSORHUB_AMBIENT_LIGHT:
        if (length < 10)
            return checkError(sh, SENSORHUB_STATUS_REPORT_INVALID_LEN);

        event->un.field32[0] = read32(&report[6]);
        break;

        /* 4 16-bit integers and a 32-bit timestamp */
    case SENSORHUB_RAW_ACCELEROMETER:
    case SENSORHUB_RAW_GYROSCOPE:
    case SENSORHUB_RAW_MAGNETOMETER:
        if (length < 18)
            return checkError(sh, SENSORHUB_STATUS_REPORT_INVALID_LEN);

        event->un.field16[0] = read16(&report[6]);
        event->un.field16[1] = read16(&report[8]);
        event->un.field16[2] = read16(&report[10]);
        event->un.field16[3] = read16(&report[12]);
        event->un.field32[2] = read32(&report[14]);
        break;

        /* Reports that are 3 16-bit integers */
    case SENSORHUB_ACCELEROMETER:
    case SENSORHUB_LINEAR_ACCELERATION:
    case SENSORHUB_GRAVITY:
    case SENSORHUB_GYROSCOPE_CALIBRATED:
    case SENSORHUB_MAGNETIC_FIELD_CALIBRATED:
        if (length < 12)
            return checkError(sh, SENSORHUB_STATUS_REPORT_INVALID_LEN);

        event->un.field16[0] = read16(&report[6]);
        event->un.field16[1] = read16(&report[8]);
        event->un.field16[2] = read16(&report[10]);
        break;

        /* Reports that are 4 16-bit integers */
    case SENSORHUB_GAME_ROTATION_VECTOR:
        if (length < 12)
            return checkError(sh, SENSORHUB_STATUS_REPORT_INVALID_LEN);

        event->un.field16[0] = read16(&report[6]);
        event->un.field16[1] = read16(&report[8]);
        event->un.field16[2] = read16(&report[10]);
        event->un.field16[3] = read16(&report[12]);
        break;

        /* Reports that are 5 16-bit integers */
    case SENSORHUB_ROTATION_VECTOR:
    case SENSORHUB_GEOMAGNETIC_ROTATION_VECTOR:
        if (length < 14)
            return checkError(sh, SENSORHUB_STATUS_REPORT_INVALID_LEN);

        event->un.field16[0] = read16(&report[6]);
        event->un.field16[1] = read16(&report[8]);
        event->un.field16[2] = read16(&report[10]);
        event->un.field16[3] = read16(&report[12]);
        event->un.field16[4] = read16(&report[14]);
        break;

        /* Reports that are 6 16-bit integers */
    case SENSORHUB_GYROSCOPE_UNCALIBRATED:
    case SENSORHUB_MAGNETIC_FIELD_UNCALIBRATED:
        if (length < 16)
            return checkError(sh, SENSORHUB_STATUS_REPORT_INVALID_LEN);

        event->un.field16[0] = read16(&report[6]);
        event->un.field16[1] = read16(&report[8]);
        event->un.field16[2] = read16(&report[10]);
        event->un.field16[3] = read16(&report[12]);
        event->un.field16[4] = read16(&report[14]);
        event->un.field16[5] = read16(&report[16]);
        break;

    case SENSORHUB_STEP_COUNTER:
        if (length < 14)
            return checkError(sh, SENSORHUB_STATUS_REPORT_INVALID_LEN);

        event->un.stepCounter.detectLatency = read32(&report[6]);
        event->un.stepCounter.steps = read16(&report[10]);
        event->un.stepCounter.reserved = read16(&report[12]);
        break;

    case SENSORHUB_PERSONAL_ACTIVITY_CLASSIFIER:
        if (length < 16)
            return checkError(sh, SENSORHUB_STATUS_REPORT_INVALID_LEN);

        event->un.personalActivityClassifier.pageNumberEOS = report[6];
        event->un.personalActivityClassifier.mostLikelyState = report[7];
        for (int i=0; i < 10; ++i) {
            event->un.personalActivityClassifier.confidence[i] = report[8 + i];
        }
        break;

    case SENSORHUB_PRODUCT_ID_RESPONSE:
    case SENSORHUB_FRS_READ_RESPONSE:
    case SENSORHUB_FRS_WRITE_RESPONSE:
    case SENSORHUB_CMD_RESP:
        /* Stale FRS read or write responses */
        /* NOTE: Don't run these through checkError, since they are such a
         *       minor annoyance that we don't want to alert the user. The
         *       calling code can do more if it wants.
         */
        return SENSORHUB_STATUS_NOT_AN_EVENT;

        /* TBD */
    case SENSORHUB_SAR:
    case SENSORHUB_TAP_DETECTOR:
    case SENSORHUB_ACTIVITY_CLASSIFICATION:
        break;
    default:
        return checkError(sh, SENSORHUB_STATUS_REPORT_UNKNOWN);
    }
    return SENSORHUB_STATUS_SUCCESS;
}

static int sensorhub_pollForReport(const sensorhub_t * sh,
                                   uint8_t * report)
{
    int rc;

    /* Check HOST_INTN to see if there are events waiting. */
    if (!sh->getDataReady(sh)) {
        return checkError(sh, SENSORHUB_STATUS_NO_REPORT_PENDING);
    }

    rc = sensorhub_i2cTransferWithRetry(sh, sh->sensorhubAddress, NULL, 0, report,
                                        BNO070_MAX_INPUT_REPORT_LEN);
    return checkError(sh, rc);
}

int sensorhub_poll(const sensorhub_t * sh, sensorhub_Event_t * events,
                   int maxEvents, int *numEvents)
{
    uint8_t report[BNO070_MAX_INPUT_REPORT_LEN];
    int rc;

    *numEvents = 0;

    do {
        rc = sensorhub_pollForReport(sh, report);
        if (rc == SENSORHUB_STATUS_NO_REPORT_PENDING)
            return SENSORHUB_STATUS_SUCCESS;
        else if (rc > 0)
            return rc;
        else if (rc != SENSORHUB_STATUS_SUCCESS)
            return checkError(sh, rc);

        sensorhub_events++;

        /* Null length reports also indicate no more events */
        if (report[0] == 0) {
	        sensorhub_empty_events++;
            return SENSORHUB_STATUS_SUCCESS;
        }

        /* Decode the event. Ignore reports that aren't events. */
        rc = sensorhub_decodeEvent(sh, report, &events[*numEvents]);
        if (rc == SENSORHUB_STATUS_NOT_AN_EVENT)
            continue;
        else if (rc > 0)
            return rc;
        else if (rc != SENSORHUB_STATUS_SUCCESS)
            return checkError(sh, rc);

        (*numEvents)++;

        /* Allow hub time to release host interrupt */
        //sh->delay(sh, 1); // !!! removed by YSB
    } while (*numEvents < maxEvents);

    /* We ran out of places to store events, so return. */
    return SENSORHUB_STATUS_MORE_EVENTS_PENDING;
}

int sensorhub_waitForEvent(const sensorhub_t * sh,
                           sensorhub_Event_t * event, uint32_t timeout)
{
    uint32_t startTime = sh->getTick(sh);
    int rc;
    int numEvents;
    do {
        rc = sensorhub_poll(sh, event, 1, &numEvents);
        /* Keep trying even if there are warnings */
    } while (rc >= 0 && numEvents == 0
             && (sh->getTick(sh) - startTime < timeout));

    return rc;
}

static int sensorhub_waitForReport(const sensorhub_t * sh,
                                   uint8_t * report, uint32_t timeout)
{
    uint32_t startTime = sh->getTick(sh);
    int rc;
    do {
        rc = sensorhub_pollForReport(sh, report);
    } while (rc == SENSORHUB_STATUS_NO_REPORT_PENDING
             && (sh->getTick(sh) - startTime < timeout));

    return rc;
}

int sensorhub_flushEvents(const sensorhub_t * sh)
{
    uint8_t report[BNO070_MAX_INPUT_REPORT_LEN];
    int rc;
    do {
        rc = sensorhub_pollForReport(sh, report);
    } while (rc == SENSORHUB_STATUS_SUCCESS);

    if (rc == SENSORHUB_STATUS_NO_REPORT_PENDING)
        return SENSORHUB_STATUS_SUCCESS;
    else
        return rc;
}

static int sensorhub_sendProductIDRequest(const sensorhub_t * sh)
{
    uint8_t payload[1];

    payload[0] = 0;
    return shhid_setReport(sh,
                           HID_REPORT_TYPE_OUTPUT,
                           SENSORHUB_PRODUCT_ID_REQUEST,
                           payload, sizeof(payload));
}

static int sensorhub_sendFRSReadRequest(const sensorhub_t * sh,
                                        sensorhub_FRS_t recordType,
                                        uint16_t offset, uint16_t length)
{
    uint8_t payload[7];

    payload[0] = 0;
    write16(&payload[1], offset);
    write16(&payload[3], recordType);
    write16(&payload[7], length);
    return shhid_setReport(sh,
                           HID_REPORT_TYPE_OUTPUT,
                           SENSORHUB_FRS_READ_REQUEST,
                           payload, sizeof(payload));
}

static int sensorhub_sendFRSWriteRequest(const sensorhub_t * sh,
        sensorhub_FRS_t recordType,
        uint16_t length)
{
    uint8_t payload[5];

    payload[0] = 0;
    write16(&payload[1], length);
    write16(&payload[3], recordType);
    return shhid_setReport(sh,
                           HID_REPORT_TYPE_OUTPUT,
                           SENSORHUB_FRS_WRITE_REQUEST,
                           payload, sizeof(payload));
}

static int sensorhub_sendFRSWriteDataRequest(const sensorhub_t * sh,
        uint16_t offset,
        const uint32_t * data,
        uint8_t length)
{
    uint8_t payload[11];

    payload[0] = 0;
    write16(&payload[1], offset);
    write32(&payload[3], data[0]);
    write32(&payload[7], length > 1 ? data[1] : 0);
    return shhid_setReport(sh,
                           HID_REPORT_TYPE_OUTPUT,
                           SENSORHUB_FRS_WRITE_DATA_REQUEST,
                           payload, sizeof(payload));
}



static int sensorhub_getFRSReadResponse(const sensorhub_t * sh,
                                        sensorhub_FRSReadResponse_t *
                                        response, uint32_t timeout)
{
    uint32_t startTime = sh->getTick(sh);
    uint32_t now = startTime;
    uint8_t payload[BNO070_MAX_INPUT_REPORT_LEN];
    while (now - startTime < timeout) {
        int rc = sensorhub_waitForReport(sh, payload,
                                         timeout - (now - startTime));
        if (rc != SENSORHUB_STATUS_SUCCESS)
            return rc;

        if (payload[0] == 18 && payload[1] == 0
                && payload[2] == SENSORHUB_FRS_READ_RESPONSE) {
            response->length = (payload[3] >> 4);
            response->status = (payload[3] & 0xf);
            response->offset = read16(&payload[4]);
            response->data[0] = read32(&payload[6]);
            response->data[1] = read32(&payload[10]);
            response->recordType = read16(&payload[14]);
            response->reserved = read16(&payload[16]);

            return SENSORHUB_STATUS_SUCCESS;
        }

        now = sh->getTick(sh);
    }
    return SENSORHUB_STATUS_NO_REPORT_PENDING;
}

static int sensorhub_getFRSWriteResponse(const sensorhub_t * sh,
        sensorhub_FRSWriteResponse_t *
        response, uint32_t timeout)
{
    uint32_t startTime = sh->getTick(sh);
    uint32_t now = startTime;
    uint8_t payload[BNO070_MAX_INPUT_REPORT_LEN];
    while (now - startTime < timeout) {
        int rc = sensorhub_waitForReport(sh, payload,
                                         timeout - (now - startTime));
        if (rc != SENSORHUB_STATUS_SUCCESS)
            return rc;

        if (payload[0] == 6 && payload[1] == 0
                && payload[2] == SENSORHUB_FRS_WRITE_RESPONSE) {
            response->status = payload[3];
            response->offset = read16(&payload[4]);
            return SENSORHUB_STATUS_SUCCESS;
        }
        now = sh->getTick(sh);
    }
    return SENSORHUB_STATUS_NO_REPORT_PENDING;
}

static int sensorhub_getProductIDResponse(const sensorhub_t * sh,
        sensorhub_ProductID_t * response,
        uint32_t timeout)
{
    uint32_t startTime = sh->getTick(sh);
    uint32_t now = startTime;
    uint8_t payload[BNO070_MAX_INPUT_REPORT_LEN];
    while (now - startTime < timeout) {
        int rc = sensorhub_waitForReport(sh, payload,
                                         timeout - (now - startTime));
        if (rc != SENSORHUB_STATUS_SUCCESS)
            return rc;

        if (payload[0] == 18 && payload[1] == 0
                && payload[2] == SENSORHUB_PRODUCT_ID_RESPONSE) {
            response->resetCause = payload[3];
            response->swVersionMajor = payload[4];
            response->swVersionMinor = payload[5];
            response->swPartNumber = read32(&payload[6]);
            response->swBuildNumber = read32(&payload[10]);
            response->swVersionPatch = read16(&payload[14]);
            return SENSORHUB_STATUS_SUCCESS;
        }
        now = sh->getTick(sh);
    }
    return SENSORHUB_STATUS_NO_REPORT_PENDING;
}

int sensorhub_readFRS(const sensorhub_t * sh, sensorhub_FRS_t recordType,
                      uint32_t * data, uint16_t offset, uint16_t maxLength,
                      uint16_t * actualLength)
{
    sensorhub_FRSReadResponse_t resp;
    int rc;
    int i;

    *actualLength = 0;

    rc = sensorhub_sendFRSReadRequest(sh, recordType, offset, maxLength);
    if (rc != SENSORHUB_STATUS_SUCCESS)
        return checkError(sh, rc);

    while (*actualLength < maxLength) {
        rc = sensorhub_getFRSReadResponse(sh, &resp, 1000);
        if (rc != SENSORHUB_STATUS_SUCCESS)
            return rc;

        switch (resp.status) {
        case SENSORHUB_FRP_RD_NO_ERR:
            if (resp.offset != offset)
                return SENSORHUB_STATUS_FRS_READ_BAD_OFFSET;
            if (resp.length > 2)
                return SENSORHUB_STATUS_FRS_READ_BAD_LENGTH;
            if (resp.recordType != recordType)
                return SENSORHUB_STATUS_FRS_READ_BAD_TYPE;

            for (i = 0; i < resp.length && *actualLength < maxLength; i++) {
                *data++ = resp.data[i];
                (*actualLength)++;
                offset++;
            }
            break;

        case SENSORHUB_FRP_RD_BAD_TYPE:
            return checkError(sh,
                              SENSORHUB_STATUS_FRS_READ_UNRECOGNIZED_FRS);

        case SENSORHUB_FRP_RD_BUSY:
            return checkError(sh, SENSORHUB_STATUS_FRS_READ_BUSY);

        case SENSORHUB_FRP_RD_BAD_OFFSET:
            return checkError(sh,
                              SENSORHUB_STATUS_FRS_READ_OFFSET_OUT_OF_RANGE);

        case SENSORHUB_FRP_RD_RECORD_EMPTY:
            return checkError(sh, SENSORHUB_STATUS_FRS_READ_EMPTY);

        case SENSORHUB_FRP_RD_COMPLETE:
        case SENSORHUB_FRP_RD_BLOCK_DONE:
        case SENSORHUB_FRP_RD_BLOCK_REC_DONE:
            if (resp.offset != offset)
                return checkError(sh,
                                  SENSORHUB_STATUS_FRS_READ_BAD_OFFSET);
            if (resp.length > 2)
                return checkError(sh,
                                  SENSORHUB_STATUS_FRS_READ_BAD_LENGTH);
            if (resp.recordType != recordType)
                return checkError(sh, SENSORHUB_STATUS_FRS_READ_BAD_TYPE);

            for (i = 0; i < resp.length && *actualLength < maxLength; i++) {
                *data++ = resp.data[i];
                (*actualLength)++;
                offset++;
            }
            return SENSORHUB_STATUS_SUCCESS;

        case SENSORHUB_FRP_RD_DEVICE_ERROR:
            return checkError(sh, SENSORHUB_STATUS_FRS_READ_DEVICE_ERROR);

        default:
            return checkError(sh, SENSORHUB_STATUS_FRS_READ_UNKNOWN_ERROR);
        }
    }

    /* Since we always specify the number of bytes that we want, it's unexpected
       to get too many */
    return checkError(sh, SENSORHUB_STATUS_FRS_READ_UNEXPECTED_LENGTH);
}

int sensorhub_writeFRS(const sensorhub_t * sh, sensorhub_FRS_t recordType,
                       const uint32_t * data, uint16_t length)
{
    sensorhub_FRSWriteResponse_t resp;
    int rc;
    uint16_t offset = 0;

    /* Step 1: Make a write request */
    rc = sensorhub_sendFRSWriteRequest(sh, recordType, length);
    if (rc != SENSORHUB_STATUS_SUCCESS)
        return checkError(sh, rc);

    rc = sensorhub_getFRSWriteResponse(sh, &resp, 1000);
    if (rc != SENSORHUB_STATUS_SUCCESS)
        return checkError(sh, rc);

    switch (resp.status) {
    case SENSORHUB_FRP_WR_READY:
        /* Expected response for length > 0 */
        break;

    case SENSORHUB_FRP_WR_COMPLETE:
        /* Expected response for length == 0. No validation step needed. */
        return SENSORHUB_STATUS_SUCCESS;

    case SENSORHUB_FRP_WR_BAD_TYPE:
        return checkError(sh, SENSORHUB_STATUS_FRS_WRITE_BAD_TYPE);

    case SENSORHUB_FRP_WR_BUSY:
        return checkError(sh, SENSORHUB_STATUS_FRS_WRITE_BUSY);

    case SENSORHUB_FRP_WR_BAD_LENGTH:
        return checkError(sh, SENSORHUB_STATUS_FRS_WRITE_BAD_LENGTH);

    case SENSORHUB_FRP_WR_BAD_MODE:
        return checkError(sh, SENSORHUB_STATUS_FRS_WRITE_BAD_MODE);

    case SENSORHUB_FRP_WR_DEVICE_ERROR:
        return checkError(sh, SENSORHUB_STATUS_FRS_WRITE_DEVICE_ERROR);

    case SENSORHUB_FRP_WR_READONLY:
        return checkError(sh, SENSORHUB_STATUS_FRS_READ_ONLY);

    case SENSORHUB_FRP_WR_FAILED:
    case SENSORHUB_FRP_WR_REC_VALID:
    case SENSORHUB_FRP_WR_REC_INVALID:
    case SENSORHUB_FRP_WR_ACK:
    default:
        return checkError(sh, SENSORHUB_STATUS_FRS_WRITE_BAD_STATUS);
    }

    /* Step 2: Send the write data requests */
    while (length > 0) {
        uint8_t towrite = (length > 2) ? 2 : length;
        rc = sensorhub_sendFRSWriteDataRequest(sh, offset, data, towrite);
        if (rc != SENSORHUB_STATUS_SUCCESS)
            return checkError(sh, rc);

        rc = sensorhub_getFRSWriteResponse(sh, &resp, 1000);
        if (rc != SENSORHUB_STATUS_SUCCESS)
            return checkError(sh, rc);
        switch (resp.status) {
        case SENSORHUB_FRP_WR_ACK:
            length -= towrite;
            data += towrite;
            offset += towrite;
            break;

        case SENSORHUB_FRP_WR_BAD_TYPE:
            return checkError(sh, SENSORHUB_STATUS_FRS_WRITE_BAD_TYPE);

        case SENSORHUB_FRP_WR_BUSY:
            return checkError(sh, SENSORHUB_STATUS_FRS_WRITE_BUSY);

        case SENSORHUB_FRP_WR_BAD_LENGTH:
            return checkError(sh, SENSORHUB_STATUS_FRS_WRITE_BAD_LENGTH);

        case SENSORHUB_FRP_WR_BAD_MODE:
            return checkError(sh, SENSORHUB_STATUS_FRS_WRITE_BAD_MODE);

        case SENSORHUB_FRP_WR_DEVICE_ERROR:
            return checkError(sh, SENSORHUB_STATUS_FRS_WRITE_DEVICE_ERROR);

        case SENSORHUB_FRP_WR_FAILED:
            return checkError(sh, SENSORHUB_STATUS_FRS_WRITE_FAILED);

        case SENSORHUB_FRP_WR_READONLY:
            return checkError(sh, SENSORHUB_STATUS_FRS_READ_ONLY);

        case SENSORHUB_FRP_WR_REC_INVALID:
        case SENSORHUB_FRP_WR_REC_VALID:
        case SENSORHUB_FRP_WR_COMPLETE:
        case SENSORHUB_FRP_WR_READY:
        default:
            return checkError(sh, SENSORHUB_STATUS_FRS_WRITE_BAD_STATUS);
        }
    }

    /* Step 3: Wait for the verification response */
    rc = sensorhub_getFRSWriteResponse(sh, &resp, 1000);
    if (rc != SENSORHUB_STATUS_SUCCESS)
        return checkError(sh, rc);
    switch (resp.status) {
    case SENSORHUB_FRP_WR_REC_VALID:
        /* Expected response */
        break;

    case SENSORHUB_FRP_WR_REC_INVALID:
        return checkError(sh, SENSORHUB_STATUS_FRS_INVALID_RECORD);

    case SENSORHUB_FRP_WR_DEVICE_ERROR:
        return checkError(sh, SENSORHUB_STATUS_FRS_WRITE_DEVICE_ERROR);

    case SENSORHUB_FRP_WR_FAILED:
        return checkError(sh, SENSORHUB_STATUS_FRS_WRITE_FAILED);
		
    case SENSORHUB_FRP_WR_COMPLETE:
		return SENSORHUB_STATUS_SUCCESS;

    case SENSORHUB_FRP_WR_READONLY:
    case SENSORHUB_FRP_WR_BAD_TYPE:
    case SENSORHUB_FRP_WR_BUSY:
    case SENSORHUB_FRP_WR_BAD_LENGTH:
    case SENSORHUB_FRP_WR_BAD_MODE:
    case SENSORHUB_FRP_WR_ACK:
    case SENSORHUB_FRP_WR_READY:
    default:
        return checkError(sh, SENSORHUB_STATUS_FRS_WRITE_BAD_STATUS);
    }

    /* Step 4: Wait for the write complete response */
    rc = sensorhub_getFRSWriteResponse(sh, &resp, 1000);
    if (rc != SENSORHUB_STATUS_SUCCESS)
        return checkError(sh, rc);
    switch (resp.status) {
    case SENSORHUB_FRP_WR_COMPLETE:
        /* Expected response */
        break;

    case SENSORHUB_FRP_WR_DEVICE_ERROR:
        return checkError(sh, SENSORHUB_STATUS_FRS_WRITE_DEVICE_ERROR);

    case SENSORHUB_FRP_WR_FAILED:
        return checkError(sh, SENSORHUB_STATUS_FRS_WRITE_FAILED);

    case SENSORHUB_FRP_WR_REC_INVALID:
    case SENSORHUB_FRP_WR_REC_VALID:
    case SENSORHUB_FRP_WR_READONLY:
    case SENSORHUB_FRP_WR_BAD_TYPE:
    case SENSORHUB_FRP_WR_BUSY:
    case SENSORHUB_FRP_WR_BAD_LENGTH:
    case SENSORHUB_FRP_WR_BAD_MODE:
    case SENSORHUB_FRP_WR_ACK:
    case SENSORHUB_FRP_WR_READY:
    default:
        return checkError(sh, SENSORHUB_STATUS_FRS_WRITE_BAD_STATUS);
    }

    return SENSORHUB_STATUS_SUCCESS;
}

int sensorhub_getProductID(const sensorhub_t * sh,
                           sensorhub_ProductID_t * pid)
{
    int rc;
    rc = sensorhub_sendProductIDRequest(sh);
    if (rc != SENSORHUB_STATUS_SUCCESS)
        return checkError(sh, rc);

    rc = sensorhub_getProductIDResponse(sh, pid, 1000);
    if (rc != SENSORHUB_STATUS_SUCCESS)
        return checkError(sh, rc);

    // Ignore the rest of the reports
    sensorhub_ProductID_t ignore;
    sensorhub_getProductIDResponse(sh, &ignore, 100);
    sensorhub_getProductIDResponse(sh, &ignore, 100);
    sensorhub_getProductIDResponse(sh, &ignore, 100);

    return rc;
}


uint32_t avr_read32(const avrDfuStream_t *dfuStream, unsigned long index)
{
    int n = 0;
    uint32_t retval = 0;

    for (n = 0; n < 4; n++) {
        uint32_t byte = pgm_read_byte_far(dfuAddr(index));
	    retval |= byte << (n*8);
		index++;
    }

    return retval;
}

uint32_t avr_read32be(const avrDfuStream_t *dfuStream, unsigned long index)
{
    uint32_t retval = 0;
    int n = 0;

    for (n = 0; n < 4; n++) {
        uint32_t byte = pgm_read_byte_far(dfuAddr(index));
	    retval <<= 8;
	    retval |= byte;
		index++;
    }

    return retval;
}

uint8_t avr_read8(const avrDfuStream_t *dfuStream, unsigned long index)
{
  return pgm_read_byte_far(dfuAddr(index));
}

void avr_readBuf(uint8_t *buf, unsigned long length,
		 const avrDfuStream_t *dfuStream,
		 unsigned long index)
{
    int n = 0;
	static int trap = 0;

	if(index == 63949) {
		trap = 1;
	}

    for (n = 0; n < length; n++) {
        uint8_t byte = pgm_read_byte_far(dfuAddr(index));
	    buf[n] = byte;
		index++;
    }
}

uint32_t dfu_index;
uint8_t dfu_writeBuf[32];

int sensorhub_dfu_avr(const sensorhub_t *sh,
                  const avrDfuStream_t *dfuStream)
{
    /* Check that the type of DFU stream is one that we can handle
     * NOTE: We only support one format for the BNO070 and that's type 0x01010101
     */
    uint32_t dfuFormat = avr_read32(dfuStream, 0);
    if (dfuFormat != 0x01010101)
        return checkError(sh, SENSORHUB_STATUS_UNEXPECTED_DFU_STREAM_TYPE);

    /* The DFU stream format is specified as big endian. Read the application
     * size to double check the length.
     */
    uint32_t applicationSize = avr_read32be(dfuStream, 4);
    int packetPayloadSize = avr_read8(dfuStream, 10);
    int packetSize = packetPayloadSize + 2;

    uint32_t expectedLength = 4 /* dfu format */ +
                              6 /* application size + CRC */ +
                              3 /* packet size + CRC */ +
                             applicationSize + /* application bytes */
                            ((applicationSize + packetPayloadSize - 1) / packetPayloadSize) * 2; /* CRC per packet */
    if (expectedLength != dfuStream->totalLength)
        return checkError(sh, SENSORHUB_STATUS_DFU_STREAM_SIZE_WRONG);

    /* Put the BNO070 into reset */
    sh->setRSTN(sh, 0);

    /* BNO070 BOOTN low (bootloader mode) */
    sh->setBOOTN(sh, 0);

    sh->delay(sh, 10);

    /* Take the BNO070 out of reset */
    sh->setRSTN(sh, 1);

    sh->delay(sh, 10);

    /* Send each packet of the DFU */
    uint32_t index = 4;
    while (index < dfuStream->totalLength) {
        int rc;
        uint8_t response;

        int lengthToWrite = packetSize;
        if (index == 4)
            lengthToWrite = 6; // First packet -> total length
        else if (index == 10)
            lengthToWrite = 3; // Second packet -> packet length
        else if (index + lengthToWrite > dfuStream->totalLength)
            lengthToWrite = dfuStream->totalLength - index; // Last packet -> could be short

	    avr_readBuf(dfu_writeBuf, lengthToWrite, dfuStream, index);

        rc = sensorhub_i2cTransferWithRetry(sh, sh->bootloaderAddress, dfu_writeBuf, lengthToWrite, 0, 0);
        if (rc != SENSORHUB_STATUS_SUCCESS)
            return checkError(sh, rc);

        rc = sensorhub_i2cTransferWithRetry(sh, sh->bootloaderAddress, 0, 0, &response, sizeof(response));
        if (rc != SENSORHUB_STATUS_SUCCESS)
            return checkError(sh, rc);

        /* Check that we got a successful response. */
        if (response != 's')
            return checkError(sh, SENSORHUB_STATUS_DFU_RECEIVED_NAK);

        /* The capture from the Bosch programmer had a 1-3 ms delay between packets.
         * Testing confirms that if we don't delay that the programmed image doesn't
         * work. 2 ms works. 1 ms doesn't.
         */
        //sh->delay(sh, 2);

        index += lengthToWrite;
		dfu_index = index;
    }

    return sensorhub_probe_internal(sh, false);
}

int sensorhub_dfu(const sensorhub_t *sh,
                  const uint8_t *dfuStream,
                  int length)
{
    /* Check that the type of DFU stream is one that we can handle
     * NOTE: We only support one format for the BNO070 and that's type 0x01010101
     */
    uint32_t dfuFormat = read32(dfuStream);
    if (dfuFormat != 0x01010101)
        return checkError(sh, SENSORHUB_STATUS_UNEXPECTED_DFU_STREAM_TYPE);

    /* The DFU stream format is specified as big endian. Read the application
     * size to double check the length.
     */
    uint32_t applicationSize = read32be(&dfuStream[4]);
    int packetPayloadSize = dfuStream[10];
    int packetSize = packetPayloadSize + 2;

    int expectedLength = 4 /* dfu format */ +
                         6 /* application size + CRC */ +
                         3 /* packet size + CRC */ +
                         applicationSize + /* application bytes */
                         ((applicationSize + packetPayloadSize - 1) / packetPayloadSize) * 2; /* CRC per packet */
    if (expectedLength != length)
        return checkError(sh, SENSORHUB_STATUS_DFU_STREAM_SIZE_WRONG);

    /* Put the BNO070 into reset */
    sh->setRSTN(sh, 0);

    /* BNO070 BOOTN low (bootloader mode) */
    sh->setBOOTN(sh, 0);

    sh->delay(sh, 10);

    /* Take the BNO070 out of reset */
    sh->setRSTN(sh, 1);

    sh->delay(sh, 10);

    /* Send each packet of the DFU */
    int index = 4;
    while (index < length) {
        int rc;
        uint8_t response;

        int lengthToWrite = packetSize;
        if (index == 4)
            lengthToWrite = 6; // First packet -> total length
        else if (index == 10)
            lengthToWrite = 3; // Second packet -> packet length
        else if (index + lengthToWrite > length)
            lengthToWrite = length - index; // Last packet -> could be short

        rc = sensorhub_i2cTransferWithRetry(sh, sh->bootloaderAddress, &dfuStream[index], lengthToWrite, 0, 0);
        if (rc != SENSORHUB_STATUS_SUCCESS)
            return checkError(sh, rc);

        rc = sensorhub_i2cTransferWithRetry(sh, sh->bootloaderAddress, 0, 0, &response, sizeof(response));
        if (rc != SENSORHUB_STATUS_SUCCESS)
            return checkError(sh, rc);

        /* Check that we got a successful response. */
        if (response != 's')
            return checkError(sh, SENSORHUB_STATUS_DFU_RECEIVED_NAK);

        /* The capture from the Bosch programmer had a 1-3 ms delay between packets.
         * Testing confirms that if we don't delay that the programmed image doesn't
         * work. 2 ms works. 1 ms doesn't.
         */
        //sh->delay(sh, 2);

        index += lengthToWrite;
    }

    return sensorhub_probe_internal(sh, false);
}

int sensorhub_tareNow(const sensorhub_t * sh, uint8_t axes, uint8_t basis)
{
	uint8_t buffer[32];  // TODO: symbol
	int rc = 0;

	// Send Tare Now
	memset(buffer, 0, sizeof(buffer));
	buffer[0] = 0;     // sequence
	buffer[1] = CMD_TARE;
	buffer[2] = SUBCMD_TARE_NOW;
	buffer[3] = axes;
	buffer[4] = basis;

	shhid_setReport(sh, HID_REPORT_TYPE_OUTPUT, SENSORHUB_CMD_REQ,
	                buffer, SENSORHUB_CMD_LEN-1);

	return rc;
}

int sensorhub_tarePersist(const sensorhub_t * sh)
{
	uint8_t buffer[32];  // TODO: symbol
	int rc = 0;

	// Send Tare Now
	memset(buffer, 0, sizeof(buffer));
	buffer[0] = 0;     // sequence
	buffer[1] = CMD_TARE;
	buffer[2] = SUBCMD_TARE_PERSIST;

	shhid_setReport(sh, HID_REPORT_TYPE_OUTPUT, SENSORHUB_CMD_REQ,
	                buffer, SENSORHUB_CMD_LEN-1);

	return rc;
}

int sensorhub_calEnable(const sensorhub_t * sh, uint8_t flags)
{
	uint8_t buffer[32];  // TODO: symbol
	int rc = 0;
    int i = 0;

	// Send Cal enable command
	memset(buffer, 0, sizeof(buffer));
	buffer[0] = 0;     // sequence
	buffer[1] = CMD_CONFIG_ME_CAL;  // command: Configure ME Calibration
	buffer[2] = (flags & ACCEL_CAL_EN) ? 1 : 0;
	buffer[3] = (flags & GYRO_CAL_EN) ? 1 : 0;
	buffer[4] = (flags & MAG_CAL_EN) ? 1 : 0;

	shhid_setReport(sh, HID_REPORT_TYPE_OUTPUT, SENSORHUB_CMD_REQ,
	                buffer, SENSORHUB_CMD_LEN-1);

	// Get Cal enable response
    rc = SENSORHUB_STATUS_UNEXPECTED_REPORT;
	for (i = 0; i < 10; ++i)  {
		sensorhub_waitForReport(sh, buffer, 100);
		if ((buffer[2] == SENSORHUB_CMD_RESP) &&
		    (buffer[4] == CMD_CONFIG_ME_CAL)) {
			if (buffer[7] != 0) {
				rc = SENSORHUB_STATUS_OP_FAILED;
			} else {
                rc = SENSORHUB_STATUS_SUCCESS;
            }
            break;
		}
	}

	return rc;
}

int sensorhub_saveDcd(const sensorhub_t *sh)
{
	uint8_t buffer[32];  // TODO: symbol
	int rc = 0;
    int i = 0;

	// Send Cal enable command
	memset(buffer, 0, sizeof(buffer));
	buffer[0] = 0;     // sequence
	buffer[1] = CMD_SAVE_DCD;  // command: Save DCD

	shhid_setReport(sh, HID_REPORT_TYPE_OUTPUT, SENSORHUB_CMD_REQ,
	                buffer, SENSORHUB_CMD_LEN-1);

	// Get Cal enable response
    rc = SENSORHUB_STATUS_UNEXPECTED_REPORT;
	for (i = 0; i < 10; ++i)  {
		sensorhub_waitForReport(sh, buffer, 100);
		if ((buffer[2] == SENSORHUB_CMD_RESP) &&
		    (buffer[4] == CMD_SAVE_DCD)) {
			if (buffer[7] != 0) {
				rc = SENSORHUB_STATUS_OP_FAILED;
			} else {
                rc = SENSORHUB_STATUS_SUCCESS;
            }
            break;
		}
	}

	return rc;
}

int sensorhub_dcdAutoSave(const sensorhub_t *sh, bool state)
{
	uint8_t buffer[32];
	int rc = 0;

	// Send command to config DCD save
	memset(buffer, 0, sizeof(buffer));
	buffer[0] = 0;     // sequence
	buffer[1] = CMD_CONFIG_DCD_SAVE;
	buffer[2] = state ? 0x00 : 0x01;  // Set P0 parameter

	shhid_setReport(sh, HID_REPORT_TYPE_OUTPUT, SENSORHUB_CMD_REQ,
	buffer, SENSORHUB_CMD_LEN-1);

	return rc;
}
