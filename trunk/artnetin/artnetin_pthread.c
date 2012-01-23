/*
 * Copyright (C) 2001 Dirk Jagdmann <doj@cubic.org>
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

/* ArtNetin es un external para Pure Data que permite leer datos de ArtNet en Pure Data en Linux
 * Utiliza la biblioteca libartnet del proyecto OLA http://code.google.com/p/linux-lighting/
 * Santiago Noreña
 * puremediaserver@googlegroups.com
 * V 0.0.3
*/

#include <string.h>
#include "artnet.h"
#include "m_pd.h"
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

static t_class *artnetin_class;

typedef struct _artnetin {
  	t_object x_obj;
	artnet_node node;
	int CHANNELS;
	t_outlet *outlet1; // outlet pointer
	t_float subnet, universe;
	pthread_attr_t artnetin_thread_attr;
	pthread_t x_threadid;	
} t_artnetin;

unsigned char *dmx;

void values (t_artnetin *x){
	int z;
	t_atom dmxa[x->CHANNELS];
	uint8_t *b = dmx;	
	for(z=0; z<x->CHANNELS; z++){ 
		SETFLOAT(dmxa+z, *b);		
		b++;
	}
	outlet_list(x->outlet1 ,gensym("list"), x->CHANNELS, dmxa);
}

static int dmx_handler(artnet_node node, int prt , void *d) {
        uint8_t *g=dmx;
	int len;
	uint8_t *data;
	data = artnet_read_dmx(node, prt, &len) ;
	memcpy(g, data,len) ;
	values(d);
	return (0);
} 

void artnetin_destroy(t_artnetin *x) {
    post("Artnetin: Destruyendo el nodo");	
    artnet_stop(x->node) ;
    artnet_destroy(x->node) ;
    while(pthread_cancel(x->x_threadid) < 0);
}

void artnetin_free(t_artnetin *x) {
    artnet_stop(x->node) ;
    artnet_destroy(x->node) ;
    while(pthread_cancel(x->x_threadid) < 0);
}

static void *artnetin_read(void *w)
{
	int artnet_sd;
	t_artnetin *x = (t_artnetin*) w;
	int c = 0;
	int n, max;
	int i = 0;
	artnet_sd = artnet_get_sd(x->node) ;
	while (c < 1 ) {
//		c++;		
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
				values(x);
		     		}
			else {
				error("Artnetin: Bucle Read Error"); 
				}	
			} 
	}
return(0);
}

void artnetin_create(t_artnetin *x, t_floatarg f1, t_floatarg f2) {
	printf("Empezando el trhead");

	// create the worker thread
    if(pthread_attr_init(&x->artnetin_thread_attr) < 0)
	{
	   sys_lock();
       printf("artnetin: could not launch receive thread");
	   sys_unlock();
       return;
    }
    if(pthread_attr_setdetachstate(&x->artnetin_thread_attr, PTHREAD_CREATE_DETACHED) < 0)
	{
       sys_lock();
	   printf("artnetin: could not launch receive thread");
       sys_unlock();
	   return;
    }
    if(pthread_create(&x->x_threadid, &x->artnetin_thread_attr, artnetin_read, x) < 0)
	{
           printf("creando el threat");	
	   sys_lock();
	   error("artnetin: could not launch receive thread");
	   sys_unlock();
           return;
    }
    else
    {
	   sys_lock();
	   post("artnetin: thread %d launched", (int)x->x_threadid );
	   sys_unlock();
    }	
	printf("Creando nodo");
	post("Artnetin: Creando nodo Artnet");
	int subnet_addr = f1;
	int port_addr = f2;    
	char * ip_addr =  NULL;
	x->node = artnet_new(ip_addr, 0 ) ;
	if(x->node == NULL) {
	error("Artnetin: Unable to connect") ;
	}
        artnet_set_dmx_handler(x->node, dmx_handler, x); 
	artnet_set_subnet_addr(x->node, subnet_addr);
	artnet_set_port_type(x->node, 0, ARTNET_ENABLE_OUTPUT, ARTNET_PORT_DMX) ;
	artnet_set_port_addr(x->node, 0, ARTNET_OUTPUT_PORT, port_addr);
	artnet_set_long_name(x->node, "Artnetin Pure Data 0.0.3");	
	artnet_set_short_name(x->node, "Artnetin-PD");
	artnet_start(x->node);
        int i = artnet_read(x->node,0);
	if (i == 0) { 
		post("Artnetin: Init OK");		
		}
	else error ("ArtNetin: Init Error");	
	artnetin_read(x);
}

int artnetin_bang(t_artnetin *x)
{
// Aquí podríamos meter algo de info del estado del nodo
return(1);
}

void *artnetin_new(void)
{
	post("**************************");
	post("Artnetin 0.0.3a");
	post("by Santiago Noreña");
	post("**************************");        
	t_artnetin *x = (t_artnetin *)pd_new(artnetin_class);
	x->outlet1 = outlet_new(&x->x_obj, &s_list); // Saca todos los canales  mediante una lista
        x->CHANNELS=512;
	dmx = malloc(512) ;
	if(!dmx) {
		error("Artnetin: malloc failed") ;
	}
	memset(dmx, 0x00, x->CHANNELS) ;
	return (void *)x;
}

void artnetin_setup(void) {
  artnetin_class = class_new(gensym("artnetin"),
        (t_newmethod)artnetin_new,
        0, sizeof(t_artnetin),
        CLASS_DEFAULT,A_GIMME, 0);
  class_addbang(artnetin_class, artnetin_bang);
  class_addmethod(artnetin_class, (t_method)artnetin_create, gensym("create"), A_DEFFLOAT, A_DEFFLOAT, 0);
  class_addmethod(artnetin_class,(t_method)artnetin_destroy, gensym("destroy"),0);
}
