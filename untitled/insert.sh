#! /bin/sh

source setup.sh
mknod /dev/motor c 61 0
insmod motor.ko
insmod pot.ko
./light3 &
./untitled -qws &
