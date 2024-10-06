sh installer.sh
sh -c "$(curl -fsSL https://raw.githubusercontent.com/ohmyzsh/ohmyzsh/master/tools/install.sh)" "" --unattended
cp ~/enterdavidsvoid/chadwm/scripts/run.sh ~/.xinitrc
cp .zshrc ~/
sudo cp config.toml /etc/greetd/config.toml
cd chadwm/chadwm
sudo make clean install
cd ~/enterdavidsvoid/slock/
sudo make clean install
cp .p10k.zsh ~/
git clone https://github.com/zsh-users/zsh-autosuggestions ${ZSH_CUSTOM:-~/.oh-my-zsh/custom}/plugins/zsh-autosuggestion
git clone https://github.com/zsh-users/zsh-syntax-highlighting.git ${ZSH_CUSTOM:-~/.oh-my-zsh/custom}/plugins/zsh-syntax-highlighting
git clone https://github.com/jeffreytse/zsh-vi-mode \$ZSH_CUSTOM/plugins/zsh-vi-mode
git clone --depth=1 https://github.com/romkatv/powerlevel10k.git ${ZSH_CUSTOM:-$HOME/.oh-my-zsh/custom}/themes/powerlevel10k
mkdir ~/Downloads
cp background.png ~/Downloads/background.png
sudo chsh -s $(which zsh)
sudo chown -R davidwmoore /home/davidwmoore
sudo ln -s /etc/sv/dbus/ /var/service
sudo ln -s /etc/sv/NetworkManager/ /var/service
sudo ln -s /etc/sv/greetd/ /var/service
