******************************
Artnetin 0.0.3 
Santiago Noreña
puremediaserver@gmail.com

Based on previous works by: 
 * 	Dirk Jagdmann (doj@cubic.org) dmxmonitor
 *	Simon Newton (nomis52@westnet.com.au) Modified dmxmonitor to use ArtNet	
 * 	Patrick Sébastien http://www.workinprogress.ca/ pure data external pthread template

******************************

Artnetin es un objeto de Pure Data que permite leer un universo entero de ArtNet y tratarlo como una lista de enteros en Pure Data.

*****************************

Instalación:

1. Compila artnetin:
	make

2. Compila e instala libartnet:

	cd libartnet-1.1.0
        ./configure
  	make
	sudo make install

3. Copia el archivo artnetin.pd_linux en una localización dentro del path de Pure Data, o añade el path a Pure Data

*****************************

Uso:

El objeto escucha en el interface activo con la IP más baja, típicamente 2.x.x.x.
Están disponibles los 512 canales; El objeto saca una lista de 512 enteros con el universo entero.
Antes de que el objeto empiece a sacar valores hay que crear el nodo. Para ello hay que mandar un mensaje [open "nº subnet" "nº universo"]. Si sólo le pasamos open, escuchará en la subnet 0, universo 0. Si queremos cambiar el universo de escucha hay que mandar un mensaje [close]. Después se puede volver a mandar un nuevo mensaje open con la nueva configuración. 

*****************************

Creditos:

- libartnet es parte del proyecto Open Lighting Arquitecture y (c) por Simon Newton

*****************************

Ayuda y soporte:

puremediaserver@googlegroups.com

*****************************

