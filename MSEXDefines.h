#ifndef MSEXDEFINES_H
#define MSEXDEFINES_H

#include "CITPDefines.h"
#include <QObject>

///////////////////////////////////////////
//
//               Defines
//
///////////////////////////////////////////

#define LISTENTCPPORT 0x2332
#define LAYER_NUMBER 0x08
#define TRANSMIT_INTERVAL_LSTA  1000
#define NAME "PureMediaServer"
#define STATE "testing..."
#define UUID "aaaaaaaa-eeee-ffff-dddd-bbbbbbbbbbbb"
#define PATHMEDIA "/home/santi/PMS/svn/media"

//////////////////////////////////////////////
//                                          //
//  Estructuras auxiliares                  //
//                                          //
//////////////////////////////////////////////
// Estructura Layer Status V1.0
struct LayerStatus {

    quint8 LayerNumber;
    quint8 PhysicalOutput;
    quint8 MediaLibraryNumber;
    quint8 MediaNumber;
    QString Name;
    quint32 MediaPosition;
    quint32 MediaLength;
    quint8  MediaFPS;
    quint32 LayerStatusFlags;
};


// Estructura Media Library ID. MSex 1.1
struct MSEXLibraryId {
    quint8 Level;  // 0 - 3
    quint8 Level1; // Sublevel 1 specifier, when Depth >= 1.
    quint8 Level2; // Sublevel 2 specifier, when Depth >= 2.
    quint8 Level3; // Sublevel 3 specifier, when Depth == 3.
};

// Media Information for MELIn packages. v1.0
struct MediaInformation {
    uint8 Number; // 0-based contiguous index of the media.
//    uint32 SerialNumber; MSEX 1.2
    uint8 DMXRangeMin;// DMX range start value.
    uint8 DMXRangeMax;// DMX range end value.
    QString MediaName;// Media name.
    uint64 MediaVersionTimestamp;// Media version in seconds since 1st January 1970.
    uint16 MediaWidth; // Media width.
    uint16 MediaHeight;// Media height.
    uint32 MediaLength;// Media length (in frames).
    uint8  MediaFPS;// Media resolution (in frames per second).
};

// Media Library for ELin packages v1.0
struct MediaLibrary {
    quint8 m_Id; // Library id.
    quint8 m_DMXRangeMin;// DMX range start value.
    quint8 m_DMXRangeMax;// DMX range end value.
    QString m_Name;// Library name.
//    quint8 m_LibraryCount;// Number of sub libraries in the library. MSEX 1.2
    quint8 m_ElementCount;// Number of elements in the library.
    QList<MediaInformation> m_MediaInformation; // Pointer to the Medias Information List of this Library
};

// for ELUP
struct AffectedItems
{
    uint8 ItemSet[32]; // A set of 256 bits used to indicate which item numbers have been changed
};

//////////////////////////////////////////////
//                                          //
//        Estructuras de paquetes           //
//                                          //
//////////////////////////////////////////////

// MSEX Header
struct CITP_MSEX_Header
{
  CITP_Header CITPHeader;    // The CITP header. CITP ContentType is "MSEX".
  uint8 VersionMajor;
  uint8 VersionMinor;
  uint32 ContentType;   // A cookie defining which MSEX message it is.
};

// Mensaje para solicitar una conexión
struct CITP_MSEX_CInf
{
    CITP_MSEX_Header CITPMSEXHeader; // CITP MSEX header. MSEX ContentType is "CInf". Version is 1.2.
    uint8 	SupportedMSEXVersionsCount; // Number of following MSEX version pairs.
    uint16 SupportedMSEXVersions[]; // Each 2 byte value is MSB = major MSEX version,	LSB = minor MSEX version.
    uint FutureMessageData[]; // A hint that future versions of this message may contain trailing data.
};

// Answer to CInf Versiones 1.0 y 1.1
struct CITP_MSEX_10_SINF
{
    CITP_MSEX_Header CITPMSEXHeader; // CITP MSEX header. MSEX ContentType is "SInf". Version is set to 1.0.
    ucs2 ProductName;// Display name of the product.
    uint8 ProductVersionMajor;// Major version number of the product.
    uint8 ProductVersionMinor;// Minor version number of the product.
    uint8 LayerCount;// Number of following layer information blocks.
/*  struct LayerInformation
    {
        ucs1 DMXSource[];// DMX-source connection string. See DMX Connection Strings in Definitions.
    };*/
};

