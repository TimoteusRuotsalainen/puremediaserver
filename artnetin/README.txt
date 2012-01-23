******************************
Artnetin 0.0.3 
Santiago Noreña
puremediaserver@googlegroups.com
******************************

Artnetin es un objeto de Pure Data que permite leer un universo entero de ArtNet y tratarlo como entrada de datos de Pure Data.

*****************************
Instalación:

Artnetin depende de la biblioteca libartnet del proyecto OLA, por lo que antes de compilar el objeto deberías instalar libartnet. Para instalar libartnet:
	wget http://linux-lighting.googlecode.com/files/libartnet-1.1.0.tar.gz
	tar -zxf libartnet-1.1.0.tar.gz
  	cd libartnet-1.1.0
        ./configure
  	make
	sudo make install

Para compilar artnetin:
	make

Ahora deberías copiar el archivo artnetin.pd_linux en una carpeta que esté dentro de las rutas de Pure Data o en la carpeta del patch que uses.
*****************************
Uso:

El objeto escucha en el interface activo con la IP más baja, típicamente 2.x.x.x.
Están disponibles los 512 canales; El objeto saca una lista de 512 enteros con el universo entero.
Antes de que el objeto empiece a sacar valores hay que crear el nodo. Para ello hay que mandar un mensaje [create "nº subnet" "nº universo"]. Si sólo le pasamos un create a secas, escuchará en la subnet 0, universo 0. Si queremos cambiar el universo de escucha hay que mandar un mensaje [destroy]. Después se puede volver a mandar un nuevo mensaje create con la nueva configuración. 

*****************************
Ayuda y soporte:

puremediaserver@googlegroups.com
*****************************

