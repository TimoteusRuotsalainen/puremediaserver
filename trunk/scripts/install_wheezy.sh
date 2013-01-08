#!/bin/bash
##############################################################################
# Script de instalación de las dependencias de Libre Media Server 
# en Debian Wheezy. 
# Necesita root para ejecutar
# (c) 2012 Santi Noreña
# GPL License 
###############################################################################
# Comprobamos root
(( EUID )) && echo ‘You need to be root.’ && exit 1
#Instalamos el paquete suministrado con el release
dpkg -i ola_0.8.26-1_i386.deb
# Install Pure Data y OLA
# libav para generar thumbs
apt-get install puredata gem tcl tk libav-tools pd-pdogg pd-pan
if [ "$?" -eq "0" ]; then 
	echo "apt-get finish ok"
	else "apt-get return errors!"	
	fi
apt-get -f install
