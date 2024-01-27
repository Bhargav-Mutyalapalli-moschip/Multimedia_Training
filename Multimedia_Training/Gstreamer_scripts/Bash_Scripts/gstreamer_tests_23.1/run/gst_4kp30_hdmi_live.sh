#!/bin/bash

#l0
./gst_run.sh -t capture_enc_dec -l ../list/l0/l0_capture_rec.lst -r 4kp30
./gst_run.sh -t capture_enc_dec -l ../list/l0/l0_capture_rec.lst -r 4kp30 -m 5
./gst_run.sh -t capture_enc_dec -l ../list/l0/l0_capture_nr.lst -r 4kp30
./gst_run.sh -t capture_enc_dec -l ../list/l0/l0_capture_nr.lst -r 4kp30 -m 5
./gst_run.sh -t low_latency -l ../list/l0/l0_low_latency.lst -r 4kp30
./gst_run.sh -t low_latency -l ../list/l0/l0_low_latency.lst -r 4kp30 -m 5
./gst_run.sh -t multistream -l ../list/l0/l0_multistream_live.lst -r 4kp30
./gst_run.sh -t multistream -l ../list/l0/l0_multistream_live.lst -r 4kp30 -m 5

#l1
#./gst_run.sh -t capture_enc_dec -l ../list/l1/l1_capture_rec.lst -r 4kp30
#./gst_run.sh -t capture_enc_dec -l ../list/l1/l1_capture_rec.lst -r 4kp30 -m 5
#./gst_run.sh -t capture_enc_dec -l ../list/l1/l1_capture_nr.lst -r 4kp30
#./gst_run.sh -t low_latency -l ../list/l1/l1_low_latency_live.lst -r 4kp30
#./gst_run.sh -t multistream -l ../list/l1/l1_multistream_live.lst -r 4kp30

#l2
#./gst_run.sh -t capture_enc_dec -l ../list/l2/l2_capture_rec.lst -r 4kp30
#./gst_run.sh -t capture_enc_dec -l ../list/l2/l2_capture_rec.lst -r 4kp30 -m 5
#./gst_run.sh -t capture_enc_dec -l ../list/l2/l2_capture_nr.lst -r 4kp30
#./gst_run.sh -t live_playback -l ../list/l2/l2_playback_live.lst -r 4kp30

sleep 10
