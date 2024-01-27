# Gstreamer Tests L0, L1, L2

## Usage

```

Folders

list: consists of list files
L0 : Level 0 test cases (contains basic sanity level tests)
L1 : Level 1 test cases (contains some more parameters changes for particular tests)
L2 : Level 2 test cases (consists of all major combinations)

run: consists of execution scripts
./gst_run.sh -t <type> -l <listfile> -m <mode>
type: decode, encode_fs, capture_enc_dec, transcode, multistream, low_latency, file_playback, live_playback, maxbitrate

decode (decode display usecase) : Decode encoded file (AVC, HEVC) to YUV file or decode->display to monitor
encode_fs (encode filesink) : Encode raw YUV file to AVC/HEVC file
capture_enc_dec (serial usecase capture->encode->decode->display) : Capturing live raw data, encoding and again decode display
transcode : Converting one codec file to another codec or decoding and then reencoding with modified parameters ( AVC -> HEVC / HEVC -> AVC )
multistream : more than one encode/decode/serial streams at time
low_latency : serial usecases using latency mode=low_latency
file_playback : decode display file based usecase (loop-playback, close the pipeline with eos-event, repeat-file-play, loop-seek)
live_playback : live serial usecases with ctrl+c. Run serial usecase and close using ctrl+c
maxbitrate : finding maximum bitrate on which we are getting expected FPS

./gst_run_dynamic.sh -t <type> -l <listfile> -m <mode>
type: dynamic

Dynamic : For dynamic feature test
Dynamic features include dynamic bitrate, dynamic GOP, and ROI.

listfile: specify list file to be used for passing parameters

L0 tests
-----------------
./gst_run.sh -t decode -l l0_decode.lst
./gst_run.sh -t encode_fs -l l0_encode.lst
./gst_run.sh -t capture_enc_dec -l l0_capture.lst
./gst_run.sh -t transcode -l l0_transcode.lst
./gst_run.sh -t multistream -l l0_multistream.lst
./gst_run.sh -t low_latency -l l0_low_latency.lst
./gst_run.sh -t file_playback -l l0_file_playback.lst
./gst_run.sh -t live_playback -l l0_live_playback.lst
./gst_run.sh -t maxbitrate -l l0_maxbitrate.lst 
./gst_run_dynamic.sh -t dynamic -l l0_dynamic.lst

L1 tests
-----------------
./gst_run.sh -t decode -l l1_decode.lst
./gst_run.sh -t encode_fs -l l1_encode.lst
./gst_run.sh -t capture_enc_dec -l l1_capture.lst
./gst_run.sh -t transcode -l l1_transcode.lst
./gst_run.sh -t multistream -l l1_multistream_file.lst
./gst_run.sh -t multistream -l l1_multistream_live.lst
./gst_run.sh -t low_latency -l l1_low_latency_file.lst
./gst_run.sh -t low_latency -l l1_low_latency_live.lst
./gst_run.sh -t maxbitrate -l l1_maxbitrate.lst

L2 tests
-----------------
./gst_run.sh -t maxbitrate -l l2_bitrate_capture.lst
./gst_run.sh -t maxbitrate -l l2_bitrate_decode.lst
./gst_run.sh -t maxbitrate -l l2_bitrate_serial.lst
./gst_run.sh -t capture_enc_dec -l l2_capture.lst
./gst_run.sh -t capture_enc_dec -l l2_capture_long.lst
./gst_run.sh -t decode -l l2_decode_long.lst
./gst_run.sh -t multistream -l l2_multisteam_long.lst
./gst_run.sh -t file_playback -l l2_playback_file.lst
./gst_run.sh -t live_playback -l l2_playback_live.lst
./gst_run_dynamic.sh -t dynamic -l l0_dynamic.lst

Running all L0 tests
---------------------
./run_full_regression.sh
```
