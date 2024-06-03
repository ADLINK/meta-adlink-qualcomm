#!/bin/sh


CWD=$(pwd)
PROGNAME="$CWD/setup-environment"


MACHINE=$MACHINE DISTRO=$DISTRO source $PROGNAME $@

if [ -f ../meta-adlink-qualcomm/conf/adlink-conf/$MACHINE/bblayers.conf.append ]; then
	grep "meta-adlink-qualcomm" ./conf/bblayers.conf  > /dev/null 2>&1 || cat ../meta-adlink-qualcomm/conf/adlink-conf/$MACHINE/bblayers.conf.append >> ./conf/bblayers.conf 
fi
if [ -f ../meta-adlink-qualcomm/conf/adlink-conf/$MACHINE/local.conf.append ]; then
	grep "adlinkweston" ./conf/local.conf > /dev/null 2>&1 || cat ../meta-adlink-qualcomm/conf/adlink-conf/$MACHINE/local.conf.append >> ./conf/local.conf
fi

