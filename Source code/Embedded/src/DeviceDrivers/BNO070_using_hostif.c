/*
 * NBO070.c
 *
 * Created: 10/30/2014 3:04:17 PM
 *  Author: Sensics Boger
 */

#include <asf.h>
#include "string.h"

#include "GlobalOptions.h"
#include "my_hardware.h"
#include "Console.h"

#include <ioport.h>
#include "BNO070.h"
#include "util/delay.h"
#include "Boot.h"

/* Set this to 1 to report GRV instead of standard RV */
#define SELECT_GRV 1
/* Define this to enable the calibrated gyro reports and stuff them in the USB reports at offset 10 */
#define REPORT_GYRO 1
#define REPORT_ACC 0
#define REPORT_MAG 0

#ifdef BNO070

#include "sensorhub.h"
extern sensorhub_t sensorhub;

bool BNO070Active=false;
uint8_t BNO070_Report[USB_REPORT_SIZE];
bool TWI_BNO070_PORT_initialized=false; // true if already initialized

sensorhub_ProductID_t BNO070id;
Bool BNO_supports_400Hz=false; // true if firmware is new enough to support higher-rate reads

struct BNO070_Config {
    /* per-sensor configs */
    struct {
        sensorhub_SensorFeature_t rv;
        sensorhub_SensorFeature_t grv;
        sensorhub_SensorFeature_t acc;
        sensorhub_SensorFeature_t gyro;
        sensorhub_SensorFeature_t mag;
    } sensors;

    /* calibration flags */
    int cal_flags;

    /* mag events to watch for before turning off, -1 = unlimited, 0 = off */
    int mag_events_left;
};

static struct BNO070_Config config_;

#ifdef PERFORM_BNO_DFU
    #if 1 // 1.7.0
        #define DFU_MAJOR 1
        #define DFU_MINOR 7
        #define DFU_PATCH 0
        #include "bno-hostif/1000-3251_1.7.0.390_avr.c"
    #else // 1.2.5
        #define DFU_MAJOR 1
        #define DFU_MINOR 2
        #define DFU_PATCH 5
        #include "bno-hostif/1000-3251_1.2.5_avr.c"
    #endif
#endif

#define DEG2RAD (3.1415926535897932384626433832795 / 180.0)

static int32_t round32(float f) {
    return (int32_t)(f + ((f >= 0) ? 0.5 : -0.5));
}

static int32_t degToRadQ28(float deg) {
    return (int32_t) (deg * DEG2RAD * (1ul << 28));
}

#define toFixed32(x, Q) round32(x * (float)(1ul << Q))

static sensorhub_ProductID_t readProductId(void)
{
    sensorhub_ProductID_t id;
    memset(&id, 0, sizeof(id));

    sensorhub.debugPrintf("Requesting product ID...\r\n");
    int rc = sensorhub_getProductID(&sensorhub, &id);
    if (rc != SENSORHUB_STATUS_SUCCESS) {
        sensorhub.debugPrintf("readProductId received error: %d\r\n", rc);
        return id;
    }

    sensorhub.debugPrintf("  Version %d.%d.%d\r\n", id.swVersionMajor, id.swVersionMinor, id.swVersionPatch);
    sensorhub.debugPrintf("  Part number: %d\r\n", id.swPartNumber);
    sensorhub.debugPrintf("  Build number: %d\r\n", id.swBuildNumber);

    return id;
}