// Answer to CINf  V1.2
struct CITP_MSEX_12_SINF
{
    CITP_MSEX_Header CITPMSEXHeader;// CITP MSEX header. MSEX ContentType is "SInf". Version is at least 1.2
                                                                    //and is the highest common version supported by both server and client.
    QByteArray uuid; 					// A standard 36 character UUID that uniquely identifies this media server
    QString ProductName;				// Display name of the product.
    uint8 ProductVersionMajor;			// Major version number of the product.
    uint8 ProductVersionMinor;			// Minor version number of the product.
    uint8 ProductVersionBugfix;			// Bugfix version number of the product.
    uint8 SupportedMSEXVersionsCount; // Number of following MSEX version pairs.
    uint16 SupportedMSEXVersions;     // Each 2 byte value is MSB = major MSEX version, LSB = minor MSEX version
    uint16 SupportedLibraryTypes;     // Bit-encoded flagword that identifies which library types are provided by
                                      //the media server.
    uint8 ThumbnailFormatsCount;	// Number of following thumbnail format cookies
    QByteArray ThumbnailFormats;	// Must include "RGB8", but can also include "JPEG" and "PNG "
    uint8 StreamFormatsCount;		// Number of following stream format cookies
    QByteArray StreamFormats;		// Must include "RGB8", but can also include "JPEG" and "PNG "
    uint8 LayerCount;				// Number of following layer information blocks.
    QString LayerInf;
};

// Nack Negative Acknowledge Message
struct CITP_MSEX_Nack
{
    CITP_MSEX_Header CITPMSEXHeader;  // CITP MSEX header. MSEX ContentType is "Nack" and version is 1.2.
    uint32 ReceivedContentType;       // MSEX message type of the message being NACKed (e.g. "GELT" if the Media
                                                                      //Server does not support library thumbnails)
};

// Layer Status Message 1.0
struct CITP_MSEX_10_LSta
{
    CITP_MSEX_Header CITPMSEXHeader; // CITP MSEX header. MSEX ContentType is "LSta" and version is 1.0.
    uint8 LayerCount;								 // Number of following layer information blocks.
    struct LayerStatus
    {
        uint8 LayerNumber; 				// 0-based layer number, corresponding to the layers reported in the SInf message.
        uint8 PhysicalOutput;     // Current physical video output index,0-based.
        uint8 MediaLibraryNumber; // Current media library number.
        uint8 MediaNumber;        // Current media number.
        ucs2 MediaName[];         // Current media name.
        uint32 MediaPosition;     // Current media position (in frames).
        uint32 MediaLength;				// Current media length (in frames).
        uint8 MediaFPS;           // Current media resolution in frames per second.
        uint32 LayerStatusFlags;  // Current layer status flags
                                                            // 0x0001 MediaPlaying
    };
};

// Layer Status Message version 1.2
struct CITP_MSEX_12_LSta
{
    CITP_MSEX_Header CITPMSEXHeader; // CITP MSEX header. MSEX ContentType is "LSta" and version is 1.2.
    uint8 LayerCount;								 // Number of following layer information blocks.
};

// Get Element Library v 1.0
struct CITP_MSEX_10_GELI
{
    CITP_MSEX_Header CITPMSEXHeader; // CITP MSEX header. MSEX ContentType is "GELI" and version is 1.0.
    uint8 LibraryType; // Content type requested.
    uint8 LibraryCount; // Number of libraries requested, set to 0 when requesting all available.
    uint8 LibraryNumbers[]; // Requested library numbers, none if LibraryCount is 0.
};

// Get Element Library v1.1
struct CITP_MSEX_11_GELI
{
    CITP_MSEX_Header CITPMSEXHeader; // CITP MSEX header. MSEX ContentType is "GELI" and version is 1.1.
    uint8 LibraryType; 							// Content type requested.
    MSEXLibraryId LibraryParentId; // Parent library id.
    uint8 LibraryCount; // Number of libraries requested, set to 0 when requesting all available.
    uint8 LibraryNumbers[]; // Requested library numbers, none if LibraryCount is 0.
};

