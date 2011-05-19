***************************************
pix_film_player 0.0.2 SVN

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

Speed: 
     0-244: Velocidad
     245-249: Autodeteccion fps, una sola vez
     250-255: Autodeteccion fps, loop	

En los modos auto el inicio y el fin del loop no son aplicables, se reproduce la película completa.





