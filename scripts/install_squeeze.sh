#!/bin/bash
##############################################################################
# Script de instalación de las dependencias de Pure Media Server 
# en Debian Squeeze. 
# Necesita root para ejecutar
# (c) 2012 Santi Noreña
# GPL License 
###############################################################################
# Comprobamos root
(( EUID )) && echo ‘You need to be root.’ && exit 1
# Instalamos OLA desde el repositorio del proyecto
# copia de seguridad de sources.list
cp /etc/apt/sources.list /etc/apt/sources.list.old
# Comprobamos que el repositorio no está en soucers.list
output=`grep apt.openlighting.org /etc/apt/sources.list | wc -l`
if [ "$output" -eq "0" ]; then
	#Añadimos el repositorio ola a sources.list
	echo "deb http://apt.openlighting.org/debian/ squeeze main" >> /etc/apt/sources.list
fi
apt-get update
apt-get -y install ola puredata tcl tk pd-zexy
if [ "$?" -eq "0" ]; then 
	echo "apt-get finish ok"
	else "apt-get return errors!"	
	fi



