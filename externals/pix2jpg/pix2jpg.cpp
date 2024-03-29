////////////////////////////////////////////////////////
//   pix2jpg v0.01
//   External for Gem and Pure Data to convert an image to jpg and send it 
//	 via Unix Domain Sockets
//   Compiled and tested in Ubuntu Precise
//   (c) 2012 Santi Noreña. puremediaserver@gmail.com 	
//
// GEM - Graphics Environment for Multimedia
//
// zmoelnig@iem.kug.ac.at
//
// Implementation file
//
//    Copyright (c) 1997-1999 Mark Danks.
//    Copyright (c) Günther Geiger.
//    Copyright (c) 2001-2011 IOhannes m zmölnig. forum::für::umläute. IEM. zmoelnig@iem.at
//    Copyright (c) 2002 James Tittle & Chris Clepper
//    For information on usage and redistribution, and for a DISCLAIMER OF ALL
//    WARRANTIES, see the file, "GEM.LICENSE.TERMS" in this distribution.
//
/////////////////////////////////////////////////////////

#include "pix2jpg.h"
#include <sys/socket.h>
#include <sys/un.h>
#include "m_pd.h"
//#include "s_stuff.h"
#define SOCK_PATH "/tmp/pmspipe"

CPPEXTERN_NEW_WITH_ONE_ARG(pix2jpg,  t_symbol *, A_DEFSYM);

  /////////////////////////////////////////////////////////
//
// pix2jpg
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
pix2jpg :: pix2jpg(t_symbol *s)
  : m_banged(false),
    m_automatic(false),
		m_layer(0)
{
	post("pix2jpg v0.01 Convert pix to jpeg and send to Unix Local Socket");
  post("(c) 2012 Santi Noreña puremediaserver@gmail.com");
	post("GPL License");
	outlet1 = outlet_new(this->x_obj, 0); 		// Saca todos los canales  mediante una lista
	// Init the Unix Socket 
	socket_fd = socket(PF_UNIX, SOCK_STREAM, 0);
	 if(socket_fd < 0)
 	 {
  	 post("pix2jpg:create socket() failed\n");
  	 return;
 	 }
   /* start with a clean address structure */
   memset(&address, 0, sizeof(struct sockaddr_un));
   address.sun_family = PF_UNIX;
	 strcpy(address.sun_path,SOCK_PATH); 	   
   if(connect(socket_fd,(struct sockaddr *) &address,sizeof(struct sockaddr_un)) != 0)
 	 {
  		post("pix2jpg:connect() failed");
  		return;
 		}
}
/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
pix2jpg :: ~pix2jpg()
{
	outlet_free(outlet1);
}

/////////////////////////////////////////////////////////
// render
//
/////////////////////////////////////////////////////////
void pix2jpg :: processImage(imageStruct &image)
{
  if (m_automatic || m_banged) 
	{
		m_banged =false;
		int counter = 0;  
		Image imagem(image.xsize, image.ysize, "RGBA", CharPixel, image.data);
		imagem.resize("64x46");
	  // Write to BLOB in png format 
  	Blob blob; 
  	imagem.magick( "jpg" ); // Set JPEG output format 
  	imagem.write( &blob );
	// Enviamos al GUI x Unix Domain Sockets	
   int size = blob.length() +2;
	 char buffer[size];
	 memcpy(buffer+2, blob.data(), blob.length());
	 memcpy(buffer,&m_layer, 2);		
   write(socket_fd, buffer, size);
	}
}

/////////////////////////////////////////////////////////
// static member functions
//
/////////////////////////////////////////////////////////
void pix2jpg :: obj_setupCallback(t_class *classPtr)
{
  class_addbang(classPtr, reinterpret_cast<t_method>(&pix2jpg::bangMessCallback));
  class_addmethod(classPtr, reinterpret_cast<t_method>(&pix2jpg::startMessCallback),gensym("start"), A_NULL);
  class_addmethod(classPtr, reinterpret_cast<t_method>(&pix2jpg::stopMessCallback),gensym("stop"),  A_NULL);
	class_addmethod(classPtr, reinterpret_cast<t_method>(&pix2jpg::layerimageMessCallback),gensym("layerimage"), A_FLOAT,A_NULL);
	class_addmethod(classPtr, reinterpret_cast<t_method>(&pix2jpg::layernameMessCallback),gensym("layername"), A_GIMME,A_NULL);
}
void pix2jpg :: startMessCallback(void *data)
{
  GetMyClass(data)->m_automatic=true;
}
void pix2jpg :: stopMessCallback(void *data)
{
  GetMyClass(data)->m_automatic=false;
}
void pix2jpg :: bangMessCallback(void *data)
{
  GetMyClass(data)->m_banged=true;
}
void pix2jpg :: layerimageMessCallback (void *data, t_float f)
{
	GetMyClass(data)->m_layer=f;
	printf("layer: %d", GetMyClass(data)->m_layer); 
}
void pix2jpg :: layernameMessCallback(void *data,  t_symbol *s, int argc, t_atom *argv)
{
 /*	 int n, length;
	 t_symbol *c;  
// check for correct number of arguments
	n = atom_getint(argv++);
  c = atom_getsymbol(argv++);
  
	length = 2 + sizeof(c);  
 	char *buffer;
	post
	memcpy(buffer+2, c, sizeof(c));
	memcpy(buffer,&n, 2);		*/
   t_binbuf *b = binbuf_new();
   char *buf;
   int length;
 //  t_atom at;
   binbuf_add(b, argc, argv);
 //  SETSEMI(&at);
 //  binbuf_add(b, 1, &at);
   binbuf_gettext(b, &buf, &length);
	write(socket_fd, buf, length);	
}

