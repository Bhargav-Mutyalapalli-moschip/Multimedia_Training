#!/bin/bash

rmmod al5e
rmmod al5d
modprobe al5e
modprobe al5d
rmmod dmaproxy
modprobe dmaproxy

