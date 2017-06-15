// Copyright 2017 Hillcrest Laboratories, Inc.
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at  http://www.apache.org/licenses/LICENSE-2.0. 
// Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an 
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  
// See the License for the specific language governing permissions and limitations under the License


#ifndef SENSORHUB_HID_H
#define SENSORHUB_HID_H

#ifdef __AVR
#define __packed
#endif

#include "sensorhub_platform.h"
#include "sensorhub.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BNO070_MAX_INPUT_REPORT_LEN 18
#define BNO070_REGISTER_HID_DESCRIPTOR    0x01
#define BNO070_REGISTER_REPORT_DESCRIPTOR 0x02
#define BNO070_REGISTER_INPUT             0x03
#define BNO070_REGISTER_OUTPUT            0x04
#define BNO070_REGISTER_COMMAND           0x05
#define BNO070_REGISTER_DATA              0x06
#define BNO070_DESC_V1_LEN                30
#define BNO070_DESC_V1_BCD                0x0100

enum sensorhub_ReportId_e {
    SENSORHUB_PRODUCT_ID_REQUEST = 0x80,
    SENSORHUB_PRODUCT_ID_RESPONSE = 0x81,
    SENSORHUB_FRS_WRITE_REQUEST = 0x82,
    SENSORHUB_FRS_WRITE_DATA_REQUEST = 0x83,
    SENSORHUB_FRS_WRITE_RESPONSE = 0x84,
    SENSORHUB_FRS_READ_REQUEST = 0x85,
    SENSORHUB_FRS_READ_RESPONSE = 0x86,
    SENSORHUB_CMD_REQ = 0x87,
    SENSORHUB_CMD_RESP = 0x88,
};
typedef uint8_t sensorhub_ReportId_t;

enum hid_ReportType_e {
    HID_REPORT_TYPE_INPUT = 0x10,
    HID_REPORT_TYPE_OUTPUT = 0x20,
    HID_REPORT_TYPE_FEATURE = 0x30
};
typedef uint8_t sensorhub_ReportType_t;

enum hid_ReportOpcode_e {
    HID_RESET_OPCODE = 0x01,
    HID_GET_REPORT_OPCODE = 0x02,
    HID_SET_REPORT_OPCODE = 0x03,
    HID_GET_IDLE_OPCODE = 0x04,
    HID_SET_IDLE_OPCODE = 0x05,
    HID_GET_PROTOCOL_OPCODE = 0x06,
    HID_SET_PROTOCOL_OPCODE = 0x07,
    HID_SET_POWER_OPCODE = 0x08
};

typedef struct hid_descriptor_s {
	uint16_t wHIDDescLength;
	uint16_t bcdVersion;
	uint16_t wReportDescriptorLength;
	uint16_t wReportDescriptorRegister;
	uint16_t wInputRegister;
	uint16_t wMaxInputLength;
	uint16_t wOutputRegister;
	uint16_t wMaxOutputLength;
	uint16_t wCommandRegister;
	uint16_t wDataRegister;
	uint16_t wVendorID;
	uint16_t wProductID;
	uint16_t wVersionID;
	uint8_t reserved[4];
} __packed hid_descriptor_t;

int shhid_setReport(const sensorhub_t * sh,
                    sensorhub_ReportType_t reportType,
                    sensorhub_ReportId_t reportId,
                    const uint8_t * payload, uint8_t payloadLength);

int shhid_getReport(const sensorhub_t * sh,
                    sensorhub_ReportType_t reportType,
                    sensorhub_ReportId_t reportId,
                    uint8_t * payload, uint8_t payloadLength);

#ifdef __cplusplus
}
#endif
#endif                          // SENSORHUB_HID_H
