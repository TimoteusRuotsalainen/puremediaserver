// -*- mode: C++ -*-
#ifndef _CITPDEFINES_H_
#define _CITPDEFINES_H_

#include <QObject>
//
// The CITP data structures and constants
//

//
// Datatypes
//

typedef qint8   int8;
typedef qint16  int16;
typedef qint32  int32;
typedef qint64  int64;

typedef quint8  uint8;
typedef quint16 uint16;
typedef quint32 uint32;
typedef quint64 uint64;

typedef QByteArray ucs1;
typedef QString ucs2;

// 
// Content Type cookies
//

#define COOKIE_CITP       0x50544943 // 'CITP'
#define COOKIE_PINF       0x464e4950 // 'PINF'
#define COOKIE_PINF_PNAM  0x6d614e50 // 'PNam'
#define COOKIE_PINF_PLOC  0x636f4c50 // 'PLoc'

#pragma pack(1)

//
// CITP, Base Layer
//
struct CITP_Header 
{ 
  uint32      Cookie;                        // Set to "CITP".      
  uint8       VersionMajor;                  // Set to 1. 
  uint8       VersionMinor;                  // Set to 0.       
  uint8       Reserved[2];                   // 4-byte alignment       
  uint32      MessageSize;                   // The size of the entire message, including this header.      
  uint16      MessagePartCount;              // Number of message fragments.      
  uint16      MessagePart;                   // Index of this message fragment (0-based).      
  uint32      ContentType;                   // Cookie identifying the type of contents (the name of the second layer). 
}; 

//
// CITP/PINF, Peer Information Layer
//

#define CITP_PINF_MULTICAST_IP   "224.0.0.180"
#define CITP_PINF_MULTICAST_PORT 4809
#define TRANSMIT_INTERVAL_MS  5000

struct CITP_PINF_Header 
{ 
  CITP_Header CITPHeader;    // The CITP header. CITP ContentType is "PINF". 
  uint32      ContentType;   // A cookie defining which PINF message it is. 
}; 

// DEPRECATED, USE PLoc instead
// Peer Name message
struct CITP_PINF_PNam 
{ 
  CITP_PINF_Header  CITPPINFHeader;   // The CITP PINF header. PINF ContentType is "PNam".          
  QByteArray        Name;             // The display name of the peer (null terminated).
                                      // This could be anything from a 
                                      // user defined alias for the peer of the name of the
                                      // product, or a combination. 
}; 

// Peer Location message
struct CITP_PINF_PLoc 
{ 
  CITP_PINF_Header  CITPPINFHeader;     // The CITP PINF header. PINF ContentType is "PLoc". 
  uint16            ListeningTCPPort;   // The port on which the peer is listening for 
                                        // incoming TCP connections. 0 if not listening. 
  ucs1              Type[];             // Can be "LightingConsole", "MediaServer", 
                                        // "Visualizer" or "OperationHub". 
  ucs1              Name[];             // The display name of the peer. Corresponds to the 
                                        //     PINF/PNam/Name field. 
  ucs1              State[];            // The display state of the peer. This can be any 
                                       //     descriptive string presentable to the user such
                                        //     as "Idle", "Running" etc. 
};


#endif // _CITPDEFINES_H_
