#!/bin/sh

xrdb merge ~/.Xresources
xrandright -set 255 &
feh --bg-fill ~/Downloads/background.png &
xset r rate 200 50 &
picom --vsync --backend=glx  &
xautolock -time 5 &
pipewire &
cbatticon &
dash ~/.config/chadwm/scripts/bar.sh &
nm-tray &
blueman-tray &
export $XDG_SESSION_TYPE="x11"
while type chadwm >/dev/null; do dbus-run-session chadwm && continue || break; done
#sxbar & 
#sxwm
