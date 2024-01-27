#!/bin/bash

Blu='\e[0;34m';
BBlu='\e[1;34m';
Pur='\e[0;35m';
Reset='\e[0m';

# Path where this script resides in
cur_path=$(dirname $(readlink -f $0))
split_input_flag=0
Ending_num_Buff=0
mins=0
Delta_For_NumBuff_60FPS=120
Delta_For_NumBuff_30FPS=60
# source path
num_Buff_delta_pos=0
num_Buff_delta_neg=0
src_path=/mnt/gst_src
max_picture_size=275
plane_id=39

usage () {
	echo -e $BBlu
	echo " Usage: ./gst_run.sh -t <type> -l <listfile> -m <mode>"
	echo "     -t or --type         : decode/encode_fs/capture_enc_dec/transcode/multistream/low_latency/file_playback/live_playback/maxbitrate"
	echo "     -l or --list-name    : listfile name along with path"
	echo "     -s or --ste          : Single Test Execution <optional>"
	echo "     -m or --mode         : io-mode-4/io-mode-5 <optional> <By default io-mode=4 is selected>"
	echo "     -i or --mode         : split-input <optional> (True/False) <By default split-input=False>"
	echo -e $Reset

	exit -1
}
usage
