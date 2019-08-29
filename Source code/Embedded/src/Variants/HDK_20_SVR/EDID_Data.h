/*
 * Copyright 2016 OSVR and contributors.
 * Copyright 2016 Dennis Yeh.
 * Copyright 2016 Sensics, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef INCLUDED_EDID_Data_h_GUID_F2172D68_7DD2_46A7_D06B_38CF0AACB921
#define INCLUDED_EDID_Data_h_GUID_F2172D68_7DD2_46A7_D06B_38CF0AACB921

#include <stdint.h>

// info for #?f
const char svrEdidInfoString[] = "SVR1019, EDID spec v1.4, HMD extension flag, with updated 90Hz timings";

#if 0
// do not enforce the core key pass system - it's "accepted" by default
static uint8_t core_key_pass = 1;
// no matter what you enter if you choose to enter a key, it will be accepted as well.
#define CORE_KEY_PASS_FAIL_VALUE 1
#define CORE_KEY_PASS_SUCCESS_VALUE 1
#else
// enforce the core key pass system
static uint8_t core_key_pass = 0;

#define CORE_KEY_PASS_FAIL_VALUE 0
#define CORE_KEY_PASS_SUCCESS_VALUE 1
#endif

/// SVR1019
#include "edid.h"

#endif  // INCLUDED_EDID_Data_h_GUID_F2172D68_7DD2_46A7_D06B_38CF0AACB921
