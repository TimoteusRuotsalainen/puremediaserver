******************************
ola2pd 0.0.1 

Based on dmxmonitor

 * Copyright (C) 2001 Dirk Jagdmann <doj@cubic.org>
 * Modified by Simon Newton (nomis52<AT>gmail.com) to use ola
 * Modified by Santiago Noreña (puremediaserver@gmail.com) to use flext for PD/MaX

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

Ver license.txt para detalles.

******************************

ola2pd es un objeto de Pure Data que permite leer un universo de Open Lighting Arquitecture, que proporciona soporte
para muchos protocoles de iluminación basados en red (Pathport, ArtNet, ACN, ShowNet, SandNet) e interfaces USB (Enttec Open DMX Pro, Velleman, Robe,...). 

El objeto está desarrollado con las cabeceras Flext de Thomas Grill, por lo que debería de compilar en Windows, Mac y
Linux para Max/MSP y Pure Data. Se proporciona el binario para Pure Data en Linux (Debian Wheezy), que es la única plataforma en que está probado.

ola2pd reads one universe from Open Lighting Arquitecture and outputs them in Pure Data/MaX like a list of 512 values. The universe can be configured before opennig the connection sending one message <universe n> where n is the number of the universe in OLA. After, you can send one message <open> to stablish the connection. You can send one message <close> to exit, and reconfigure the universe if you need it. The bang method shows the number of the universe it's currently selected.

It's based on flext headers from Thomas Grill, so it should compile in Windows, Linux and Mac, and for Pure Data and Max/MSP. You must have the flext and the Open Lighting Arquitecture installed in order to compile it. I've only tested in Debian Wheezy with Pure Data, it's the only binary in the download. 

*****************************

Instalación:

- Instala y configura OLA siguiendo las instrucciones 

	http://code.google.com/p/linux-lighting/

- Copia el archivo artnetin.pd_linux en una carpeta que esté dentro de las rutas de Pure Data (la carpeta pd-externals en la carpeta de usuario es el sitio indicado) o en la carpeta del patch que uses. Está compilado en Debian Wheezy.

- Si el binario no te funciona puedes probar a compilarlo tú mismo. Necesitas tener instalado flext y las fuentes de Pure Data/Max. Después se puede compilar con el build.sh de flext (bash /home/user/flext/build.sh <system> <compiler>, por ejemplo, mi línea de comandos es bash /home/santi/PMS/flext/build.sh pd gcc ).

*****************************

Uso:

- El objeto escucha en el interface activo con la IP más baja, típicamente 2.x.x.x.
- Están disponibles los 512 canales; El objeto saca una lista de 512 enteros con el universo entero.
- Antes de que el objeto empiece a sacar valores hay que crear el nodo. Para ello hay que mandar un mensaje [open]. Antes de ello podemos configurar el universo de OLA al que escuchará con el mensaje <universe x>. donde x es el número de universo OLA. Por defecto escucha en el universo 0. Si queremos cambiar el universo de escucha hay que mandar un mensaje [close], mandar el mensaje <universe x> y reabrirlo. 

Por supuesto el demonio olad debe de estar corriendo en el sistema y con un universo al menos configurado como salida para que podamos tomar datos de él.

*****************************

Requerimientos/Requirements:

Los siguientes paquetes de software son requeridos para compilar y ejecutar ola2pd

	- Flext http://grrrr.org/research/software/flext/
	- Open Lighting Arquitecture http://code.google.com/p/linux-lighting/
	- Fuentes de Pure Data o Max/MSP (para compilar flext) http://puredata.info/downloads/pure-data
	- Pure Data 0.43.2 (apt-get install puredata-core)
	- Herramientas de compilación (gcc, make, ...)

*****************************

Errores conocidos:

- El binario compilado en modo release hace que Pure Data casque en el momento de cargar la librería. Compilando el binario en debug mode, carga y funciona normalmente. Esta opción puede ser cambiada en el archivo package.txt en la opción
BUILDMODE=debug (or release)

Know issues:

The Release mode of the binary crash Pure Data at object creation. If the binary is compiled in debug mode works fine. This option can be changed in the file package.txt with the option 
BUILDMODE=debug (or release)

*****************************

Credits:

- All the credit to Simon Newton for OLA and ola_dmxmonitor, this is a simple modification of that code.

*****************************

Página del proyecto:

http://code.google.com/p/puremediaserver/externals/ola2pd

Lista de correo:

puremediaserver@googlegroups.com

Contacto:

puremediaserver@gmail.com

*****************************