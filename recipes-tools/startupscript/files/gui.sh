#!/bin/sh

#remove delay in weston bringup and force weston from background

systemctl start weston
#openvt -c 1 -f weston &
