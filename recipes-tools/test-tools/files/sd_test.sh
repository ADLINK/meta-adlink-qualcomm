#!/bin/bash
echo "Script started"
while true; do dd if=/dev/mmcblk0 of=/dev/null conv=fdatasync bs=512; done




