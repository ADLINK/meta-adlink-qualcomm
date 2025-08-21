#!/bin/sh

###Enable adb for usb peripheral mode###

lsmod | grep libcomposite
var=$?
if [ $var -eq 0 ]
then
	exit 
fi

modprobe libcomposite
mount -t configfs none /sys/kernel/config
mkdir -p /sys/kernel/config/usb_gadget/g1
echo 0x0200 > /sys/kernel/config/usb_gadget/g1/bcdUSB
mkdir -p /sys/kernel/config/usb_gadget/g1/strings/0x409
echo 1 > /sys/kernel/config/usb_gadget/g1/os_desc/use 1
echo "0x18d1" > /sys/kernel/config/usb_gadget/g1/idVendor
echo "0xd002" > /sys/kernel/config/usb_gadget/g1/idProduct	
echo "0x5113f6b7" > /sys/kernel/config/usb_gadget/g1/strings/0x409/serialnumber
echo "Qualcomm Inc" > /sys/kernel/config/usb_gadget/g1/strings/0x409/manufacturer
echo "qrb5165 Prod" >  /sys/kernel/config/usb_gadget/g1/strings/0x409/product
mkdir -p /sys/kernel/config/usb_gadget/g1/functions/ffs.adb
mkdir -p /sys/kernel/config/usb_gadget/g1/configs/b.1
mkdir -p /sys/kernel/config/usb_gadget/g1/configs/b.1/strings/0x409
echo "ffs.adb" > /sys/kernel/config/usb_gadget/g1/configs/b.1/strings/0x409/configuration
ln -s /sys/kernel/config/usb_gadget/g1/functions/ffs.adb /sys/kernel/config/usb_gadget/g1/configs/b.1
mkdir -p /dev/usb-ffs
mkdir -p /dev/usb-ffs/adb
mount -t functionfs adb /dev/usb-ffs/adb
start-stop-daemon --start --background --oknodo --quiet --exec /usr/bin/adbd
echo 0x1 > /sys/kernel/config/usb_gadget/g1/os_desc/b_vendor_code
sleep 2
echo "a600000.usb" > /sys/kernel/config/usb_gadget/g1/UDC
