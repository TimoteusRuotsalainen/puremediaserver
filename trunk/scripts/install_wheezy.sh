# Comprobamos que el repositorio no está
# Hasta que no haya repositorio...
#cat /etc/apt/sources.list | while read fileline
#do
#if ["$fileline" == "deb http://apt.openlighting.org/debian/ wheezy main"]; then
#	exit		
#fi
#done

# Añadimos el repositorio ola
#echo "deb http://apt.openlighting.org/debian/ wheezy main" >> /etc/apt/sources.list

# Instalamos el paquete suministrado con el release
dpkg -i ola_0.8.26-1_i386.deb

# Install Pure Data y OLA
# libav para generar thumbs

apt-get install puredata gem tcl tk pd-zexy pd-cyclone pd-hcs libav-tools 

apt-get -f install
