******************************
Artnetin v0.0.2 
Santiago Noreña 2011
belfegor@gmail.com
******************************

Artnetin es un objeto de Pure Data que permite leer un universo entero de DMX512 y tratarlo canal a canal como entrada de datos de Pure Data.

*****************************

Disclaimer:

Yo no soy programador ni de cerca, soy técnico de iluminación y el código lo demuestra. Seguramente esté haciendo cosas mal y sucias que un programador de verdad se llevaría las manos a la cabeza, pero por lo menos puedo asegurar que funciona. Cualquier sugerencia siempre es bien recibida.

*****************************

Instalación:

Supongo que ya tienes Pure Data instalado, si no dirígete a la página del proyecto.

Artnetin depende de la biblioteca libartnet del proyecto OLA, por lo que antes de compilar el objeto deberías instalar libartnet. Se pueden descargar en 

http://code.google.com/p/linux-lighting/downloads/detail?name=libartnet-1.1.0.tar.gz

De momento esta versión compila y funciona en Debian Squeezy. No he probado en más sistemas. 

Para compilar:

	make
	make install

y ya está, si todo va bien. Ahora deberías copiar el archivo artnetin.pd_linux en una carpeta que esté dentro de la ruta de Pure Data. Se supone que debería de funcionar en cualquier Linux con Pure Data y libartnet instalados.

*****************************

Uso:

El objeto escucha en el interface activo con la IP más baja, típicamente 2.x.x.x.

Están disponibles los 512 canales; El objeto saca una lista de 512 enteros con el universo entero.

Antes de que el objeto empiece a sacar valores hay que crear el nodo. Para ello hay que mandar un mensaje [create "nº subnet" "nº universo"]. Si sólo le pasamos un create a secas, escuchará en la subnet 0, universo 0. Si queremos cambiar el universo de escucha hay que mandar un mensaje [destroy]. Después se puede volver a mandar un nuevo mensaje create con la nueva configuración. 

El objeto realiza una lectura por cada bang, por lo que necesita un metro para hacer medidas continuas. Si hacemos un bucle parece que se ralentiza el ordenador, imagino que habría que hacer un fork a otro proceso o algo parecido. En próximas versiones depuraré esto.


*****************************

Ayuda y soporte:

puremediaserver@googlegroups.com

*****************************

