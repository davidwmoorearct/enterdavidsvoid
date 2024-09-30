sudo xbps-install -Sy $(cat packages.txt)
sh -c "$(curl -fsSL https://raw.githubusercontent.com/ohmyzsh/ohmyzsh/master/tools/install.sh)" "" --unattended
cp .xinitrc ~/
cp .zshrc ~/
sudo cp config.toml /etc/greetd/config.toml
cd dwm
sudo make clean install
cd .
cd davidstatus
sudo make clean install
