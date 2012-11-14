# Añadimos el repositorio ola
echo "deb http://apt.openlighting.org/debian/ wheeze main" >> /etc/apt/sources.list

# Install Pure Data y OLA
# libav para generar thumbs
sudo apt-get update
sudo apt-get install ola puredata gem tcl tk pd-zexy pd-cyclone pd-hcs libav-tools