// Get Element Library v1.2
struct CITP_MSEX_12_GELI
{
    CITP_MSEX_Header CITPMSEXHeader; // CITP MSEX header. MSEX ContentType is "GELI" and version is 1.2.
    uint8 LibraryType; // Content type requested.
    MSEXLibraryId LibraryParentId; // Parent library id.
    uint16 LibraryCount; // Number of libraries requested, set to 0 when requesting all available.
    uint8 LibraryNumbers[]; // Requested library numbers, none if LibraryCount is 0.
};

// Elemnt Library Information v1.0
struct CITP_MSEX_ELIn
{
    CITP_MSEX_Header CITPMSEXHeader; // CITP MSEX header. MSEX ContentType is "ELIn" and version is 1.1.
    uint8 LibraryType; // Content type requested.
    uint8 LibraryCount;// Number of following element library information blocks.
};

// Element Library Updated v1.2
struct CITP_MSEX_12_ELUp
{
    CITP_MSEX_Header CITPMSEXHeader; // CITP MSEX header. MSEX ContentType is "ELUp" and version is 1.2.
    uint8 LibraryType; // Content type of updated library.
    MSEXLibraryId LibraryId; // Library that has been updated.
    uint8 UpdateFlags;	// Additional information flags.
                                        // 0x01 Existing elements have been updated
                                            // 0x02 Elements have been added or removed
                                            // 0x04 Sub libraries have been updated
                                            // 0x08 Sub libraries have been added or removed
                                            // 0x10 All elements have been affected (ignore AffectedElements)
                                            // 0x20 All sub libraries have been affected (ignore AffectedLibraris)
    AffectedItems AffectedElements; 	// Which elements have been affected
    AffectedItems AffectedLibraries; // Which sub-libraries have been affected
};

// Get Element Information v1.1
struct CITP_MSEX_11_GEIn
{
    CITP_MSEX_Header CITPMSEXHeader; // CITP MSEX header. MSEX ContentType is "GEIn" and version is 1.1.
    uint8 LibraryType; // Content type requested.
    MSEXLibraryId LibraryId; // Library for which to retrieve elements
    uint8 ElementCount; // Number of elements for which information is requested, set to 0 when requesting all available.
    uint8 ElementNumbers[]; // Numbers of the elements for which information is requested.
};

// Get Element Information v1.0
struct CITP_MSEX_10_GEIn
{
    CITP_MSEX_Header CITPMSEXHeader; // CITP MSEX header. MSEX ContentType is "GEIn" and version is 1.1.
    quint8 LibraryType; // Content type requested.
    quint8 LibraryId; // Library for which to retrieve elements
    quint8 ElementCount; // Number of elements for which information is requested, set to 0 when requesting all available.
    quint8 ElementNumbers; // Numbers of the elements for which information is requested.
};

// Media Element Information v1.0
struct CITP_MSEX_10_MEIn
{
    CITP_MSEX_Header CITPMSEXHeader; // CITP MSEX header. MSEX ContentType is "MEIn" and version is 1.0
    quint8 LibraryId; // Library containing the media elements.
    qint8 ElementCount; // Number of following Media Information structs.
};

// Media Element Information v1.1
struct CITP_MSEX_11_MEIn
{
    CITP_MSEX_Header CITPMSEXHeader; // CITP MSEX header. MSEX ContentType is "MEIn" and version is 1.1.
    MSEXLibraryId LibraryId; // Library containing the media elements.
    qint8 ElementCount; // Number of following Media Information blocks.
};

// Media Element Information v1.2
struct CITP_MSEX_12_MEIn
{
    CITP_MSEX_Header CITPMSEXHeader; // CITP MSEX header. MSEX ContentType is "MEIn" and version is 1.1.
    MSEXLibraryId LibraryId; // Library containing the media elements.
    uint16 ElementCount; // Number of following Media Information blocks.
};

// Effect Element Information v1.2
struct CITP_MSEX_12_EEIn
{
    CITP_MSEX_Header CITPMSEXHeader; // CITP MSEX header. MSEX ContentType is "EEIn" and version is 1.1.
    MSEXLibraryId LibraryId;// Library containing the effect elements.
    uint16 ElementCount;// Number of following (effect) information blocks.
    struct EffectInformation
    {
        uint8 ElementNumber;
        uint32 SerialNumber;
        uint8 DMXRangeMin;
        uint8 DMXRangeMax;
        ucs2 EffectName[];
        uint8 EffectParameterCount;
        ucs2 EffectParameterNames[]; // List of effect parameter names.
    };
};

