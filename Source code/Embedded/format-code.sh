#!/bin/sh

# Takes each line as being a filename, turns it into a null-delimited string
# and hands it to xargs -0 to pass to clang-format efficiently (because running
# clang-format once for each file is very slow)
run_clang_format_on_input_lines() {
    (while read fn; do
        printf "%s\0" "$fn"
    done) | xargs -0 clang-format -style=file -i
}

(
cd $(dirname $0)/src

# root src directory
ls *.c *.h | run_clang_format_on_input_lines

(
    # config directory, excluding conf_ files
    cd config
    ls *.h | grep -v "^conf_" | run_clang_format_on_input_lines
)

(
    # DeviceDrivers directory (not recursive), excluding the SCD file for the Bosch tracker
    cd DeviceDrivers
    ls *.h *.c | grep -v "^SCD-Bosch-BNO070" | run_clang_format_on_input_lines
)

# The Variants directories
find Variants -name "*.h" | run_clang_format_on_input_lines

)
