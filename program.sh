#!/bin/bash
avrdude	 -pm8 -cdragon_isp -Pusb -b1200 -B500 -i50 -u -Uflash:w:Release/binclock.hex:a -Ulfuse:w:0xc9:m -Uhfuse:w:0xd9:m

