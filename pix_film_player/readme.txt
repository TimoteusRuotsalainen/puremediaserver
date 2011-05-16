***************************************
pix_film_player 0.0.1 (16-05-2011)

Santiago Noreña

blefegor@gmail.com

***************************************

pix_film_player es un subpatch de Pure Data que interactúa con el objeto pix_film y permite la reproducción avanzada de películas a través de éste.

Consta de 4 canales de control, cada uno de ellos de rango 0-255:

- Selección de reproducción

     0-10 Normal loop 
     11-20 Back loop 
     21-30 Ping-pong loop 
     31-40 Normal una vez
     41-50 Back una vez 
     51-60 Ping-Pong una vez 
     61-70 Stop/pause
     71-80 Continue

- Inicio loop: 0% - Inicio peli
               FF - Fin peli

- Final loop : 0% Inicio peli
               FF fin peli

- Velocidad son los milisegundos a los que setea el metronomo:
  40 ms =  25 fps
  50 ms =  20 fps
  100 ms = 10 fps

Bugs:

- en algunos casos de loops he observado que se pasa por el frame 0; hay que investigar en qué casos y si afecta realmente a la reproduccion, por lo menos a velocidades normales (25 fps) no creo que sea apreciable.
- Lanza errores de stack overflow cuando acaba un loop

