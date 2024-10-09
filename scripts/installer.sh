#!/bin/bash
sudo xbps-install -Suy
filename='packages.txt'
while read line; do
  sudo xbps-install $line -y
done <$filename
