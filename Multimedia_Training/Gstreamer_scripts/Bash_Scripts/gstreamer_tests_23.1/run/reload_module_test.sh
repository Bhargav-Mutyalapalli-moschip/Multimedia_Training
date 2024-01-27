#!/bin/bash

rmmod al5d
rmmod al5e
rmmod allegro
rmmod xlnx_vcu
rmmod xlnx_vcu_core
rmmod xlnx_vcu_clk
modprobe xlnx_vcu_clk
modprobe xlnx_vcu_core
modprobe xlnx_vcu
modprobe allegro
modprobe al5e
modprobe al5d
