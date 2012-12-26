/*-----------------------------------------------------------------
   pix2jpg v0.01 
   External for Gem and Pure Data to convert an image to jpg and send it 
	 via Unix Domain Sockets
   Compiled and tested in Ubuntu Precise
   (c) 2012 Santi Noreña. puremediaserver@gmail.com 	

   GEM - Graphics Environment for Multimedia
    Copyright (c) 1997-1999 Mark Danks. mark@danks.org
    Copyright (c) Günther Geiger. geiger@epy.co.at
    Copyright (c) 2001-2011 IOhannes m zmölnig. forum::für::umläute. IEM. zmoelnig@iem.at
    For information on usage and redistribution, and for a DISCLAIMER OF ALL
    WARRANTIES, see the file, "GEM.LICENSE.TERMS" in this distribution.

-----------------------------------------------------------------*/

#ifndef _INCLUDE__GEM_PIXES_pix2jpg_H_
#define _INCLUDE__GEM_PIXES_pix2jpg_H_

#include "Base/GemPixObj.h"
#include "Base/GemBase.h"
#include "Gem/Manager.h"
#include "Gem/ImageIO.h"
#include "Gem/Image.h"
#include <stdio.h>
#include <Magick++.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>

using namespace Magick;
/*-----------------------------------------------------------------
-------------------------------------------------------------------
CLASS
    pix2jpg

	  Convert a image to raw RGB and outputs in a list

KEYWORDS
    pix

DESCRIPTION

    "bang" - do write now
    
    outlet : The list of raw data.	
-----------------------------------------------------------------*/
class GEM_EXPORT pix2jpg : public GemPixObj
{
    CPPEXTERN_HEADER(pix2jpg, GemPixObj);

    public:

        //////////
        // Constructor
    	pix2jpg(t_symbol *s);

    protected:

    	//////////
    	// Destructor
    	virtual ~pix2jpg();

    	//////////
    	// Do the rendering
    	virtual void 	processImage(imageStruct&image);

    	//////////
    	// Set the filename and filetype
 //   	virtual void	fileMess(int argc, t_atom *argv);

    	//////////
    	// The original pix_writer
    	imageStruct 	*m_originalImage;

      //////////
      // Manual writing
      bool            m_banged;

      //////////
      // Automatic writing
      bool            m_automatic;
	
      //////////
      // Layer to write
			
			int m_layer;		

			t_outlet *outlet1; 			// outlet pointer

		 struct sockaddr_un address;
		 int  socket_fd, nbytes;

		
 private:

    	//////////
    	// static member functions
      static void 	startMessCallback(void *data);
		 	static void 	stopMessCallback(void *data);    
			static void 	bangMessCallback(void *data);
			static void layerMessCallback(void *data, t_float f); 
};
#endif	// for header file
