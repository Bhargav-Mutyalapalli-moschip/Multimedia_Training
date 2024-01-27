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

# This function will display error message
ErrorMsg () {
	echo -e $BRed
	echo "$1"
	echo -e $Reset
	usage
}

parse_param()
{
	tc=$1
	declare -a FILE_SRC_ARR
	declare -a DEC_PARSE_ARR
	declare -a OMX_DEC_ARR
	declare -a OMX_ENC_ARR
	#declare -a CODEC_PARSE
	declare -a VIDEO_X
	declare -a VIDEO_X_RAW_ARR

	export CODEC_PARSE=" h265parse "
	export GST_COMMAND="gst-launch-1.0"
	export FILE_SRC="filesrc"
	export HDMI_SRC=""
	export TPG_SRC=""
	export VIDEO_PARSE="videoparse"
	export FORMAT_AVC_PARSE="h264parse"
	export FORMAT_HEVC_PARSE="h265parse"

	export OMXH265DEC="omxh265dec"
	export OMXH264DEC="omxh264dec"
        export OMXH265ENC="omxh265enc"
        export OMXH264ENC="omxh264enc"

	export OMXHENC="omxhenc"
	export OMXHDEC="omxhdec"

	export FILE_SINK="filesink"
	export VIDEO_X_H264="video/x-h264"
	export VIDEO_X_H265="video/x-h265"
	export VIDEO_X_H="video/x-H26"
	export VIDEO_X_RAW="video/x-raw,"
	export VIDEO_CONVERT="videoconvert "
	export QUEUE="queue"

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
	SLICE_SIZE=$(echo $tc | awk -F"," '{print $22}' | tr -d " " | tr -d "\t")
	DPEND_SLICE=$(echo $tc | awk -F"," '{print $23}' | tr -d " " | tr -d "\t")
	CPB_SIZE=$(echo $tc | awk -F"," '{print $24}' | tr -d " " | tr -d "\t")
	INI_DELAY=$(echo $tc | awk -F"," '{print $25}' | tr -d " " | tr -d "\t")
	MIN_QP=$(echo $tc | awk -F"," '{print $26}' | tr -d " " | tr -d "\t")
	MAX_QP=$(echo $tc | awk -F"," '{print $27}' | tr -d " " | tr -d "\t")
        QUANT_IFRAME=$(echo $tc | awk -F"," '{print $28}' | tr -d " " | tr -d "\t")                                                            
        QUANT_PFRAME=$(echo $tc | awk -F"," '{print $29}' | tr -d " " | tr -d "\t")                                                            
        QUANT_BFRAME=$(echo $tc | awk -F"," '{print $30}' | tr -d " " | tr -d "\t")
        FILL_DATA=$(echo $tc | awk -F"," '{print $31}' | tr -d " " | tr -d "\t") 
        LOW_BW=$(echo $tc | awk -F"," '{print $32}' | tr -d " " | tr -d "\t") 
        PERIODICITY_IDR=$(echo $tc | awk -F"," '{print $33}' | tr -d " " | tr -d "\t") 
        GDR_MODE=$(echo $tc | awk -F"," '{print $34}' | tr -d " " | tr -d "\t")
        ENTROPY_MODE=$(echo $tc | awk -F"," '{print $35}' | tr -d " " | tr -d "\t")
        CONST_INTRA_PREDICTION=$(echo $tc | awk -F"," '{print $36}' | tr -d " " | tr -d "\t")
        LOOP_FILTER_MODE=$(echo $tc | awk -F"," '{print $37}' | tr -d " " | tr -d "\t")	
        SCALING_LIST=$(echo $tc | awk -F"," '{print $38}' | tr -d " " | tr -d "\t")                                                             
        ASPECT_RATIO=$(echo $tc | awk -F"," '{print $39}' | tr -d " " | tr -d "\t")  
	LEVEL=$(echo $tc | awk -F"," '{print $40}' | tr -d " " | tr -d "\t")
	TIER=$(echo $tc | awk -F"," '{print $41}' | tr -d " " | tr -d "\t")
	ENTROPY_BUFF=$(echo $tc | awk -F"," '{print $42}' | tr -d " " | tr -d "\t")
	MAX_SIZE_BYTES=0

	if [ "$MAX_BITRATE" == "NA" ]; then
		TBR="0"
		if [ "$CTRL_RATE" == "variable" ]; then
			TBR=`expr $TARGET_BITRATE / 4`
			MAX_BITRATE=`expr $TBR + $TARGET_BITRATE + 2000`
		else
			MAX_BITRATE="NA"
		fi
	fi

	if [ "$TYPE" == "multistream" ]; then
		STREAMS=$(echo $RESOLUTION | awk -F"-" '{print $1}' | tr -d " " | tr -d "\t")
	        RESOLUTION=$(echo $RESOLUTION | awk -F"-" '{print $2}' | tr -d " " | tr -d "\t")                                                          
	fi

	if [ "$RESOLUTION" == "1080p30" ]; then                                                                                                
                WIDTH=1920                                                                                      
                HEIGHT=1080                                                                                                                    
                FRAME_RATE="30/1"
		BUS_ID="a0070000.v_mix"
        elif [ "$RESOLUTION" == "1080p60" ]; then                                                                                              
                WIDTH=1920                                                                                                                     
                HEIGHT=1080                                                                                                                    
                FRAME_RATE="60/1"
		BUS_ID="a0070000.v_mix"                                                                                                                         
        elif [ "$RESOLUTION" == "4kp30" ]; then                                                                                                
                WIDTH=3840                                                                                                                     
                HEIGHT=2160                                                                                                                    
                FRAME_RATE="30/1"
		BUS_ID="a0070000.v_mix"                                                                                                      
        elif [ "$RESOLUTION" == "4kp60" ]; then                                                                                      
                WIDTH=3840                                                                                                           
                HEIGHT=2160                                                                                                                    
                FRAME_RATE="60/1"
		BUS_ID="a0070000.v_mix" 
        else                                                                                                                                   
                echo "Wrong resolution."
		echo "res: $RESOLUTION"                                                                                                       
                exit -1                                                                                                                        
        fi

	FILE_SRC_ARR=( "location"="$src_path/$BITSTREAM" )
	export FILE_SRC=$(update_element_properties $FILE_SRC ${FILE_SRC_ARR[@]})

	ALIGNMENT="au"
	LOW_LATENCY="0"
	if [ "$LATENCY_MODE" == "normal" ]; then
		ALIGNMENT="au"
		LOW_LATENCY="0"
	elif [ "$LATENCY_MODE" == "reduced-latency" ]; then
		ALIGNMENT="au"
		LOW_LATENCY="1"
	elif [ "$LATENCY_MODE" == "low-latency" ]; then
		ALIGNMENT="nal"
		LOW_LATENCY="1"
	fi

	if [ $TYPE == "maxbitrate" ]; then
		FILL_DATA="false"
	fi

## Without max-picture-size
	OMX_ENC_ARR=( "target-bitrate"=$TARGET_BITRATE "max-bitrate"=$MAX_BITRATE "control-rate"=$CTRL_RATE "num-slices"=$NUM_SLICE "gop-mode"=$GOP_MODE "gop-length"=$GOP_LENGTH "b-frames"=$B_FRAMES "qp-mode"=$QP_MODE "prefetch-buffer"=$PREF_BUFF "slice-size"=$SLICE_SIZE "dependent-slice"=$DPEND_SLICE "cpb-size"=$CPB_SIZE "initial-delay"=$INI_DELAY "min-qp"=$MIN_QP "max-qp"=$MAX_QP "quant-i-frames"=$QUANT_IFRAME "quant-p-frames"=$QUANT_PFRAME "quant-b-frames"=$QUANT_BFRAME "filler-data"=$FILL_DATA "low-bandwidth"=$LOW_BW "periodicity-idr"=$PERIODICITY_IDR "gdr-mode"=$GDR_MODE "entropy-mode"=$ENTROPY_MODE "constrained-intra-prediction"=$CONST_INTRA_PREDICTION "loop-filter-mode"=$LOOP_FILTER_MODE "scaling-list"=$SCALING_LIST "aspect-ratio"=$ASPECT_RATIO )
## With max-picture-size
#	OMX_ENC_ARR=( "target-bitrate"=$TARGET_BITRATE "max-bitrate"=$MAX_BITRATE "control-rate"=$CTRL_RATE "num-slices"=$NUM_SLICE "gop-mode"=$GOP_MODE "gop-length"=$GOP_LENGTH "b-frames"=$B_FRAMES "qp-mode"=$QP_MODE "prefetch-buffer"=$PREF_BUFF "slice-size"=$SLICE_SIZE "dependent-slice"=$DPEND_SLICE "cpb-size"=$CPB_SIZE "initial-delay"=$INI_DELAY "min-qp"=$MIN_QP "max-qp"=$MAX_QP "quant-i-frames"=$QUANT_IFRAME "quant-p-frames"=$QUANT_PFRAME "quant-b-frames"=$QUANT_BFRAME "filler-data"=$FILL_DATA "low-bandwidth"=$LOW_BW "periodicity-idr"=$PERIODICITY_IDR "gdr-mode"=$GDR_MODE "entropy-mode"=$ENTROPY_MODE "constrained-intra-prediction"=$CONST_INTRA_PREDICTION "loop-filter-mode"=$LOOP_FILTER_MODE "scaling-list"=$SCALING_LIST "aspect-ratio"=$ASPECT_RATIO "max-picture-size"="$max_picture_size")
	if [ $split_input_flag == 1 ];then
	
		OMX_DEC_ARR=( "low-latency"=$LOW_LATENCY "internal-entropy-buffers"=$ENTROPY_BUFF "split-input"=$split_input)
	else
		OMX_DEC_ARR=( "low-latency"=$LOW_LATENCY "internal-entropy-buffers"=$ENTROPY_BUFF)
	fi
		
	QUEUE_ARR=( "max-size-bytes"=$MAX_SIZE_BYTES )                                                                                         
       	export QUEUE=$(update_element_properties $QUEUE ${QUEUE_ARR[@]})

        VIDEO_PARSE_ARR=( "width"=$WIDTH "height"=$HEIGHT "format"=$FORMAT "framerate"=$FRAME_RATE  )
       	export VIDEO_PARSE=$(update_element_properties $VIDEO_PARSE ${VIDEO_PARSE_ARR[@]})

	if [ $LEVEL == "NA" ]; then
		VIDEO_X=( "profile"=$PROFILE  "alignment"=$ALIGNMENT ) 
	else
		VIDEO_X=( "profile"=$PROFILE "level=\(string\)"$LEVEL "tier"=$TIER "alignment"=$ALIGNMENT ) 
	fi
	VIDEO_X_RAW_ARR=( "width"=$WIDTH, "height"=$HEIGHT, "format"=$FORMAT, "framerate"=$FRAME_RATE  )                                  
	export VIDEO_X_RAW=$(update_element_properties $VIDEO_X_RAW ${VIDEO_X_RAW_ARR[@]})

	export SINK=( "fpsdisplaysink name=fpssink text-overlay=false video-sink=\"kmssink bus-id=\"$BUS_ID\" hold-extra-sample=TRUE fullscreen-overlay=1 sync=true\" -v" )

	if [ $TYPE == "decode" ]; then

		BUS_ID="a0070000.v_mix"
		export SINK=( "fpsdisplaysink name=fpssink text-overlay=false video-sink=\"kmssink hold-extra-sample=TRUE bus-id=\"$BUS_ID\" fullscreen-overlay=1 plane-id=$plane_id sync=true\" -v" )

	        if [ $CODEC_TYPE == "AVC" ]; then                                                                                                      
        	        export OMXHDEC=$(update_element_properties $OMXH264DEC ${OMX_DEC_ARR[@]})
			export CODEC_PARSE=( " h264parse " )
                	export OMXHENC=$(update_element_properties $OMXH264ENC ${OMX_ENC_ARR[@]})               
                        export VIDEO_X_H=$(update_cap_properties $VIDEO_X_H264 ${VIDEO_X[@]})
	        else                                                                                
	                export OMXHDEC=$(update_element_properties $OMXH265DEC ${OMX_DEC_ARR[@]})
			export CODEC_PARSE=( " h265parse " )
                        export OMXHENC=$(update_element_properties $OMXH265ENC ${OMX_ENC_ARR[@]})   
                        export VIDEO_X_H=$(update_cap_properties $VIDEO_X_H265 ${VIDEO_X[@]})
	        fi
	elif [ $TYPE == "encode_fs" ]; then

		export SINK=( "fpsdisplaysink name=fpssink text-overlay=false video-sink=\"fakesink sync=true\" -v" )

		if [ $CODEC_TYPE == "AVC" ]; then
                        export OMXHENC=$(update_element_properties $OMXH264ENC ${OMX_ENC_ARR[@]})               
                        export VIDEO_X_H=$(update_cap_properties $VIDEO_X_H264 ${VIDEO_X[@]})
		else
                        export OMXHENC=$(update_element_properties $OMXH265ENC ${OMX_ENC_ARR[@]})   
                        export VIDEO_X_H=$(update_cap_properties $VIDEO_X_H265 ${VIDEO_X[@]})
		fi
        elif [ $TYPE == "transcode" ]; then                                     
                                                                                      
                export SINK=( "fpsdisplaysink name=fpssink text-overlay=false video-sink=\"fakesink sync=true\" -v" )

	        if [ $CODEC_TYPE == "AVC->AVC" ]; then                                                                                          
			INPUT_CODEC="AVC"
			OUTPUT_CODEC="AVC"
	        elif [ $CODEC_TYPE == "AVC->HEVC" ]; then                                                                                         
			INPUT_CODEC="AVC"
			OUTPUT_CODEC="HEVC"
	        elif [ $CODEC_TYPE == "HEVC->AVC" ]; then                                                                                         
			INPUT_CODEC="HEVC"
			OUTPUT_CODEC="AVC"
		elif [ $CODEC_TYPE == "HEVC->HEVC" ]; then                                                                                         
			INPUT_CODEC="HEVC"
			OUTPUT_CODEC="HEVC"
		fi
		
                if [ $INPUT_CODEC == "AVC" ]; then                          
                        export OMXHDEC=$(update_element_properties $OMXH264DEC ${OMX_DEC_ARR[@]})
                        export CODEC_PARSE=( " h264parse " )                                                         
                else                                                          
                        export OMXHDEC=$(update_element_properties $OMXH265DEC ${OMX_DEC_ARR[@]})                                          
                        export CODEC_PARSE=( " h265parse " )                                                 
                fi

                if [ $OUTPUT_CODEC == "AVC" ]; then                          
                        export OMXHENC=$(update_element_properties $OMXH264ENC ${OMX_ENC_ARR[@]})
			export VIDEO_X_H=$(update_cap_properties $VIDEO_X_H264 ${VIDEO_X[@]})
                else                                                          
                        export OMXHENC=$(update_element_properties $OMXH265ENC ${OMX_ENC_ARR[@]})
			export VIDEO_X_H=$(update_cap_properties $VIDEO_X_H265 ${VIDEO_X[@]})
                fi
        elif [ $TYPE == "low_latency" ]; then                                                                                                       

                if [ $SOURCE == "HDMI" ]; then
                        SRC="video0"
                elif [ $SOURCE == "MIPI" ]; then
                        SRC="video0"
                fi                                                              

		if [ "$RESOLUTION" == "4kp60" -o "$RESOLUTION" == "1080p30" -o "$RESOLUTION" == "1080p60" -o "$RESOLUTION" == "4kp30" ];then
			BUS_ID="a0070000.v_mix"
		else
			echo "Wrong resolution"
			exit -1
		fi

                export INPUT_SRC=( "-v v4l2src device=/dev/$SRC io-mode=$io_mode num-buffers=$NUM_BUFF " ) 
		if [ $BUS_ID == "fd4a0000.display" ]; then
			export SINK=( "fpsdisplaysink name=fpssink text-overlay=false video-sink=\"kmssink bus-id=\"$BUS_ID\" hold-extra-sample=TRUE fullscreen-overlay=1 plane-id=$plane_id sync=true\" -v" )
		else
			export SINK=( "fpsdisplaysink name=fpssink text-overlay=false video-sink=\"kmssink bus-id=\"$BUS_ID\" hold-extra-sample=TRUE fullscreen-overlay=1 sync=true\" -v" )
		fi

		if [ "$RESOLUTION" == "4kp60" ]; then
			if [ "$FORMAT" == "nv12-10le32" -o "$FORMAT" ==  "nv16-10le32" -o "$FORMAT" == "nv16" ]; then
				export SINK=( "fpsdisplaysink name=fpssink text-overlay=false video-sink=\"fakevideosink sync=true\" -v" )
			fi
		fi

                if [ $CODEC_TYPE == "AVC" ]; then                                                                                              
                        export OMXHENC=$(update_element_properties $OMXH264ENC ${OMX_ENC_ARR[@]})                                              
                        export VIDEO_X_H=$(update_cap_properties $VIDEO_X_H264 ${VIDEO_X[@]})                                                  
                        export CODEC_PARSE=( " h264parse " )                                                                                   
                        export OMXHDEC=$(update_element_properties $OMXH264DEC ${OMX_DEC_ARR[@]})                                              
                else                                                                                                                           
                        export OMXHENC=$(update_element_properties $OMXH265ENC ${OMX_ENC_ARR[@]})                                              
                        export VIDEO_X_H=$(update_cap_properties $VIDEO_X_H265 ${VIDEO_X[@]})                                                  
                        export CODEC_PARSE=( " h265parse " )                                                 
                        export OMXHDEC=$(update_element_properties $OMXH265DEC ${OMX_DEC_ARR[@]})               
                fi                                                              
	elif [ $TYPE == "capture_enc_dec" -o $TYPE == "maxbitrate" -a $MODE == "serial" ]; then

                if [ $SOURCE == "HDMI" ]; then                                                         
                        SRC="video0"                                                                            
                elif [ $SOURCE == "MIPI" ]; then                                          
                        SRC="video0"                                               
                fi                                                              
 
                export INPUT_SRC=( "-v v4l2src device=/dev/$SRC io-mode=$io_mode num-buffers=$NUM_BUFF " ) 

                if [ $CODEC_TYPE == "AVC" ]; then                                                                                              
                        export OMXHENC=$(update_element_properties $OMXH264ENC ${OMX_ENC_ARR[@]})                                              
                        export VIDEO_X_H=$(update_cap_properties $VIDEO_X_H264 ${VIDEO_X[@]})                                                  
                        export CODEC_PARSE=( " h264parse " )                                                                                   
                        export OMXHDEC=$(update_element_properties $OMXH264DEC ${OMX_DEC_ARR[@]})                                              
                else                                                                                                                           
                        export OMXHENC=$(update_element_properties $OMXH265ENC ${OMX_ENC_ARR[@]})                                              
                        export VIDEO_X_H=$(update_cap_properties $VIDEO_X_H265 ${VIDEO_X[@]})                                                  
                        export CODEC_PARSE=( " h265parse " )                                                 
                        export OMXHDEC=$(update_element_properties $OMXH265DEC ${OMX_DEC_ARR[@]})               
                fi                                                              
	elif [ $TYPE == "multistream" ]; then

		BUS_ID="a0070000.v_mix"

		if [ $STREAMS == "2" ]; then
			export INPUT_SRC_1=( "-v v4l2src device=/dev/video3 io-mode=$io_mode num-buffers=$NUM_BUFF " )
			export INPUT_SRC_2=( "-v v4l2src device=/dev/video0 io-mode=$io_mode num-buffers=$NUM_BUFF " )
		fi

		if [ $STREAMS == "4" ]; then
			export INPUT_SRC_1=( "-v v4l2src device=/dev/video4 io-mode=$io_mode num-buffers=$NUM_BUFF " )
			export INPUT_SRC_2=( "-v v4l2src device=/dev/video3 io-mode=$io_mode num-buffers=$NUM_BUFF " )
			export INPUT_SRC_3=( "-v v4l2src device=/dev/video2 io-mode=$io_mode num-buffers=$NUM_BUFF " )
			export INPUT_SRC_4=( "-v v4l2src device=/dev/video0 io-mode=$io_mode num-buffers=$NUM_BUFF " )
		fi

		if [ $STREAMS == "8" ]; then
			export INPUT_SRC_1=( "-v v4l2src device=/dev/video7 io-mode=$io_mode num-buffers=$NUM_BUFF " )
			export INPUT_SRC_2=( "-v v4l2src device=/dev/video6 io-mode=$io_mode num-buffers=$NUM_BUFF " )
			export INPUT_SRC_3=( "-v v4l2src device=/dev/video5 io-mode=$io_mode num-buffers=$NUM_BUFF " )
			export INPUT_SRC_4=( "-v v4l2src device=/dev/video4 io-mode=$io_mode num-buffers=$NUM_BUFF " )
			export INPUT_SRC_5=( "-v v4l2src device=/dev/video3 io-mode=$io_mode num-buffers=$NUM_BUFF " )
			export INPUT_SRC_6=( "-v v4l2src device=/dev/video2 io-mode=$io_mode num-buffers=$NUM_BUFF " )
			export INPUT_SRC_7=( "-v v4l2src device=/dev/video1 io-mode=$io_mode num-buffers=$NUM_BUFF " )
			export INPUT_SRC_8=( "-v v4l2src device=/dev/video0 io-mode=$io_mode num-buffers=$NUM_BUFF " )

		fi

		if [ $MODE == "decode" ]; then  
			if [ $BUS_ID == "fd4a0000.display" ]; then
				export SINK=( "fpsdisplaysink name=fpssink text-overlay=false video-sink=\"kmssink bus-id=\"$BUS_ID\" hold-extra-sample=TRUE fullscreen-overlay=1 plane-id=$plane_id sync=true\" -v" )
			else
				export SINK=( "fpsdisplaysink name=fpssink text-overlay=false video-sink=\"kmssink bus-id=\"$BUS_ID\" hold-extra-sample=TRUE fullscreen-overlay=1 sync=true\" -v" )
			fi

			if [ $SOURCE == "File" ]; then
				export SINK=( "fpsdisplaysink name=fpssink text-overlay=false video-sink=\"fakevideosink sync=true\" -v" )
			else
				export FAKE_SINK=( "fpsdisplaysink name=fpssink text-overlay=false video-sink=\"fakevideosink sync=true\" -v" )
			fi
			if [ $CODEC_TYPE == "AVC" ]; then
				export OMXHDEC=$(update_element_properties $OMXH264DEC ${OMX_DEC_ARR[@]})
				export CODEC_PARSE=( " h264parse " )
			else
				export OMXHDEC=$(update_element_properties $OMXH265DEC ${OMX_DEC_ARR[@]})
				export CODEC_PARSE=( " h265parse " )
			fi
		elif [ $MODE == "encode" ]; then
			export SINK=( "fpsdisplaysink name=fpssink text-overlay=false video-sink=\"fakesink sync=true\" -v" )


			if [ $SOURCE == "File" ]; then
				export FAKE_SINK=( "fakesink" )
			else
				export FAKE_SINK=( "fpsdisplaysink name=fpssink text-overlay=false video-sink=\"fakesink sync=true\" -v" )
			fi

			if [ $CODEC_TYPE == "AVC" ]; then
				export OMXHENC=$(update_element_properties $OMXH264ENC ${OMX_ENC_ARR[@]})
				export VIDEO_X_H=$(update_cap_properties $VIDEO_X_H264 ${VIDEO_X[@]})
			else
				export OMXHENC=$(update_element_properties $OMXH265ENC ${OMX_ENC_ARR[@]})
			export VIDEO_X_H=$(update_cap_properties $VIDEO_X_H265 ${VIDEO_X[@]})
			fi
		elif [ $MODE == "serial" ]; then

			if [ $SOURCE == "File" ]; then
				export FAKE_SINK=( "fakevideosink" )
				export SINK=( "fakevideosink" )
			else
				if [ "$RESOLUTION" == "4kp30" ]; then

					if [ $STREAMS == "2" ]; then
					      export SINK_1=( "fpsdisplaysink name=fpssink text-overlay=false video-sink=\"kmssink bus-id=\"$BUS_ID\" hold-extra-sample=TRUE  plane-id=34 render-rectangle=<0,0,1920,2160>\" sync=true -v" )
					      export SINK_2=( "fpsdisplaysink name=fpssink text-overlay=false video-sink=\"kmssink bus-id=\"$BUS_ID\" hold-extra-sample=TRUE  plane-id=36 render-rectangle=<1920,0,1920,2160>\" sync=true -v" ) 
					fi
				else

					if [ $STREAMS == "2" ]; then                                                                                                 
                              		      export SINK_1=( "fpsdisplaysink name=fpssink text-overlay=false video-sink=\"kmssink bus-id=\"$BUS_ID\" hold-extra-sample=TRUE  plane-id=34 render-rectangle=<0,0,960,1080>\" sync=true -v" )
                                              export SINK_2=( "fpsdisplaysink name=fpssink text-overlay=false video-sink=\"kmssink bus-id=\"$BUS_ID\" hold-extra-sample=TRUE  plane-id=36 render-rectangle=<960,0,960,1080>\" sync=true -v" )
					     
					elif [ $STREAMS == "4" ]; then
					        export SINK_1=( "fpsdisplaysink name=fpssink text-overlay=false video-sink=\"kmssink bus-id=\"$BUS_ID\" hold-extra-sample=TRUE  plane-id=34 render-rectangle=<0,0,960,540>\" sync=true -v" )
					        export SINK_2=( "fpsdisplaysink name=fpssink text-overlay=false video-sink=\"kmssink bus-id=\"$BUS_ID\" hold-extra-sample=TRUE  plane-id=36 render-rectangle=<960,0,960,540>\" sync=true -v" )
						export SINK_3=( "fpsdisplaysink name=fpssink text-overlay=false video-sink=\"kmssink bus-id=\"$BUS_ID\" hold-extra-sample=TRUE  plane-id=38 render-rectangle=<0,540,960,540>\" sync=true -v" )
						export SINK_4=( "fpsdisplaysink name=fpssink text-overlay=false video-sink=\"kmssink bus-id=\"$BUS_ID\" hold-extra-sample=TRUE  plane-id=40 render-rectangle=<960,540,960,540>\" sync=true -v" )
				
					elif [ $STREAMS == "8" ]; then
					        export SINK_1=( "fpsdisplaysink name=fpssink text-overlay=false video-sink=\"kmssink bus-id=\"$BUS_ID\" hold-extra-sample=TRUE  plane-id=34 render-rectangle=<0,0,960,270>\" sync=true -v" )
						export SINK_2=( "fpsdisplaysink name=fpssink text-overlay=false video-sink=\"kmssink bus-id=\"$BUS_ID\" hold-extra-sample=TRUE  plane-id=36 render-rectangle=<960,0,960,270>\" sync=true -v" )
						export SINK_3=( "fpsdisplaysink name=fpssink text-overlay=false video-sink=\"kmssink bus-id=\"$BUS_ID\" hold-extra-sample=TRUE  plane-id=38 render-rectangle=<0,270,960,270>\" sync=true -v" )
						export SINK_4=( "fpsdisplaysink name=fpssink text-overlay=false video-sink=\"kmssink bus-id=\"$BUS_ID\" hold-extra-sample=TRUE  plane-id=40 render-rectangle=<960,270,960,270>\" sync=true -v" )
						export SINK_5=( "fpsdisplaysink name=fpssink text-overlay=false video-sink=\"kmssink bus-id=\"$BUS_ID\" hold-extra-sample=TRUE  plane-id=42 render-rectangle=<0,540,960,270>\" sync=true -v" )
						export SINK_6=( "fpsdisplaysink name=fpssink text-overlay=false video-sink=\"kmssink bus-id=\"$BUS_ID\" hold-extra-sample=TRUE  plane-id=44 render-rectangle=<960,540,960,270>\" sync=true -v" )
						export SINK_7=( "fpsdisplaysink name=fpssink text-overlay=false video-sink=\"kmssink bus-id=\"$BUS_ID\" hold-extra-sample=TRUE  plane-id=46 render-rectangle=<0,810,960,270>\" sync=true -v" )
						export SINK_8=( "fpsdisplaysink name=fpssink text-overlay=false video-sink=\"kmssink bus-id=\"$BUS_ID\" hold-extra-sample=TRUE  plane-id=48 render-rectangle=<960,810,960,270>\" sync=true -v" )
					fi
				fi
			fi

			if [ $CODEC_TYPE == "AVC" ]; then
				export OMXHENC=$(update_element_properties $OMXH264ENC ${OMX_ENC_ARR[@]})
				export VIDEO_X_H=$(update_cap_properties $VIDEO_X_H264 ${VIDEO_X[@]})
				export CODEC_PARSE=( " h264parse " )
				export OMXHDEC=$(update_element_properties $OMXH264DEC ${OMX_DEC_ARR[@]})
			else
				export OMXHENC=$(update_element_properties $OMXH265ENC ${OMX_ENC_ARR[@]})
				export VIDEO_X_H=$(update_cap_properties $VIDEO_X_H265 ${VIDEO_X[@]})
				export CODEC_PARSE=( " h265parse " )
				export OMXHDEC=$(update_element_properties $OMXH265DEC ${OMX_DEC_ARR[@]})
			fi
		fi
	elif [ $TYPE == "maxbitrate" ]; then

                if [ $SOURCE == "HDMI" ]; then                                                         
                        SRC="video0"                                                                            
                elif [ $SOURCE == "MIPI" ]; then                                          
                        SRC="video0"                                               
                fi                                                              

		if [ $MODE == "encode" ]; then
	                export SINK=( "fpsdisplaysink name=fpssink text-overlay=false video-sink=\"fakesink sync=true\" -v" )
		fi
                export INPUT_SRC=( "-v v4l2src device=/dev/$SRC io-mode=$io_mode num-buffers=$NUM_BUFF " ) 

                if [ $CODEC_TYPE == "AVC" ]; then                                                                                              
                        export OMXHENC=$(update_element_properties $OMXH264ENC ${OMX_ENC_ARR[@]})                                              
                        export VIDEO_X_H=$(update_cap_properties $VIDEO_X_H264 ${VIDEO_X[@]})                                                  
                        export CODEC_PARSE=( " h264parse " )                                                                                   
                        export OMXHDEC=$(update_element_properties $OMXH264DEC ${OMX_DEC_ARR[@]})                                              
                else                                                                                                                           
                        export OMXHENC=$(update_element_properties $OMXH265ENC ${OMX_ENC_ARR[@]})                                              
                        export VIDEO_X_H=$(update_cap_properties $VIDEO_X_H265 ${VIDEO_X[@]})                                                  
                        export CODEC_PARSE=( " h265parse " )                                                 
                        export OMXHDEC=$(update_element_properties $OMXH265DEC ${OMX_DEC_ARR[@]})               
                fi                                                              
	fi	
}



