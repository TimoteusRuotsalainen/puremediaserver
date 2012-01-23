/*
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
 * puremediaserver@googlegroups.com
 * Versión 0.0.3
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
	artnet_node node;
	int CHANNELS;
	t_outlet *outlet1; // outlet pointer
	t_float subnet, universe;
	pthread_attr_t 	artnetin_thread_attr;
	pthread_t		x_threadid;
} t_artnetin;

unsigned char *dmx; // El array DMX

// ==============================================================================
// Function Prototypes
// ------------------------------------------------------------------------------
//void *artnetin_new(t_symbol *s);
//void artnetin_close(t_artnetin *x);
//void artnetin_open(t_artnetin *x, t_floatarg f1, t_floatarg f2 );

// ==============================================================================
// subroutines
// ------------------------------------------------------------------------------

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
	int z;
	t_artnetin *x = (t_artnetin*) d;
        uint8_t *g=dmx;
	int len;
	uint8_t *data;
	data = artnet_read_dmx(node, prt, &len) ;
	memcpy(g, data,len) ;
//	values(d);
	t_atom dmxa[x->CHANNELS];
	uint8_t *b = dmx;	
	for(z=0; z<x->CHANNELS; z++){ 
		SETFLOAT(dmxa+z, *b);		
		b++;
	}
	outlet_list(x->outlet1 ,gensym("list"), x->CHANNELS, dmxa);
	return (0);
} 

//--------------------------------------------------------------------------
// - bang
//--------------------------------------------------------------------------
/*
void artnetin_bang(t_artnetin *x) {
//	post("bang");
	int n, max;
	int i = 0;
	int artnet_sd;	
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
			values(x);
	     		}
		else {
			error("Artnetin: Bucle Read Error"); 
			}	
		} 
}
*/
// =============================================================================
// Thread
// =============================================================================
static void *thread_read(void *w)
{
	t_artnetin *x = (t_artnetin*) w;
	int n, max, z;
	int i = 0;
	int artnet_sd;	
	while(1) {
		pthread_testcancel();
//		artnetin_bang(x);
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
//				values(x);
			
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
       error("artnetin: could not launch receive thread");
	   sys_unlock();
       return;
    }
    if(pthread_attr_setdetachstate(&x->artnetin_thread_attr, PTHREAD_CREATE_DETACHED) < 0)
	{
       	   sys_lock();
	   error("artnetin: could not launch receive thread");
           sys_unlock();
	   return;
    }
    if(pthread_create(&x->x_threadid, &x->artnetin_thread_attr, thread_read, x) < 0)
	{
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
}

//--------------------------------------------------------------------------
// - Message: open
//--------------------------------------------------------------------------
void artnetin_open(t_artnetin *x, t_floatarg f1, t_floatarg f2)
{
	
	int subnet_addr = f1;
	int port_addr = f2;    
	post("Artnetin: Creando nodo Artnet Subnet %d Universo %d", subnet_addr, port_addr);
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
	else error ("Artnetin: Init Error");	
	thread_start(x);
//	thread_read(x);
}

//--------------------------------------------------------------------------
// - Message: close
//--------------------------------------------------------------------------
void artnetin_close(t_artnetin *x)
{	
	post("Artnetin:Cerrando");
	artnet_stop(x->node) ;
    	artnet_destroy(x->node) ;
	while(pthread_cancel(x->x_threadid) < 0);
}
//--------------------------------------------------------------------------
// - Initialization
//--------------------------------------------------------------------------

void *artnetin_new(void)
{
	post("**************************");
	post("Artnetin 0.0.3");
	post("by Santiago Noreña");
	post("**************************"); 
	t_artnetin *x = (t_artnetin *)pd_new(artnetin_class);	// local variable (pointer to a t_artnetin data structure)
	x->outlet1 = outlet_new(&x->x_obj, &s_list); 		// Saca todos los canales  mediante una lista
	x->CHANNELS = 512;
	dmx = malloc(512) ;
	if(!dmx) {
		error("Artnetin: malloc failed") ;
	}
	memset(dmx, 0x05, x->CHANNELS) ;
	return (void *)x;
}

//--------------------------------------------------------------------------
// - Object destruction
//--------------------------------------------------------------------------

void artnetin_free(t_artnetin *x)
{
	post("Artnetin: Cerrando");	
	artnet_stop(x->node) ;
    	artnet_destroy(x->node) ;
	while(pthread_cancel(x->x_threadid) < 0);	
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
//	class_addbang(artnetin_class, artnetin_bang);
}
