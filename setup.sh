sh installer.sh
sh -c "$(curl -fsSL https://raw.githubusercontent.com/ohmyzsh/ohmyzsh/master/tools/install.sh)" "" --unattended
cp .xinitrc ~/
cp .zshrc ~/
sudo cp config.toml /etc/greetd/config.toml
cd dwm
sudo make clean install
cd ~/david-dwm/davidstatus
sudo make clean install
mkdir ~/Downloads
cp .background.png ~/Downloads/background.png
sudo chsh -s $(which zsh)
sudo ln -s /var/service/NetworkManager/ /etc/sv
sudo ln -s /var/service/greetd /etc/sv
