#!/bin/bash
sudo xbps-install -Suy
filename=~/'enterdavidsvoid/config/t480packages.txt'
while read line; do
  sudo xbps-install $line -y
done <$filename
