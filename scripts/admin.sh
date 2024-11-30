sudo useradd greeter -G video
mkdir ~/Downloads
ln -s ~/enterdavidsvoid/config/background.png ~/Downloads/background.png
fc-cache -fv
sudo chsh -s $(which zsh)
flatpak remote-add --if-not-exists fedora oci+https://registry.fedoraproject.org
flatpak remote-add --system elementary https://flatpak.elementary.io/repo.flatpakrepo
flatpak remote-add --if-not-exists flathub https://dl.flathub.org/repo/flathub.flatpakrepo
sudo rm /var/service/wpa_supplicant
sudo ln -s /etc/sv/dbus/ /var/service
sudo ln -s /etc/sv/NetworkManager/ /var/service
sudo ln -s /etc/sv/greetd/ /var/service
