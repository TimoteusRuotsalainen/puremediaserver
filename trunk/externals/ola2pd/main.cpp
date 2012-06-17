/* 
 * ola2pd - interface from Open Lighting Arquitecture to Pure Data
 *
 * Based on dmxmonitor
 * Copyright (C) 2001 Dirk Jagdmann <doj@cubic.org>
 * Modified by Simon Newton (nomis52<AT>gmail.com) to use ola
 * Modified by Santiago Noreña (puremediaserver@gmail.com) to use flext for PD/MaX 
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
 *
 */

// Define flext

#define FLEXT_ATTRIBUTES 1
#include <flext.h>
#if !defined(FLEXT_VERSION) || (FLEXT_VERSION < 500)
#error You need at least flext version 0.5.0
#endif

//#define FLEXT_SHARED 1
//#define FLEXT_USE_CMEM 1

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

// Define general

#include <errno.h>
#include <string>

// Define ola

#include <BaseTypes.h>
#include <Callback.h>
#include <OlaCallbackClient.h>
#include <OlaClientWrapper.h>
#include <DmxBuffer.h>
#include <io/SelectServer.h>


using ola::DmxBuffer;
using ola::OlaCallbackClient;
using ola::OlaCallbackClientWrapper;
using ola::io::SelectServer;
using std::string;


class ola2pd:

	public flext_base
{

	FLEXT_HEADER_S(ola2pd,flext_base,setup)

public:

	// constructor with no arguments
	ola2pd():
	// initialize data members
	  i_universe(0),
          m_universe(0),
          m_counter(0)
//          m_stdin_descriptor(STDIN_FILENO)
    	{
		// --- define inlets and outlets ---
		AddInAnything(); // default inlet
		AddOutList();	// outlet for DMX list
        	post("ola2pd v0.0.2-SVN - an interface to Open Lighting Arquitecture");
		post("Santi Noreña puremediaserver@gmail.com");
	}
	void NewDmx(unsigned int universe,
                const DmxBuffer &buffer,
                const string &error);
        void RegisterComplete(const string &error);
//      void StdinReady();
        bool CheckDataLoss();

protected:

	void m_open() {
        m_universe = i_universe;	    
        // set up ola connection 
        if (!m_client.Setup()) {
            post("%s: %s",thisName(), strerror(errno));
        }
        OlaCallbackClient *client = m_client.GetClient();
        client->SetDmxCallback(ola::NewCallback(this, &ola2pd::NewDmx));
        client->RegisterUniverse(m_universe,ola::REGISTER,ola::NewSingleCallback(this, &ola2pd::RegisterComplete));
//        m_client.GetSelectServer()->AddReadDescriptor(&m_stdin_descriptor);
//        m_stdin_descriptor.SetOnData(ola::NewCallback(this, &ola2pd::StdinReady));
        m_client.GetSelectServer()->RegisterRepeatingTimeout(500,ola::NewCallback(this, &ola2pd::CheckDataLoss));
        m_buffer.Blackout();	
        post("ola2pd: Init complete");
	m_client.GetSelectServer()->Run();
        post("ola2pd: Close complete");
        }    
    
	void m_close() {
        OlaCallbackClient *client = m_client.GetClient();
        client->RegisterUniverse(m_universe,ola::UNREGISTER,ola::NewSingleCallback(this, &ola2pd::RegisterComplete));        
        m_client.GetSelectServer()->Terminate();
        }

	void m_bang()  // Utilidad del bang?
	{
		post("%s: universe %d",thisName(),i_universe);
	}

private:
    int i_universe;
    unsigned int m_universe;
    unsigned int m_counter;
//    ola::io::UnmanagedFileDescriptor m_stdin_descriptor;
    struct timeval m_last_data;
    OlaCallbackClientWrapper m_client;
    DmxBuffer m_buffer;
//    void Clean(); //o alternativa a free en flext
    static void setup(t_classid c)
	{
	// --- set up methods (class scope) ---
	// register a bang method to the default inlet (0)
	FLEXT_CADDBANG(c,0,m_bang);
	// set up tagged methods for the default inlet (0)
	FLEXT_CADDMETHOD_(c,0,"open",m_open);
	FLEXT_CADDMETHOD_(c,0,"close",m_close);
	// --- set up attributes (class scope) ---
	FLEXT_CADDATTR_VAR1(c,"universe",i_universe);  
	}
	FLEXT_CALLBACK(m_bang)
	FLEXT_THREAD(m_open)
	FLEXT_CALLBACK(m_close) 
	FLEXT_ATTRVAR_I(i_universe) // wrapper functions (get and set) for integer variable universe


};

// instantiate the class (constructor takes no arguments)
FLEXT_NEW("ola2pd",ola2pd)

/*
 * Called when there is new DMX data
 */

void ola2pd::NewDmx(unsigned int universe,
                        const DmxBuffer &buffer,
                        const string &error) {
  m_buffer.Set(buffer);
  m_counter++;
  gettimeofday(&m_last_data, NULL);   
  int z = 0;
  AtomList dmxlist;
  dmxlist(512);  
  for(z=0; z < 512; z++){SetFloat(dmxlist[z],(m_buffer.Get(z)));}
  ToOutList(0, dmxlist);
}

/*
 * Check for data loss.
 */

bool ola2pd::CheckDataLoss() {
  struct timeval now, diff;

  if (timerisset(&m_last_data)) {
    gettimeofday(&now, NULL);
    timersub(&now, &m_last_data, &diff);
    if (diff.tv_sec > 2 || (diff.tv_sec == 2 && diff.tv_usec > 500000)) {
      // loss of data
      post("ola2pd:Data Loss!");
    }
  }
  return true;
}

/*
 * Control de errores en el registro de Universos en OLA
 */ 

void ola2pd::RegisterComplete(const string &error) {
  if (!error.empty()) {
    post("ola2pd:Register command failed");
    m_client.GetSelectServer()->Terminate();
  }
}

/*
 * Called when there is input from the keyboard. Necesarios??
 *

void ola2pd::StdinReady() {
    post("ola2pd:Stdinready");
 }

/*
 * Método free, ¿cómo hay que declaralos en flext?
 *


void ola2pd::Clean() {
  m_close();
}
*/

