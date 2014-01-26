#!/bin/bash
# ##############################
# program binclock via avrdude

REVISION=a
PROGRAMMER=dragon_isp
PORT=usb
VERBOSE=false

while getopts ":c:P:r:hv" opt; do

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
                   echo "  -v              - verbose mode."
		   exit 0
		   ;;   
                v) VERBOSE=1
                   ;;
		\?) echo "Unknown Argument $OPT"
			exit 1
			;;
		 :) echo "Argument needs Parameter"
			exit 1
			;;

	esac
			
done

PARAM="-pm8 -c$PROGRAMMER -P$PORT -b1200 -B50 -i50 -u -Uflash:w:bin/revision-$REVISION.hex:a -Ulfuse:w:0xc9:m -Uhfuse:w:0xd9:m "
if [ $VERBOSE != 0 ]
then
 PARAM="$PARAM -vvv"
fi

avrdude $PARAM

