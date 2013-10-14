#!/bin/bash
# ##############################
# program binclock via avrdude

REVISION=a
PROGRAMMER=dragon_isp
PORT=usb

while getopts ":c:P:r:h" opt; do

	case $opt in

		c) PROGRAMMER=$OPTARG
			echo "choosing programmer $PROGRAMMER"
			;;
		P) PORT=$OPTARG
			echo "choosing port $PORT"
			;;
		r) shopt -s nocasematch 
		   if [ $OPTARG = 'a' ]
				then REVISION=a
				else REVISION=b
		   fi
		   shopt -n nocasematch
		   ;;	 
		h) echo "program.sh [OPTIONS]"
		   echo "  OPTIONS "
		   echo "  -h              - shows this help."
		   echo "  -c [programmer] - avr programmer. Standard is dragon_isp."
		   echo "  -P [port]       - programming port. Standard is usb."
		   echo "  -r [revision]   - revision A or B. Standard is a."
		   exit 0
		   ;;   
		\?) echo "Unknown Argument $OPT"
			exit 1
			;;
		 :) echo "Argument needs Parameter"
			exit 1
			;;

	esac
			
done

avrdude	 -pm8 -c$PROGRAMMER -P$PORT -b1200 -B500 -i50 -u -Uflash:w:bin/revision-$REVISION.hex:a -Ulfuse:w:0xc9:m -Uhfuse:w:0xd9:m