// Generic Element Informatio v1.2
struct CITP_MSEX_12_GLEI
{
    CITP_MSEX_Header CITPMSEXHeader; // CITP MSEX header. MSEX ContentType is "GLEI" and version is 1.1.
    MSEXLibraryId LibraryId;
    uint8 ElementCount;
    uint16 LibraryType;
    struct GenericInformation
    {
        uint8 ElementNumber;
        uint32 SerialNumber;
        uint8 DMXRangeMin;
        uint8 DMXRangeMax;
        ucs2 Name[];
        uint64 VersionTimestamp;
    };
};

// Get Element Libary Thumbnail
struct CITP_MSEX_12_GELT
{
    CITP_MSEX_Header CITPMSEXHeader;// CITP MSEX header. MSEX ContentType is "GELT" and version is 1.2.
    uint32 ThumbnailFormat;// Format of the thumbnail. Can be "RGB8" or "JPEG" (or "PNG " for MSEX 1.2 and up).
    uint16 ThumbnailWidth;// Preferred thumbnail image width.
    uint16 ThumbnailHeight;// Preferred thumbnail image height.
    uint8 ThumbnailFlags;// Additional information flags.
                                            // 0x01 Preserve aspect ratio of image (use width and height as maximum)
    uint8 LibraryType;// 1 for Media, 2 for Effects.
    uint16 LibraryCount;// Number of libraries requested, set to 0 when requesting all available.
    MSEXLibraryId LibraryIds[];// Ids of the libraries requested, not present if LibraryCount is 0.
};

// Element Library Thumbnail
struct CITP_MSEX_11_ELTh
{
    CITP_MSEX_Header CITPMSEXHeader;// CITP MSEX header. MSEX ContentType is "ELTh" and version is 1.1.
    uint8 LibraryType;// 1 for Media, 2 for Effects.
    MSEXLibraryId LibraryId;// Id of the library that the thumbnail belongs to.
    uint32 ThumbnailFormat;// Format of the thumbnail. Can be "RGB8" or "JPEG" (or "PNG " for MSEX 1.2 and up).
    uint16 ThumbnailWidth;// Thumbnail width.
    uint16 ThumbnailHeight;// Thumbnail height.
    uint16 ThumbnailBufferSize;// Size of the thumbnail buffer.
    uint8 ThumbnailBuffer;// Thumbnail image buffer.
};

// Get Element Thumbnail v1.0
struct CITP_MSEX_10_GETh
{
CITP_MSEX_Header CITPMSEXHeader;
uint32 ThumbnailFormat; // Can be "RGB8" or "JPEG" (or "PNG " for MSEX 1.2 and up).
uint16 ThumbnailWidth;
uint16 ThumbnailHeight;
uint8 ThumbnailFlags; // Additional information flags. 0x01 Preserve aspect ratio of image (use width and height as maximum)
uint8 LibraryType; // 1 for Media, 2 for Effects.
uint8 LibraryNumber; // Number of the media's library.
uint8 ElementCount; // Number of medias for which information is requested, set to 0 when requesting all available.
uint8 ElementNumber; // The numbers of the requested elements.Not present if ElementCount is 0.
};

// Get Element Thumbnail v1.2
struct CITP_MSEX_12_GETh
{
    CITP_MSEX_Header CITPMSEXHeader;// CITP MSEX header. MSEX ContentType is "GETh" and version is 1.2.
    uint32 ThumbnailFormat;// Format of the thumbnail.Can be "RGB8" or "JPEG" (or "PNG " for MSEX 1.2 and up).
    uint16 ThumbnailWidth; //Preferred thumbnail image width.
    uint16 ThumbnailHeight; //	Preferred thumbnail image height.
    uint8 ThumbnailFlags; //	Additional information flags.
                    //0x01 Preserve aspect ratio of image (use width and height as maximum)
    uint8 LibraryType; //1 for Media, 2 for Effects.
    MSEXLibraryId LibraryId; //Id of the media's library.
    uint16 ElementCount; //	Number of medias for which information is requested, set to 0 when requesting all available.
    uint8 ElementNumbers[]; //The numbers of the requested elements. Not present if ElementCount = 0. For MSEX 1.2 these are 0-based contiguous index values.
};

