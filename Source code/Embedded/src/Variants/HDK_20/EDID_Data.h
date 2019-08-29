/*
 * Copyright 2016 OSVR and contributors.
 * Copyright 2016 Dennis Yeh.
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
const char svrEdidInfoString[] = "AUO1111 - identical to 1.01";

// enforce the core key pass system
static uint8_t core_key_pass = 0;

#define CORE_KEY_PASS_FAIL_VALUE 0
#define CORE_KEY_PASS_SUCCESS_VALUE 1

/// AUO1111
#include "edid.h"

#endif  // INCLUDED_EDID_Data_h_GUID_F2172D68_7DD2_46A7_D06B_38CF0AACB921
