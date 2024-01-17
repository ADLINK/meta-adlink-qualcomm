#!/bin/sh


for i in 0 1 2 3 4 5 6 7
do
   var=$(cat /sys/devices/system/cpu/cpu$i/cpufreq/scaling_cur_freq )
   echo -e "cpu $i : $var Hz"
done