static void checkDfu(void) {
#ifdef PERFORM_BNO_DFU
#if 1 // Skip vesrion check and always DFU
    if (1) {
        sensorhub.debugPrintf("Performing DFU . . . \r\n");
#else
    sensorhub_ProductID_t id = readProductId();
    if ((id.swVersionMajor != DFU_MAJOR) ||
        (id.swVersionMinor != DFU_MINOR) ||
        (id.swVersionPatch != DFU_PATCH)) {
        sensorhub.debugPrintf("BNO is not at %d.%d.%d.  Performing DFU . . . \r\n", DFU_MAJOR, DFU_MINOR, DFU_PATCH);
#endif
        int rc = sensorhub_dfu_avr(&sensorhub, &dfuStream);
        if (rc != SENSORHUB_STATUS_SUCCESS) {
            sensorhub.debugPrintf("dfu received error: %d\r\n", rc);
            return;
        }
        sensorhub.debugPrintf("DFU Completed Successfully\r\n");
        // Re-probe:
        sensorhub_probe(&sensorhub);

        // Get the updated version number
        readProductId();
    }
#endif // PERFORM_BNO_DFU
}

// TODO - skip the write FRS if the FRS on the device does not differ
static void configureARVRStabilizationFRS(void) {
    int status;
    int32_t arvrConfig[4] = {
        toFixed32(0.2f,   30), // scaling
        degToRadQ28(7.3f), // maxRotation
        degToRadQ28(90.0f), // maxError
        degToRadQ28(0.0f), // stability
    };

    sensorhub.debugPrintf("Configuring ARVR Stabilization.\r\n");
    status = sensorhub_writeFRS(&sensorhub, SENSORHUB_FRS_ARVR_CONFIG,
                               (uint32_t*)arvrConfig, 4);
    if (status != SENSORHUB_STATUS_SUCCESS) {
        sensorhub.debugPrintf("Write FRS failed: %d", status);
    }

    sensorhub.debugPrintf("Configuring ARVR Game Stabilization.\r\n");
    status = sensorhub_writeFRS(&sensorhub, SENSORHUB_FRS_ARVR_GAME_CONFIG,
                                (uint32_t*)arvrConfig, 4);
    if (status != SENSORHUB_STATUS_SUCCESS) {
        sensorhub.debugPrintf("Write FRS failed: %d", status);
    }
}

uint8_t const scd[] = {
    //#include "bno-hostif/SCD-Bosch-BNO070-8G-lowGyroNoise.c"
    //#include "bno-hostif/SCD-Bosch-BNO070-8G.c"
    #include "bno-hostif/SCD-Bosch-BNO070-sqtsNoise.c"
};

// TODO - skip the write FRS if the FRS on the device does not differ
static void configureScdFrs(void) {
    int status;
    sensorhub.debugPrintf("Configuring SCD.\r\n");
    status = sensorhub_writeFRS(&sensorhub, SENSORHUB_FRS_SCD_ACTIVE,
        (uint32_t const*)scd, sizeof(scd)/sizeof(uint32_t));
    if (status != SENSORHUB_STATUS_SUCCESS) {
        sensorhub.debugPrintf("Write FRS of SCD failed: %d", status);
    }
}

static void clearScdFrs(void) {
    int status;
    sensorhub.debugPrintf("Clearing SCD.\r\n");
    status = sensorhub_writeFRS(&sensorhub, SENSORHUB_FRS_SCD_ACTIVE, NULL, 0);
    if (status != SENSORHUB_STATUS_SUCCESS) {
        sensorhub.debugPrintf("Clear of SCD failed: %d", status);
    }
}

static inline int32_t hz2us(int hz) {
    if (hz == 0) {
        return 0;
    }
    return 1000000 / hz;
}

static void loadDefaultConfig(struct BNO070_Config * cfg) {

    int32_t common_period;
    memset(cfg, 0x00, sizeof(*cfg));

    if (BNO_supports_400Hz) {
        common_period = hz2us(400);
    } else {
        common_period = hz2us(200);
    }

    /* rv and grv are mutually exclusive */
    cfg->sensors.rv.reportInterval = !SELECT_GRV ? common_period : 0;
    cfg->sensors.grv.reportInterval = SELECT_GRV ? common_period : 0;
    /* enable the other reports as needed */
    cfg->sensors.gyro.reportInterval = REPORT_GYRO ? common_period : 0;
    cfg->sensors.acc.reportInterval = REPORT_ACC ? common_period : 0;
    cfg->sensors.mag.reportInterval = REPORT_MAG ? common_period : 0;

    cfg->cal_flags = 0;
}

static void printEvent(const sensorhub_Event_t * event)
{
    switch (event->sensor) {
        case SENSORHUB_ACCELEROMETER:
        {
            sensorhub.debugPrintf("Acc: 0x%04x 0x%04x 0x%04x\r\n",
                event->un.accelerometer.x_16Q8,
                event->un.accelerometer.y_16Q8,
                event->un.accelerometer.z_16Q8);
        }
        break;

        case SENSORHUB_ROTATION_VECTOR:
        {
            sensorhub.debugPrintf("RV: %5d %5d %5d %5d\r\n",
                event->un.rotationVector.i_16Q14,
                event->un.rotationVector.j_16Q14,
                event->un.rotationVector.k_16Q14,
                event->un.rotationVector.real_16Q14);
        }
        break;

        case SENSORHUB_GAME_ROTATION_VECTOR:
        {
            sensorhub.debugPrintf("RV: %5d %5d %5d %5d\r\n",
                event->un.gameRotationVector.i_16Q14,
                event->un.gameRotationVector.j_16Q14,
                event->un.gameRotationVector.k_16Q14,
                event->un.gameRotationVector.real_16Q14);
        }
        break;
    }
}

static void dispatchEvent(const sensorhub_Event_t * event)
{
    switch (event->sensor) {
        case SENSORHUB_ROTATION_VECTOR:
        {

            BNO070_Report[1] = event->sequenceNumber;
            memcpy(&BNO070_Report[2], &event->un.rotationVector.i_16Q14, 8); // copy quaternion data
            udi_hid_generic_send_report_in(BNO070_Report);
        }
        break;

        case SENSORHUB_GAME_ROTATION_VECTOR:
        {
            BNO070_Report[1] = event->sequenceNumber;
            memcpy(&BNO070_Report[2], &event->un.gameRotationVector.i_16Q14, 8); // copy quaternion data
            udi_hid_generic_send_report_in(BNO070_Report);
        }
        break;

        case SENSORHUB_GYROSCOPE_CALIBRATED:
        {
            memcpy(&BNO070_Report[10], &event->un.gyroscope.x_16Q9, 6); // copy gyroscope values
            while (ioport_get_value(Int_BNO070) == 0);  // wait for interrupt to deassert.
        }
        break;
    }
}


static inline int checkError(int status, const char * msg) {
    if (status < 0) {
        sensorhub.debugPrintf("Err %d: %s", status, msg);
    }
    return status;
}

/**
 * Apply settings in activeConfig_ to BNO
 */
static void applyConfig(struct BNO070_Config * cfg) {

    int status;

    status = sensorhub_calEnable(&sensorhub, cfg->cal_flags);
    if (checkError(status, "error setting cal enable flags") < 0) {
        return;
    }

    status = sensorhub_setDynamicFeature(&sensorhub,
        SENSORHUB_ROTATION_VECTOR, &cfg->sensors.rv);
    if (checkError(status, "error setting RV FRS") < 0) {
        return;
    }

    status = sensorhub_setDynamicFeature(&sensorhub,
        SENSORHUB_GAME_ROTATION_VECTOR, &cfg->sensors.grv);
    if (checkError(status, "error setting GRV FRS") < 0) {
        return;
    }

    status = sensorhub_setDynamicFeature(&sensorhub,
        SENSORHUB_ACCELEROMETER, &cfg->sensors.gyro);
    if (checkError(status, "error setting GYRO FRS") < 0) {
        return;
    }

    status = sensorhub_setDynamicFeature(&sensorhub,
        SENSORHUB_GYROSCOPE_CALIBRATED, &cfg->sensors.gyro);
    if (checkError(status, "error setting GYRO FRS") < 0) {
        return;
    }

    status = sensorhub_setDynamicFeature(&sensorhub,
        SENSORHUB_MAGNETIC_FIELD_CALIBRATED, &cfg->sensors.mag);
    if (checkError(status, "error setting MAG FRS") < 0) {
        return;
    }
}

bool init_BNO070(void)
{
    int result;

    // Clear BNO070_Report so we don't send garbage out the USB.
    memset(BNO070_Report, 0, sizeof(BNO070_Report));

    ioport_configure_pin(BNO_070_Reset_Pin,IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH); // reset is active low
    ioport_configure_pin(Side_by_side_A,IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);  // Actually the BootN pin
    ioport_configure_pin(Int_BNO070, IOPORT_DIR_INPUT|IOPORT_MODE_PULLUP);

    if (!TWI_BNO070_PORT_initialized)
    {
        twi_master_options_t opt_BNO070;

        opt_BNO070.speed = BNO_TWI_SPEED;
        opt_BNO070.chip  = BNO070_ADDR;
        result=twi_master_setup(TWI_BNO070_PORT, &opt_BNO070);
        if (result==STATUS_OK)
        {
            TWI_BNO070_PORT_initialized=true;
            //WriteLn("Init success");
        }
        else
            return false;
    }

#ifdef PERFORM_BNO_DFU
    return dfu_BNO070();
#endif

    if (sensorhub_probe(&sensorhub) != SENSORHUB_STATUS_SUCCESS) {
        return false;
    }

    BNO070id = readProductId();
    if ((BNO070id.swVersionMajor*10+BNO070id.swVersionMinor) >= 18) //version>1.8
        BNO_supports_400Hz=true;

    // restore normal setting
    configureARVRStabilizationFRS();
    configureScdFrs();

    // WTF? why
    // Probe again to reboot the hub
    if (sensorhub_probe(&sensorhub) != SENSORHUB_STATUS_SUCCESS) {
        return false;
    }

    loadDefaultConfig(&config_);
    applyConfig(&config_);

    // setup report
#ifdef REPORT_GYRO
    if (BNO_supports_400Hz)
        BNO070_Report[0]=2; // this indicates the version number of the report
    else
        BNO070_Report[0]=1; // this indicates the version number of the report
#else
    BNO070_Report[0]=1; // this indicates the version number of the report
#endif
    BNO070_Report[1]=0; // this indicates the sequence number

    return true;
}

void SimReset_BNO070(void)
{
   // TODO: sensorhub_simReset(&sensorhub);
}

bool Check_BNO070(void)
{
#define MAX_EVENTS_AT_A_TIME 1
    sensorhub_Event_t shEvents[MAX_EVENTS_AT_A_TIME];
    int numEvents = 0;
    int i;
    int rc;

    /* Get the shEvents - we may get 0 */
    rc = sensorhub_poll(&sensorhub, shEvents, MAX_EVENTS_AT_A_TIME, &numEvents);

    if (rc == SENSORHUB_STATUS_HUB_RESET) {
        /* reset event received */
        sensorhub.debugPrintf("Hub reset event received\r\n");
        applyConfig(&config_);
    }

    for (i = 0; i < numEvents; i++) {
        dispatchEvent(&shEvents[i]);
        //printEvent(&shEvents[i]);
    }

    return numEvents > 0;
}

bool Tare_BNO070(void)
{
    // execute tare commands

    // In HID parlance, this is a write to the command register, where the command is set output report. The 07 byte is the axes (P2) and the preceding 00 byte is the tare basis.

    const uint8_t tare_now[] = {0x3f,0x03, 0x05 ,0x00 ,0x2F ,0x03 ,0x87 ,0x06 ,0x00 ,0x0D ,0x00 ,0x00 ,0x03 ,0x00 ,0x07 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00};

    twi_package_t packet_write = {
        .addr[0]      = 5, //regNum,      // TWI slave memory address data
        .addr[1]      = 0, //regNum,      // TWI slave memory address data
        .addr_length  = sizeof (uint16_t),    // TWI slave memory address data size
        .chip         = BNO070_ADDR,      // TWI slave bus address
        .buffer       = (void *)tare_now, // transfer data source buffer
        .length       = sizeof(tare_now)  // transfer data size (bytes)
    };

    if (twi_master_write(TWI_BNO070_PORT, &packet_write)!=STATUS_OK)
        return false;

    const uint8_t persist_tare[] = {0x3f,0x03 ,0x05 ,0x00 ,0x2F ,0x03 ,0x87 ,0x06 ,0x00 ,0x0D ,0x00 ,0x00 ,0x01 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00};

    packet_write.addr[0]      = 5; //regNum,      // TWI slave memory address data
    packet_write.addr[1]      = 0; //regNum,      // TWI slave memory address data
    packet_write.addr_length  = sizeof (uint16_t);    // TWI slave memory address data size
    packet_write.chip         = BNO070_ADDR;      // TWI slave bus address
    packet_write.buffer       = (void *)persist_tare; // transfer data source buffer
    packet_write.length       = sizeof(persist_tare);  // transfer data size (bytes)

    if (twi_master_write(TWI_BNO070_PORT, &packet_write)!=STATUS_OK)
        return false;

    WriteLn("Tare completed");
    return true;
}

bool SetDcdEn_BNO070(uint8_t flags)
{
    // TODO
    return false;
}

bool SaveDcd_BNO070(void)
{
    int status = sensorhub_saveDcd(&sensorhub);
    return (status == SENSORHUB_STATUS_SUCCESS);
}

bool MagOn_BNO070(uint8_t samples)
{
    // TODO
    return false;
}

uint8_t MagStatus_BNO070(void)  // 0 - Unreliable, 1 - Low, 2 - Medium, 3 - High Accuracy.
{
    // TODO
    return 99;
}

void GetStats_BNO070(BNO070_Stats_t *stats)
{
    stats->resets = sensorhub_resets;
    stats->events = sensorhub_events;
    stats->empty_events = sensorhub_empty_events;
}

bool dfu_BNO070(void) {
    checkDfu();
    PrepareForSoftwareUpgrade();
    return true;
}


#endif