parse_playback_param()
{
	tc=$1
	declare -a FILE_SRC_ARR
	declare -a DEC_PARSE_ARR
	declare -a OMX_DEC_ARR
	declare -a OMX_ENC_ARR
	#declare -a CODEC_PARSE
	declare -a VIDEO_X
	declare -a VIDEO_X_RAW_ARR

	export CODEC_PARSE=" h265parse "
	export GST_COMMAND="gst-launch-1.0"
	export FILE_SRC="filesrc"
	export HDMI_SRC=""
	export TPG_SRC=""
	export VIDEO_PARSE="videoparse"
	export FORMAT_AVC_PARSE="h264parse"
	export FORMAT_HEVC_PARSE="h265parse"

	export OMXH265DEC="omxh265dec"
	export OMXH264DEC="omxh264dec"
        export OMXH265ENC="omxh265enc"
        export OMXH264ENC="omxh264enc"

	export OMXHENC="omxhenc"
	export OMXHDEC="omxhdec"

	export FILE_SINK="filesink"
	export VIDEO_X_H264="video/x-h264"
	export VIDEO_X_H265="video/x-h265"
	export VIDEO_X_H="video/x-H26"
	export VIDEO_X_RAW="video/x-raw,"
	export VIDEO_CONVERT="videoconvert "
	export QUEUE="queue"

	export NUM_BUFF=""
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
	DISPLAY=$(echo $tc | awk -F"," '{print $22}' | tr -d " " | tr -d "\t")
	MAX_SIZE_BYTES=0

	if [ "$RESOLUTION" == "1080p30" ]; then                                                                                                
                WIDTH=1920                                                                                      
                HEIGHT=1080                                                                                                                    
                FRAME_RATE="30/1"
		BUS_ID="a0070000.v_mix"
        elif [ "$RESOLUTION" == "1080p60" ]; then                                                                                              
                WIDTH=1920                                                                                                                     
                HEIGHT=1080                                                                                                                    
                FRAME_RATE="60/1"
		BUS_ID="a0070000.v_mix"                                                                                                                         
        elif [ "$RESOLUTION" == "4kp30" ]; then                                                                                                
                WIDTH=3840                                                                                                                     
                HEIGHT=2160                                                                                                                    
                FRAME_RATE="30/1"
		BUS_ID="a0070000.v_mix"                                                                                                      
        elif [ "$RESOLUTION" == "4kp60" ]; then                                                                                      
                WIDTH=3840                                                                                                           
                HEIGHT=2160                                                                                                                    
                FRAME_RATE="60/1"
		BUS_ID="a0070000.v_mix" 
        else                                                                                                                                   
                echo "Wrong resolution."
		echo "res: $RESOLUTION"                                                                                                       
                exit -1                                                                                                                        
        fi

	FILE_SRC_ARR=( "location"="$src_path/$BITSTREAM" num-buffers=1000 )
	export FILE_SRC=$(update_element_properties $FILE_SRC ${FILE_SRC_ARR[@]})

	OMX_ENC_ARR=( "target-bitrate"=$TARGET_BITRATE "max-bitrate"=$MAX_BITRATE "control-rate"=$CTRL_RATE "num-slices"=$NUM_SLICE "gop-mode"=$GOP_MODE "gop-length"=$GOP_LENGTH "b-frames"=$B_FRAMES "qp-mode"=$QP_MODE "prefetch-buffer"=$PREF_BUFF )
	QUEUE_ARR=( "max-size-bytes"=$MAX_SIZE_BYTES )                                                                                         
       	export QUEUE=$(update_element_properties $QUEUE ${QUEUE_ARR[@]})

        VIDEO_PARSE_ARR=( "width"=$WIDTH "height"=$HEIGHT "format"=$FORMAT "framerate"=$FRAME_RATE  )
       	export VIDEO_PARSE=$(update_element_properties $VIDEO_PARSE ${VIDEO_PARSE_ARR[@]})

	VIDEO_X=( "profile"=$PROFILE ) 
	VIDEO_X_RAW_ARR=( "width"=$WIDTH, "height"=$HEIGHT, "format"=$FORMAT, "framerate"=$FRAME_RATE  )                                  
	export VIDEO_X_RAW=$(update_element_properties $VIDEO_X_RAW ${VIDEO_X_RAW_ARR[@]})

	if [ "BUS_ID" == "fd4a0000.display" ]; then
		export SINK=( "fpsdisplaysink name=fpssink text-overlay=false video-sink=\"kmssink bus-id=\"$BUS_ID\" hold-extra-sample=TRUE fullscreen-overlay=1 plane-id=$plane_id sync=true\" -v" )
	else
		export SINK=( "fpsdisplaysink name=fpssink text-overlay=false video-sink=\"kmssink bus-id=\"$BUS_ID\" hold-extra-sample=TRUE fullscreen-overlay=1 sync=true\" -v" )
	fi

	if [ $TYPE == "file_playback" ]; then
		#We have all files of avc
		if [ $CODEC_TYPE == "AVC" ]; then                                     
			export OMXHENC=$(update_element_properties $OMXH264ENC ${OMX_ENC_ARR[@]})
			export VIDEO_X_H=$(update_cap_properties $VIDEO_X_H264 ${VIDEO_X[@]})
			export OMXHDEC=$OMXH264DEC
			export CODEC_PARSE=( " h264parse " )
		else
			export OMXHENC=$(update_element_properties $OMXH265ENC ${OMX_ENC_ARR[@]})
			export VIDEO_X_H=$(update_cap_properties $VIDEO_X_H265 ${VIDEO_X[@]})
			export OMXHDEC=$OMXH265DEC
			export CODEC_PARSE=( " h265parse " )
		fi
	fi
}


