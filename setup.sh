sh installer.sh
sh -c "$(curl -fsSL https://raw.githubusercontent.com/ohmyzsh/ohmyzsh/master/tools/install.sh)" "" --unattended
cp .xinitrc ~/
cp .zshrc ~/
sudo cp config.toml /etc/greetd/config.toml
cd dwm
sudo make clean install
cd ~/enterdavidsvoid/davidstatus
sudo make clean install
mkdir ~/Downloads
cp background.png ~/Downloads/background.png
sudo chsh -s $(which zsh)
sudo chown -R davidwmoore /home/davidwmoore
git clone https://github.com/LazyVim/starter ~/.config/nvim
rm -rf ~/.config/nvim/.git
sudo ln -s /etc/sv/dbus/ /var/service
sudo ln -s /etc/sv/NetworkManager/ /var/service
sudo ln -s /etc/sv/greetd/ /var/service
