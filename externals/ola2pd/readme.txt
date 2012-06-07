******************************
ola2pd 0.0.1 

Santiago Noreña

puremediaserver@gmail.com

Por supuesto, todo esto es GPL.
Ver license.txt para detalles.

Based on dmxmonitor

 * Copyright (C) 2001 Dirk Jagdmann <doj@cubic.org>
 * Modified by Simon Newton (nomis52<AT>gmail.com) to use ola
 * Modified by Santiago Noreña (puremediaserver@gmail.com) to use flext for PD/MaX 

******************************

ola2pd es un objeto de Pure Data que permite leer un universo de Open Lighting Arquitecture, que proporciona soporte
para muchos protocoles de iluminación basados en red (Pathport, ArtNet, ACN, ShowNet, SandNet) e interfaces USB (Enttec Open DMX Pro, Velleman, Robe,...). 

El objeto está desarrollado con las cabeceras Flext de Thomas Miller, por lo que debería de compilar en Windows, Mac y
Linux para Max/MSP y Pure Data. Se proporciona el binario para Pure Data en Linux (Debian Wheezy), que es la única plataforma en que está probado.

*****************************
Instalación:

- Instala y configura OLA siguiendo las instrucciones 

	http://code.google.com/p/linux-lighting/

- Copia el archivo artnetin.pd_linux en una carpeta que esté dentro de las rutas de Pure Data o en la carpeta del patch que uses. Está compilado en Debian Wheezy.
- Si necesitas compilar porque el binario no te funciona, necesitas tener instalado flext y las fuentes de Pure Data/Max. Después se puede compilar con el build.sh de flext (bash /home/user/flext/build.sh <system> <compiler>)

*****************************

Uso:

- El objeto escucha en el interface activo con la IP más baja, típicamente 2.x.x.x.
- Están disponibles los 512 canales; El objeto saca una lista de 512 enteros con el universo entero.
- Antes de que el objeto empiece a sacar valores hay que crear el nodo. Para ello hay que mandar un mensaje [open]. Antes de ello podemos configurar el universo de OLA al que escuchará con el mensaje <universe x>. donde x es el número de universo OLA. Por defecto escucha en el universo 0. Si queremos cambiar el universo de escucha hay que mandar un mensaje [close], mandar el mensaje universe y reabrirlo.

*****************************

Página del proyecto:

http://code.google.com/p/puremediaserver/externals/ola2pd

Ayuda y soporte:

puremediaserver@gmail.com
puremediaserver@googlegroups.com
*****************************

