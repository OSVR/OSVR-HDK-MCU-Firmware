#include "VariantOptions.h"

// Guard since Atmel Studio doesn't allow conditional compilation of files in just some build configurations
#ifdef SVR_IS_HDK_20_SVR
const char svrEdidInfoString[] = "SVR1019, based on 1.01, all dims 26x15 (r3)";
#endif

/* svr1020.edid.bin swapped in as the contents of section .rodata.EDID_LUT in libhdk20.1.01.a to produce libhdk20.1.01.svr1020.a */
