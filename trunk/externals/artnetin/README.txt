******************************
Artnetin 0.0.3 
Santiago Noreña
puremediaserver@gmail.com

Based on previous works by: 
 * 	Dirk Jagdmann (doj@cubic.org) dmxmonitor
 *	Simon Newton (nomis52@westnet.com.au) Modified dmxmonitor to use ArtNet	
 * 	Patrick Sébastien (http://www.workinprogress.ca/) pure data external pthread template

libartnet es parte del proyecto Open Lighting Arquitecture y (c) por Simon Newton

******************************

Artnetin es un objeto de Pure Data que permite leer un universo entero de ArtNet y tratarlo como una lista de enteros en Pure Data.

*****************************

Instalación:

1. Compila e instala libartnet:

	cd libartnet-1.1.0
        ./configure
  	make
	sudo make install

2. Compila artnetin:
	make

3. Copia el archivo artnetin.pd_linux en una localización dentro del path de Pure Data, o añade el path a Pure Data

*****************************

Uso:

Antes de que el objeto empiece a sacar valores hay que crear el nodo. Para ello hay que mandar un mensaje [open "nº subnet" "nº universo"].
También podemos especificar la IP del interface ene l que escuchará mandando el mensaje "ip + dirección ip" antes de mandar el mensaje open.
Si no le mandamos ningún parámetro con el mesaje open el objeto escucha en el interface activo con la IP más baja, típicamente 2.x.x.x, en la subnet 0 y universo 0.
El objeto saca una lista de 512 enteros con el universo entero.
Si queremos cambiar el universo, a subnet o la dirección ip hay que mandar un mensaje [close] y luego mandar un nuevo mensaje open con la nueva configuración. 
Mandando bang al objeto nos imprime en la ventana de estado si el nodo está activo y en caso afirmativo en qué subnet, unverso y direcicń ip está escuchando.

*****************************

Ayuda y soporte:

puremediaserver@googlegroups.com

*****************************

