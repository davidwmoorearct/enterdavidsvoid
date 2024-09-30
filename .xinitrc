feh --bg-scale ~/Downloads/background.png & 
xautolock -time 5 -locker slock &
slstatus &
exec dbus-run-session dwm
