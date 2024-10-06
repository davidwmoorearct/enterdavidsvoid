#!/bin/sh

xrdb merge ~/.Xresources
xbacklight -set 100 &
feh --bg-fill ~/Downloads/background.png &
xset r rate 200 50 &
picom &

dash ~/.config/chadwm/scripts/bar.sh &
while type chadwm >/dev/null; do chadwm && continue || break; done
