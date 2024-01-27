#!/bin/bash

Blu='\e[0;34m';
BBlu='\e[1;34m';
Pur='\e[0;35m';
Reset='\e[0m';

# Path where this script resides in
cur_path=$(dirname $(readlink -f $0))

# source path
#src_path=$cur_path/../../gstreamer_regressions/gst_src
src_path=/mnt/gst_src

usage () {
	echo -e $BBlu
	echo " Usage: ./gst_run.sh -t <type> -l <listfile> -m <mode>"
	echo "     -t or --type         : dynamic"
	echo "     -l or --list-name    : listfile name along with path"
	echo "     -s or --ste          : Single Test Execution <optional>"
	echo "     -m or --mode         : single/multiple/all instances <optional>"
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

############################################################################
#
# Name:         process_params
#
# Argument:     List file
#
# Description:  This function will parse list file & executes gstreamer pipeline
#               for all usecase.
#
# Return Value: None
#
############################################################################
process_params () {

	file_name=`eval basename $LIST_FILE`

	if [ $TYPE == "dynamic" ]; then
		test_type="Encoder Dynamic Feature testcases"
	fi

	output_sheet=$(echo $file_name | awk -F"_" '{print $1}' | tr -d " " | tr -d "\t")
	output_sheet="${output_sheet}_tests.xls"                                     

	echo -e "\n\n\n\n\n\t\t$test_type" >> $output_sheet

	echo -e "S.No\tTestcase\tType\tSource\tBitstream\tResolution&Fps\tCodec\tTarget Bitrate\tMax Bitrate\tControl Rate\tGOP Mode\tGOP Length\tB-frames\tFormat\tLatency Mode\tProfile\tQP Mode\tNum Slice\tNum buff\tPrefetch Buffer\tMode\tOverride\tQuality\tCommand\tResult\tComments" >> $output_sheet
	i=1

	for tc in $run_list; do

		size=${#single_test_execute}

		NUM=$(echo $tc | awk -F"," '{print $1}' | tr -d " " | tr -d "\t")
		TC_NUM=$(echo $tc | awk -F"," '{print $2}' | tr -d " " | tr -d "\t")
		TYPE=$(echo $tc | awk -F"," '{print $3}' | tr -d " " | tr -d "\t")
		SOURCE=$(echo $tc | awk -F"," '{print $4}' | tr -d " " | tr -d "\t")
		BITSTREAM=$(echo $tc | awk -F"," '{print $5}' | tr -d " " | tr -d "\t")
		RESOLUTION=$(echo $tc | awk -F"," '{print $6}' | tr -d " " | tr -d "\t")
		CODEC_TYPE=$(echo $tc | awk -F"," '{print $7}' | tr -d " " | tr -d "\t")
		TARGET_BITRATE=$(echo $tc | awk -F"," '{print $8}' | tr -d " " | tr -d "\t")
		MAX_BITRATE=$(echo $tc | awk -F"," '{print $9}' | tr -d " " | tr -d "\t")
		CTRL_RATE=$(echo $tc | awk -F"," '{print $10}' | tr -d " " | tr -d "\t")
		GOP_MODE=$(echo $tc | awk -F"," '{print $11}' | tr -d " " | tr -d "\t")
		GOP_LENGTH=$(echo $tc | awk -F"," '{print $12}' | tr -d " " | tr -d "\t")
		B_FRAMES=$(echo $tc | awk -F"," '{print $13}' | tr -d " " | tr -d "\t")
		FORMAT=$(echo $tc | awk -F"," '{print $14}' | tr -d " " | tr -d "\t")
		LATENCY_MODE=$(echo $tc | awk -F"," '{print $15}' | tr -d " " | tr -d "\t")
		PROFILE=$(echo $tc | awk -F"," '{print $16}' | tr -d " " | tr -d "\t")
		QP_MODE=$(echo $tc | awk -F"," '{print $17}' | tr -d " " | tr -d "\t")
		NUM_SLICE=$(echo $tc | awk -F"," '{print $18}' | tr -d " " | tr -d "\t")
		NUM_BUFF=$(echo $tc | awk -F"," '{print $19}' | tr -d " " | tr -d "\t")
		PREF_BUFF=$(echo $tc | awk -F"," '{print $20}' | tr -d " " | tr -d "\t")
		MODE=$(echo $tc | awk -F"," '{print $21}' | tr -d " " | tr -d "\t")
		OVERRIDE=$(echo $tc | awk -F"," '{print $22}' | tr -d " " | tr -d "\t")
		QUALITY=$(echo $tc | awk -F"," '{print $23}' | tr -d " " | tr -d "\t")

		if [ $size -ne 0 -a "$single_test_execute" != "$TC_NUM" ]; then
			continue
		fi

		RESULT="PASS"
		FAIL_REASON=""                                                                                                                       

		if [ "$RESOLUTION" == "1080p30" ]; then
			WIDTH=1920
			HEIGHT=1080
			FPS=30
		elif [ "$RESOLUTION" == "1080p60" ]; then
			WIDTH=1920
			HEIGHT=1080
			FPS=60
		elif [ "$RESOLUTION" == "4kp30" ]; then
			WIDTH=3840
			HEIGHT=2160
			FPS=30
		elif [ "$RESOLUTION" == "4kp60" ]; then
			WIDTH=3840
			HEIGHT=2160
			FPS=60
		else
			echo "Wrong resolution."
			exit -1
		fi

		op_path="$op_dir/$TC_NUM"
		if [ -d "$op_path" ]; then
			rm -rf $op_path
		fi
		mkdir -p $op_path

		op_file="$op_path/$BITSTREAM.$CODEC_TYPE"
		op_log_file=$op_path/output.log

		PIPELINE=""

		if [ "NA" == "$B_FRAMES" ]; then
			B_FRAMES_NO=""
		else
			B_FRAMES_NO="-b $B_FRAMES"
		fi

		echo "######################### $TC_NUM #################################"		
		if [ "$MODE" == "roi" ]; then
			echo "################################# APP COMMAND ROI #################################"
			echo "gst-roi-feature-exe -i $src_path/$BITSTREAM -w $WIDTH -h $HEIGHT -f $FPS -x $FORMAT -e $CODEC_TYPE -c $CTRL_RATE -g $GOP_MODE -l $GOP_LENGTH -r $TARGET_BITRATE -m $MAX_BITRATE $B_FRAMES_NO -L $OVERRIDE -q $QUALITY -o $op_file" | tee -a $op_log_file

			PIPELINE="gst-roi-feature-exe -i $src_path/$BITSTREAM -w $WIDTH -h $HEIGHT -f $FPS -x $FORMAT -e $CODEC_TYPE -c $CTRL_RATE -g $GOP_MODE -l $GOP_LENGTH -r $TARGET_BITRATE -m $MAX_BITRATE $B_FRAMES_NO -L $OVERRIDE -q $QUALITY -o $op_file"

			############################################# Excel Sheet Output #############################################
			echo -ne "$NUM\t$TC_NUM\t$TYPE\t$SOURCE\t$BITSTREAM\t$RESOLUTION_STREAM\t$CODEC_TYPE\t$TARGET_BITRATE\t$MAX_BITRATE\t$CTRL_RATE\t$GOP_MODE\t$GOP_LENGTH\t$B_FRAMES\t$FORMAT\t$LATENCY_MODE\t$PROFILE\t$QP_MODE\t$NUM_SLICE\t$NUM_BUFF\t$PREF_BUFF\t$MODE\t$OVERRIDE\t$QUALITY\t$PIPELINE" >> $output_sheet
			##############################################################################################################

			echo "###############################################################################"

			gst-roi-feature-exe -i $src_path/$BITSTREAM -w $WIDTH -h $HEIGHT -f $FPS -x $FORMAT -e $CODEC_TYPE -c $CTRL_RATE -g $GOP_MODE -l $GOP_LENGTH -r $TARGET_BITRATE -m $MAX_BITRATE $B_FRAMES_NO -L $OVERRIDE -q $QUALITY -o $op_file 2>&1 | tee -a $op_log_file

		else
			############################################# Excel Sheet Output #############################################
			#echo -ne "$NUM\t$TC_NUM\t$TYPE\t$SOURCE\t$BITSTREAM\t$RESOLUTION_STREAM\t$CODEC_TYPE\t$TARGET_BITRATE\t$MAX_BITRATE\t$CTRL_RATE\t$GOP_MODE\t$GOP_LENGTH\t$B_FRAMES\t$FORMAT\t$LATENCY_MODE\t$PROFILE\t$QP_MODE\t$NUM_SLICE\t$NUM_BUFF\t$PREF_BUFF\t$MODE\t$OVERRIDE\t$QUALITY" >> $output_sheet
			##############################################################################################################

			RUN_COMMAND="gst-dynamic-rgr-exe filesrc $src_path/$BITSTREAM resolution $RESOLUTION format $FORMAT"

			#Set Encoder parameters
			RUN_COMMAND+=" enc $CODEC_TYPE"

			if [ "$PROFILE" != "NA" ]; then
				RUN_COMMAND+=" profile $PROFILE"
			fi

			RUN_COMMAND+=" control-rate $CTRL_RATE bitrate $TARGET_BITRATE"
			if [ "$CTRL_RATE" == "variable" ]; then
				if [ "$RESOLUTION" == "4kp60" ]; then
					MAX_BITRATE=$TARGET_BITRATE
				else
					MAX_BITRATE=`expr $TARGET_BITRATE \* 5 / 4`
				fi
				RUN_COMMAND+=" max-bitrate $MAX_BITRATE"
			fi

			if [ "$GOP_LENGTH" != "NA" ]; then
				RUN_COMMAND+=" gop-len $GOP_LENGTH"
			fi

			if [ "$B_FRAMES" != "NA" ]; then
				RUN_COMMAND+=" b-frames $B_FRAMES"
			fi

			RUN_COMMAND+=" gop-mode $GOP_MODE"

			if [ "$NUM_SLICE" != "NA" ]; then
				RUN_COMMAND+=" slices $NUM_SLICE"
			fi

			if [ "$PREF_BUFF" == "TRUE" ]; then
				RUN_COMMAND+=" prefetch-buffer 1"
			else
				RUN_COMMAND+=" prefetch-buffer 0"
			fi

			RUN_COMMAND+=" latency-mode $LATENCY_MODE"

			if [ "$MODE" == "bitrate" ]; then
				RUN_COMMAND+=" dy-feature BR"
			elif [ "$MODE" == "gop" ]; then
				RUN_COMMAND+=" dy-feature GOP"
			fi

			RUN_COMMAND+=" sink filesink op_file $op_file"

			############################################# Excel Sheet Output #############################################
			echo -ne "$NUM\t$TC_NUM\t$TYPE\t$SOURCE\t$BITSTREAM\t$RESOLUTION_STREAM\t$CODEC_TYPE\t$TARGET_BITRATE\t$MAX_BITRATE\t$CTRL_RATE\t$GOP_MODE\t$GOP_LENGTH\t$B_FRAMES\t$FORMAT\t$LATENCY_MODE\t$PROFILE\t$QP_MODE\t$NUM_SLICE\t$NUM_BUFF\t$PREF_BUFF\t$MODE\t$OVERRIDE\t$QUALITY\t$RUN_COMMAND" >> $output_sheet
			##############################################################################################################
			echo "################################# APP COMMAND #################################"
			#echo "\t$RUN_COMMAND" >> $output_sheet
			echo "$RUN_COMMAND" 2>&1 | tee -a $op_log_file
			eval $RUN_COMMAND 2>&1 | tee -a $op_log_file
			echo "###############################################################################"
		fi
		
		#################### Check for errors ####################
		grep -iE 'Segmentation fault' $op_log_file
		if [ $? -eq 0 ]; then
			RESULT="FAIL"
			FAIL_REASON="segmentation fault"
		fi
		grep -iE 'ERROR' $op_log_file
		if [ $? -eq 0 ]; then
			RESULT="FAIL"
			FAIL_REASON="Error in pipeline playing."
		fi
		grep -iE 'erroneous pipeline' $op_log_file
		if [ $? -eq 0 ]; then
			RESULT="FAIL"
			FAIL_REASON="Wrong element in pipeline."
		fi
		grep -iE 'Assertion|failed' $op_log_file
		if [ $? -eq 0 ]; then
			RESULT="FAIL"
			FAIL_REASON="Assertion '0' failed in pipeline."
		fi
		grep -iE 'Failed to create Encoder' $op_log_file
		if [ $? -eq 0 ]; then
			RESULT="FAIL"
			FAIL_REASON="Encoder Resource Error."
		fi
		grep -iE 'Cannot allocate memory' $op_log_file
		if [ $? -eq 0 ]; then
			RESULT="FAIL"
			FAIL_REASON="CMA allocation Error."
		fi

		echo -e "\t$RESULT\t$FAIL_REASON" >> $output_sheet
		
		if [ $size -ne 0 ]; then
        	        echo ":::::::: Test Complete $single_test_execute :::::::::"
	        fi
		
	done
}

# Main Starts From Here
TYPE=""
LIST_FILE=""
single_test_execute=""
MODE="all"

if [ $# -eq 0 ]; then
	usage
fi

# Command Line Argument Parsing
args=$(getopt -o "t:l:h:s:m:" --long "type:,list-file:,help:,ste:,mode:" -- "$@")

[ $? -ne 0 ] && usage && exit -1

eval set -- "${args}"

while true; do
	case $1 in
		-h | --help)
			usage; exit 0;
			shift;
			;;
		-t | --type)
			TYPE=$2;
			if [ "$TYPE" != "dynamic" ]; then
				ErrorMsg "Invalid test type."
			fi
			shift; shift;
			;;
		-l | --list-file)
			LIST_FILE=$2;
			if [ -z $LIST_FILE ]; then
				ErrorMsg "List name should not be empty"
			fi
			shift; shift;
			;;
		-s | --ste)
			single_test_execute=$2;
			if [ -z $single_test_execute ]; then
				ErrorMsg "testcase name must be provided"
			fi
			shift; shift;
			;;
		-m | --mode)
			MODE=$2;
			if [ ! -z "$MODE" ]; then
				if [ "$MODE" != "single" -a "$MODE" != "multiple" -a "$MODE" != "all" ]; then
					ErrorMsg "Invalid mode type."
				fi
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

# Check for the list file exist or not
if [ ! -f $LIST_FILE ]; then
	ErrorMsg "List File not found."
fi
lst_file_name=$(basename "$LIST_FILE")
lst_name="${lst_file_name%.*}"
op_dir=$cur_path/../output/"$lst_name"
mkdir -p $op_dir
lst_run=$(readlink -f $LIST_FILE)
source $lst_run

# Execute function to create the encoded output file
if [ "$TYPE" == "dynamic" ]; then
	process_params
else
	# Apply list file to get list of cfg files in run_lists
	killall -9 gst-launch-1.0
	source $lst_run
fi
