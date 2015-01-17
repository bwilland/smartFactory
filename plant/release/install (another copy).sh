#!/bin/sh

cd /plant
rm -rf www
rm smartplant


cd /plant/update

chmod -R 666 www


cp -r www ../
rm -rf www
mv * ../



cd /plant
chmod 666 plant.db


echo "--" >log/ErrorLog


chmod 777 smartplant



rm -rf update
rm update.tar.gz
rm install.sh