############################################################################
#
# Name:         update_element_properties
#
# Argument:     1) gstremer element 2) element properties array with values
#
# Description:  This function will update gstremer element properties for
#               those which has "NA" values
#
# Return Value: Element with properties
#
###########################################################################
update_element_properties()
{
	element=$1 && shift
	prop_arr=($@)
	return_ele=$element
	
	for prop in ${prop_arr[@]}; do
		val_c=$(echo $prop | cut -d "=" -f2 )
		if  [ "${val_c}" != "NA" ]; then
			return_ele="${return_ele} ${prop}"
		fi

		if [ ${prop_arr[-1]} == $prop ]; then
			echo $return_ele
		fi

	done | sort
}

############################################################################
#
# Name:         update_cap_properties
#
# Argument:     1) gstremer element 2) element properties array with values
#
# Description:  This function will update gstremer element capabilities for
#               those which has "NA" values
#
# Return Value: Element with capabilities
#
###########################################################################
update_cap_properties()
{
	element=$1 && shift
	prop_arr=($@)
	return_ele=$element

	for prop in ${prop_arr[@]}; do
		val_c=$(echo $prop | cut -d "=" -f2 )
		if  [ ${val_c} != "NA" ]; then
			return_ele="${return_ele}, ${prop}"
		fi

		if [ ${prop_arr[-1]} == $prop ]; then
			echo $return_ele
		fi
        done | sort
}

