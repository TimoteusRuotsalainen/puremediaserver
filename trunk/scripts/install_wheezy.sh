# Install Pure Data
sudo apt-get update
sudo apt-get install puredata gem tcl tk pd-zexy pd-cyclone pd-hcs 

# Añadimos el repositorio ola - Todavía no hay para Wheezy
#echo "deb http://apt.openlighting.org/debian/ squeeze main" >> /etc/apt/sources.list

# Compiling and installing Ola

sudo apt-get install libmicrohttpd10 libmicrohttpd-dev flex bison protobuf-compiler libprotobuf7 protobuf-c-compiler libprotoc-dev uuid-dev libcppunit-dev pkg-config libncurses5-dev libtool autoconf automake  g++ python-protobuf make zlib1g-dev

wget http://open-lighting.googlecode.com/files/ola-0.8.23.tar.gz
tar -xf ola-0.8.23.tar.gz
cd ola-0.8.23.tar.gz
./configure
make
sudo make install
cd ..

# Add enviroment path variables to .bashrc
echo "export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib" >> ~/.bashrc
echo "export CPLUS_INCLUDE_PATH=$CPLUS_INCLUDE_PATH:/usr/local/include" >> ~/.bashrc 
echo "export C_INCLUDE_PATH=$C_INCLUDE_PATH:/usr/local/include" >> ~/.bashrc

# Instalación y compilación de flext

wget http://crca.ucsd.edu/~msp/Software/pd-0.43-2.src.tar.gz
tar -xf pd-0.43-2.src.tar.gz
sudo apt-get install subversion
svn co https://svn.grrrr.org/ext/trunk/flext flext
cd flext
./build.sh pd gcc

# Edit the path to pd sources line 3
gedit ./buildsys/config-lnx-pd-gcc.txtcd 
./build.sh pd gcc
sudo ./build.sh pd gcc install
cd ..

# Build and install ola2pd 

cd svn/externals/ola2pd/
/home/santi/PMS/flext/build.sh pd gcc
/home/santi/PMS/flext/build.sh pd gcc install

# libav para generar thumbs
sudo apt-get install libav-tools
