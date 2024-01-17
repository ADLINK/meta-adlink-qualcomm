#!/bin/sh

#TOKEN_PRE="ghp_"
#TOKEN_POST="DiFPUbMC2PjG06sFvpuDrsBdJAH5bO0bSQd9"

CWD=$(pwd)
PROGNAME="$CWD/setup-environment"


MACHINE=$MACHINE DISTRO=$DISTRO source $PROGNAME $@

if [ -f ../layers/meta-adlink-qualcomm/conf/adlink-conf/$MACHINE/bblayers.conf.append ]; then
	cat ../layers/meta-adlink-qualcomm/conf/adlink-conf/$MACHINE/bblayers.conf.append >> ./conf/bblayers.conf
fi
if [ -f ../layers/meta-adlink-qualcomm/conf/adlink-conf/$MACHINE/local.conf.append ]; then
	cat ../layers/meta-adlink-qualcomm/conf/adlink-conf/$MACHINE/local.conf.append >> ./conf/local.conf
fi

#echo "PA_USER ?= \"adlink-guest\"" >> ./conf/local.conf
#echo "PA_TOKEN ?= \""${TOKEN_PRE}${TOKEN_POST}"\"" >> ./conf/local.conf