// Element Thumbnail v1.0
struct CITP_MSEX_10_ETHN
{
    CITP_MSEX_Header CITPMSEXHeader;// CITP MSEX header. MSEX ContentType is "EThn" and version is 1.1.
    uint8 LibraryType;// 1 for Media, 2 for Effects.
    uint8 LibraryId;// Id of the element's library.
    uint8 ElementNumber;// Number of the element (For MSEX 1.2 this is a 0-based contiguous index value).
    uint32 ThumbnailFormat;// Format of the thumbnail. Can be "RGB8" or "JPEG" (or "PNG " for MSEX 1.2 and up).
    uint16 ThumbnailWidth;// Thumbnail width.
    uint16 ThumbnailHeight;// Thumbnail height.
    uint16 ThumbnailBufferSize;// Size of the thumbnail buffer.
//    uint8 ThumbnailBuffer[];// Thumbnail image buffer.
};

// Element Thumbnail v1.1
struct CITP_MSEX_11_EThn
{
    CITP_MSEX_Header CITPMSEXHeader;// CITP MSEX header. MSEX ContentType is "EThn" and version is 1.1.
    uint8 LibraryType;// 1 for Media, 2 for Effects.
    MSEXLibraryId LibraryId;// Id of the element's library.
    uint8 ElementNumber;// Number of the element (For MSEX 1.2 this is a 0-based contiguous index value).
    uint32 ThumbnailFormat;// Format of the thumbnail. Can be "RGB8" or "JPEG" (or "PNG " for MSEX 1.2 and up).
    uint16 ThumbnailWidth;// Thumbnail width.
    uint16 ThumbnailHeight;// Thumbnail height.
    uint16 ThumbnailBufferSize;// Size of the thumbnail buffer.
    uint8 ThumbnailBuffer;// Thumbnail image buffer.
};

// Get Video Sources
struct CITP_MSEX_GVSr
{
    CITP_MSEX_Header CITPMSEXHeader; // CITP MSEX header. MSEX ContentType is "GVSr".
};

// Video Sources
struct CITP_MSEX_VSrc
{
    CITP_MSEX_Header CITPMSEXHeader;// CITP MSEX header. MSEX ContentType is "VSrc".
    uint16 SourceCount;// Number of following source information blocks.
    struct SourceInformation
    {
        uint16 SourceIdentifier; // Source identifier.
        signed char SourceName[];// Display name of the source (ie "Output 1", "Layer 2", "Camera 1" etc).
        uint8 PhysicalOutput;// If applicable, 0-based index designating the physical video output index. Otherwise 0xFF.
        uint8 LayerNumber;// If applicable, 0-based layer number, corresponding to the layers reported in the SInf message. Otherwise 0xFF.
        uint16 Flags;// Information flags.
                                 // 0x0001 Without effects
        uint16 Width;// Full width.
        uint16 Height;// Full height.
    };
};

// Request Stream
struct CITP_MSEX_RqSt
{
    CITP_MSEX_Header CITPMSEXHeader;// CITP MSEX header. MSEX ContentType is "RqSt".
    uint16 SourceIdentifier;// Identifier of the source requested.
    uint32 FrameFormat;// Requested frame format. Can be "RGB8" or "JPEG" (or "PNG " for MSEX 1.2 and up).
    uint16 FrameWidth;// Preferred minimum frame width.
    uint16 FrameHeight;// Preferred minimum frame height.
    uint8 fps;// Preferred minimum frames per second.
    uint8 Timeout;// Timeout in seconds (for instance 5 seconds, 0 to ask for only one frame).
};

// Stream Frame
struct CITP_MSEX_12_StFr
{
    CITP_MSEX_Header CITPMSEXHeader;// The CITP MSEX header. MSEX ContentType is "StFr".
    ucs1 MediaServerUUID[36];// Source media server UUID.
    uint16 SourceIdentifier;// Identifier of the frame's source.
    uint32 FrameFormat;// Requested frame format. Can be "RGB8" or "JPEG" (or "PNG " for MSEX 1.2 and up).
    uint16 FrameWidth;// Preferred minimum frame width.
    uint16 FrameHeight;// Preferred minimum frame height.
    uint16 FrameBufferSize;// Size of the frame image buffer.
    uint8 FrameBuffer[];// Frame image buffer.
};

#endif // MSEXDEFINES_H

