#!/bin/bash
# Copyright 2019, Collabora Ltd.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


set -e
ROOT="$(cd $(dirname $0) && pwd)"
for variant in HDK_20 HDK_20_SVR HDK_OLED HDK_Sharp_LCD; do
    (
        cd "$(dirname $0)/src/Variants/$variant"
        "$ROOT/vendor/cros-chameleon/json2edid.py" edid.json edid.bin
        (
            echo "/* THIS IS A GENERATED FILE - DO NOT EDIT */"
            echo "/* Edit the edid.json file and run generate-edid.sh instead. */"
            echo -n "static const "
            xxd -i edid.bin
        ) | sed -r 's/unsigned int edid_bin_len = ([0-9]+);/#define EDID_LEN \1/' > edid.h
    )
done
