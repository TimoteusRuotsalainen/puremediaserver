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

/*
 * Modified by Simon Newton (nomis52<AT>westnet.com.au)
 * to use artnet
 *
 */

/*
 * Modificado por Santiago Noreña (belfegor@gmail.com)
 * para Pure Data
 * 
*/


#include <string.h>
#include "artnet.h"
#include "m_pd.h"
#include <malloc.h>

static t_class *artnetin_class;

typedef struct _artnetin {
  	t_object  x_obj;
	artnet_node node;
	int CHANNELS;
	t_outlet *outlet1; // outlet pointer
} t_artnetin;

unsigned char *dmx;

// Limpiador-destructor. Parece que con atexit no le podemos pasar el nodo
// Crear método destructor, invocado manualmente? método pure data?

void cleanup(t_artnetin *x) {
    artnet_stop(x->node) ;
    artnet_destroy(x->node) ;
}

void values (t_artnetin *x){
	int z;
	t_atom dmxa[x->CHANNELS];
	uint8_t *b = dmx;	
	for(z=0; z<x->CHANNELS; z++){ 
		SETFLOAT(dmxa+z, *b);		
		b++;
//		post("Canal:%1d Valor:%1d\n",z+1 , dmx[z]);
	}
	outlet_list(x->outlet1 ,gensym("list"), x->CHANNELS, dmxa);
}

static int dmx_handler(artnet_node node, int prt , void *d) {
        uint8_t *g=dmx;
	int len;
	uint8_t *data;
//	if( prt == 0 ) {
		data = artnet_read_dmx(node, prt, &len) ;
		memcpy(g, data,len) ;
		values(d);
//	}
	return (0);
} 

int artnetin_bang(t_artnetin *x)
{
	int c=0;
	int i=3;	
	int n, max;
	int artnet_sd;
	artnet_sd = artnet_get_sd(x->node) ;
// main loop 
	while (c < 1 ) {
		c++;
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
//				post("artnetin: Bucle Read OK\n");		
				values(x);
		     		}
			else {
//				error("artnetin: Bucle Read Error\n"); 
				break;						
				}	
			} 
	}
//	artnet_stop(x->node) ;
//	artnet_destroy(x->node) ;
	return (0);
}

void *artnetin_new(void)
{
        t_artnetin *x = (t_artnetin *)pd_new(artnetin_class);
        x->CHANNELS=120;
//	floatinlet_new(&x->x_obj, &x->lower);
//	floatinlet_new(&x->x_obj, &x->upper);
//	floatinlet_new(&x->x_obj, &x->step);
	x->outlet1 = outlet_new(&x->x_obj, &s_list); // Saca todos los canales  mediante una lista
	int subnet_addr = 0 , port_addr = 0 ;
	char *ip_addr=NULL;
//	atexit(cleanup());  No le podemos pasar a atexit una función con puntero. hay que encontrar otra forma de ejecutar el destructor.????
	dmx = malloc(512) ;
	if(!dmx) {
		error("artnetin: malloc failed\n") ;
//		return (1) ;
	}
	memset(dmx, 0x00, 512) ;
	// set up artnet connection
	x->node = artnet_new(ip_addr, 0 ) ;
	if(x->node == NULL) {
		error("artnetin: Unable to connect\n") ;
//		return (1) ;
	}
       artnet_set_dmx_handler(x->node, dmx_handler, x); 
	artnet_set_subnet_addr(x->node, subnet_addr) ;
	artnet_set_port_type(x->node, 0, ARTNET_ENABLE_OUTPUT, ARTNET_PORT_DMX) ;
	artnet_set_port_addr(x->node, 0, ARTNET_OUTPUT_PORT, port_addr);
	artnet_start(x->node) ;
        int i = artnet_read(x->node,0);
	if (i == 0) { 
		post("artnetin: Read OK\n");		
		}
	else error ("artNetin: Read Error\n");	
	return (void *)x;
}

void artnetin_setup(void) {
  artnetin_class = class_new(gensym("artnetin"),
        (t_newmethod)artnetin_new,
        0, sizeof(t_artnetin),
        CLASS_DEFAULT, 0);
  class_addbang(artnetin_class, artnetin_bang);
}
