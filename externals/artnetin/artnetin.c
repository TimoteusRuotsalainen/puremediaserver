/*
 * artnetin is an external for Pure Data that reads an artnet universe
 * and outputs it in an outlet like a list of 512 int.
 *
 * artnetin needs libartnet installed in the sytem to work
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
/*
 * Santiago Noreña 
 * puremediaserver@gmail.com
 * Versión 0.0.4
 *
 * Based on previous works by: 
 * 	Dirk Jagdmann (doj@cubic.org) dmxmonitor
 *	Simon Newton (nomis52@westnet.com.au) libartnet and modified dmxmonitor to use ArtNet
 * 	Patrick Sébastien http://www.workinprogress.ca/ pure data external pthread template
 *
*/
/*
 * ToDo:
 * - Cambiar funciones Unix por funciones PD (getbytes, freebytes,...)
 * - cambiar variables a syntaxis pure data (x_subnet)
 * - método max channels distinto de 512
 * - meter thread_start en función principal
 * 
*/ 

#include "m_pd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "pthread.h"
#include "artnet.h"
#include <malloc.h>

// ==============================================================================
// Our External's Memory structure
// ------------------------------------------------------------------------------

t_class *artnetin_class;

typedef struct _artnetin
{
	t_object x_obj;
	artnet_node node; 			// el nodo artnet 
	int CHANNELS;     			// la longitud del array dmx
	t_outlet *outlet1; 			// outlet pointer
	int subnet;  				// variables subnet y universo; solo utiles para mostrar informacion del nodo en bang
	int universe;	
	pthread_attr_t 	artnetin_thread_attr;
	pthread_t		x_threadid; 	// Control de thread
	char *ip; 				// variable para definir el interface donde se creará el nodo.
} t_artnetin;

unsigned char *dmx; 				// El array DMX

// ==============================================================================
// dmx handler - 
// ------------------------------------------------------------------------------

static int dmx_handler(artnet_node node, int prt , void *d) {
	int z;
	int len;
	t_artnetin *x = (t_artnetin*) d;
	uint8_t *g = dmx; 			  		// Puntero al array dmx 
	uint8_t *data; 				  		// Puntero 
	data = artnet_read_dmx(node, prt, &len) ; 		// Nos devuelve el array dmx en data?
	memcpy(g, data, len) ; 			  		// Copia el array dmx devuelto a g

	t_atom dmxa[x->CHANNELS];		  		// Crea un array dmx en formato float pure dat
	uint8_t *b = dmx;			  		// Otro puntero a dmx; no podemos usar g?	
	for(z=0; z < x->CHANNELS; z++){ 	  		// Copiamos el array dmx al array dmxa	
		SETFLOAT(dmxa+z, *b);		
		b++;
	}
	outlet_list(x->outlet1 ,gensym("list"), x->CHANNELS, dmxa); // Saca el array dmxa mediante una lista
	return (0);
} 

//--------------------------------------------------------------------------
// - bang - Devuelve información del estado del nodo
//--------------------------------------------------------------------------
void artnetin_bang(t_artnetin *x) {
	if ((int)x->x_threadid == 0) {
		post ("Artnetin: Node not created");
	}
	else
	{
		post ("Artnetin: Node created on thread %d", x->x_threadid); 
		post ("Artnetin: listening on Subnet %d Universe %d and IP address %s", x->subnet, x->universe, x->ip);  
	}
return;
}

// =============================================================================
// Worker Thread
// =============================================================================
static void *thread_read(void *w)
{
	t_artnetin *x = (t_artnetin*) w;
	int n, max, z;
	int i = 0;
	int artnet_sd;	
	pthread_testcancel();	
	while(1) {
		artnet_sd = artnet_get_sd(x->node) ;
		fd_set rd_fds;
		struct timeval tv;
		FD_ZERO(&rd_fds);
		FD_SET(0, &rd_fds);
		FD_SET(artnet_sd, &rd_fds) ;
		max = artnet_sd ;
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		n = select(max+1, &rd_fds, NULL, NULL, &tv);
		if(n>0) {
			if (FD_ISSET(artnet_sd , &rd_fds)){
			        i = artnet_read(x->node,0);
				}
			if (i == 0) { 

			
			t_atom dmxa[x->CHANNELS];
			uint8_t *b = dmx;	
			for(z=0; z<x->CHANNELS; z++){ 
				SETFLOAT(dmxa+z, *b);		
			b++;
		}
		outlet_list(x->outlet1 ,gensym("list"), x->CHANNELS, dmxa);
	     		}
		else {
			error("Artnetin: Bucle Read Error"); 
			}	
		} 
	}
return 0;
}

// =============================================================================
// Start Thread
// =============================================================================

