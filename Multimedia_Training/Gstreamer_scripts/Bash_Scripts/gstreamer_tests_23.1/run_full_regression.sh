#!/bin/bash

Blu='\e[0;34m';
BBlu='\e[1;34m';
Pur='\e[0;35m';
Reset='\e[0m';

# Path where this script resides in
cur_path=$(dirname $(readlink -f $0))

usage () {
	echo -e $BBlu
	echo " Usage: ./run_full_regression.sh -t <type> -m <mode>"
	echo "     -t or --type         : all/without_capture (DEF --> all)"
	echo -e $Reset

	exit -1
}

# This function will display error message
ErrorMsg () {
	echo -e $BRed
	echo "$1"
	echo -e $Reset
	usage
}

# Main Starts From Here
TYPE="all"
MODE="all"

# Command Line Argument Parsing
args=$(getopt -o "t:m:h" --long "type:mode:,help" -- "$@")

[ $? -ne 0 ] && usage && exit -1

eval set -- "${args}"

while true; do
	case $1 in
		-h | --help)
			usage; exit 0;
			shift;
			;;
		-t | --type)
			if [ "$2" != "all" -a "$2" != "without_capture" ]; then
				echo "Invalid regression type."	
				echo "Using Default --> all."
			else
				TYPE=$2
			fi
			shift; shift;
			;;
		-m | --mode)
			if [ "$2" != "single" -a "$2" != "multiple" -a "$2" != "all" ]; then
				echo "Invalid mode type."
				echo "Using Default --> all."
			else
				MODE=$2
			fi
			shift; shift;
			;;
		--)
			shift; break;
			;;
		*)
			usage; exit -1;
	esac
done

if [ -d $cur_path/test_results ]; then
	rm -rf $cur_path/test_results/*
else
	mkdir $cur_path/test_results
fi

if [ "$TYPE" == "all" ]; then

	cd $cur_path/
	if [ -d "output" ]; then
		rm -rf output
	fi
	cd run/

	if [ -f "l0_test.xls" ]; then
		rm *.xls
	fi

	echo "path::: $cur_path"
	./gst_run.sh -t decode -l ../list/l0_decode.lst
	./gst_run.sh -t encode -l ../list/l0_encode.lst
	./gst_run.sh -t transcode -l ../list/l0_transcode.lst
	./gst_run.sh -t multistream -l ../list/l0_multistream.lst

	mv *.xls $cur_path/test_results/.
fi

if [ "$TYPE" == "decode" -o "$TYPE" == "all" ]; then

	cd $cur_path/
	if [ -d "output" ]; then
		rm -rf output
	fi
	cd run/

	if [ -f "l0_tests.xls" ]; then
		rm *.xls
	fi

        ./gst_run.sh -t decode -l ../list/l0_decode.lst                                                    
	./gst_run.sh -t encode -l ../list/l0_encode.lst                        
        ./gst_run.sh -t transcode -l ../list/l0_transcode.lst                         
        ./gst_run.sh -t multistream -l ../list/l0_multistream.lst              
        ./gst_run.sh -t low_latency -l ../list/l0_low_latency.lst
	./gst_run.sh -t capture -l ../list/l0_capture.lst

	mv *.xls $cur_path/test_results/.
fi
