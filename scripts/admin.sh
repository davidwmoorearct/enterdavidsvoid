sudo useradd greeter -G video
mkdir ~/Downloads
ln -s ~/enterdavidsvoid/background.png ~/Downloads/background.png
fc-cache -fv
sudo chsh -s $(which zsh)
sudo ln -s /etc/sv/dbus/ /var/service
sudo ln -s /etc/sv/NetworkManager/ /var/service
sudo ln -s /etc/sv/greetd/ /var/service
