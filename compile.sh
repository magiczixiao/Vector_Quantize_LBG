#!/bin/bash

gcc LBG_VQ.c -o LBG_VQ -std=c99 -lm
gcc decode_VQ.c -o Decode_VQ -std=c99 -lm
#gcc raw2bmp.c -o raw2bmp -std=c99 -lm
#gcc raw2bmp-2.c -o raw2bmp-2 -std=c99 -lm
#gcc bmp2raw.c -o bmp2raw -std=c99 -lm