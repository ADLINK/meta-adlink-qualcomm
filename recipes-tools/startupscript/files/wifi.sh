#!/bin/sh

ip link set wlp1s0 down
ip link set uap0 down
ip link set wfd0 down
rmmod moal
rmmod mlan
#insmod /etc/mlan.ko
#insmod /etc/moal.ko cfg80211_wext=0xf  cal_data_cfg=none fw_name=nxp/pcieuart8997_combo_v4.bin

modprobe mlan
modprobe moal cfg80211_wext=0xf  cal_data_cfg=none fw_name=nxp/pcieuart8997_combo_v4.bin

hciattach /dev/ttyHS0 any 115200 flow
hciconfig hci0 up
