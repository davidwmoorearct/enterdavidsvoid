#!/bin/bash
sudo -v
while true; do sudo -n true; sleep 60; kill -0 "$$" || exit; done 2>/dev/null &
sh ~/enterdavidsvoid/scripts/installer.sh
sh ~/enterdavidsvoid/scripts/dotfiles.sh
sh ~/enterdavidsvoid/scripts/suckless.sh
sh ~/enterdavidsvoid/scripts/zplugins.sh
sh ~/enterdavidsvoid/scripts/admin.sh

