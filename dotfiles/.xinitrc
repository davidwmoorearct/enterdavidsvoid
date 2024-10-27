#!/bin/sh

xrdb merge ~/.Xresources
xrandr -- output DisplayPort-2 --mode 2560x1440 --rate 144
xbacklight -set 100 &
feh --bg-fill ~/Downloads/background.png &
xset r rate 200 50 &
picom &
pipewire &
cbatticon &
dash ~/.config/chadwm/scripts/bar.sh &
while type chadwm >/dev/null; do dbus-run-session chadwm && continue || break; done
