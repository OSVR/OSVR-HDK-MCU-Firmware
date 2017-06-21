// Copyright 2017 Hillcrest Laboratories, Inc.
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at  http://www.apache.org/licenses/LICENSE-2.0. 
// Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an 
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  
// See the License for the specific language governing permissions and limitations under the License


#include "sensorhub_hid.h"
#include "sensorhub.h"
#include <string.h>

#define GET_REPORT_BUFSIZE (64)
#define SET_REPORT_BUFSIZE (64)
#define MAX_INPUT_REPORT_BUFSIZE (64)

int shhid_setReport(const sensorhub_t * sh,
                    sensorhub_ReportType_t reportType,
                    sensorhub_ReportId_t reportId,
                    const uint8_t * payload, uint8_t payloadLength)
{
    uint8_t cmd[32];
    int ix;

    cmd[0] = BNO070_REGISTER_COMMAND;
    cmd[1] = 0;

    if (reportId < 0xf) {
        cmd[2] = reportType | reportId;
        cmd[3] = HID_SET_REPORT_OPCODE;
        ix = 4;
    } else {
        cmd[2] = reportType | 0xf;
        cmd[3] = HID_SET_REPORT_OPCODE;
        cmd[4] = reportId;
        ix = 5;
    }

    cmd[ix++] = BNO070_REGISTER_DATA;
    cmd[ix++] = 0;

    cmd[ix++] = payloadLength + 2;
    cmd[ix++] = 0;

    memcpy(&cmd[ix], payload, payloadLength);
    ix += payloadLength;

    return sensorhub_i2cTransferWithRetry(sh, sh->sensorhubAddress, cmd, ix, NULL, 0);
}

int shhid_getReport(const sensorhub_t * sh,
                    sensorhub_ReportType_t reportType,
                    sensorhub_ReportId_t reportId,
                    uint8_t * payload, uint8_t payloadLength)
{
    uint8_t cmd[7];
    int ix;

    cmd[0] = BNO070_REGISTER_COMMAND;
    cmd[1] = 0;

    if (reportId < 0xf) {
        cmd[2] = reportType | reportId;
        cmd[3] = HID_GET_REPORT_OPCODE;
        ix = 4;
    } else {
        cmd[2] = reportType | 0xf;
        cmd[3] = HID_GET_REPORT_OPCODE;
        cmd[4] = reportId;
        ix = 5;
    }

    cmd[ix++] = BNO070_REGISTER_DATA;
    cmd[ix++] = 0;

    return sensorhub_i2cTransferWithRetry(sh, sh->sensorhubAddress, cmd, ix, payload,
                                          payloadLength);
}