############################################################################
#
# Name:         create_gst_command
#
# Argument:     gstremer elements string separated by pipeline
#
# Description:  This function will create gstremer pipeline command by appending
#               all elements with their properties
#
# Return Value: gstreamer pipeline command
#
###########################################################################
create_gst_command()
{
	GST_ELEMENT=$1
	GST_ELE_COUNT=`echo $GST_ELEMENT | grep -o '|' | wc -l `
	GST_ELE_COUNT=`expr $GST_ELE_COUNT + 1`

	for (( i=1; i<=${GST_ELE_COUNT}; i++ ))
	do
		element=$(echo $GST_ELEMENT | awk -F"|" "{print \$$i}")
		if [ $i -eq 1 ]; then
			GST_COMMAND=$GST_COMMAND" "$element
		else
			GST_COMMAND=$GST_COMMAND" ! "$element
		fi
		if [ $i -eq ${GST_ELE_COUNT} ]; then
			echo $GST_COMMAND
		fi
	done
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

	test_type="";                                                                                
        if [ $TYPE == "encode_fs" ]; then                                                                                                         
                test_type="Input file->Encode->Filesink ( Pipeline should exit properly): io-mode=$io_mode"                                                                                             
        elif [ $TYPE == "decode" ]; then                                                                                                       
                test_type="Decode->Display ( Pass Criteria Achieved FPS value): io-mode=$io_mode"                                                                                                    
        elif [ $TYPE == "capture_enc_dec" ]; then                                                                                                      
                test_type="Live Serial Usecase - (Check for performance and errors (FPS)): io-mode=$io_mode"
        elif [ $TYPE == "low_latency" ]; then                                                                                                  
                test_type="Low-latency-mode tests (Live Serial usecase) - (Check end to end latency Value and Fps.): io-mode=$io_mode"
        elif [ $TYPE == "transcode" ]; then                                                                                                    
                test_type="Transcode code use cases: io-mode=$io_mode" 
        elif [ $TYPE == "multistream" ]; then                                                                                                  
                test_type="Multistream Encode & Decode tests: io-mode=$io_mode"
	elif [ $TYPE == "maxbitrate" ]; then
		test_type="Maxbitrate: io-mode=$io_mode"
        fi

	file_name=`eval basename $LIST_FILE`
	output_sheet=$(echo $file_name | awk -F"_" '{print $1}' | tr -d " " | tr -d "\t")
	
	if [ $split_input_flag == 1 ];then
		output_sheet="${output_sheet}_tests_split_true.xls"
	else
		output_sheet="${output_sheet}_tests.xls"
	fi
	
	echo -e "\n\n\n\n\n\t\t$test_type" >> $output_sheet
	echo -e "S.No\tTestcase\tType\tSource\tBitstream\tResolution&Fps\tCodec\tTarget Bitrate\tMax Bitrate\tControl Rate\tGOP Mode\tGOP Length\tB-frames\tFormat\tLatency Mode\tProfile\tQP Mode\tNum Slice\tNum buff\tPrefetch Buffer\tMode\tSlice size\tDependent slice\tCPB size\tInitial delay\tMin QP\tMax QP\tQuant-I-frames\tQuant-P-frames\tQuant-B-frames\tFiller-data\tLow-bandwidth\tPeriodicity-idr\tGDR Mode\tEntropy Mode\tIntra-prediction\tLoop-filter-mode\tScaling-list\tAspect-ratio\tLevel\tTier\tEntropy Buffer\tPipeline\tEndNumBuff\tResult\tComments\tLatency\tCMA Used" >> $output_sheet
	i=1

	while read tc
	do
		test $i -eq 1 && ((i=i+1)) && continue
		size=${#single_test_execute}

		NUM=$(echo $tc | awk -F"," '{print $1}' | tr -d " " | tr -d "\t")
		TC_NUM=$(echo $tc | awk -F"," '{print $2}' | tr -d " " | tr -d "\t")
		TYPE=$(echo $tc | awk -F"," '{print $3}' | tr -d " " | tr -d "\t")
		SOURCE=$(echo $tc | awk -F"," '{print $4}' | tr -d " " | tr -d "\t")
		INPUT_FILE_NAME=$(echo $tc | awk -F"," '{print $5}' | tr -d " " | tr -d "\t")
		RESOLUTION_STREAM=$(echo $tc | awk -F"," '{print $6}' | tr -d " " | tr -d "\t")
		CODEC_TYPE=$(echo $tc | awk -F"," '{print $7}' | tr -d " " | tr -d "\t")
		FORMAT=$(echo $tc | awk -F"," '{print $14}' | tr -d " " | tr -d "\t")
		MODE=$(echo $tc | awk -F"," '{print $21}' | tr -d " " | tr -d "\t")
#		echo $tc
		if [ $size -ne 0 -a "$single_test_execute" != "$TC_NUM" ]; then
			continue
		fi

		RESULT="PASS"
		FAIL_REASON=""

		parse_param $tc

        	if [ $TYPE == "multistream" ]; then                                                                                                    
	                STREAM=$(echo $RESOLUTION_STREAM | awk -F"-" '{print $1}' | tr -d " " | tr -d "\t")
                	RESOLUTIONS=$(echo $RESOLUTION_STREAM | awk -F"-" '{print $2}' | tr -d " " | tr -d "\t")
        	else
			STREAMS=1
			RESOLUTIONS=$RESOLUTION_STREAM
		fi                                                                                                                                     

	        if [ "$RES" != "$RESOLUTIONS" ]; then
			continue
	        fi

	        if [ "$RESOLUTIONS" == "1080p30" ]; then                                                                 
	                FRAME_RATE="30"                                                                                                              
	        elif [ "$RESOLUTIONS" == "1080p60" ]; then                                                                                        
	                FRAME_RATE="60"                                                                                                              
	        elif [ "$RESOLUTIONS" == "4kp30" ]; then                                                                     
	                FRAME_RATE="30"                                                                                                              
	        elif [ "$RESOLUTIONS" == "4kp60" ]; then                                             
	                FRAME_RATE="60"                                                                                 
	        else                                                                                             
	                echo "Wrong resolution."                                                              
	                echo "res: $RESOLUTIONS"                                                               
	        fi

		op_path="$op_dir/$TC_NUM"

		if [ -d "$op_path" ]; then
			rm -rf $op_path
		fi

		mkdir -p $op_path
		fps_log_file="$op_path/output_fps.log"

		if [ $CODEC_TYPE = "AVC" ]
		then
			if [ -z $INPUT_FILE_NAME ]; then
				OUTPUT_FILE="$op_dir/$TC_NUM/${TC_NUM}.avc"
			else
				OUTPUT_FILE="$op_dir/$TC_NUM/${INPUT_FILE_NAME}.avc"
			fi
		else
			if [ -z $INPUT_FILE_NAME ]; then
				OUTPUT_FILE="$op_dir/$TC_NUM/${INPUT_FILE_NAME}.avc"
			else
				OUTPUT_FILE="$op_dir/$TC_NUM/${TC_NUM}.hevc"
			fi
		fi

		declare -a FILESINK_ARR=( "location"="$OUTPUT_FILE"  )
		export FILE_SINK=$(update_element_properties $FILE_SINK ${FILESINK_ARR[@]})

		CMA_FREE_BEFORE=$(cat /proc/meminfo | grep CmaFree | awk -F":" '{printf("%d", $2)}')

                if [ $TYPE == "multistream" ]
                then
			if [ $SOURCE == "File" ]; then
				if [ $MODE == "encode" ]; then 
					GST_ELEMENT=$FILE_SRC"|"$VIDEO_PARSE"|"$OMXHENC"|"$VIDEO_X_H
				elif [ $MODE == "decode" ]; then
					GST_ELEMENT=$FILE_SRC"|"$CODEC_PARSE"|"$OMXHDEC"|"$QUEUE
				elif [ $MODE == "serial" ]; then
					GST_ELEMENT=$FILE_SRC"|"$VIDEO_PARSE"|"$OMXHENC"|"$VIDEO_X_H"|""queue""|"$OMXHDEC"|"$QUEUE
				fi
			elif [ $SOURCE == "HDMI" -o $SOURCE == "TPG" -o $SOURCE == "MIPI" ]; then
				if [ $MODE == "encode" ]; then
					echo "************************MS ENCODE **************************"
					GST_ELEMENT=$INPUT_SRC"|"$VIDEO_X_RAW"|"$OMXHENC"|"$VIDEO_X_H
				elif [ $MODE == "serial" ]; then
					GST_ELEMENT=$INPUT_SRC"|"$VIDEO_X_RAW"|"$OMXHENC"|"$VIDEO_X_H"|""queue""|"$OMXHDEC"|"$QUEUE
				fi
			else
				echo "Wrong device selected"
			fi
		elif [ $TYPE == "low_latency" ]                                                                                                   
                then
			eval "export GST_DEBUG=\"GST_TRACER:7\" GST_TRACERS=latency GST_DEBUG_FILE=\"/tmp/x.log\""
			if [ $SOURCE == "File" ]; then
				GST_ELEMENT=$FILE_SRC"|"$VIDEO_PARSE"|"$OMXHENC"|"$VIDEO_X_H"|"$CODEC_PARSE"|"$OMXHDEC"|"$QUEUE"|"$SINK
			elif [ $SOURCE == "HDMI" -o $SOURCE == "TPG" -o $SOURCE == "MIPI" ]; then
                        	GST_ELEMENT=$INPUT_SRC"|"$VIDEO_X_RAW"|"$OMXHENC"|"$VIDEO_X_H"|""queue""|"$OMXHDEC"|"$QUEUE"|"$SINK
			else
				echo "Wrong device selected"
			fi
		elif [ $TYPE == "capture_enc_dec" ]
                then                                                                                                                           
			eval "export GST_DEBUG=\"GST_TRACER:7\" GST_TRACERS=latency GST_DEBUG_FILE=\"/tmp/x.log\""
                        GST_ELEMENT=$INPUT_SRC"|"$VIDEO_X_RAW"|"$OMXHENC"|"$VIDEO_X_H"|""queue""|"$OMXHDEC"|"$QUEUE"|"$SINK
                elif [ $TYPE == "transcode" ]
                then
			GST_ELEMENT=$FILE_SRC"|"$CODEC_PARSE"|"$OMXHDEC"|"$QUEUE"|"$OMXHENC"|"$VIDEO_X_H"|"$SINK
		elif [ $TYPE == "encode_fs" ]
		then
			GST_ELEMENT=$FILE_SRC"|"$VIDEO_PARSE"|"$OMXHENC"|"$VIDEO_X_H"|"$SINK
		elif [ $TYPE == "decode" ]
		then
			GST_ELEMENT=$FILE_SRC"|"$VIDEO_PARSE"|"$OMXHENC"|"$VIDEO_X_H"|"$FILE_SINK
		elif [ $TYPE == "maxbitrate" -a $MODE == "serial" ]
		then
                        GST_ELEMENT=$INPUT_SRC"|"$VIDEO_X_RAW"|"$OMXHENC"|"$VIDEO_X_H"|""queue""|"$OMXHDEC"|"$QUEUE"|"$SINK
		elif [ $TYPE == "maxbitrate" -a $MODE == "encode" ]
		then
                        GST_ELEMENT=$INPUT_SRC"|"$VIDEO_X_RAW"|"$OMXHENC"|"$VIDEO_X_H"|"$SINK
		elif [ $TYPE == "maxbitrate" -a $MODE == "decode" ]
		then
                        GST_ELEMENT=$INPUT_SRC"|"$VIDEO_X_RAW"|"$OMXHENC"|"$VIDEO_X_H"|"$FILE_SINK
		fi

		echo "$GST_ELEMENT"

		GST_COMMAND=$(create_gst_command "${GST_ELEMENT}")

		echo -e "\n\t\t\t::::::::: Test $TC_NUM is started :::::::::"

		echo -ne "$NUM\t$TC_NUM\t$TYPE\t$SOURCE\t$BITSTREAM\t$RESOLUTION_STREAM\t$CODEC_TYPE\t$TARGET_BITRATE\t$MAX_BITRATE\t$CTRL_RATE\t$GOP_MODE\t$GOP_LENGTH\t$B_FRAMES\t$FORMAT\t$LATENCY_MODE\t$PROFILE\t$QP_MODE\t$NUM_SLICE\t$NUM_BUFF\t$PREF_BUFF\t$MODE\t$SLICE_SIZE\t$DPEND_SLICE\t$CPB_SIZE\t$INI_DELAY\t$MIN_QP\t$MAX_QP\t$QUANT_IFRAME\t$QUANT_PFRAME\t$QUANT_BFRAME\t$FILL_DATA\t$LOW_BW\t$PERIODICITY_IDR\t$GDR_MODE\t$ENTROPY_MODE\t$CONST_INTRA_PREDICTION\t$LOOP_FILTER_MODE\t$SCALING_LIST\t$ASPECT_RATIO\t$LEVEL\t$TIER\t$ENTROPY_BUFF" >> $output_sheet
		echo -e "\n================================== Gst pipeline ==================================\n"

		if [ $TYPE == "multistream" -a $SOURCE == "File" ]; then
			echo "Multistream - File"
			i=1    
			echo -ne "\t\"" >> $output_sheet
                        for ((i = 1; i < $STREAMS; i++)); do
				echo "$GST_COMMAND ! $SINK" | tee -a $op_path/op_multi_${i}.log
				echo -e "\n================================== Gst pipeline ==================================\n"
				eval "$GST_COMMAND ! $SINK" 2>&1 | tee -a $op_path/op_multi_${i}.log &
				echo -ne "$i: \"$GST_COMMAND ! $SINK  " >> $output_sheet
			done                                       
			echo "$GST_COMMAND ! $SINK" | tee -a $fps_log_file                                                                            
			echo -e "\n================================== Gst pipeline ==================================\n"
			eval "$GST_COMMAND ! $SINK" 2>&1 | tee -a $fps_log_file &
			echo -ne "$i: $GST_COMMAND ! $SINK " >> $output_sheet
			echo -ne "\"" >> $output_sheet
			unset IFS
		elif [ $TYPE == "multistream" -a $SOURCE != "File" ]; then
			IFS='#'
			echo "Multistream - Live"
			i=1
			echo -ne "\t\"" >> $output_sheet
			if [ $MODE == "serial" ];then 
			                                                                                                       
				for ((i = 1; i < $STREAMS; i++)); do
					export GST_COMMAND="gst-launch-1.0"
	
					INPUT="INPUT_SRC_$i"
					echo $INPUT
					echo ${!INPUT}
					if [ $MODE == "encode" ]; then
						GST_ELEMENT=${!INPUT}"|"$VIDEO_X_RAW"|"$OMXHENC"|"$VIDEO_X_H
					elif [ $MODE == "serial" ]; then
						GST_ELEMENT=${!INPUT}"|"$VIDEO_X_RAW"|"$OMXHENC"|"$VIDEO_X_H"|""queue""|"$OMXHDEC"|"$QUEUE
					fi
	
					GST_COMMAND=$(create_gst_command "${GST_ELEMENT}")
					SINK_ELEMENT="SINK_${i}"
					echo ${!SINK_ELEMENT}
					echo "$GST_COMMAND ! ${!SINK_ELEMENT}" | tee -a $op_path/op_multi_${i}.log
					echo -e "\n================================== ----Gst pipeline New ------==================================\n"
					eval "$GST_COMMAND ! ${!SINK_ELEMENT}" 2>&1 | tee -a $op_path/op_multi_${i}.log &
					echo -ne "$i: \"$GST_COMMAND ! ${!SINK_ELEMENT}   " >> $output_sheet
				done
				INPUT="INPUT_SRC_$STREAMS"
				echo $INPUT
				echo ${!INPUT}
				export GST_COMMAND="gst-launch-1.0" 
				SinkLastInstance="SINK_$STREAM"
				if [ $MODE == "encode" ]; then
					GST_ELEMENT=${!INPUT}"|"$VIDEO_X_RAW"|"$OMXHENC"|"$VIDEO_X_H
				elif [ $MODE == "serial" ]; then
					GST_ELEMENT=${!INPUT}"|"$VIDEO_X_RAW"|"$OMXHENC"|"$VIDEO_X_H"|""queue""|"$OMXHDEC"|"$QUEUE
				fi
				GST_COMMAND=$(create_gst_command "${GST_ELEMENT}")
				echo "$GST_COMMAND ! ${!SinkLastInstance}" | tee -a $fps_log_file
				echo -e "\n================================== ****Gst pipeline *****==================================\n"
				eval "$GST_COMMAND ! ${!SinkLastInstance}" 2>&1 | tee -a $fps_log_file &
				echo -ne "$i: $GST_COMMAND ! ${!SinkLastInstance}" >> $output_sheet
				echo -ne "\"" >> $output_sheet
				unset IFS
			else
				
				i=1                                                                                                        
				for ((i = 1; i < $STREAMS; i++)); do
					export GST_COMMAND="gst-launch-1.0"
					INPUT="INPUT_SRC_$i"
					echo $INPUT
					echo ${!INPUT}
					if [ $MODE == "encode" ]; then
						GST_ELEMENT=${!INPUT}"|"$VIDEO_X_RAW"|"$OMXHENC"|"$VIDEO_X_H
					elif [ $MODE == "serial" ]; then
						GST_ELEMENT=${!INPUT}"|"$VIDEO_X_RAW"|"$OMXHENC"|"$VIDEO_X_H"|""queue""|"$OMXHDEC"|"$QUEUE
					fi
		
					GST_COMMAND=$(create_gst_command "${GST_ELEMENT}")
		
					echo "$GST_COMMAND ! $FAKE_SINK" | tee -a $op_path/op_multi_${i}.log
					echo -e "\n================================== Gst pipeline ==================================\n"
					eval "$GST_COMMAND ! $FAKE_SINK" 2>&1 | tee -a $op_path/op_multi_${i}.log &
	
				done
				INPUT="INPUT_SRC_$STREAMS"
				echo $INPUT
				echo ${!INPUT}
				export GST_COMMAND="gst-launch-1.0" 
		
				if [ $MODE == "encode" ]; then
					GST_ELEMENT=${!INPUT}"|"$VIDEO_X_RAW"|"$OMXHENC"|"$VIDEO_X_H
				elif [ $MODE == "serial" ]; then
					GST_ELEMENT=${!INPUT}"|"$VIDEO_X_RAW"|"$OMXHENC"|"$VIDEO_X_H"|""queue""|"$OMXHDEC"|"$QUEUE
				fi
				GST_COMMAND=$(create_gst_command "${GST_ELEMENT}")
		
				echo "$GST_COMMAND ! $SINK" | tee -a $fps_log_file
				echo -e "\n================================== Gst pipeline ==================================\n"
				eval "$GST_COMMAND ! $SINK" 2>&1 | tee -a $fps_log_file &
			fi

		else
			echo "++++++++++++Decode +++++++++++++++++++++++"
			echo $GST_COMMAND | tee -a $fps_log_file
			echo -e "\n================================== Gst pipeline ==================================\n"
			eval $GST_COMMAND 2>&1 | tee -a $fps_log_file
			echo -ne "\t$GST_COMMAND" >> $output_sheet
		fi
	

		sleep 2
		CMA_FREE_AFTER=$(cat /proc/meminfo | grep CmaFree | awk -F":" '{printf("%d", $2)}')

		Delta=10
		if [ $TYPE == "multistream" ]; then
			echo "FRAME_RATE = $FRAME_RATE"
			echo "NUM_BUFF = $NUM_BUFF"
			if [ $NUM_BUFF != "NA" ]; then
				TIME=$(echo $NUM_BUFF $FRAME_RATE / p | dc)
				echo "TIME=$TIME"
				sleep $TIME
			fi
			sleep 10
		fi

		killall -2 gst-launch-1.0

		if [ $TYPE == "maxbitrate" -a $MODE == "decode" ]
		then
			sleep 2
			FILE_SOURCE="filesrc location=$OUTPUT_FILE"
			GST_ELEMENT=$FILE_SOURCE"|"$CODEC_PARSE"|"$OMXHDEC"|"$QUEUE"|"$SINK
			echo "$GST_ELEMENT"
			GST_COMMAND="gst-launch-1.0"
			GST_COMMAND=$(create_gst_command "${GST_ELEMENT}")
			echo $GST_COMMAND | tee -a $fps_log_file
			echo -e "\n================================== Gst pipeline ==================================\n"
			eval $GST_COMMAND 2>&1 | tee -a $fps_log_file

			rm -f $OUTPUT_FILE
			killall -9 gst-launch-1.0
		fi

		if [ $TYPE == "decode" ]
		then
			echo "Codec parse ====================== $CODEC_PARSE =========="
			FILE_SOURCE="filesrc location=$OUTPUT_FILE"
			GST_ELEMENT=$FILE_SOURCE"|"$CODEC_PARSE"|"$OMXHDEC"|"$QUEUE"|"$SINK
			echo "$GST_ELEMENT"
			GST_COMMAND="gst-launch-1.0"
			GST_COMMAND=$(create_gst_command "${GST_ELEMENT}")
			echo $GST_COMMAND | tee -a $fps_log_file
			echo -e "\n================================== Gst pipeline ==================================\n"
			eval $GST_COMMAND 2>&1 | tee -a $fps_log_file

			rm -f $OUTPUT_FILE
			killall -9 gst-launch-1.0
		fi
		############################################# Excel Sheet Output #############################################
		#echo -ne "$NUM\t$TC_NUM\t$TYPE\t$SOURCE\t$BITSTREAM\t$RESOLUTION_STREAM\t$CODEC_TYPE\t$TARGET_BITRATE\t$MAX_BITRATE\t$CTRL_RATE\t$GOP_MODE\t$GOP_LENGTH\t$B_FRAMES\t$FORMAT\t$LATENCY_MODE\t$PROFILE\t$QP_MODE\t$NUM_SLICE\t$NUM_BUFF\t$PREF_BUFF\t$MODE\t$SLICE_SIZE\t$DPEND_SLICE\t$CPB_SIZE\t$INI_DELAY\t$MIN_QP\t$MAX_QP\t$QUANT_IFRAME\t$QUANT_PFRAME\t$QUANT_BFRAME\t$FILL_DATA\t$LOW_BW\t$PERIODICITY_IDR\t$GDR_MODE\t$ENTROPY_MODE\t$CONST_INTRA_PREDICTION\t$LOOP_FILTER_MODE\t$SCALING_LIST\t$ASPECT_RATIO\t$LEVEL\t$TIER\t$ENTROPY_BUFF\t$GST_COMMAND" >> $output_sheet
		##############################################################################################################

		#################### Check for errors ####################
		if [ "$STREAMS" != "1" ]; then
			for ((i = 1; i < $STREAMS; i++)); do
				grep -iE 'ERROR' $op_path/op_multi_${i}.log
				if [ $? -eq 0 ]; then
					RESULT="FAIL"
					FAIL_REASON="Error in pipeline playing."
					continue;
				fi
				grep -iE 'erroneous pipeline' $op_path/op_multi_${i}.log
				if [ $? -eq 0 ]; then
					RESULT="FAIL"
					FAIL_REASON="Wrong element in pipeline."
					continue;
				fi
				grep -iE 'Assertion|failed' $op_path/op_multi_${i}.log
				if [ $? -eq 0 ]; then
					RESULT="FAIL"
					FAIL_REASON="Assertion '0' failed in pipeline."
					echo "#################### Assertion #####################"
					continue;
				fi
				grep -iE 'Failed to create Encoder' $op_path/op_multi_${i}.log
				if [ $? -eq 0 ]; then
					RESULT="FAIL"
					FAIL_REASON="Encoder Resource Error."
					continue;
				fi
				grep -iE 'Cannot allocate memory' $op_path/op_multi_${i}.log
				if [ $? -eq 0 ]; then
					RESULT="FAIL"
					FAIL_REASON="CMA allocation Error."
					continue;
				fi
				grep -iE 'killed:' $op_path/op_multi_${i}.log
				if [ $? -eq 0 ]; then
					RESULT="FAIL"
					FAIL_REASON="memory Error(out of memory)."
					continue;
				fi
				grep -iE ' corruption' $op_path/op_multi_${i}.log
				if [ $? -eq 0 ]; then
					RESULT="FAIL"
					FAIL_REASON="memory corruption"
					continue;
				fi
				
				mins=$( grep -inr "Execution ended after " $op_path/op_multi_${i}.log )
				mins=$(echo $mins | awk -F"Execution ended after " '{print $2}' )
				mins=$(echo $mins | awk -F: '{ print ($1 * 3600) + ($2 * 60) + $3 }' )
				mins=$(echo $mins | awk -F. '{ print ($1) }' )
				Ending_num_Buff=$(($mins*"$FRAME_RATE"))
				if [ $FRAME_RATE=="60/1" ];then
					num_Buff_delta_pos=$(($NUM_BUFF+"$Delta_For_NumBuff_60FPS"))
					num_Buff_delta_neg=$(($NUM_BUFF-"$Delta_For_NumBuff_60FPS"))
				fi

				if [ $FRAME_RATE=="30/1" ];then
					num_Buff_delta_pos=$(($NUM_BUFF+"$Delta_For_NumBuff_30FPS"))
					num_Buff_delta_neg=$(($NUM_BUFF-"$Delta_For_NumBuff_30FPS"))
				fi

				if [ "$NUM_BUFF" != "NA" ]; then				
					if [ $Ending_num_Buff -gt $num_Buff_delta_pos ];then
						RESULT="FAIL"
						FAIL_REASON+="Pipe line not run for Total Num buffers."
					fi
					if [ $Ending_num_Buff -lt $num_Buff_delta_neg ];then
						RESULT="FAIL"
						FAIL_REASON+="Pipe line not run for Total Num buffers."
				fi	fi
			done
		fi

		grep -iE 'ERROR' $fps_log_file
		if [ $? -eq 0 ]; then
			RESULT="FAIL"
			FAIL_REASON="Error in pipeline playing."
		fi
		grep -iE 'erroneous pipeline' $fps_log_file
		if [ $? -eq 0 ]; then
			RESULT="FAIL"
			FAIL_REASON="Wrong element in pipeline."
		fi
		grep -iE 'Assertion|failed' $fps_log_file
		if [ $? -eq 0 ]; then
			RESULT="FAIL"
			FAIL_REASON="Assertion '0' failed in pipeline."
		fi
		grep -iE 'Failed to create Encoder' $fps_log_file
		if [ $? -eq 0 ]; then
			RESULT="FAIL"
			FAIL_REASON="Encoder Resource Error."
		fi
		grep -iE 'Cannot allocate memory' $fps_log_file
		if [ $? -eq 0 ]; then
			RESULT="FAIL"
			FAIL_REASON="CMA allocation Error."
		fi
		grep -iE ' corruption' $fps_log_file
		if [ $? -eq 0 ]; then
			RESULT="FAIL"
			FAIL_REASON="memory corruption"
		fi
		mins=$( grep -inr "Execution ended after " $fps_log_file )
		mins=$(echo $mins | awk -F"Execution ended after " '{print $2}' )
		mins=$(echo $mins | awk -F: '{ print ($1 * 3600) + ($2 * 60) + $3 }' )
		mins=$(echo $mins | awk -F. '{ print ($1) }' )
		Ending_num_Buff=$(($mins*"$FRAME_RATE"))
		if [ $FRAME_RATE=="60/1" ];then
			num_Buff_delta_pos=$(($NUM_BUFF+"$Delta_For_NumBuff_60FPS"))
			num_Buff_delta_neg=$(($NUM_BUFF-"$Delta_For_NumBuff_60FPS"))
		fi

		if [ $FRAME_RATE=="30/1" ];then
			num_Buff_delta_pos=$(($NUM_BUFF+"$Delta_For_NumBuff_30FPS"))
			num_Buff_delta_neg=$(($NUM_BUFF-"$Delta_For_NumBuff_30FPS"))
		fi
	
		if [ "$NUM_BUFF" != "NA" ]; then
			if [ $Ending_num_Buff -gt $num_Buff_delta_pos ];then
				RESULT="FAIL"
				FAIL_REASON+="Pipe line not run for Total Num buffers."
			fi
			if [ $Ending_num_Buff -lt $num_Buff_delta_neg ];then
				RESULT="FAIL"
				FAIL_REASON+="Pipe line not run for Total Num buffers."
			fi
		fi

		IDROPV=0
		EDROPV=0
		DROP=0
		if [ $TYPE != "multistream" -a $TYPE != "low_latency" -a $TYPE != "encode_fs" ]; then

			ln_count_1=$( grep -E '/GstPipeline:pipeline0/GstFPSDisplaySink:fpssink' $fps_log_file | sed -e 's/\r//g' | wc -l )

			if [ $ln_count_1 -ne 0 ]
			then
				for ((i=1; i<=20; i++))
				do
					ln=$( grep -E '/GstPipeline:pipeline0/GstFPSDisplaySink:fpssink' $fps_log_file |  awk 'NR=='${i} | sed -e 's/\r//g' )
					DROP=$(echo $ln | awk -F"," '{print $2}' | tr -d " " | tr -d "\t")
					IDROPV=$(echo $DROP | awk -F":" '{print $2}' | tr -d " " | tr -d "\t")
				done

				for (( i=21; i<=$ln_count_1; i++ ))
				do
					ln=$( grep -E '/GstPipeline:pipeline0/GstFPSDisplaySink:fpssink' $fps_log_file |  awk 'NR=='${i} | sed -e 's/\r//g' )
					echo -e $ln | tee -a /tmp/run.log
					DROP=$(echo $ln | awk -F"," '{print $2}' | tr -d " " | tr -d "\t")
					EDROPV=$(echo $DROP | awk -F":" '{print $2}' | tr -d " " | tr -d "\t")
				done
			fi
			if [ -f /tmp/run.log ]; then
				grep -iE 'A lot of buffers are being dropped.' /tmp/run.log  
				if [ $? -eq 0 ]; then
					RESULT="FAIL"
					FAIL_REASON+="A lot of buffers are being dropped"
				fi
				cp -f /tmp/run.log $op_path
				rm -f /tmp/run.log
			fi
		fi
		echo "________________________________________"
		echo "EDDROP = $EDROPV"
		echo "IDDROP = $IDROPV"
		if [ "$EDROPV" != "$IDROPV" ]; then
			RESULT="FAIL"
			FAIL_REASON+="Frame Drop"
		fi
		grep -iE 'killed' $fps_log_file
		if [ $? -eq 0 ]; then
			RESULT="FAIL"
			FAIL_REASON+="memory Error(out of memory)."
		fi

		FPS_VALIDATE=false

		if [ $TYPE == "decode" -o $TYPE == "capture_enc_dec" -o $TYPE == "transcode" -o $TYPE == "maxbitrate" ]; then
			FPS_VALIDATE=true
		fi

		if [ $TYPE == "low_latency" ] && [ $SOURCE != "File" ]; then
			FPS_VALIDATE=true
		fi

		if [ $TYPE == "multistream" ] && [ $SOURCE != "File" ]; then
			FPS_VALIDATE=true
		fi

		##################### FPS validation #####################
		if [ $FPS_VALIDATE == "true" ]; then

			let EXPT_FPS="$FRAME_RATE - 1"
			MIN_FPS=$EXPT_FPS
			LOW_FPS_COUNT=0
			ln_count=$( grep -E 'fpssink: last-message = rendered:' $fps_log_file | sed -e 's/\r//g' | wc -l )

			if [ $ln_count -ne 0 ]
			then
				for ((i=10; i<=$ln_count; i++))
				do
					ln=$( grep -E 'fpssink: last-message = rendered:' $fps_log_file |  awk 'NR=='${i} | sed -e 's/\r//g' )

					current_val=$( echo $ln | awk -F"," '{print $3}' | awk -F":" '{printf ( "%.2f", $2 )}' )
					awk 'BEGIN{if('$current_val' < '$EXPT_FPS') exit 1}'

					if [ $? -eq 1 ]
					then
						LOW_FPS_COUNT=$((LOW_FPS_COUNT+1))
						if [ "$(awk 'BEGIN{ print "'$current_val'" < "'$MIN_FPS'" }')" -eq "1" ]; then
							MIN_FPS=$current_val
							frame_no=$( echo $ln | awk -F"=" '{print $2}' | awk -F"," '{print $1}' | awk -F":" '{printf ( "%d", $2 )}' )
						fi
					fi

				done
			fi
			
			if [ "$(awk 'BEGIN{ print "'$MIN_FPS'" < "'$EXPT_FPS'" }')" -eq "1" ]; then
				RESULT="FAIL"
				FAIL_REASON+="Low FPS $LOW_FPS_COUNT times, min "$MIN_FPS" on "$frame_no" Frame"
			fi
		fi
		##########################################################

		if [ "$TYPE" == "low_latency" -o "$TYPE" == "capture_enc_dec" ]; then
			latency_count=0
			latency=$( cat /tmp/x.log  | awk -F"time=" '{print $2}' | awk -F")" '{print $2}' | tr -d "," | tee -a $op_path/latency_count.log )
			latency=$( sed -n '100p' < $op_path/latency_count.log )
		fi

		FAIL_REASON+="$FPS_FAIL_REASON"

		if [ $TYPE == "multistream" ]; then
			CMA_USED=`expr $CMA_FREE_BEFORE - $CMA_FREE_AFTER`
			CMA_USED=`expr $CMA_USED / 1024`
			CMA_USED="$CMA_USED MB"
			echo "CMA_USED=$CMA_USED"
			echo $CMA_USED > $op_dir/$TC_no/cma_used.txt
		fi
		mins=0
		echo -e "\t$Ending_num_Buff\t$RESULT\t$FAIL_REASON\t${latency}\t$CMA_USED" >> ${output_sheet}
		if [ $size -ne 0 ]; then
        	        echo ":::::::: Test Complete $single_test_execute :::::::::"
	        fi
	
	done < $LIST_FILE
}

process_playback () {

	file_name=`eval basename $LIST_FILE`
	if [ $TYPE == "live_playback" ]; then
		test_type="Live Playback tests"
	elif [ $TYPE == "file_playback" ]; then
		test_type="File Playback tests"
	fi

        output_sheet=$(echo $file_name | awk -F"_" '{print $1}' | tr -d " " | tr -d "\t")                                                      
        output_sheet="${output_sheet}_tests.xls"

	echo -e "\n\n\n\n\n\t\t$test_type" >> $output_sheet

	i=1

	echo -e "S.no\tTestcase\tTest type\tSource\tBitstream\tResolution & Fps\tCodec\tBitrate\tTarget bitrate\tControl rate\tGop Mode\tGOP length\tB_Frames\tFormat\tLatency mode\tProfile\tQP Mode\tNum slices\tNum buffers\tPrefetch buffer\tMode\tDisplay\tFile\tPipeline\tResults\tComments\tCMA USED" >> ${output_sheet}

	for tc in $run_list; do

		size=${#single_test_execute}

		NUM=$(echo $tc | awk -F"," '{print $1}' | tr -d " " | tr -d "\t")
		TC_NUM=$(echo $tc | awk -F"," '{print $2}' | tr -d " " | tr -d "\t")
	
		TYPE=$(echo $tc | awk -F"," '{print $3}' | tr -d " " | tr -d "\t")
	        SOURCE=$(echo $tc | awk -F"," '{print $4}' | tr -d " " | tr -d "\t")
        	INPUT_FILE_NAME=$(echo $tc | awk -F"," '{print $5}' | tr -d " " | tr -d "\t")
	        RESOLUTION=$(echo $tc | awk -F"," '{print $6}' | tr -d " " | tr -d "\t")
        	CODEC=$(echo $tc | awk -F"," '{print $7}' | tr -d " " | tr -d "\t")
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
        	PLAYBACK_MODE=$(echo $tc | awk -F"," '{print $21}' | tr -d " " | tr -d "\t")
		DISPLAY=$(echo $tc | awk -F"," '{print $22}' | tr -d " " | tr -d "\t")

		if [ $size -ne 0 -a "$single_test_execute" != "$TC_NUM" ]; then
			continue
		fi

		if [ "$RES" != "$RESOLUTION" ]; then
			continue
		fi

		RESULT="PASS"
		FAIL_REASON=""
		parse_playback_param $tc

		op_path="$op_dir/$TC_NUM"

		if [ -d "$op_path" ]; then 
			rm -rf $op_path
		fi

		mkdir -p $op_path
		fps_log_file="$op_path/output_fps.log"

		declare -a FILESINK_ARR=( "location"="$OUTPUT_FILE"  )
		export FILE_SINK=$(update_element_properties $FILE_SINK ${FILESINK_ARR[@]})

		FILE=$(echo $INPUT_FILE_NAME | awk -F"." '{print $1}' | tr -d " " | tr -d "\t")

		RUN_COMMAND="playback-exe"
	        if [ "$PLAYBACK_MODE" == "loop-play-eos" ]; then                                                                                     
        	        PLAYBACK_MODE="loop-playback eos-event"                                                                                      
	        fi 

               	echo -e "\n\t\t\t::::::::: Test $TC_NUM is started :::::::::"                                                                  

                if [ $TYPE == "file_playback" ] ;
                then
			PLAYBACK_FILE="${FILE}_${TC_NUM}_${CODEC}.mkv"

                        GST_ELEMENT=$FILE_SRC"|""qtdemux""|""h264parse""|""omxh264dec""|""queue max-size-bytes=0""|"$OMXHENC"|"$VIDEO_X_H"|"$CODEC_PARSE"|""matroskamux""|""filesink location="${op_path}/${PLAYBACK_FILE}""

        	        GST_COMMAND=$(create_gst_command "${GST_ELEMENT}")

	                echo -e "\n================================== Gst pipeline ==================================\n"                               
                                                                                                                                               
        	        echo $GST_COMMAND | tee -a $fps_log_file                                                                                       
                	echo -e "\n================================== Gst pipeline ==================================\n"                               
	                eval $GST_COMMAND 2>&1 | tee -a $fps_log_file 

			RUN_COMMAND+=" file $op_path/$PLAYBACK_FILE"                                                                             
			RUN_COMMAND+=" sink kmssink"                                                                                                   
			if [ "$DISPLAY" == "HDMI" ]; then
				RUN_COMMAND+=" hdmi_display"
			fi
		else
                                                                                               
	                # Modify this as TPD design changes
	                # 2018.2 Design :HDMI-->video0 || TPG--> video1 || MIPI-->video3
	                if [ "$SOURCE" == "HDMI" ]; then
        	                v4l2_node="/dev/video0"
	                elif [ "$SOURCE" == "TPG" ]; then
	                        v4l2_node="/dev/video1"
	                elif [ "$SOURCE" == "MIPI" ]; then
	                        v4l2_node="/dev/video0"
        	        else
                	        echo "Invalid v4l2 source." 
                        	exit -1
	                fi

        	        RUN_COMMAND+=" v4l2src $v4l2_node"

			echo "br= $MAX_BITRATE "
			echo "tr=$TARGET_BITRATE"                                                                                                             
                	if [ $CODEC == "AVC" ]; then
                        	RUN_COMMAND+=" serial avc"
	                else
        	                RUN_COMMAND+=" serial hevc"
                	fi
                                                                                                                                               
	                if [ "$RESOLUTION" != "NA" ]; then
        	                RUN_COMMAND+=" resolution $RESOLUTION"
                	fi
                                                                                                                                               
	                if [ "$PROFILE" != "NA" ]; then
        	                RUN_COMMAND+=" profile $PROFILE"
                	fi

	                if [ "$TARGET_BITRATE" != "NA" ]; then
        	                RUN_COMMAND+=" bitrate $TARGET_BITRATE"
                	fi                                                                             
                                                                                               
	                if [ "$CTRL_RATE" != "NA" ]; then
        	                RUN_COMMAND+=" control-rate $CTRL_RATE"
                	fi
                                                                                                                                               
	                if [ "$GOP_MODE" != "NA" ]; then
        	                RUN_COMMAND+=" gop-mode $GOP_MODE"
			fi
                                                                                                                                               
	                if [ "$GOP_LENGTH" != "NA" ]; then
        	                RUN_COMMAND+=" gop-len $GOP_LENGTH"
			fi

	                if [ "$B_FRAMES" != "NA" ]; then
        	                RUN_COMMAND+=" b-frames $B_FRAMES"
                	fi

	                RUN_COMMAND+=" slices 4"
                                                                                                                                               
        	        #Default HDMI display                                                                                                          
                	RUN_COMMAND+=" sink kmssink hdmi_display"
		fi

		RUN_COMMAND+=" latency-mode $LATENCY_MODE"

		if [ "$PLAYBACK_MODE" == "loop-seek" ]; then
			RUN_COMMAND+=" loop-seek 2000"
		else
			RUN_COMMAND+=" $PLAYBACK_MODE"
		fi
		
		if [ $split_input_flag == 1 ];then
			RUN_COMMAND+=" split-input $split_input"
		fi
		
		echo $RUN_COMMAND

		CMA_FREE_BEFORE=$(cat /proc/meminfo | grep CmaFree | awk -F":" '{printf("%d", $2)}')

	        # Run playback-exe
        	echo "################################# playback-exe PIPELINE #################################" 
	        echo "$RUN_COMMAND" | tee -a $fps_log_file

        	################################ Excel Sheet Output ################################
	        if [ "$TYPE" == "live_playback" ]; then
			PLAYBACK_FILE="NA"
			echo -ne "$NUM\t$TC_NUM\t$TYPE\t$SOURCE\t$INPUT_FILE_NAME\t$RESOLUTION\t$CODEC\t$TARGET_BITRATE\t$MAX_BITRATE\t$CTRL_RATE\t$GOP_MODE\t$GOP_LENGTH\t$B_FRAMES\t$FORMAT\t$LATENCY_MODE\t$PROFILE\t$QP_MODE\t$NUM_SLICE\t$NUM_BUFF\t$PREF_BUFF\t$PLAYBACK_MODE\t$DISPLAY\t$PLAYBACK_FILE\t$RUN_COMMAND\t" >> ${output_sheet}
	        else
			echo -ne "$NUM\t$TC_NUM\t$TYPE\t$SOURCE\t$INPUT_FILE_NAME\t$RESOLUTION\t$CODEC\t$TARGET_BITRATE\t$MAX_BITRATE\t$CTRL_RATE\t$GOP_MODE\t$GOP_LENGTH\t$B_FRAMES\t$FORMAT\t$LATENCY_MODE\t$PROFILE\t$QP_MODE\t$NUM_SLICE\t$NUM_BUFF\t$PREF_BUFF\t$PLAYBACK_MODE\t$DISPLAY\t$PLAYBACK_FILE\t\"$GST_COMMAND\t$RUN_COMMAND\"\t" >> ${output_sheet}
	        fi
        	####################################################################################

	        echo "################################################################################"
	       	eval "$RUN_COMMAND" 2>&1 | tee -a $fps_log_file &
	       	#eval "$RUN_COMMAND" 2>&1  &
                                                                              
	        sleep 2.5
        	CMA_FREE_AFTER=$(cat /proc/meminfo | grep CmaFree | awk -F":" '{printf("%d", $2)}')
	        
		if [ "$PLAYBACK_MODE" == "repeat-file-play" -o "$PLAYBACK_MODE" == "eos-event" ]; then
        	        #run test for 6min(File should not be longer than 6min)
                	echo "sleep"
	                sleep 360
        	else
                	#wait for 1min
	                echo "sleep" 
	       	        sleep 60
		fi
        	
		killall -9 playback-exe
		sleep 1
		
		CMA_USED=`expr $CMA_FREE_BEFORE - $CMA_FREE_AFTER`
		CMA_USED=`expr $CMA_USED / 1024` 
                                                                                                                                               
		grep -iE 'ERROR' $fps_log_file
		if [ $? -eq 0 ]; then
			RESULT="FAIL"
			FAIL_REASON="Error in pipeline playing."
		fi
		grep -iE 'erroneous pipeline' $fps_log_file
		if [ $? -eq 0 ]; then
			RESULT="FAIL"
			FAIL_REASON="Wrong element in pipeline."
		fi
		grep -iE 'Assertion|failed' $fps_log_file
		if [ $? -eq 0 ]; then
			RESULT="FAIL"
			FAIL_REASON="Assertion '0' failed in pipeline."
		fi
		grep -iE 'Failed to create Encoder' $fps_log_file
		if [ $? -eq 0 ]; then
			RESULT="FAIL"
			FAIL_REASON="Encoder Resource Error."
		fi
		grep -iE 'Cannot allocate memory' $fps_log_file
		if [ $? -eq 0 ]; then
			RESULT="FAIL"
			FAIL_REASON="CMA allocation Error."
		fi

		grep -iE 'A lot of buffers are being dropped.' $fps_log_file
		if [ $? -eq 0 ]; then
			RESULT="FAIL"
			FAIL_REASON="A lot of buffers are being dropped"
		fi
		grep -iE 'Out of memory:' $fps_log_file
		if [ $? -eq 0 ]; then
			RESULT="FAIL"
			FAIL_REASON="Memory error(out of memory)"
		fi
		grep -iE 'playback-exe: command not found' $fps_log_file
		if [ $? -eq 0 ]; then
			RESULT="FAIL"
			FAIL_REASON="playback-exe: application missing"
		fi
		grep -iE ' corruption' $fps_log_file
		if [ $? -eq 0 ]; then
			RESULT="FAIL"
			FAIL_REASON="memory corruption"
		fi
                                                                                                                               
		echo -e "$RESULT\t$FAIL_REASON\t$CMA_USED" >> ${output_sheet}

        	if [ "$TYPE" == "file_playback" ]; then                                                                                                       
			rm ${op_path}/${PLAYBACK_FILE}
	        fi                                                                                                                                                                                                                                                     
                                                                                                                                               
		mins=$( grep -inr "Execution ended after " $fps_log_file )
		mins=$(echo $mins | awk -F"Execution ended after " '{print $2}' )
		mins=$(echo $mins | awk -F: '{ print ($1 * 3600) + ($2 * 60) + $3 }' )
		mins=$(echo $mins | awk -F. '{ print ($1) }' )
		Ending_num_Buff=$(($mins*"$FRAME_RATE"))
		if [ $FRAME_RATE=="60/1" ];then
			num_Buff_delta_pos=$(($NUM_BUFF+"$Delta_For_NumBuff_60FPS"))
			num_Buff_delta_neg=$(($NUM_BUFF-"$Delta_For_NumBuff_60FPS"))
		fi

		if [ $FRAME_RATE=="30/1" ];then
			num_Buff_delta_pos=$(($NUM_BUFF+"$Delta_For_NumBuff_30FPS"))
			num_Buff_delta_neg=$(($NUM_BUFF-"$Delta_For_NumBuff_30FPS"))
		fi

		echo "******************************"
		echo "Ending_num_Buff = $Ending_num_Buff"
		echo "num_Buff_delta_pos = $num_Buff_delta_pos"
		echo "num_Buff_delta_neg = $num_Buff_delta_neg"
		echo "NUM_BUFF = $NUM_BUFF"				
		if [ "$NUM_BUFF" != "NA" ]; then
			if [ $Ending_num_Buff -gt $num_Buff_delta_pos ];then
				RESULT="FAIL"
				FAIL_REASON="Pipe line not run for Total Num buffers."
			fi
			if [ $Ending_num_Buff -lt $num_Buff_delta_neg ];then
				RESULT="FAIL"
				FAIL_REASON="Pipe line not run for Total Num buffers."
			fi
		fi
	        if [ $size -ne 0 ]; then                                                                                                               
        	        echo ":::::::: Test Complete $single_test_execute :::::::::"
	        fi
	done
}

# Main Starts From Here
TYPE=""
LIST_FILE=""
single_test_execute=""
RES="4kp60"
io_mode=4
split_input="False"

if [ $# -eq 0 ]; then
	usage
fi

# Command Line Argument Parsing
args=$(getopt -o "t:l:h:s:r:m:i:" --long "type:,list-file:,help:,ste:,res:,mode:,sinput" -- "$@")

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
			if [ "$TYPE" != "encode_fs" -a "$TYPE" != "decode" -a "$TYPE" != "low_latency" -a "$TYPE" != "transcode" -a "$TYPE" != "capture_enc_dec" -a "$TYPE" != "multistream" -a "$TYPE" != "maxbitrate" -a "$TYPE" != "file_playback" -a "$TYPE" != "live_playback" ]; then
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
		-r | --res)
			RES=$2;
			if [ ! -z "$RES" ]; then
				if [ "$RES" != "4kp60" -a "$RES" != "4kp30" -a "$RES" != "1080p60" -a "$RES" != "1080p30" ]; then
					ErrorMsg "Invalid Resolution."
				fi
			fi
			shift; shift;
			;;
		-m | --mode)
			io_mode=$2;
			shift; shift;
			;;
		-i | --sinput)
			split_input=$2;
			split_input_flag=1
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

# Execute function to create the encoded output file
if [ "$TYPE" == "decode" ]; then
	process_params
elif [ "$TYPE" == "encode_fs" ]; then
	process_params
elif [ "$TYPE" == "capture_enc_dec" ]; then
	process_params
elif [ "$TYPE" == "transcode" ]; then
	process_params
elif [ "$TYPE" == "multistream" ]; then
	process_params
elif [ "$TYPE" == "low_latency" -o "$TYPE" == "maxbitrate" ]; then
	process_params
elif [ $TYPE == "file_playback" -o $TYPE == "live_playback" ]; then
	source $lst_run
	process_playback
else
	# Apply list file to get list of cfg files in run_lists
	killall -9 gst-launch-1.0
	source $lst_run
fi
