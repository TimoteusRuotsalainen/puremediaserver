#!/bin/bash
# Comprobamos root
(( EUID )) && echo ‘You need to be root.’ && exit 1
# copia de seguridad de sources.list
cp /etc/apt/sources.list /etc/apt/sources.list.old
# Comprobamos que el repositorio no está en soucers.list
output=`grep apt.openlighting.org /etc/apt/sources.list | wc -l`
if [ "$output" -eq "0" ]; then
	#Añadimos el repositorio ola a sources.list
	echo "deb http://apt.openlighting.org/debian/ squeeze main" >> /etc/apt/sources.list
fi
apt-get update
# Install Pure Data and OLA
# libav para generar thumbs
apt-get -y install ola puredata gem pd-zexy 
#necesarios, ¿dónde están en squeeze?????s
# pd-cyclone pd-hcs libav-tools pd-pdogg pd-pan
if [ "$?" -eq "0" ]; then 
	echo "apt-get finish ok"
	else "apt-get return errors!"	
	fi

