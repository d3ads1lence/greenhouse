#!/bin/sh

gnome-terminal -x sh -c "JLinkRTTClient; RTTClient"
JLinkExe -device NRF52840_XXAA -if SWD -speed 4000 -autoconnect 1
