#!/usr/sbin/bash
while true; do sudo -n true; sleep 60; kill -0 "$$" || exit; > done 2>/dev/null &
sh installer.sh
sh -c "$(curl -fsSL https://raw.githubusercontent.com/ohmyzsh/ohmyzsh/master/tools/install.sh)" "" --unattended
sudo useradd greeter -G video
ln -s ~/enterdavidsvoid/.xinitrc ~/.xinitrc
ln -s ~/enterdavidsvoid/.zshrc ~/.zshrc
sudo ln -s ~/enterdavidsvoid/config.toml /etc/greetd/config.toml
sudo make clean install -C ~/enterdavidsvoid/chadwm/chadwm
sudo make clean install -C ~/enterdavidsvoid/st
sudo make clean install -C ~/enterdavidsvoid/slock
sudo make clean install -C ~/enterdavidsvoid/scroll
ln -s ~/enterdavidsvoid/.p10k.zsh ~/
ln -s ~/enterdavidsvoid/chadwm/ ~/.config/
git clone https://github.com/zsh-users/zsh-autosuggestions ${ZSH_CUSTOM:-~/.oh-my-zsh/custom}/plugins/zsh-autosuggestion
git clone https://github.com/zsh-users/zsh-syntax-highlighting.git ${ZSH_CUSTOM:-~/.oh-my-zsh/custom}/plugins/zsh-syntax-highlighting
git clone https://github.com/jeffreytse/zsh-vi-mode \$ZSH_CUSTOM/plugins/zsh-vi-mode
git clone --depth=1 https://github.com/romkatv/powerlevel10k.git ${ZSH_CUSTOM:-$HOME/.oh-my-zsh/custom}/themes/powerlevel10k
mkdir ~/Downloads
ln -s ~/enterdavidsvoid/background.png ~/Downloads/background.png
sudo chsh -s $(which zsh)
sudo ln -s /etc/sv/dbus/ /var/service
sudo ln -s /etc/sv/NetworkManager/ /var/service
sudo ln -s /etc/sv/greetd/ /var/service
