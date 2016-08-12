rem Copies the library to a new name, in the process, weakens the EDID_LUT symbol, renames the existing data section for it
avr-objcopy --weaken-symbol .rodata.EDID_LUT --weaken-symbol EDID_LUT --rename-section .rodata.EDID_LUT=.rodata.EDID_LUT_OLD %1 %~n1.weakedid.a
