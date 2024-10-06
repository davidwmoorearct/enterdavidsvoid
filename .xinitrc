feh --bg-scale ~/Downloads/background.png & 
xautolock -time 5 -locker slock &
xrandr --output DisplayPort-2 --mode 2560x1440 --rate 144
slstatus &
pipewire &
sunshine &
exec dbus-run-session dwm
