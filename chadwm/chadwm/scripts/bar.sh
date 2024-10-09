#!/bin/dash

# ^c$var^ = fg color
# ^b$var^ = bg color

interval=0

# load colors
. ~/.config/chadwm/scripts/bar_themes/onedark

cpu() {
	cpu_val=$(grep -o "^[^ ]*" /proc/loadavg)
	printval=$(echo "%")
	printf "^c$black^ ^b$green^ "
	printf "^c$white^ ^b$grey^ $cpu_val $printval"
}

pkg_updates() {
	updates=$({ timeout 20 xbps-install -un 2>/dev/null || true; } | wc -l) # void
	#updates=$({ timeout 20 checkupdates 2>/dev/null || true; } | wc -l) # arch
	# updates=$({ timeout 20 aptitude search '~U' 2>/dev/null || true; } | wc -l)  # apt (ubuntu, debian etc)

	if [-z "$updates"]; then
		printf "  ^c$green^    Fully Updated"
	else
		printf "  ^c$green^    $updates"" Updates"
	fi
}

battery() {
	chargestat="$(cat /sys/class/power_supply/BAT0/status)"
	get_capacity="$(cat /sys/class/power_supply/BAT0/capacity)"
	printf "^c$blue^   $get_capacity $chargestat"
}

brightness() {
	printf "^c$red^   "
	printf "^c$red^%.0f\n" $(cat /sys/class/backlight/*/brightness)
}

mem() {
	printf "^c$blue^^b$black^  "
	printf "^c$blue^ $(free -h | awk '/^Mem/ { print $3 }' | sed s/i//g)"
}

wlan() {
	netname = "$(iwgetid -r)"
	case "$(cat /sys/class/net/wl*/operstate 2>/dev/null)" in
	up) printf "^c$black^ ^b$blue^ 󰤨 ^d^%s" " ^c$blue^$(iwgetid -r)" ;;
	down) printf "^c$black^ ^b$blue^ 󰤭 ^d^%s" " ^c$blue^Disconnected" ;;
	esac
}

clock() {
	printf "^c$black^ ^b$darkblue^ 󱑆 "
	printf "^c$black^^b$blue^ $(date '+%H:%M')  "
}

while true; do

	[ $interval = 0 ] || [ $(($interval % 3600)) = 0 ] && updates=$(pkg_updates)
	interval=$((interval + 1))

	sleep 1 && xsetroot -name "$(pkg_updates) $(battery) $(brightness) $(cpu) $(mem) $(wlan) $(clock)"
done
