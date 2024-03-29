/* 
 * ola2pd - interface from Open Lighting Arquitecture to Pure Data
 * v 0.02 - 5/1/2013
 *
 * Copyright (c) 2012-2013 Santiago Noreña (libremediaserver@gmail.com) 
 *
 * ola2pd is an external for Pure Data and Max that reads one DMX512 
 * universe from the Open Lighting Arquitecture and output it like a list
 * of 512 channels.  
 *
 * Based on dmxmonitor Copyright (C) 2001 Dirk Jagdmann <doj@cubic.org> 
 * and ola_dmxmonitor by Simon Newton (nomis52<AT>gmail.com) 
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
#define FLEXT_USE_CMEM 1
//#define FLEXT_SHARED 1
#define FLEXT_ATTRIBUTES 1
#include <flext.h>
#if !defined(FLEXT_VERSION) || (FLEXT_VERSION < 500)
#error You need at least flext version 0.5.0
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

// Define general

#include <errno.h>
#include <string>

// Define ola

#include <ola/BaseTypes.h>
#include <ola/Callback.h>
#include <ola/OlaCallbackClient.h>
#include <ola/OlaClientWrapper.h>
#include <ola/DmxBuffer.h>
#include <ola/io/SelectServer.h>
#include <ola/network/TCPSocket.h>

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
    m_universe(0),
		m_clientpointer(NULL),    
		m_counter(0)
//          m_stdin_descriptor(STDIN_FILENO)
    	{
		// --- define inlets and outlets ---
		AddInAnything(); // default inlet
		AddOutList();	// outlet for DMX list
   	post("ola2pd v0.02 - an interface to Open Lighting Arquitecture");
		post("(C) 2012-2013 Santi Noreña libremediaserver@gmail.com");
}
	void NewDmx(unsigned int universe,
                const DmxBuffer &buffer,
                const string &error);
        void RegisterComplete(const string &error);
//      void StdinReady();
        bool CheckDataLoss();

protected:

void m_open() {
 	if (m_clientpointer == NULL) 
	{     
        // set up ola connection 
        if (!m_client.Setup()) {
            post("%s: %s",thisName(), strerror(errno));
        }
        m_clientpointer = m_client.GetClient();
        m_clientpointer->SetDmxCallback(ola::NewCallback(this, &ola2pd::NewDmx));
        m_clientpointer->RegisterUniverse(m_universe,ola::REGISTER,ola::NewSingleCallback(this, &ola2pd::RegisterComplete));
//      m_client.GetSelectServer()->AddReadDescriptor(&m_stdin_descriptor);
//      m_stdin_descriptor.SetOnData(ola::NewCallback(this, &ola2pd::StdinReady));
        m_client.GetSelectServer()->RegisterRepeatingTimeout(5000,ola::NewCallback(this, &ola2pd::CheckDataLoss));
        post("ola2pd: Init complete. Start listening...");
			  m_client.GetSelectServer()->Run();
        }    
	}

void m_close() {
    if (m_clientpointer != NULL)
		{
   		m_clientpointer->RegisterUniverse(m_universe,ola::UNREGISTER,ola::NewSingleCallback(this, &ola2pd::RegisterComplete));        
    	m_client.GetSelectServer()->Terminate();
			post("ola2pd: Close complete");		
		  m_clientpointer = NULL;	
	  }
}

void m_bang() {
    if (m_clientpointer != NULL) {post("%s listening on universe %d",thisName(),m_universe);}
		else {post("%s configured on universe %d. Send open to start listening",thisName(),m_universe);}
	}

private:
    unsigned int m_universe;
    unsigned int m_counter;
//    ola::io::UnmanagedFileDescriptor m_stdin_descriptor;
    struct timeval m_last_data;
    OlaCallbackClientWrapper m_client;
		OlaCallbackClient *m_clientpointer;

  static void setup(t_classid c)
	{
	// --- set up methods (class scope) ---
	// register a bang method to the default inlet (0)
	FLEXT_CADDBANG(c,0,m_bang);
	// set up tagged methods for the default inlet (0)
	FLEXT_CADDMETHOD_(c,0,"open",m_open);
	FLEXT_CADDMETHOD_(c,0,"close",m_close);
	// --- set up attributes (class scope) ---
	FLEXT_CADDATTR_VAR1(c,"universe",m_universe);  
	}
	FLEXT_CALLBACK(m_bang)
	FLEXT_THREAD(m_open)
	FLEXT_CALLBACK(m_close) 
	FLEXT_ATTRVAR_I(m_universe) // wrapper functions (get and set) for integer variable universe
};
// instantiate the class (constructor takes no arguments)
FLEXT_NEW("ola2pd",ola2pd)

/*
 * Called when there is new DMX data
 */

void ola2pd::NewDmx(unsigned int universe,
                        const DmxBuffer &buffer,
                        const string &error) {
  m_counter++;
  gettimeofday(&m_last_data, NULL);   
  int z;
  AtomList dmxlist;
  dmxlist(512);  
  for(z=0; z < 512; z++){SetFloat(dmxlist[z],(buffer.Get(z)));}
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
    if (diff.tv_sec > 4 || (diff.tv_sec == 4 && diff.tv_usec > 4000000)) {
      // loss of data
      post("ola2pd: Can not read DMX!");
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