static void thread_start(t_artnetin *x)
{

	// create the worker thread
    if(pthread_attr_init(&x->artnetin_thread_attr) < 0)
	{
	   sys_lock();
       error("Artnetin: could not launch receive thread");
	   sys_unlock();
       return;
    }
    if(pthread_attr_setdetachstate(&x->artnetin_thread_attr, PTHREAD_CREATE_DETACHED) < 0)
	{
       	   sys_lock();
	   error("Artnetin: could not launch receive thread");
           sys_unlock();
	   return;
    }
    if(pthread_create(&x->x_threadid, &x->artnetin_thread_attr, thread_read, x) < 0)
	{
   	   sys_lock();
   	   error("Artnetin: could not launch receive thread");
	   sys_unlock();
       	   return;
    }
    else
    	   {
	   sys_lock();
	   post("Artnetin: thread %d launched", (int)x->x_threadid );
	   sys_unlock();
    }
}

//--------------------------------------------------------------------------
// - Message: ip - define el interface donde escuchará el nodo mediante la ip
//--------------------------------------------------------------------------

void artnetin_ip(t_artnetin *x, t_symbol *ip_addres)
{
	x->ip = getbytes(15);	
	strcpy(x->ip, ip_addres->s_name);
	post("Artnetin: IP %s", x->ip);	
}

//--------------------------------------------------------------------------
// - Message: open - Empieza el nodo artnet y lanza el thread que escuchará en ese nodo
//--------------------------------------------------------------------------
void artnetin_open(t_artnetin *x, t_floatarg f1, t_floatarg f2)
{
	

	if ((int)x->x_threadid == 0) {
		x->subnet = f1;
		x->universe = f2;    
		post("Artnetin: opening node Artnet Subnet %d Universe %d IP %s", x->subnet, x->universe, x->ip);

//		char * ip_addr =  NULL;
		
		x->node = artnet_new(x->ip, 0 );
		if(x->node == NULL) {
			error("Artnetin: Unable to connect") ;
		}
        	artnet_set_dmx_handler(x->node, dmx_handler, x); 
		artnet_set_subnet_addr(x->node, x->subnet);
		artnet_set_port_type(x->node, 0, ARTNET_ENABLE_OUTPUT, ARTNET_PORT_DMX) ;
		artnet_set_port_addr(x->node, 0, ARTNET_OUTPUT_PORT, x->universe);
		artnet_set_long_name(x->node, "Artnetin Pure Data 0.0.4");	
		artnet_set_short_name(x->node, "Artnetin-PD");
		artnet_start(x->node);
	        int i = artnet_read(x->node,0);
		if (i == 0) { 
			post("Artnetin: Init OK");	
			thread_start(x);
		}
		else error ("Artnetin: Init Error");	
		
	}
	else 
		{
		error ("Artnetin: Node created. Close it before try reopen");
		}	
}

//--------------------------------------------------------------------------
// - Message: close
//--------------------------------------------------------------------------
void artnetin_close(t_artnetin *x)
{	
	if ((int)x->x_threadid == 0) {
		error("Artnetin: node not created. Doing nothing");
		}
	else {
		post("Artnetin: closing node");
		artnet_stop(x->node) ;
    		artnet_destroy(x->node) ;
		while(pthread_cancel(x->x_threadid) < 0);
		x->x_threadid = 0;
	}
}
//--------------------------------------------------------------------------
// - Initialization
//--------------------------------------------------------------------------

void *artnetin_new(void)
{

	post("Artnetin: an ArtNet interface : v0.0.4"); 
	post("          written by Santiago Noreña (puremediaserver@gmail.com)");
	t_artnetin *x = (t_artnetin *)pd_new(artnetin_class);	// local variable (pointer to a t_artnetin data structure)
	x->outlet1 = outlet_new(&x->x_obj, &s_list); 		// Saca todos los canales  mediante una lista
	x->CHANNELS = 512;					// Define el número de canales del array dmx
	dmx = malloc(512) ;					// localiza memoria para el array dmx; Cambiar por getbytes
	if(!dmx) {
		error("Artnetin: malloc failed") ;
	}
	memset(dmx, 0x00, x->CHANNELS) ;  			// Inicializa el array dmx a 0
	x->ip = NULL;						// Inicializa la dirección ip a NULL = interface con ip más baja
	return (void *)x;
}

//--------------------------------------------------------------------------
// - Object destruction
//--------------------------------------------------------------------------

void artnetin_free(t_artnetin *x)
{
	artnetin_close(x);
	freebytes(x->ip, 15);
}

//--------------------------------------------------------------------------
// - Object creation and setup
//--------------------------------------------------------------------------

void artnetin_setup(void)
{
	artnetin_class = class_new ( gensym("artnetin"),(t_newmethod)artnetin_new, 0, sizeof(t_artnetin), CLASS_DEFAULT, A_GIMME, 0);
	// Add message handlers
	class_addmethod(artnetin_class, (t_method)artnetin_open, gensym("open"), A_DEFFLOAT, A_DEFFLOAT, 0);
	class_addmethod(artnetin_class, (t_method)artnetin_close, gensym("close"), 0);
	class_addmethod(artnetin_class, (t_method)artnetin_ip, gensym("ip"), A_SYMBOL, 0);
	class_addbang(artnetin_class, artnetin_bang);
}

